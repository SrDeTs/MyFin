#pragma once

#include <QObject>
#include <QStringList>
#include <QVariantList>

namespace MyFin::Infrastructure::Playback {
class PlaybackController;
}

namespace MyFin::Infrastructure::Settings {
class SettingsService;
class AppPaths;
}

namespace MyFin::Presentation {

class LibraryViewModel;

class AudioSettingsViewModel final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList outputDevices READ outputDevices NOTIFY stateChanged)
    Q_PROPERTY(int currentOutputDeviceIndex READ currentOutputDeviceIndex NOTIFY stateChanged)
    Q_PROPERTY(QString currentOutputDeviceName READ currentOutputDeviceName NOTIFY stateChanged)
    Q_PROPERTY(QString backendName READ backendName NOTIFY stateChanged)
    Q_PROPERTY(QString outputFormatSummary READ outputFormatSummary NOTIFY stateChanged)
    Q_PROPERTY(QString signalConfidence READ signalConfidence NOTIFY stateChanged)
    Q_PROPERTY(QString signalPath READ signalPath NOTIFY stateChanged)
    Q_PROPERTY(QString streamOrigin READ streamOrigin NOTIFY stateChanged)
    Q_PROPERTY(QStringList qualityProfileOptions READ qualityProfileOptions CONSTANT)
    Q_PROPERTY(int qualityProfileIndex READ qualityProfileIndex WRITE setQualityProfileIndex NOTIFY stateChanged)
    Q_PROPERTY(bool advancedMode READ advancedMode WRITE setAdvancedMode NOTIFY stateChanged)
    Q_PROPERTY(QStringList replayGainOptions READ replayGainOptions CONSTANT)
    Q_PROPERTY(int replayGainIndex READ replayGainIndex WRITE setReplayGainIndex NOTIFY stateChanged)
    Q_PROPERTY(bool gaplessEnabled READ gaplessEnabled WRITE setGaplessEnabled NOTIFY stateChanged)
    Q_PROPERTY(int crossfadeSeconds READ crossfadeSeconds WRITE setCrossfadeSeconds NOTIFY stateChanged)
    Q_PROPERTY(bool preloadNextTrack READ preloadNextTrack WRITE setPreloadNextTrack NOTIFY stateChanged)
    Q_PROPERTY(int streamCacheLimitMb READ streamCacheLimitMb WRITE setStreamCacheLimitMb NOTIFY stateChanged)
    Q_PROPERTY(QString cacheLocation READ cacheLocation CONSTANT)
    Q_PROPERTY(QString cacheUsageSummary READ cacheUsageSummary NOTIFY stateChanged)
    Q_PROPERTY(int cachedTrackCount READ cachedTrackCount NOTIFY stateChanged)

public:
    AudioSettingsViewModel(Infrastructure::Playback::PlaybackController& playback,
                           Infrastructure::Settings::SettingsService& settings,
                           Infrastructure::Settings::AppPaths& paths,
                           LibraryViewModel& library,
                           QObject* parent = nullptr);

    QVariantList outputDevices() const;
    int currentOutputDeviceIndex() const;
    QString currentOutputDeviceName() const;
    QString backendName() const;
    QString outputFormatSummary() const;
    QString signalConfidence() const;
    QString signalPath() const;
    QString streamOrigin() const;
    QStringList qualityProfileOptions() const;
    int qualityProfileIndex() const;
    bool advancedMode() const;
    QStringList replayGainOptions() const;
    int replayGainIndex() const;
    bool gaplessEnabled() const;
    int crossfadeSeconds() const;
    bool preloadNextTrack() const;
    int streamCacheLimitMb() const;
    QString cacheLocation() const;
    QString cacheUsageSummary() const;
    int cachedTrackCount() const;

    Q_INVOKABLE void selectOutputDevice(int index);
    Q_INVOKABLE void setQualityProfileIndex(int index);
    Q_INVOKABLE void setAdvancedMode(bool value);
    Q_INVOKABLE void setReplayGainIndex(int index);
    Q_INVOKABLE void setGaplessEnabled(bool value);
    Q_INVOKABLE void setCrossfadeSeconds(int value);
    Q_INVOKABLE void setPreloadNextTrack(bool value);
    Q_INVOKABLE void setStreamCacheLimitMb(int value);
    Q_INVOKABLE void clearCoverCache();

signals:
    void stateChanged();

private:
    static QStringList kQualityProfiles();
    static QStringList kReplayGainModes();
    static QString qualityProfileLabel(const QString& key);
    static QString replayGainLabel(const QString& key);
    static QString qualityProfileKeyForIndex(int index);
    static QString replayGainKeyForIndex(int index);
    static QString formatBytes(qint64 bytes);

    qint64 coverCacheBytes() const;

    Infrastructure::Playback::PlaybackController& m_playback;
    Infrastructure::Settings::SettingsService& m_settings;
    Infrastructure::Settings::AppPaths& m_paths;
    LibraryViewModel& m_library;
};

}  // namespace MyFin::Presentation
