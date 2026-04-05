#include "playbackcontroller.h"

#include <algorithm>
#include <QRandomGenerator>

PlaybackController::PlaybackController(QObject *parent)
    : QObject(parent)
{
    m_player.setAudioOutput(&m_audioOutput);
    m_audioOutput.setVolume(0.75);

    connect(&m_player, &QMediaPlayer::playbackStateChanged, this, &PlaybackController::playbackStateChanged);
    connect(&m_player, &QMediaPlayer::positionChanged, this, &PlaybackController::positionChanged);
    connect(&m_player, &QMediaPlayer::durationChanged, this, &PlaybackController::durationChanged);
    connect(&m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            handleEndOfTrack();
        }
    });
    connect(&m_player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString) {
        Q_UNUSED(error)
        m_errorString = errorString;
        emit errorChanged();
    });
}

TrackListModel *PlaybackController::queueModel() { return &m_queueModel; }
bool PlaybackController::isPlaying() const { return m_player.playbackState() == QMediaPlayer::PlayingState; }
bool PlaybackController::isPaused() const { return m_player.playbackState() == QMediaPlayer::PausedState; }
qint64 PlaybackController::position() const { return m_player.position(); }
qint64 PlaybackController::duration() const { return m_player.duration(); }
int PlaybackController::volume() const { return qRound(m_audioOutput.volume() * 100.0); }
bool PlaybackController::shuffle() const { return m_shuffle; }
PlaybackController::RepeatMode PlaybackController::repeatMode() const { return m_repeatMode; }
int PlaybackController::currentIndex() const { return m_currentIndex; }

QVariantMap PlaybackController::currentTrack() const
{
    return m_queueModel.trackAt(m_currentIndex).toVariantMap();
}

QString PlaybackController::errorString() const
{
    return m_errorString;
}

void PlaybackController::setVolume(int value)
{
    value = std::clamp(value, 0, 100);
    if (volume() == value) {
        return;
    }
    m_audioOutput.setVolume(value / 100.0);
    emit volumeChanged();
}

void PlaybackController::setShuffle(bool value)
{
    if (m_shuffle == value) {
        return;
    }
    m_shuffle = value;
    emit playbackModesChanged();
}

void PlaybackController::setRepeatMode(RepeatMode value)
{
    if (m_repeatMode == value) {
        return;
    }
    m_repeatMode = value;
    emit playbackModesChanged();
}

void PlaybackController::setQueue(const QVector<Track> &tracks)
{
    m_queueModel.setTracks(tracks);
    if (tracks.isEmpty()) {
        m_currentIndex = -1;
        m_player.stop();
    } else if (m_currentIndex >= tracks.size()) {
        m_currentIndex = 0;
    }
    emit currentTrackChanged();
}

void PlaybackController::enqueue(const QVariantMap &trackMap)
{
    enqueue(trackFromMap(trackMap));
}

void PlaybackController::enqueue(const Track &track)
{
    if (!track.isValid()) {
        return;
    }

    m_queueModel.appendTrack(track);
    if (m_currentIndex < 0) {
        playIndex(0);
    }
}

void PlaybackController::removeFromQueue(int index)
{
    m_queueModel.removeTrack(index);
    if (index == m_currentIndex) {
        if (m_queueModel.rowCount() == 0) {
            stop();
            m_currentIndex = -1;
        } else {
            m_currentIndex = qMin(index, m_queueModel.rowCount() - 1);
            loadTrack(m_currentIndex, true);
        }
        emit currentTrackChanged();
    } else if (index < m_currentIndex) {
        --m_currentIndex;
        emit currentTrackChanged();
    }
}

void PlaybackController::play()
{
    if (m_currentIndex < 0 && m_queueModel.rowCount() > 0) {
        playIndex(0);
        return;
    }
    m_player.play();
}

void PlaybackController::pause()
{
    m_player.pause();
}

void PlaybackController::stop()
{
    m_player.stop();
    m_player.setSource(QUrl());
    emit playbackStateChanged();
}

void PlaybackController::next()
{
    if (m_queueModel.rowCount() == 0) {
        return;
    }

    int nextIndex = m_currentIndex;
    if (m_shuffle && m_queueModel.rowCount() > 1) {
        do {
            nextIndex = QRandomGenerator::global()->bounded(m_queueModel.rowCount());
        } while (nextIndex == m_currentIndex);
    } else {
        nextIndex = m_currentIndex + 1;
    }

    if (nextIndex >= m_queueModel.rowCount()) {
        if (m_repeatMode == RepeatQueue) {
            nextIndex = 0;
        } else {
            stop();
            return;
        }
    }

    playIndex(nextIndex);
}

void PlaybackController::previous()
{
    if (m_queueModel.rowCount() == 0) {
        return;
    }

    if (position() > 5000) {
        seek(0);
        return;
    }

    int previousIndex = m_currentIndex - 1;
    if (previousIndex < 0) {
        previousIndex = m_repeatMode == RepeatQueue ? m_queueModel.rowCount() - 1 : 0;
    }

    playIndex(previousIndex);
}

void PlaybackController::seek(qint64 value)
{
    m_player.setPosition(value);
}

void PlaybackController::playIndex(int index)
{
    loadTrack(index, true);
}

Track PlaybackController::trackFromMap(const QVariantMap &trackMap) const
{
    Track track;
    track.id = trackMap.value(QStringLiteral("id")).toString();
    track.title = trackMap.value(QStringLiteral("title")).toString();
    track.artist = trackMap.value(QStringLiteral("artist")).toString();
    track.album = trackMap.value(QStringLiteral("album")).toString();
    track.durationMs = trackMap.value(QStringLiteral("durationMs")).toLongLong();
    track.sourceUrl = trackMap.value(QStringLiteral("sourceUrl")).toUrl();
    track.artUrl = trackMap.value(QStringLiteral("artUrl")).toUrl();
    track.remote = trackMap.value(QStringLiteral("remote")).toBool();
    track.sourceName = trackMap.value(QStringLiteral("sourceName")).toString();
    return track;
}

void PlaybackController::loadTrack(int index, bool autoplay)
{
    const Track track = m_queueModel.trackAt(index);
    if (!track.isValid()) {
        return;
    }

    m_currentIndex = index;
    if (!m_errorString.isEmpty()) {
        m_errorString.clear();
        emit errorChanged();
    }
    m_player.setSource(track.sourceUrl);
    emit currentTrackChanged();

    if (autoplay) {
        m_player.play();
    }
}

void PlaybackController::handleEndOfTrack()
{
    if (m_repeatMode == RepeatTrack) {
        playIndex(m_currentIndex);
        return;
    }

    next();
}
