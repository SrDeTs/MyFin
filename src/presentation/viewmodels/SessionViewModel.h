#pragma once

#include <QObject>

namespace MyFin::Infrastructure::Jellyfin {
class JellyfinApiClient;
}

namespace MyFin::Infrastructure::Settings {
class SettingsService;
}

namespace MyFin::Presentation {

class SessionViewModel final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY formChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY formChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY formChanged)
    Q_PROPERTY(bool authenticated READ authenticated NOTIFY stateChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY stateChanged)
    Q_PROPERTY(QString stateLabel READ stateLabel NOTIFY stateChanged)
    Q_PROPERTY(QString summary READ summary NOTIFY stateChanged)
    Q_PROPERTY(QString errorText READ errorText NOTIFY stateChanged)

public:
    SessionViewModel(Infrastructure::Jellyfin::JellyfinApiClient& jellyfin,
                     Infrastructure::Settings::SettingsService& settings,
                     QObject* parent = nullptr);

    QString serverUrl() const;
    void setServerUrl(const QString& value);

    QString username() const;
    void setUsername(const QString& value);

    QString password() const;
    void setPassword(const QString& value);

    bool authenticated() const;
    bool busy() const;
    QString stateLabel() const;
    QString summary() const;
    QString errorText() const;

    Q_INVOKABLE void login();
    Q_INVOKABLE void logout();

signals:
    void formChanged();
    void stateChanged();

private:
    Infrastructure::Jellyfin::JellyfinApiClient& m_jellyfin;
    Infrastructure::Settings::SettingsService& m_settings;
    QString m_serverUrl;
    QString m_username;
    QString m_password;
};

}  // namespace MyFin::Presentation
