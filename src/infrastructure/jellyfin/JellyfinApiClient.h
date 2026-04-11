#pragma once

#include "domain/entities/Track.h"
#include "domain/value_objects/ServerProfile.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QVector>

namespace MyFin::Infrastructure::Settings {
class SettingsService;
}

namespace MyFin::Linux {
class SecretStore;
}

namespace MyFin::Infrastructure::Storage {
class LibraryCache;
}

namespace MyFin::Infrastructure::Jellyfin {

class JellyfinApiClient final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString serverLabel READ serverLabel NOTIFY sessionChanged)
    Q_PROPERTY(QString userLabel READ userLabel NOTIFY sessionChanged)
    Q_PROPERTY(bool authenticated READ authenticated NOTIFY sessionChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString connectionStateLabel READ connectionStateLabel NOTIFY sessionChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
    explicit JellyfinApiClient(Settings::SettingsService& settings,
                               Infrastructure::Storage::LibraryCache& libraryCache,
                               Linux::SecretStore& secretStore,
                               QObject* parent = nullptr);

    QString serverLabel() const;
    QString userLabel() const;
    bool authenticated() const;
    bool busy() const;
    QString connectionStateLabel() const;
    QString lastError() const;
    const Domain::ServerProfile& serverProfile() const;
    const QVector<Domain::Track>& currentHomeTracks() const;
    QUrl buildPlaybackUrl(const QString& itemId) const;
    QUrl buildArtworkUrl(const QString& artworkKey, const QSize& requestedSize) const;
    QString coverSourceForArtworkKey(const QString& artworkKey) const;
    void reportPlaybackStart(const QString& itemId, const QString& playSessionId, qint64 positionMs = 0);
    void reportPlaybackProgress(const QString& itemId,
                                const QString& playSessionId,
                                qint64 positionMs,
                                bool paused,
                                int volumeLevel);
    void reportPlaybackStopped(const QString& itemId, const QString& playSessionId, qint64 positionMs);
    void markTrackPlayed(const QString& itemId);

    Q_INVOKABLE void login(const QString& serverUrl, const QString& username, const QString& password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE void fetchHomeFeed(int limit = 500);

signals:
    void homeFeedReady();
    void loginSucceeded();
    void loggedOut();
    void requestFailed(const QString& message);
    void sessionChanged();
    void busyChanged();
    void lastErrorChanged();

private:
    enum class ConnectionState {
        Disconnected,
        Authenticating,
        Connected,
        Syncing,
    };

    static QUrl normalizeServerUrl(const QString& value);
    static QString escapeHeaderValue(QString value);
    static QString extractArtist(const QJsonObject& item);
    static QString extractContainerLabel(const QJsonObject& item);
    static qint64 runtimeTicksToMs(qint64 runtimeTicks);
    static qint64 msToTicks(qint64 durationMs);
    static QString secretAccountKey(const QUrl& baseUrl, const QString& userId);

    QString buildAuthorizationHeader() const;
    QNetworkRequest buildRequest(const QString& path, bool includeToken, const QUrlQuery& query = {}) const;
    QJsonObject buildPlaybackPayload(const QString& itemId,
                                     const QString& playSessionId,
                                     qint64 positionMs,
                                     bool paused,
                                     int volumeLevel) const;
    QVector<Domain::Track> parseTracks(const QJsonArray& items) const;
    QString responseErrorMessage(QNetworkReply* reply, const QByteArray& body, const QString& fallback) const;
    void postJsonSilently(const QString& path, const QJsonObject& payload, const QString& operationName);
    void postWithoutBodySilently(const QString& path, const QString& operationName);
    void restorePersistedSession();
    void setState(ConnectionState state);
    void setLastError(const QString& error);
    void beginRequest();
    void endRequest();
    void updateServerProfileLabel();
    QUrl buildServerRelativeUrl(const QString& path, const QUrlQuery& query = {}) const;

    Settings::SettingsService& m_settings;
    Infrastructure::Storage::LibraryCache& m_libraryCache;
    Linux::SecretStore& m_secretStore;
    QNetworkAccessManager m_network;
    Domain::ServerProfile m_serverProfile;
    QVector<Domain::Track> m_homeTracks;
    QString m_accessToken;
    ConnectionState m_state = ConnectionState::Disconnected;
    int m_activeRequests = 0;
    QString m_lastError;
};

}  // namespace MyFin::Infrastructure::Jellyfin
