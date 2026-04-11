#include "presentation/viewmodels/AudioSettingsViewModel.h"

#include "infrastructure/playback/PlaybackController.h"
#include "infrastructure/settings/AppPaths.h"
#include "presentation/viewmodels/LibraryViewModel.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

namespace MyFin::Presentation {

AudioSettingsViewModel::AudioSettingsViewModel(Infrastructure::Playback::PlaybackController& playback,
                                               Infrastructure::Settings::AppPaths& paths,
                                               LibraryViewModel& library,
                                               QObject* parent)
    : QObject(parent)
    , m_playback(playback)
    , m_paths(paths)
    , m_library(library)
{
    connect(&m_playback, &Infrastructure::Playback::PlaybackController::stateChanged,
            this, &AudioSettingsViewModel::stateChanged);
    connect(&m_playback, &Infrastructure::Playback::PlaybackController::audioDevicesChanged,
            this, &AudioSettingsViewModel::stateChanged);
    connect(&m_library, &LibraryViewModel::contentChanged, this, &AudioSettingsViewModel::stateChanged);
}

QVariantList AudioSettingsViewModel::outputDevices() const
{
    QVariantList devices;
    for (const auto& device : m_playback.outputDevices()) {
        QVariantMap item;
        item.insert(QStringLiteral("id"), device.id);
        item.insert(QStringLiteral("name"), device.isDefault
                                            ? QStringLiteral("%1 (Padrao)").arg(device.name)
                                            : device.name);
        item.insert(QStringLiteral("sampleRate"), device.preferredSampleRate);
        item.insert(QStringLiteral("channels"), device.preferredChannelCount);
        item.insert(QStringLiteral("format"), device.preferredSampleFormat);
        devices.push_back(item);
    }
    return devices;
}

int AudioSettingsViewModel::currentOutputDeviceIndex() const
{
    const QString currentId = m_playback.currentOutputDeviceId();
    const auto devices = m_playback.outputDevices();
    for (qsizetype index = 0; index < devices.size(); ++index) {
        if (devices.at(index).id == currentId) {
            return static_cast<int>(index);
        }
    }
    return devices.isEmpty() ? -1 : 0;
}

QString AudioSettingsViewModel::currentOutputDeviceName() const
{
    return m_playback.currentOutputDeviceName();
}

QString AudioSettingsViewModel::backendName() const
{
    return m_playback.backendName();
}

QString AudioSettingsViewModel::streamOrigin() const
{
    return m_playback.state().trackId.isEmpty()
               ? QStringLiteral("Sem stream ativo")
               : QStringLiteral("Jellyfin");
}

QString AudioSettingsViewModel::cacheUsageSummary() const
{
    return formatBytes(coverCacheBytes());
}

int AudioSettingsViewModel::cachedTrackCount() const
{
    return m_library.cachedTrackCount();
}

void AudioSettingsViewModel::selectOutputDevice(int index)
{
    const auto devices = m_playback.outputDevices();
    if (index < 0 || index >= devices.size()) {
        return;
    }

    m_playback.setOutputDevice(devices.at(index).id);
    emit stateChanged();
}

void AudioSettingsViewModel::clearCoverCache()
{
    QDir cacheDir(m_paths.coverCacheDir());
    if (cacheDir.exists()) {
        cacheDir.removeRecursively();
    }
    QDir().mkpath(m_paths.coverCacheDir());
    emit stateChanged();
}

QString AudioSettingsViewModel::formatBytes(qint64 bytes)
{
    const double kb = bytes / 1024.0;
    const double mb = kb / 1024.0;
    if (mb >= 1024.0) {
        return QStringLiteral("%1 GB").arg(mb / 1024.0, 0, 'f', 2);
    }
    if (mb >= 1.0) {
        return QStringLiteral("%1 MB").arg(mb, 0, 'f', 1);
    }
    if (kb >= 1.0) {
        return QStringLiteral("%1 KB").arg(kb, 0, 'f', 1);
    }
    return QStringLiteral("%1 B").arg(bytes);
}

qint64 AudioSettingsViewModel::coverCacheBytes() const
{
    qint64 total = 0;
    QDirIterator it(m_paths.coverCacheDir(), QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo info(it.next());
        total += info.size();
    }
    return total;
}

}  // namespace MyFin::Presentation
