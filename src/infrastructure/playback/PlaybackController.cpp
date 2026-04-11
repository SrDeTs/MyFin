#include "infrastructure/playback/PlaybackController.h"

#include "infrastructure/jellyfin/JellyfinApiClient.h"
#include "infrastructure/logging/Logging.h"
#include "infrastructure/playback/QtMediaPlaybackBackend.h"
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
    , m_backend(new QtMediaPlaybackBackend(this))
    , m_progressTimer(new QTimer(this))
{
    m_backend->setVolume(settings.outputVolume());
    m_progressTimer->setInterval(kPlaybackProgressIntervalMs);

    connect(m_progressTimer, &QTimer::timeout, this, [this] {
        flushPlaybackProgress(false);
    });

    connect(m_backend, &QtMediaPlaybackBackend::playingChanged, this, [this] {
        m_state.playing = m_backend->isPlaying();
        emit stateChanged();
    });

    connect(m_backend, &QtMediaPlaybackBackend::positionChanged, this, [this](qint64 positionMs) {
        m_state.positionMs = positionMs;
    });

    connect(m_backend, &QtMediaPlaybackBackend::durationChanged, this, [this](qint64 durationMs) {
        if (durationMs > 0) {
            m_state.durationMs = durationMs;
        }
    });

    connect(m_backend, &QtMediaPlaybackBackend::mediaFinished, this, [this] {
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
        emit stateChanged();
    });

    connect(m_backend, &QtMediaPlaybackBackend::errorOccurred, this, [this](const QString& message) {
        m_progressTimer->stop();
        stopCurrentTrack(false);
        m_state.errorText = message;
        m_state.playing = false;
        emit stateChanged();
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
    if (m_currentIndex >= 0 && m_currentIndex < m_queue.size()) {
        stopCurrentTrack(false);
    }

    m_queue.append(track);
    m_currentIndex = m_queue.size() - 1;
    playCurrent();
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

void PlaybackController::next()
{
    if (m_currentIndex < 0 || m_currentIndex + 1 >= m_queue.size()) {
        return;
    }

    stopCurrentTrack(completionReached());
    ++m_currentIndex;
    playCurrent();
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
    m_state.queueLength = m_queue.size();
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
    const qint64 durationMs = m_state.durationMs > 0 ? m_state.durationMs : m_backend->duration();
    const qint64 positionMs = m_state.positionMs > 0 ? m_state.positionMs : m_backend->position();
    if (durationMs <= 0) {
        return false;
    }

    return positionMs >= durationMs - 2000 || positionMs >= (durationMs * 9) / 10;
}

}  // namespace MyFin::Infrastructure::Playback
