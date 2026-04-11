#include "infrastructure/playback/QtMediaPlaybackBackend.h"

#include "infrastructure/logging/Logging.h"

#include <QAudioOutput>
#include <QMediaPlayer>

namespace MyFin::Infrastructure::Playback {

QtMediaPlaybackBackend::QtMediaPlaybackBackend(QObject* parent)
    : QObject(parent)
    , m_audioOutput(new QAudioOutput(this))
    , m_player(new QMediaPlayer(this))
{
    m_player->setAudioOutput(m_audioOutput);

    connect(m_player, &QMediaPlayer::playbackStateChanged, this, [this] {
        emit playingChanged();
    });

    connect(m_player, &QMediaPlayer::positionChanged, this, &QtMediaPlaybackBackend::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &QtMediaPlaybackBackend::durationChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            emit mediaFinished();
        }
    });

    connect(m_player, &QMediaPlayer::errorOccurred, this,
            [this](QMediaPlayer::Error error, const QString& errorString) {
                if (error == QMediaPlayer::NoError) {
                    return;
                }

                qWarning(lcPlayback) << "Playback backend error:" << errorString;
                emit errorOccurred(errorString);
            });
}

QtMediaPlaybackBackend::~QtMediaPlaybackBackend() = default;

void QtMediaPlaybackBackend::setSource(const QUrl& source)
{
    m_player->setSource(source);
}

void QtMediaPlaybackBackend::play()
{
    m_player->play();
}

void QtMediaPlaybackBackend::pause()
{
    m_player->pause();
}

void QtMediaPlaybackBackend::stop()
{
    m_player->stop();
}

void QtMediaPlaybackBackend::setVolume(float value)
{
    m_audioOutput->setVolume(value);
}

void QtMediaPlaybackBackend::setPosition(qint64 positionMs)
{
    m_player->setPosition(positionMs);
}

bool QtMediaPlaybackBackend::isPlaying() const
{
    return m_player->playbackState() == QMediaPlayer::PlayingState;
}

qint64 QtMediaPlaybackBackend::position() const
{
    return m_player->position();
}

qint64 QtMediaPlaybackBackend::duration() const
{
    return m_player->duration();
}

}  // namespace MyFin::Infrastructure::Playback
