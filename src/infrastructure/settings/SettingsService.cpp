#include "infrastructure/settings/SettingsService.h"

#include "infrastructure/settings/AppPaths.h"

#include <QSettings>
#include <QUuid>

namespace MyFin::Infrastructure::Settings {

SettingsService::SettingsService(const AppPaths& paths)
    : m_settings(new QSettings(paths.settingsFilePath(), QSettings::IniFormat))
{
    if (!m_settings->contains(key(QStringLiteral("audio/volume")))) {
        setOutputVolume(0.88F);
    }
    if (!m_settings->contains(key(QStringLiteral("window/size")))) {
        setWindowSize(QSize(1480, 920));
    }
    if (!m_settings->contains(key(QStringLiteral("session/deviceId")))) {
        setDeviceId(QUuid::createUuid().toString(QUuid::WithoutBraces));
    }
}

SettingsService::~SettingsService()
{
    delete m_settings;
}

QString SettingsService::serverName() const
{
    return settings().value(key(QStringLiteral("server/name"))).toString();
}

void SettingsService::setServerName(const QString& value)
{
    settings().setValue(key(QStringLiteral("server/name")), value);
}

QUrl SettingsService::serverUrl() const
{
    return settings().value(key(QStringLiteral("server/url"))).toUrl();
}

void SettingsService::setServerUrl(const QUrl& value)
{
    settings().setValue(key(QStringLiteral("server/url")), value);
}

QString SettingsService::username() const
{
    return settings().value(key(QStringLiteral("server/username"))).toString();
}

void SettingsService::setUsername(const QString& value)
{
    settings().setValue(key(QStringLiteral("server/username")), value);
}

QString SettingsService::userId() const
{
    return settings().value(key(QStringLiteral("server/userId"))).toString();
}

void SettingsService::setUserId(const QString& value)
{
    settings().setValue(key(QStringLiteral("server/userId")), value);
}

QString SettingsService::sessionToken() const
{
    return settings().value(key(QStringLiteral("session/accessToken"))).toString();
}

void SettingsService::setSessionToken(const QString& value)
{
    settings().setValue(key(QStringLiteral("session/accessToken")), value);
    settings().sync();
}

QString SettingsService::deviceId() const
{
    return settings().value(key(QStringLiteral("session/deviceId"))).toString();
}

void SettingsService::setDeviceId(const QString& value)
{
    settings().setValue(key(QStringLiteral("session/deviceId")), value);
}

float SettingsService::outputVolume() const
{
    return settings().value(key(QStringLiteral("audio/volume")), 0.88).toFloat();
}

void SettingsService::setOutputVolume(float value)
{
    settings().setValue(key(QStringLiteral("audio/volume")), value);
}

QSize SettingsService::windowSize() const
{
    return settings().value(key(QStringLiteral("window/size")), QSize(1480, 920)).toSize();
}

void SettingsService::setWindowSize(const QSize& value)
{
    settings().setValue(key(QStringLiteral("window/size")), value);
}

QSettings& SettingsService::settings()
{
    return *m_settings;
}

const QSettings& SettingsService::settings() const
{
    return *m_settings;
}

QString SettingsService::key(const QString& suffix) const
{
    return QStringLiteral("myfin/%1").arg(suffix);
}

}  // namespace MyFin::Infrastructure::Settings
