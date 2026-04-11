#include "infrastructure/jellyfin/JellyfinApiClient.h"

#include "infrastructure/logging/Logging.h"
#include "infrastructure/settings/SettingsService.h"
#include "infrastructure/storage/LibraryCache.h"
#include "linux/SecretStore.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrlQuery>
#include <functional>
#include <memory>

namespace MyFin::Infrastructure::Jellyfin {

namespace {

constexpr auto kAppName = "MyFin";
constexpr auto kAppVersion = "0.1.0";
constexpr auto kDeviceName = "Linux Desktop";

}

JellyfinApiClient::JellyfinApiClient(Settings::SettingsService& settings,
                                     Infrastructure::Storage::LibraryCache& libraryCache,
                                     Linux::SecretStore& secretStore,
                                     QObject* parent)
    : QObject(parent)
    , m_settings(settings)
    , m_libraryCache(libraryCache)
    , m_secretStore(secretStore)
{
    m_serverProfile.baseUrl = m_settings.serverUrl();
    m_serverProfile.username = m_settings.username();
    m_serverProfile.userId = m_settings.userId();
    m_homeTracks = m_libraryCache.loadHomeTracks();
    updateServerProfileLabel();
    restorePersistedSession();

    if (authenticated()) {
        QTimer::singleShot(0, this, [this] {
            fetchHomeFeed();
        });
    }
}

QString JellyfinApiClient::serverLabel() const
{
    return m_serverProfile.name;
}

QString JellyfinApiClient::userLabel() const
{
    return m_serverProfile.username;
}

bool JellyfinApiClient::authenticated() const
{
    return !m_accessToken.isEmpty() && !m_serverProfile.userId.isEmpty() && m_state != ConnectionState::Disconnected;
}

bool JellyfinApiClient::busy() const
{
    return m_activeRequests > 0;
}

QString JellyfinApiClient::connectionStateLabel() const
{
    switch (m_state) {
    case ConnectionState::Disconnected:
        return QStringLiteral("Disconnected");
    case ConnectionState::Authenticating:
        return QStringLiteral("Signing in…");
    case ConnectionState::Connected:
        return QStringLiteral("Connected");
    case ConnectionState::Syncing:
        return QStringLiteral("Syncing library…");
    }

    return QStringLiteral("Unknown");
}

QString JellyfinApiClient::lastError() const
{
    return m_lastError;
}

const Domain::ServerProfile& JellyfinApiClient::serverProfile() const
{
    return m_serverProfile;
}

const QVector<Domain::Track>& JellyfinApiClient::currentHomeTracks() const
{
    return m_homeTracks;
}

QUrl JellyfinApiClient::buildPlaybackUrl(const QString& itemId) const
{
    if (!authenticated() || itemId.isEmpty()) {
        return {};
    }

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("static"), QStringLiteral("true"));
    query.addQueryItem(QStringLiteral("userId"), m_serverProfile.userId);
    query.addQueryItem(QStringLiteral("deviceId"), m_settings.deviceId());
    query.addQueryItem(QStringLiteral("api_key"), m_accessToken);
    return buildServerRelativeUrl(QStringLiteral("/Audio/%1/stream").arg(itemId), query);
}

QUrl JellyfinApiClient::buildArtworkUrl(const QString& artworkKey, const QSize& requestedSize) const
{
    if (!authenticated() || artworkKey.isEmpty()) {
        return {};
    }

    const QStringList parts = artworkKey.split(QLatin1Char(':'), Qt::KeepEmptyParts);
    if (parts.isEmpty() || parts.constFirst().isEmpty()) {
        return {};
    }

    QUrlQuery query;
    const int width = requestedSize.isValid() ? requestedSize.width() : 320;
    const int height = requestedSize.isValid() ? requestedSize.height() : 320;
    if (width > 0) {
        query.addQueryItem(QStringLiteral("maxWidth"), QString::number(width));
    }
    if (height > 0) {
        query.addQueryItem(QStringLiteral("maxHeight"), QString::number(height));
    }
    query.addQueryItem(QStringLiteral("quality"), QStringLiteral("90"));
    query.addQueryItem(QStringLiteral("api_key"), m_accessToken);
    if (parts.size() > 1 && !parts.at(1).isEmpty()) {
        query.addQueryItem(QStringLiteral("tag"), parts.at(1));
    }

    return buildServerRelativeUrl(QStringLiteral("/Items/%1/Images/Primary").arg(parts.constFirst()), query);
}

QString JellyfinApiClient::coverSourceForArtworkKey(const QString& artworkKey) const
{
    if (artworkKey.isEmpty()) {
        return {};
    }

    return QStringLiteral("image://cover/%1").arg(artworkKey);
}

void JellyfinApiClient::reportPlaybackStart(const QString& itemId,
                                            const QString& playSessionId,
                                            qint64 positionMs)
{
    if (!authenticated() || itemId.isEmpty() || playSessionId.isEmpty()) {
        return;
    }

    postJsonSilently(QStringLiteral("/Sessions/Playing"),
                     buildPlaybackPayload(itemId, playSessionId, positionMs, false, -1),
                     QStringLiteral("playback start"));
}

void JellyfinApiClient::reportPlaybackProgress(const QString& itemId,
                                               const QString& playSessionId,
                                               qint64 positionMs,
                                               bool paused,
                                               int volumeLevel)
{
    if (!authenticated() || itemId.isEmpty() || playSessionId.isEmpty()) {
        return;
    }

    postJsonSilently(QStringLiteral("/Sessions/Playing/Progress"),
                     buildPlaybackPayload(itemId, playSessionId, positionMs, paused, volumeLevel),
                     QStringLiteral("playback progress"));
}

void JellyfinApiClient::reportPlaybackStopped(const QString& itemId,
                                              const QString& playSessionId,
                                              qint64 positionMs)
{
    if (!authenticated() || itemId.isEmpty() || playSessionId.isEmpty()) {
        return;
    }

    postJsonSilently(QStringLiteral("/Sessions/Playing/Stopped"),
                     buildPlaybackPayload(itemId, playSessionId, positionMs, false, -1),
                     QStringLiteral("playback stopped"));
}

void JellyfinApiClient::markTrackPlayed(const QString& itemId)
{
    if (!authenticated() || itemId.isEmpty() || m_serverProfile.userId.isEmpty()) {
        return;
    }

    const QString path = QStringLiteral("/Users/%1/PlayedItems/%2").arg(m_serverProfile.userId, itemId);
    postWithoutBodySilently(path, QStringLiteral("mark track played"));
}

void JellyfinApiClient::login(const QString& serverUrl, const QString& username, const QString& password)
{
    if (busy()) {
        return;
    }

    const QString trimmedServerUrl = serverUrl.trimmed();
    const QString trimmedUsername = username.trimmed();
    if (trimmedServerUrl.isEmpty() || trimmedUsername.isEmpty() || password.isEmpty()) {
        setLastError(QStringLiteral("Server URL, username, and password are required."));
        emit requestFailed(m_lastError);
        return;
    }

    const QUrl normalizedUrl = normalizeServerUrl(trimmedServerUrl);
    if (!normalizedUrl.isValid()) {
        setLastError(QStringLiteral("Invalid server URL. Include the full http:// or https:// address."));
        emit requestFailed(m_lastError);
        return;
    }

    m_serverProfile.baseUrl = normalizedUrl;
    m_serverProfile.username = trimmedUsername;
    updateServerProfileLabel();
    emit sessionChanged();

    setLastError({});
    setState(ConnectionState::Authenticating);

    QNetworkRequest request = buildRequest(QStringLiteral("/Users/AuthenticateByName"), false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    const QJsonObject payload{
        {QStringLiteral("Username"), trimmedUsername},
        {QStringLiteral("Pw"), password},
    };

    beginRequest();
    QNetworkReply* reply = m_network.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply, normalizedUrl, trimmedUsername] {
        const QByteArray body = reply->readAll();
        const bool hasNetworkError = reply->error() != QNetworkReply::NoError;
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        endRequest();
        reply->deleteLater();

        if (hasNetworkError || statusCode >= 400) {
            const QString error = responseErrorMessage(reply, body, QStringLiteral("Failed to authenticate with Jellyfin."));
            qWarning(lcJellyfin) << "Authentication failed:" << error;
            m_accessToken.clear();
            setState(ConnectionState::Disconnected);
            setLastError(error);
            emit requestFailed(error);
            return;
        }

        const QJsonDocument document = QJsonDocument::fromJson(body);
        const QJsonObject root = document.object();
        const QString accessToken = root.value(QStringLiteral("AccessToken")).toString();
        const QJsonObject user = root.value(QStringLiteral("User")).toObject();
        const QString userId = user.value(QStringLiteral("Id")).toString();
        const QString userName = user.value(QStringLiteral("Name")).toString(trimmedUsername);

        if (accessToken.isEmpty() || userId.isEmpty()) {
            const QString error = QStringLiteral("Jellyfin authentication response is missing session information.");
            qWarning(lcJellyfin) << error;
            m_accessToken.clear();
            setState(ConnectionState::Disconnected);
            setLastError(error);
            emit requestFailed(error);
            return;
        }

        m_accessToken = accessToken;
        m_serverProfile.baseUrl = normalizedUrl;
        m_serverProfile.username = userName;
        m_serverProfile.userId = userId;
        updateServerProfileLabel();

        m_settings.setServerUrl(normalizedUrl);
        m_settings.setUsername(userName);
        m_settings.setUserId(userId);
        if (m_settings.serverName().trimmed().isEmpty() || m_settings.serverName().trimmed() == QStringLiteral("Demo Jellyfin")) {
            m_settings.setServerName(normalizedUrl.host());
        }
        bool persistedSecurely = false;
        if (!m_secretStore.isAvailable()) {
            qInfo(lcJellyfin) << "Secret store not available; persisting session token in local settings fallback";
        } else {
            const QString accountKey = secretAccountKey(normalizedUrl, userId);
            persistedSecurely = m_secretStore.storeToken(accountKey, accessToken);
            if (!persistedSecurely) {
                qWarning(lcJellyfin) << "Failed to persist Jellyfin access token in the secret store; using local settings fallback";
            }
        }

        if (persistedSecurely) {
            m_settings.setSessionToken({});
        } else {
            m_settings.setSessionToken(accessToken);
        }

        qInfo(lcJellyfin) << "Authenticated against" << normalizedUrl << "as user" << userName;
        setState(ConnectionState::Connected);
        setLastError({});
        emit sessionChanged();
        emit loginSucceeded();
        fetchHomeFeed();
    });
}

void JellyfinApiClient::logout()
{
    const QString accountKey = secretAccountKey(m_serverProfile.baseUrl, m_serverProfile.userId);
    if (!accountKey.isEmpty() && m_secretStore.isAvailable()) {
        m_secretStore.removeToken(accountKey);
    }

    m_accessToken.clear();
    m_serverProfile.userId.clear();
    m_settings.setUserId({});
    m_settings.setSessionToken({});
    setState(ConnectionState::Disconnected);
    setLastError({});
    emit sessionChanged();
    emit loggedOut();
}

void JellyfinApiClient::fetchHomeFeed(int limit)
{
    if (!authenticated()) {
        const QString error = QStringLiteral("Sign in to Jellyfin before refreshing the library.");
        setLastError(error);
        emit requestFailed(error);
        return;
    }

    if (busy()) {
        return;
    }

    setLastError({});
    setState(ConnectionState::Syncing);

    const int pageSize = qBound(100, limit, 500);
    auto aggregatedTracks = std::make_shared<QVector<Domain::Track>>();
    auto fetchPage = std::make_shared<std::function<void(int)>>();

    *fetchPage = [this, pageSize, aggregatedTracks, fetchPage](int startIndex) {
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("userId"), m_serverProfile.userId);
        query.addQueryItem(QStringLiteral("recursive"), QStringLiteral("true"));
        query.addQueryItem(QStringLiteral("includeItemTypes"), QStringLiteral("Audio"));
        query.addQueryItem(QStringLiteral("enableUserData"), QStringLiteral("true"));
        query.addQueryItem(QStringLiteral("sortBy"), QStringLiteral("DateCreated"));
        query.addQueryItem(QStringLiteral("sortOrder"), QStringLiteral("Descending"));
        query.addQueryItem(QStringLiteral("startIndex"), QString::number(startIndex));
        query.addQueryItem(QStringLiteral("limit"), QString::number(pageSize));

        beginRequest();
        QNetworkReply* reply = m_network.get(buildRequest(QStringLiteral("/Items"), true, query));
        connect(reply, &QNetworkReply::finished, this, [this, reply, startIndex, pageSize, aggregatedTracks, fetchPage] {
            const QByteArray body = reply->readAll();
            const bool hasNetworkError = reply->error() != QNetworkReply::NoError;
            const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

            endRequest();
            reply->deleteLater();

            if (hasNetworkError || statusCode >= 400) {
                const QString error = responseErrorMessage(reply, body, QStringLiteral("Failed to load the Jellyfin library."));
                qWarning(lcJellyfin) << "Home feed request failed:" << error;
                if (statusCode == 401) {
                    qWarning(lcJellyfin) << "Cached Jellyfin session is no longer valid; clearing the persisted token";
                    logout();
                    setLastError(error);
                    emit requestFailed(error);
                    return;
                }
                setState(ConnectionState::Connected);
                setLastError(error);
                emit requestFailed(error);
                return;
            }

            const QJsonDocument document = QJsonDocument::fromJson(body);
            const QJsonObject root = document.object();
            const QVector<Domain::Track> pageTracks = parseTracks(root.value(QStringLiteral("Items")).toArray());
            const int totalRecordCount = root.value(QStringLiteral("TotalRecordCount")).toInt(-1);

            aggregatedTracks->reserve(aggregatedTracks->size() + pageTracks.size());
            for (const Domain::Track& track : pageTracks) {
                aggregatedTracks->push_back(track);
            }

            const bool reachedKnownEnd = totalRecordCount >= 0 && aggregatedTracks->size() >= totalRecordCount;
            const bool reachedShortPage = pageTracks.size() < pageSize;
            if (!pageTracks.isEmpty() && !reachedKnownEnd && !reachedShortPage) {
                (*fetchPage)(startIndex + pageTracks.size());
                return;
            }

            m_homeTracks = *aggregatedTracks;
            m_libraryCache.saveHomeTracks(m_homeTracks);

            qInfo(lcJellyfin) << "Fetched" << m_homeTracks.size() << "tracks from Jellyfin";
            setState(ConnectionState::Connected);
            emit homeFeedReady();
        });
    };

    (*fetchPage)(0);
}

QUrl JellyfinApiClient::normalizeServerUrl(const QString& value)
{
    const QUrl parsed = QUrl::fromUserInput(value.trimmed());
    if (!parsed.isValid() || parsed.scheme().isEmpty() || parsed.host().isEmpty()) {
        return {};
    }

    QUrl normalized = parsed.adjusted(QUrl::StripTrailingSlash);
    return normalized;
}

QString JellyfinApiClient::escapeHeaderValue(QString value)
{
    value.replace(QLatin1Char('\\'), QStringLiteral("\\\\"));
    value.replace(QLatin1Char('"'), QStringLiteral("\\\""));
    return value;
}

QString JellyfinApiClient::extractArtist(const QJsonObject& item)
{
    const QJsonArray artists = item.value(QStringLiteral("Artists")).toArray();
    if (!artists.isEmpty()) {
        return artists.at(0).toString();
    }

    const QJsonArray artistItems = item.value(QStringLiteral("ArtistItems")).toArray();
    if (!artistItems.isEmpty()) {
        return artistItems.at(0).toObject().value(QStringLiteral("Name")).toString();
    }

    return item.value(QStringLiteral("AlbumArtist")).toString(QStringLiteral("Unknown Artist"));
}

QString JellyfinApiClient::extractContainerLabel(const QJsonObject& item)
{
    const QJsonArray mediaSources = item.value(QStringLiteral("MediaSources")).toArray();
    if (!mediaSources.isEmpty()) {
        const QString container = mediaSources.at(0).toObject().value(QStringLiteral("Container")).toString();
        if (!container.isEmpty()) {
            return QStringLiteral("Direct %1").arg(container.toUpper());
        }
    }

    const QString container = item.value(QStringLiteral("Container")).toString();
    if (!container.isEmpty()) {
        return QStringLiteral("Direct %1").arg(container.toUpper());
    }

    return QStringLiteral("Jellyfin Stream");
}

qint64 JellyfinApiClient::runtimeTicksToMs(qint64 runtimeTicks)
{
    return runtimeTicks <= 0 ? 0 : runtimeTicks / 10000;
}

qint64 JellyfinApiClient::msToTicks(qint64 durationMs)
{
    return durationMs <= 0 ? 0 : durationMs * 10000;
}

QString JellyfinApiClient::secretAccountKey(const QUrl& baseUrl, const QString& userId)
{
    if (!baseUrl.isValid() || userId.isEmpty()) {
        return {};
    }

    return QStringLiteral("%1|%2").arg(baseUrl.toString(QUrl::RemoveQuery | QUrl::RemoveFragment), userId);
}

QString JellyfinApiClient::buildAuthorizationHeader() const
{
    QStringList parts{
        QStringLiteral("MediaBrowser Client=\"%1\"").arg(escapeHeaderValue(QString::fromLatin1(kAppName))),
        QStringLiteral("Device=\"%1\"").arg(escapeHeaderValue(QString::fromLatin1(kDeviceName))),
        QStringLiteral("DeviceId=\"%1\"").arg(escapeHeaderValue(m_settings.deviceId())),
        QStringLiteral("Version=\"%1\"").arg(escapeHeaderValue(QString::fromLatin1(kAppVersion))),
    };

    if (!m_accessToken.isEmpty()) {
        parts.push_back(QStringLiteral("Token=\"%1\"").arg(escapeHeaderValue(m_accessToken)));
    }

    return parts.join(QStringLiteral(", "));
}

QNetworkRequest JellyfinApiClient::buildRequest(const QString& path, bool includeToken, const QUrlQuery& query) const
{
    QUrl url = buildServerRelativeUrl(path, query);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QStringLiteral("%1/%2").arg(QString::fromLatin1(kAppName),
                                                  QString::fromLatin1(kAppVersion)));
    request.setRawHeader("Accept", "application/json");

    const QByteArray auth = buildAuthorizationHeader().toUtf8();
    request.setRawHeader("Authorization", auth);
    request.setRawHeader("X-Emby-Authorization", auth);

    if (includeToken && !m_accessToken.isEmpty()) {
        request.setRawHeader("X-Emby-Token", m_accessToken.toUtf8());
    }

    return request;
}

QJsonObject JellyfinApiClient::buildPlaybackPayload(const QString& itemId,
                                                    const QString& playSessionId,
                                                    qint64 positionMs,
                                                    bool paused,
                                                    int volumeLevel) const
{
    QJsonObject payload{
        {QStringLiteral("ItemId"), itemId},
        {QStringLiteral("CanSeek"), true},
        {QStringLiteral("IsMuted"), false},
        {QStringLiteral("IsPaused"), paused},
        {QStringLiteral("PlayMethod"), QStringLiteral("DirectStream")},
        {QStringLiteral("PlaySessionId"), playSessionId},
        {QStringLiteral("PositionTicks"), QJsonValue::fromVariant(msToTicks(positionMs))},
        {QStringLiteral("RepeatMode"), QStringLiteral("RepeatNone")},
    };

    if (volumeLevel >= 0) {
        payload.insert(QStringLiteral("VolumeLevel"), volumeLevel);
    }

    return payload;
}

void JellyfinApiClient::postJsonSilently(const QString& path,
                                         const QJsonObject& payload,
                                         const QString& operationName)
{
    QNetworkRequest request = buildRequest(path, true);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    QNetworkReply* reply = m_network.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [reply, operationName] {
        const QByteArray body = reply->readAll();
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const bool failed = reply->error() != QNetworkReply::NoError || statusCode >= 400;
        if (failed) {
            qWarning(lcJellyfin) << operationName << "request failed:" << statusCode
                                 << reply->errorString() << body;
        }
        reply->deleteLater();
    });
}

void JellyfinApiClient::postWithoutBodySilently(const QString& path, const QString& operationName)
{
    QNetworkReply* reply = m_network.post(buildRequest(path, true), QByteArray());
    connect(reply, &QNetworkReply::finished, this, [reply, operationName] {
        const QByteArray body = reply->readAll();
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const bool failed = reply->error() != QNetworkReply::NoError || statusCode >= 400;
        if (failed) {
            qWarning(lcJellyfin) << operationName << "request failed:" << statusCode
                                 << reply->errorString() << body;
        }
        reply->deleteLater();
    });
}

void JellyfinApiClient::restorePersistedSession()
{
    if (!m_accessToken.isEmpty() || !m_serverProfile.baseUrl.isValid() || m_serverProfile.userId.isEmpty()) {
        return;
    }

    QString restoredToken;

    if (m_secretStore.isAvailable()) {
        const QString accountKey = secretAccountKey(m_serverProfile.baseUrl, m_serverProfile.userId);
        if (!accountKey.isEmpty()) {
            restoredToken = m_secretStore.readToken(accountKey);
            if (!restoredToken.isEmpty()) {
                qInfo(lcJellyfin) << "Restored Jellyfin session token from Secret Service for" << m_serverProfile.baseUrl;
            }
        }
    }

    if (restoredToken.isEmpty()) {
        restoredToken = m_settings.sessionToken();
        if (!restoredToken.isEmpty()) {
            qWarning(lcJellyfin) << "Restored Jellyfin session token from local settings fallback";
        }
    }

    if (restoredToken.isEmpty()) {
        return;
    }

    m_accessToken = restoredToken;
    setState(ConnectionState::Connected);
}

QUrl JellyfinApiClient::buildServerRelativeUrl(const QString& path, const QUrlQuery& query) const
{
    const QString normalizedPath = path.startsWith(QLatin1Char('/')) ? path.mid(1) : path;
    QUrl base = m_serverProfile.baseUrl;
    QString basePath = base.path();
    if (!basePath.endsWith(QLatin1Char('/'))) {
        basePath.append(QLatin1Char('/'));
    }
    base.setPath(basePath);

    QUrl url = base.resolved(QUrl(normalizedPath));
    url.setQuery(query);
    return url;
}

QVector<Domain::Track> JellyfinApiClient::parseTracks(const QJsonArray& items) const
{
    QVector<Domain::Track> parsed;
    parsed.reserve(items.size());

    for (const QJsonValue& value : items) {
        const QJsonObject item = value.toObject();
        const QString id = item.value(QStringLiteral("Id")).toString();
        if (id.isEmpty()) {
            continue;
        }

        const QString imageTag = item.value(QStringLiteral("ImageTags")).toObject().value(QStringLiteral("Primary")).toString();
        const QString albumImageTag = item.value(QStringLiteral("AlbumPrimaryImageTag")).toString();
        const QString artworkKey = !imageTag.isEmpty()
                                       ? QStringLiteral("%1:%2").arg(id, imageTag)
                                       : QStringLiteral("%1:%2")
                                             .arg(item.value(QStringLiteral("AlbumId")).toString(id),
                                                  albumImageTag);

        Domain::Track track;
        track.id = id;
        track.title = item.value(QStringLiteral("Name")).toString(QStringLiteral("Unknown Track"));
        track.artist = extractArtist(item);
        track.album = item.value(QStringLiteral("Album")).toString(QStringLiteral("Unknown Album"));
        track.artworkKey = artworkKey;
        track.streamProfile = extractContainerLabel(item);
        track.durationMs = runtimeTicksToMs(item.value(QStringLiteral("RunTimeTicks")).toVariant().toLongLong());
        track.favorite = item.value(QStringLiteral("UserData")).toObject().value(QStringLiteral("IsFavorite")).toBool(false);

        parsed.push_back(track);
    }

    return parsed;
}

QString JellyfinApiClient::responseErrorMessage(QNetworkReply* reply,
                                                const QByteArray& body,
                                                const QString& fallback) const
{
    const QJsonDocument document = QJsonDocument::fromJson(body);
    if (document.isObject()) {
        const QJsonObject root = document.object();
        const QString message = root.value(QStringLiteral("Message")).toString();
        if (!message.isEmpty()) {
            return message;
        }
        const QString error = root.value(QStringLiteral("error")).toString();
        if (!error.isEmpty()) {
            return error;
        }
    }

    if (reply) {
        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 401) {
            return QStringLiteral("Authentication failed. Check your username and password.");
        }
        if (statusCode == 403) {
            return QStringLiteral("Jellyfin denied access to this request.");
        }
        if (statusCode >= 500) {
            return QStringLiteral("The Jellyfin server returned an internal error.");
        }

        if (reply->error() != QNetworkReply::NoError && !reply->errorString().isEmpty()) {
            return reply->errorString();
        }
    }

    return fallback;
}

void JellyfinApiClient::setState(ConnectionState state)
{
    if (m_state == state) {
        return;
    }

    m_state = state;
    emit sessionChanged();
}

void JellyfinApiClient::setLastError(const QString& error)
{
    if (m_lastError == error) {
        return;
    }

    m_lastError = error;
    emit lastErrorChanged();
}

void JellyfinApiClient::beginRequest()
{
    const bool wasBusy = busy();
    ++m_activeRequests;
    if (!wasBusy && busy()) {
        emit busyChanged();
    }
}

void JellyfinApiClient::endRequest()
{
    const bool wasBusy = busy();
    m_activeRequests = qMax(0, m_activeRequests - 1);
    if (wasBusy && !busy()) {
        emit busyChanged();
    }
}

void JellyfinApiClient::updateServerProfileLabel()
{
    const QString configuredName = m_settings.serverName().trimmed();
    if (!configuredName.isEmpty() && configuredName != QStringLiteral("Demo Jellyfin")) {
        m_serverProfile.name = configuredName;
        return;
    }

    if (!m_serverProfile.baseUrl.host().isEmpty()) {
        m_serverProfile.name = m_serverProfile.baseUrl.host();
        return;
    }

    m_serverProfile.name = QStringLiteral("Jellyfin");
}

}  // namespace MyFin::Infrastructure::Jellyfin
