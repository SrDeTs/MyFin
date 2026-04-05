#include "jellyfinclient.h"

#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QRandomGenerator>
#include <QUrlQuery>
#include <QUuid>

namespace {
QString headerValue(const QString &deviceId, const QString &token = {})
{
    QString value = QStringLiteral("MediaBrowser Client=\"MyFin\", Device=\"Linux Desktop\", DeviceId=\"%1\", Version=\"0.1.0\"")
                        .arg(deviceId);
    if (!token.isEmpty()) {
        value += QStringLiteral(", Token=\"%1\"").arg(token);
    }
    return value;
}
}

JellyfinClient::JellyfinClient(QObject *parent)
    : QObject(parent)
    , m_deviceId(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

bool JellyfinClient::isConnected() const { return m_connected; }
bool JellyfinClient::isBusy() const { return m_busy; }
QString JellyfinClient::errorString() const { return m_errorString; }

void JellyfinClient::connectToServer(const QString &serverUrl,
                                     const QString &username,
                                     const QString &password)
{
    m_serverUrl = normalizedServerUrl(serverUrl);
    m_username = username.trimmed();
    m_password = password;
    m_accessToken.clear();
    m_userId.clear();

    if (!m_serverUrl.isValid() || m_username.isEmpty() || m_password.isEmpty()) {
        setErrorString(QStringLiteral("Preencha URL, usuario e senha do Jellyfin."));
        return;
    }

    setBusy(true);
    setErrorString({});

    const QUrl authUrl = m_serverUrl.resolved(QUrl(QStringLiteral("/Users/AuthenticateByName")));
    QNetworkRequest request = makeJsonRequest(authUrl);

    QJsonObject payload{
        {QStringLiteral("Username"), m_username},
        {QStringLiteral("Pw"), m_password}
    };

    QNetworkReply *reply = m_networkAccessManager.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray body = reply->readAll();
        const bool hasError = reply->error() != QNetworkReply::NoError;
        reply->deleteLater();

        if (hasError) {
            setBusy(false);
            setErrorString(QStringLiteral("Falha ao autenticar no Jellyfin: %1").arg(reply->errorString()));
            return;
        }

        const QJsonDocument document = QJsonDocument::fromJson(body);
        const QJsonObject object = document.object();
        const QJsonObject userObject = object.value(QStringLiteral("User")).toObject();
        m_accessToken = object.value(QStringLiteral("AccessToken")).toString();
        m_userId = userObject.value(QStringLiteral("Id")).toString();

        if (m_accessToken.isEmpty() || m_userId.isEmpty()) {
            setBusy(false);
            setErrorString(QStringLiteral("Resposta do Jellyfin invalida durante a autenticacao."));
            return;
        }

        m_connected = true;
        emit connectedChanged();
        emit authenticationSucceeded();
        fetchMusicLibrary();
    });
}

void JellyfinClient::disconnectFromServer()
{
    m_connected = false;
    m_accessToken.clear();
    m_userId.clear();
    emit connectedChanged();
}

void JellyfinClient::refreshLibrary()
{
    if (!m_connected) {
        setErrorString(QStringLiteral("Nenhuma conexao Jellyfin ativa."));
        return;
    }

    fetchMusicLibrary();
}

QNetworkRequest JellyfinClient::makeJsonRequest(const QUrl &url) const
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("X-Emby-Authorization", headerValue(m_deviceId).toUtf8());
    return request;
}

QNetworkRequest JellyfinClient::makeAuthenticatedRequest(const QUrl &url) const
{
    QNetworkRequest request(url);
    request.setRawHeader("X-Emby-Authorization", headerValue(m_deviceId, m_accessToken).toUtf8());
    request.setRawHeader("X-Emby-Token", m_accessToken.toUtf8());
    return request;
}

QUrl JellyfinClient::normalizedServerUrl(const QString &value) const
{
    QUrl url = QUrl::fromUserInput(value.trimmed());
    if (!url.path().endsWith('/')) {
        QString path = url.path();
        if (path.isEmpty()) {
            path = QStringLiteral("/");
        }
        url.setPath(path);
    }
    return url;
}

void JellyfinClient::setBusy(bool value)
{
    if (m_busy == value) {
        return;
    }
    m_busy = value;
    emit busyChanged();
}

void JellyfinClient::setErrorString(const QString &value)
{
    if (m_errorString == value) {
        return;
    }
    m_errorString = value;
    emit errorStringChanged();
}

void JellyfinClient::fetchMusicLibrary()
{
    setBusy(true);
    setErrorString({});

    QUrl url = m_serverUrl.resolved(QUrl(QStringLiteral("/Users/%1/Items").arg(m_userId)));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("Recursive"), QStringLiteral("true"));
    query.addQueryItem(QStringLiteral("IncludeItemTypes"), QStringLiteral("Audio"));
    query.addQueryItem(QStringLiteral("Fields"), QStringLiteral("Path,Album,RunTimeTicks,ImageTags"));
    url.setQuery(query);

    QNetworkReply *reply = m_networkAccessManager.get(makeAuthenticatedRequest(url));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray body = reply->readAll();
        const bool hasError = reply->error() != QNetworkReply::NoError;
        reply->deleteLater();

        setBusy(false);

        if (hasError) {
            setErrorString(QStringLiteral("Falha ao carregar biblioteca Jellyfin: %1").arg(reply->errorString()));
            return;
        }

        const QJsonDocument document = QJsonDocument::fromJson(body);
        const QJsonArray items = document.object().value(QStringLiteral("Items")).toArray();

        QVector<Track> tracks;
        tracks.reserve(items.size());

        for (const QJsonValue &value : items) {
            const QJsonObject item = value.toObject();
            const QString id = item.value(QStringLiteral("Id")).toString();
            const QString name = item.value(QStringLiteral("Name")).toString();
            const QJsonArray artists = item.value(QStringLiteral("Artists")).toArray();
            const QString artist = artists.isEmpty() ? QStringLiteral("Jellyfin") : artists.first().toString();
            const QString album = item.value(QStringLiteral("Album")).toString();
            const qint64 durationMs = item.value(QStringLiteral("RunTimeTicks")).toVariant().toLongLong() / 10000;

            QUrl streamUrl = m_serverUrl.resolved(QUrl(QStringLiteral("/Items/%1/Download").arg(id)));
            QUrlQuery streamQuery;
            streamQuery.addQueryItem(QStringLiteral("api_key"), m_accessToken);
            streamQuery.addQueryItem(QStringLiteral("download"), QStringLiteral("false"));
            streamQuery.addQueryItem(QStringLiteral("static"), QStringLiteral("true"));
            streamUrl.setQuery(streamQuery);

            QUrl artUrl = m_serverUrl.resolved(QUrl(QStringLiteral("/Items/%1/Images/Primary").arg(id)));
            QUrlQuery artQuery;
            artQuery.addQueryItem(QStringLiteral("api_key"), m_accessToken);
            artQuery.addQueryItem(QStringLiteral("maxWidth"), QStringLiteral("900"));
            artQuery.addQueryItem(QStringLiteral("quality"), QStringLiteral("90"));
            artUrl.setQuery(artQuery);

            tracks.append(Track{
                id,
                name.isEmpty() ? QStringLiteral("Faixa remota") : name,
                artist,
                album,
                durationMs,
                streamUrl,
                artUrl,
                true,
                QStringLiteral("Jellyfin")
            });
        }

        emit libraryReceived(tracks);
    });
}
