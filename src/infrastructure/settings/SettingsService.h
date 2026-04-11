#pragma once

#include <QString>
#include <QSize>
#include <QUrl>

class QSettings;

namespace MyFin::Infrastructure::Settings {

class AppPaths;

class SettingsService final {
public:
    explicit SettingsService(const AppPaths& paths);
    ~SettingsService();

    QString serverName() const;
    void setServerName(const QString& value);

    QUrl serverUrl() const;
    void setServerUrl(const QUrl& value);

    QString username() const;
    void setUsername(const QString& value);

    QString userId() const;
    void setUserId(const QString& value);

    QString sessionToken() const;
    void setSessionToken(const QString& value);

    QString deviceId() const;
    void setDeviceId(const QString& value);

    QString preferredAudioDeviceId() const;
    void setPreferredAudioDeviceId(const QString& value);

    float outputVolume() const;
    void setOutputVolume(float value);
    float outputVolumeForDevice(const QString& deviceId, float fallbackValue) const;
    void setOutputVolumeForDevice(const QString& deviceId, float value);

    QString audioQualityProfile() const;
    void setAudioQualityProfile(const QString& value);

    QString replayGainMode() const;
    void setReplayGainMode(const QString& value);

    bool gaplessEnabled() const;
    void setGaplessEnabled(bool value);

    int crossfadeSeconds() const;
    void setCrossfadeSeconds(int value);

    bool preloadNextTrack() const;
    void setPreloadNextTrack(bool value);

    int streamCacheLimitMb() const;
    void setStreamCacheLimitMb(int value);

    bool audioAdvancedMode() const;
    void setAudioAdvancedMode(bool value);

    QSize windowSize() const;
    void setWindowSize(const QSize& value);

private:
    QSettings& settings();
    const QSettings& settings() const;

    QString key(const QString& suffix) const;

    QSettings* m_settings = nullptr;
};

}  // namespace MyFin::Infrastructure::Settings
