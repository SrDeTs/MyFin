#include "presentation/viewmodels/AudioSettingsViewModel.h"

#include "infrastructure/playback/PlaybackController.h"
#include "infrastructure/settings/AppPaths.h"
#include "infrastructure/settings/SettingsService.h"
#include "presentation/viewmodels/LibraryViewModel.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

namespace MyFin::Presentation {

namespace {

QString sampleRateLabel(int sampleRate)
{
    return sampleRate > 0 ? QStringLiteral("%1 kHz").arg(sampleRate / 1000.0, 0, 'f', sampleRate % 1000 == 0 ? 0 : 1)
                          : QStringLiteral("Aguardando stream");
}

QString channelLabel(int channels)
{
    return channels > 0 ? QStringLiteral("%1 ch").arg(channels) : QStringLiteral("?");
}

}  // namespace

AudioSettingsViewModel::AudioSettingsViewModel(Infrastructure::Playback::PlaybackController& playback,
                                               Infrastructure::Settings::SettingsService& settings,
                                               Infrastructure::Settings::AppPaths& paths,
                                               LibraryViewModel& library,
                                               QObject* parent)
    : QObject(parent)
    , m_playback(playback)
    , m_settings(settings)
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

QString AudioSettingsViewModel::outputFormatSummary() const
{
    return QStringLiteral("%1 • %2 • %3")
        .arg(sampleRateLabel(m_playback.currentOutputSampleRate()),
             m_playback.currentOutputSampleFormat(),
             channelLabel(m_playback.currentOutputChannelCount()));
}

QString AudioSettingsViewModel::signalConfidence() const
{
    if (!m_playback.state().trackId.isEmpty() && m_playback.currentOutputSampleRate() > 0) {
        return QStringLiteral("Formato negociado em runtime via %1").arg(backendName());
    }

    return QStringLiteral("Device dedicado via %1").arg(backendName());
}

QString AudioSettingsViewModel::signalPath() const
{
    return m_playback.state().signalPath.isEmpty()
               ? QStringLiteral("Sem reproducao ativa")
               : m_playback.state().signalPath;
}

QString AudioSettingsViewModel::streamOrigin() const
{
    return m_playback.state().trackId.isEmpty()
               ? QStringLiteral("Sem stream ativo")
               : QStringLiteral("Jellyfin");
}

QStringList AudioSettingsViewModel::qualityProfileOptions() const
{
    return kQualityProfiles();
}

int AudioSettingsViewModel::qualityProfileIndex() const
{
    const QString current = qualityProfileLabel(m_settings.audioQualityProfile());
    const QStringList options = kQualityProfiles();
    return options.indexOf(current);
}

bool AudioSettingsViewModel::advancedMode() const
{
    return m_settings.audioAdvancedMode();
}

QStringList AudioSettingsViewModel::replayGainOptions() const
{
    return kReplayGainModes();
}

int AudioSettingsViewModel::replayGainIndex() const
{
    const QString current = replayGainLabel(m_settings.replayGainMode());
    const QStringList options = kReplayGainModes();
    return options.indexOf(current);
}

bool AudioSettingsViewModel::gaplessEnabled() const
{
    return m_settings.gaplessEnabled();
}

int AudioSettingsViewModel::crossfadeSeconds() const
{
    return m_settings.crossfadeSeconds();
}

bool AudioSettingsViewModel::preloadNextTrack() const
{
    return m_settings.preloadNextTrack();
}

int AudioSettingsViewModel::streamCacheLimitMb() const
{
    return m_settings.streamCacheLimitMb();
}

QString AudioSettingsViewModel::cacheLocation() const
{
    return m_paths.cacheDir();
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

void AudioSettingsViewModel::setQualityProfileIndex(int index)
{
    const QString key = qualityProfileKeyForIndex(index);
    if (key.isEmpty() || m_settings.audioQualityProfile() == key) {
        return;
    }

    m_settings.setAudioQualityProfile(key);

    if (key == QStringLiteral("fidelity")) {
        m_settings.setGaplessEnabled(true);
        m_settings.setCrossfadeSeconds(0);
        m_settings.setPreloadNextTrack(true);
        m_settings.setReplayGainMode(QStringLiteral("album"));
        m_settings.setStreamCacheLimitMb(512);
    } else if (key == QStringLiteral("balanced")) {
        m_settings.setGaplessEnabled(true);
        m_settings.setCrossfadeSeconds(2);
        m_settings.setPreloadNextTrack(true);
        m_settings.setReplayGainMode(QStringLiteral("track"));
        m_settings.setStreamCacheLimitMb(256);
    } else if (key == QStringLiteral("cpu")) {
        m_settings.setGaplessEnabled(false);
        m_settings.setCrossfadeSeconds(0);
        m_settings.setPreloadNextTrack(false);
        m_settings.setReplayGainMode(QStringLiteral("off"));
        m_settings.setStreamCacheLimitMb(128);
    } else if (key == QStringLiteral("network")) {
        m_settings.setGaplessEnabled(true);
        m_settings.setCrossfadeSeconds(0);
        m_settings.setPreloadNextTrack(true);
        m_settings.setReplayGainMode(QStringLiteral("track"));
        m_settings.setStreamCacheLimitMb(768);
    }

    emit stateChanged();
}

void AudioSettingsViewModel::setAdvancedMode(bool value)
{
    if (m_settings.audioAdvancedMode() == value) {
        return;
    }
    m_settings.setAudioAdvancedMode(value);
    emit stateChanged();
}

void AudioSettingsViewModel::setReplayGainIndex(int index)
{
    const QString key = replayGainKeyForIndex(index);
    if (key.isEmpty() || m_settings.replayGainMode() == key) {
        return;
    }
    m_settings.setReplayGainMode(key);
    emit stateChanged();
}

void AudioSettingsViewModel::setGaplessEnabled(bool value)
{
    if (m_settings.gaplessEnabled() == value) {
        return;
    }
    m_settings.setGaplessEnabled(value);
    emit stateChanged();
}

void AudioSettingsViewModel::setCrossfadeSeconds(int value)
{
    const int clamped = qBound(0, value, 12);
    if (m_settings.crossfadeSeconds() == clamped) {
        return;
    }
    m_settings.setCrossfadeSeconds(clamped);
    emit stateChanged();
}

void AudioSettingsViewModel::setPreloadNextTrack(bool value)
{
    if (m_settings.preloadNextTrack() == value) {
        return;
    }
    m_settings.setPreloadNextTrack(value);
    emit stateChanged();
}

void AudioSettingsViewModel::setStreamCacheLimitMb(int value)
{
    const int clamped = qBound(64, value, 2048);
    if (m_settings.streamCacheLimitMb() == clamped) {
        return;
    }
    m_settings.setStreamCacheLimitMb(clamped);
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

QStringList AudioSettingsViewModel::kQualityProfiles()
{
    return {
        QStringLiteral("Maxima fidelidade"),
        QStringLiteral("Equilibrado"),
        QStringLiteral("CPU minima"),
        QStringLiteral("Rede ruim"),
    };
}

QStringList AudioSettingsViewModel::kReplayGainModes()
{
    return {
        QStringLiteral("Desligado"),
        QStringLiteral("Por faixa"),
        QStringLiteral("Por album"),
    };
}

QString AudioSettingsViewModel::qualityProfileLabel(const QString& key)
{
    if (key == QStringLiteral("fidelity")) return kQualityProfiles().at(0);
    if (key == QStringLiteral("cpu")) return kQualityProfiles().at(2);
    if (key == QStringLiteral("network")) return kQualityProfiles().at(3);
    return kQualityProfiles().at(1);
}

QString AudioSettingsViewModel::replayGainLabel(const QString& key)
{
    if (key == QStringLiteral("track")) return kReplayGainModes().at(1);
    if (key == QStringLiteral("album")) return kReplayGainModes().at(2);
    return kReplayGainModes().at(0);
}

QString AudioSettingsViewModel::qualityProfileKeyForIndex(int index)
{
    switch (index) {
    case 0: return QStringLiteral("fidelity");
    case 1: return QStringLiteral("balanced");
    case 2: return QStringLiteral("cpu");
    case 3: return QStringLiteral("network");
    default: return {};
    }
}

QString AudioSettingsViewModel::replayGainKeyForIndex(int index)
{
    switch (index) {
    case 0: return QStringLiteral("off");
    case 1: return QStringLiteral("track");
    case 2: return QStringLiteral("album");
    default: return {};
    }
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
