#include "presentation/viewmodels/SessionViewModel.h"

#include "infrastructure/jellyfin/JellyfinApiClient.h"
#include "infrastructure/settings/SettingsService.h"

namespace MyFin::Presentation {

SessionViewModel::SessionViewModel(Infrastructure::Jellyfin::JellyfinApiClient& jellyfin,
                                   Infrastructure::Settings::SettingsService& settings,
                                   QObject* parent)
    : QObject(parent)
    , m_jellyfin(jellyfin)
    , m_settings(settings)
    , m_serverUrl(m_settings.serverUrl().toString())
    , m_username(m_settings.username())
{
    connect(&m_jellyfin, &Infrastructure::Jellyfin::JellyfinApiClient::sessionChanged,
            this, &SessionViewModel::stateChanged);
    connect(&m_jellyfin, &Infrastructure::Jellyfin::JellyfinApiClient::busyChanged,
            this, &SessionViewModel::stateChanged);
    connect(&m_jellyfin, &Infrastructure::Jellyfin::JellyfinApiClient::lastErrorChanged,
            this, &SessionViewModel::stateChanged);
    connect(&m_jellyfin, &Infrastructure::Jellyfin::JellyfinApiClient::loginSucceeded, this, [this] {
        m_password.clear();
        emit formChanged();
        emit stateChanged();
    });
    connect(&m_jellyfin, &Infrastructure::Jellyfin::JellyfinApiClient::loggedOut,
            this, &SessionViewModel::stateChanged);
}

QString SessionViewModel::serverUrl() const
{
    return m_serverUrl;
}

void SessionViewModel::setServerUrl(const QString& value)
{
    if (m_serverUrl == value) {
        return;
    }

    m_serverUrl = value;
    emit formChanged();
}

QString SessionViewModel::username() const
{
    return m_username;
}

void SessionViewModel::setUsername(const QString& value)
{
    if (m_username == value) {
        return;
    }

    m_username = value;
    emit formChanged();
}

QString SessionViewModel::password() const
{
    return m_password;
}

void SessionViewModel::setPassword(const QString& value)
{
    if (m_password == value) {
        return;
    }

    m_password = value;
    emit formChanged();
}

bool SessionViewModel::authenticated() const
{
    return m_jellyfin.authenticated();
}

bool SessionViewModel::busy() const
{
    return m_jellyfin.busy();
}

QString SessionViewModel::stateLabel() const
{
    return m_jellyfin.connectionStateLabel();
}

QString SessionViewModel::summary() const
{
    if (!authenticated()) {
        return QStringLiteral("No active Jellyfin session");
    }

    return QStringLiteral("%1 on %2")
        .arg(m_jellyfin.userLabel(), m_jellyfin.serverLabel());
}

QString SessionViewModel::errorText() const
{
    return m_jellyfin.lastError();
}

void SessionViewModel::login()
{
    m_jellyfin.login(m_serverUrl, m_username, m_password);
}

void SessionViewModel::logout()
{
    m_jellyfin.logout();
}

}  // namespace MyFin::Presentation
