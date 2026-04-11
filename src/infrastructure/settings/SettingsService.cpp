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
    if (!m_settings->contains(key(QStringLiteral("audio/qualityProfile")))) {
        setAudioQualityProfile(QStringLiteral("balanced"));
    }
    if (!m_settings->contains(key(QStringLiteral("audio/replayGainMode")))) {
        setReplayGainMode(QStringLiteral("track"));
    }
    if (!m_settings->contains(key(QStringLiteral("audio/gapless")))) {
        setGaplessEnabled(true);
    }
    if (!m_settings->contains(key(QStringLiteral("audio/crossfadeSeconds")))) {
        setCrossfadeSeconds(0);
    }
    if (!m_settings->contains(key(QStringLiteral("audio/preloadNextTrack")))) {
        setPreloadNextTrack(true);
    }
    if (!m_settings->contains(key(QStringLiteral("audio/streamCacheLimitMb")))) {
        setStreamCacheLimitMb(256);
    }
    if (!m_settings->contains(key(QStringLiteral("ui/audioAdvancedMode")))) {
        setAudioAdvancedMode(false);
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

QString SettingsService::preferredAudioDeviceId() const
{
    return settings().value(key(QStringLiteral("audio/outputDeviceId"))).toString();
}

void SettingsService::setPreferredAudioDeviceId(const QString& value)
{
    settings().setValue(key(QStringLiteral("audio/outputDeviceId")), value);
}

float SettingsService::outputVolume() const
{
    return settings().value(key(QStringLiteral("audio/volume")), 0.88).toFloat();
}

void SettingsService::setOutputVolume(float value)
{
    settings().setValue(key(QStringLiteral("audio/volume")), value);
}

float SettingsService::outputVolumeForDevice(const QString& deviceId, float fallbackValue) const
{
    if (deviceId.isEmpty()) {
        return fallbackValue;
    }

    return settings().value(key(QStringLiteral("audio/deviceVolume/%1").arg(deviceId)), fallbackValue).toFloat();
}

void SettingsService::setOutputVolumeForDevice(const QString& deviceId, float value)
{
    if (deviceId.isEmpty()) {
        return;
    }

    settings().setValue(key(QStringLiteral("audio/deviceVolume/%1").arg(deviceId)), value);
}

QString SettingsService::audioQualityProfile() const
{
    return settings().value(key(QStringLiteral("audio/qualityProfile")), QStringLiteral("balanced")).toString();
}

void SettingsService::setAudioQualityProfile(const QString& value)
{
    settings().setValue(key(QStringLiteral("audio/qualityProfile")), value);
}

QString SettingsService::replayGainMode() const
{
    return settings().value(key(QStringLiteral("audio/replayGainMode")), QStringLiteral("track")).toString();
}

void SettingsService::setReplayGainMode(const QString& value)
{
    settings().setValue(key(QStringLiteral("audio/replayGainMode")), value);
}

bool SettingsService::gaplessEnabled() const
{
    return settings().value(key(QStringLiteral("audio/gapless")), true).toBool();
}

void SettingsService::setGaplessEnabled(bool value)
{
    settings().setValue(key(QStringLiteral("audio/gapless")), value);
}

int SettingsService::crossfadeSeconds() const
{
    return settings().value(key(QStringLiteral("audio/crossfadeSeconds")), 0).toInt();
}

void SettingsService::setCrossfadeSeconds(int value)
{
    settings().setValue(key(QStringLiteral("audio/crossfadeSeconds")), value);
}

bool SettingsService::preloadNextTrack() const
{
    return settings().value(key(QStringLiteral("audio/preloadNextTrack")), true).toBool();
}

void SettingsService::setPreloadNextTrack(bool value)
{
    settings().setValue(key(QStringLiteral("audio/preloadNextTrack")), value);
}

int SettingsService::streamCacheLimitMb() const
{
    return settings().value(key(QStringLiteral("audio/streamCacheLimitMb")), 256).toInt();
}

void SettingsService::setStreamCacheLimitMb(int value)
{
    settings().setValue(key(QStringLiteral("audio/streamCacheLimitMb")), value);
}

bool SettingsService::audioAdvancedMode() const
{
    return settings().value(key(QStringLiteral("ui/audioAdvancedMode")), false).toBool();
}

void SettingsService::setAudioAdvancedMode(bool value)
{
    settings().setValue(key(QStringLiteral("ui/audioAdvancedMode")), value);
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
