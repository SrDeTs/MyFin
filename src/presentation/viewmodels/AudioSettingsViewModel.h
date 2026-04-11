#pragma once

#include <QObject>
#include <QVariantList>

namespace MyFin::Infrastructure::Playback {
class PlaybackController;
}

namespace MyFin::Infrastructure::Settings {
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
    Q_PROPERTY(QString streamOrigin READ streamOrigin NOTIFY stateChanged)
    Q_PROPERTY(QString cacheUsageSummary READ cacheUsageSummary NOTIFY stateChanged)
    Q_PROPERTY(int cachedTrackCount READ cachedTrackCount NOTIFY stateChanged)

public:
    AudioSettingsViewModel(Infrastructure::Playback::PlaybackController& playback,
                           Infrastructure::Settings::AppPaths& paths,
                           LibraryViewModel& library,
                           QObject* parent = nullptr);

    QVariantList outputDevices() const;
    int currentOutputDeviceIndex() const;
    QString currentOutputDeviceName() const;
    QString backendName() const;
    QString streamOrigin() const;
    QString cacheUsageSummary() const;
    int cachedTrackCount() const;

    Q_INVOKABLE void selectOutputDevice(int index);
    Q_INVOKABLE void clearCoverCache();

signals:
    void stateChanged();

private:
    static QString formatBytes(qint64 bytes);

    qint64 coverCacheBytes() const;

    Infrastructure::Playback::PlaybackController& m_playback;
    Infrastructure::Settings::AppPaths& m_paths;
    LibraryViewModel& m_library;
};

}  // namespace MyFin::Presentation
