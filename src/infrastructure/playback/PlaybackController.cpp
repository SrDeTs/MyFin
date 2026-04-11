#include "infrastructure/playback/PlaybackController.h"

#include "infrastructure/jellyfin/JellyfinApiClient.h"
#include "infrastructure/logging/Logging.h"
#include "infrastructure/playback/GStreamerPlaybackBackend.h"
#include "infrastructure/settings/SettingsService.h"

#include <QTimer>
#include <QUuid>

namespace MyFin::Infrastructure::Playback {

namespace {

constexpr auto kPlaybackProgressIntervalMs = 10000;

}

PlaybackController::PlaybackController(Infrastructure::Jellyfin::JellyfinApiClient& jellyfin,
                                       Infrastructure::Settings::SettingsService& settings,
                                       QObject* parent)
    : QObject(parent)
    , m_jellyfin(jellyfin)
    , m_settings(settings)
    , m_backend(new GStreamerPlaybackBackend(this))
    , m_progressTimer(new QTimer(this))
{
    const QString preferredDeviceId = settings.preferredAudioDeviceId();
    if (!preferredDeviceId.isEmpty()) {
        m_backend->setOutputDeviceById(preferredDeviceId);
    }

    const QString currentDeviceId = m_backend->currentOutputDeviceId();
    const float initialVolume = settings.outputVolumeForDevice(currentDeviceId, settings.outputVolume());
    m_backend->setVolume(initialVolume);
    m_progressTimer->setInterval(kPlaybackProgressIntervalMs);

    connect(m_progressTimer, &QTimer::timeout, this, [this] {
        flushPlaybackProgress(false);
    });

    connect(m_backend, &GStreamerPlaybackBackend::playingChanged, this, [this] {
        m_state.playing = m_backend->isPlaying();
        emit stateChanged();
    });

    connect(m_backend, &GStreamerPlaybackBackend::positionChanged, this, [this](qint64 positionMs) {
        m_state.positionMs = positionMs;
        emit stateChanged();
    });

    connect(m_backend, &GStreamerPlaybackBackend::durationChanged, this, [this](qint64 durationMs) {
        if (durationMs > 0) {
            m_state.durationMs = durationMs;
            emit stateChanged();
        }
    });

    connect(m_backend, &GStreamerPlaybackBackend::mediaFinished, this, [this] {
        const bool markPlayed = completionReached();
        stopCurrentTrack(markPlayed);

        if (m_currentIndex + 1 < m_queue.size()) {
            ++m_currentIndex;
            playCurrent();
            return;
        }

        m_progressTimer->stop();
        m_state.playing = false;
        m_state.positionMs = m_state.durationMs;
        m_state.queueLength = qMax(0, m_queue.size() - m_currentIndex - 1);
        emit stateChanged();
    });

    connect(m_backend, &GStreamerPlaybackBackend::errorOccurred, this, [this](const QString& message) {
        m_progressTimer->stop();
        stopCurrentTrack(false);
        m_state.errorText = message;
        m_state.playing = false;
        emit stateChanged();
    });

    connect(m_backend, &GStreamerPlaybackBackend::outputDeviceChanged, this, [this] {
        m_settings.setPreferredAudioDeviceId(m_backend->currentOutputDeviceId());
        const float deviceVolume = m_settings.outputVolumeForDevice(m_backend->currentOutputDeviceId(), m_settings.outputVolume());
        m_backend->setVolume(deviceVolume);
        emit stateChanged();
        emit audioDevicesChanged();
    });

    connect(m_backend, &GStreamerPlaybackBackend::outputDevicesChanged, this, [this] {
        const QString preferredDeviceId = m_settings.preferredAudioDeviceId();
        if (!preferredDeviceId.isEmpty()) {
            m_backend->setOutputDeviceById(preferredDeviceId);
        }
        emit stateChanged();
        emit audioDevicesChanged();
    });
}

const PlaybackController::State& PlaybackController::state() const
{
    return m_state;
}

const QVector<Domain::Track>& PlaybackController::queue() const
{
    return m_queue;
}

void PlaybackController::playNow(const Domain::Track& track)
{
    playQueue(QVector<Domain::Track>{track}, 0);
}

void PlaybackController::playQueue(const QVector<Domain::Track>& tracks, int startIndex)
{
    if (tracks.isEmpty() || startIndex < 0 || startIndex >= tracks.size()) {
        return;
    }

    if (m_currentIndex >= 0 && m_currentIndex < m_queue.size()) {
        stopCurrentTrack(false);
    }

    m_queue = tracks;
    m_currentIndex = startIndex;
    playCurrent();
}

bool PlaybackController::hasPrevious() const
{
    return m_currentIndex > 0 && m_currentIndex < m_queue.size();
}

bool PlaybackController::hasNext() const
{
    return m_currentIndex >= 0 && m_currentIndex + 1 < m_queue.size();
}

float PlaybackController::outputVolume() const
{
    return m_backend->volume();
}

QVector<AudioOutputInfo> PlaybackController::outputDevices() const
{
    return m_backend->outputDevices();
}

QString PlaybackController::currentOutputDeviceId() const
{
    return m_backend->currentOutputDeviceId();
}

QString PlaybackController::currentOutputDeviceName() const
{
    return m_backend->currentOutputDeviceName();
}

int PlaybackController::currentOutputSampleRate() const
{
    return m_backend->currentOutputSampleRate();
}

int PlaybackController::currentOutputChannelCount() const
{
    return m_backend->currentOutputChannelCount();
}

QString PlaybackController::currentOutputSampleFormat() const
{
    return m_backend->currentOutputSampleFormat();
}

QString PlaybackController::backendName() const
{
    return m_backend->backendName();
}

void PlaybackController::togglePlaying()
{
    if (m_currentIndex < 0 || m_currentIndex >= m_queue.size()) {
        return;
    }

    if (m_backend->isPlaying()) {
        m_backend->pause();
        m_progressTimer->stop();
        flushPlaybackProgress(true);
    } else {
        if (m_playSessionId.isEmpty()) {
            m_playSessionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
            m_jellyfin.reportPlaybackStart(m_queue.at(m_currentIndex).id, m_playSessionId, m_state.positionMs);
        }

        m_backend->play();
        m_progressTimer->start();
        flushPlaybackProgress(false);
    }

    m_state.playing = m_backend->isPlaying();
    emit stateChanged();
}

void PlaybackController::previous()
{
    if (!hasPrevious()) {
        return;
    }

    stopCurrentTrack(false);
    --m_currentIndex;
    playCurrent();
}

void PlaybackController::next()
{
    if (!hasNext()) {
        return;
    }

    stopCurrentTrack(completionReached());
    ++m_currentIndex;
    playCurrent();
}

void PlaybackController::seek(qint64 positionMs)
{
    if (m_currentIndex < 0 || m_currentIndex >= m_queue.size()) {
        return;
    }

    const qint64 clampedPosition = qBound<qint64>(0, positionMs, m_state.durationMs > 0 ? m_state.durationMs : positionMs);
    m_backend->setPosition(clampedPosition);
    m_state.positionMs = clampedPosition;
    emit stateChanged();
    flushPlaybackProgress(!m_backend->isPlaying());
}

void PlaybackController::setOutputVolume(float value)
{
    const float clampedVolume = qBound(0.0F, value, 1.0F);
    m_settings.setOutputVolume(clampedVolume);
    m_settings.setOutputVolumeForDevice(m_backend->currentOutputDeviceId(), clampedVolume);
    m_backend->setVolume(clampedVolume);
    emit stateChanged();
}

void PlaybackController::setOutputDevice(const QString& deviceId)
{
    const QString previousDeviceId = m_backend->currentOutputDeviceId();
    if (!previousDeviceId.isEmpty()) {
        m_settings.setOutputVolumeForDevice(previousDeviceId, m_backend->volume());
    }

    if (!m_backend->setOutputDeviceById(deviceId)) {
        return;
    }

    const QString currentDeviceId = m_backend->currentOutputDeviceId();
    m_settings.setPreferredAudioDeviceId(currentDeviceId);
    const float deviceVolume = m_settings.outputVolumeForDevice(currentDeviceId, m_settings.outputVolume());
    m_backend->setVolume(deviceVolume);
    emit stateChanged();
    emit audioDevicesChanged();
}

void PlaybackController::playCurrent()
{
    if (m_currentIndex < 0 || m_currentIndex >= m_queue.size()) {
        return;
    }

    const Domain::Track& track = m_queue.at(m_currentIndex);
    const QUrl streamUrl = m_jellyfin.buildPlaybackUrl(track.id);
    if (!streamUrl.isValid()) {
        m_state.errorText = QStringLiteral("No playback URL available for this track.");
        m_state.playing = false;
        emit stateChanged();
        return;
    }

    m_playSessionId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_backend->setSource(streamUrl);
    m_backend->play();
    m_progressTimer->start();

    m_state.errorText.clear();
    m_state.playing = true;
    m_state.positionMs = 0;
    m_state.durationMs = track.durationMs;
    syncStateFromCurrent();

    m_jellyfin.reportPlaybackStart(track.id, m_playSessionId, 0);
    qInfo(lcPlayback) << "Playback started for track" << track.id;
    emit stateChanged();
}

void PlaybackController::syncStateFromCurrent()
{
    if (m_currentIndex < 0 || m_currentIndex >= m_queue.size()) {
        m_state = {};
        m_state.queueLength = m_queue.size();
        return;
    }

    const Domain::Track& track = m_queue.at(m_currentIndex);
    const bool playing = m_state.playing;
    const qint64 positionMs = m_state.positionMs;
    const qint64 durationMs = m_state.durationMs > 0 ? m_state.durationMs : track.durationMs;
    const QString errorText = m_state.errorText;

    m_state.trackId = track.id;
    m_state.title = track.title;
    m_state.artist = track.artist;
    m_state.album = track.album;
    m_state.coverSource = m_jellyfin.coverSourceForArtworkKey(track.artworkKey);
    m_state.signalPath = track.streamProfile;
    m_state.queueLength = qMax(0, m_queue.size() - m_currentIndex - 1);
    m_state.positionMs = positionMs;
    m_state.durationMs = durationMs;
    m_state.playing = playing;
    m_state.errorText = errorText;
}

void PlaybackController::flushPlaybackProgress(bool paused)
{
    if (m_currentIndex < 0 || m_currentIndex >= m_queue.size() || m_playSessionId.isEmpty()) {
        return;
    }

    m_state.positionMs = m_backend->position();
    m_jellyfin.reportPlaybackProgress(m_queue.at(m_currentIndex).id,
                                      m_playSessionId,
                                      m_state.positionMs,
                                      paused,
                                      static_cast<int>(m_settings.outputVolume() * 100.0F));
}

void PlaybackController::stopCurrentTrack(bool markPlayed)
{
    if (m_currentIndex < 0 || m_currentIndex >= m_queue.size() || m_playSessionId.isEmpty()) {
        return;
    }

    m_progressTimer->stop();
    flushPlaybackProgress(!m_backend->isPlaying());

    const Domain::Track& track = m_queue.at(m_currentIndex);
    m_state.positionMs = m_backend->position();
    m_jellyfin.reportPlaybackStopped(track.id, m_playSessionId, m_state.positionMs);
    if (markPlayed || completionReached()) {
        m_jellyfin.markTrackPlayed(track.id);
    }

    m_playSessionId.clear();
}

bool PlaybackController::completionReached() const
{
    if (m_state.durationMs <= 0) {
        return false;
    }

    return m_state.positionMs >= m_state.durationMs - 1500;
}

}  // namespace MyFin::Infrastructure::Playback
