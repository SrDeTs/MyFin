#include "presentation/viewmodels/PlaybackViewModel.h"

#include "infrastructure/playback/PlaybackController.h"

namespace MyFin::Presentation {

PlaybackViewModel::PlaybackViewModel(Infrastructure::Playback::PlaybackController& playback, QObject* parent)
    : QObject(parent)
    , m_playback(playback)
{
    connect(&m_playback, &Infrastructure::Playback::PlaybackController::stateChanged,
            this, &PlaybackViewModel::stateChanged);
}

QString PlaybackViewModel::title() const
{
    return m_playback.state().title;
}

QString PlaybackViewModel::artist() const
{
    return m_playback.state().artist;
}

QString PlaybackViewModel::album() const
{
    return m_playback.state().album;
}

QString PlaybackViewModel::coverSource() const
{
    return m_playback.state().coverSource;
}

QString PlaybackViewModel::signalPath() const
{
    return m_playback.state().signalPath;
}

bool PlaybackViewModel::playing() const
{
    return m_playback.state().playing;
}

bool PlaybackViewModel::hasTrack() const
{
    return !m_playback.state().trackId.isEmpty();
}

bool PlaybackViewModel::canGoPrevious() const
{
    return m_playback.hasPrevious();
}

bool PlaybackViewModel::canGoNext() const
{
    return m_playback.hasNext();
}

bool PlaybackViewModel::repeatCurrent() const
{
    return m_playback.state().repeatCurrent;
}

int PlaybackViewModel::queueLength() const
{
    return m_playback.state().queueLength;
}

qint64 PlaybackViewModel::positionMs() const
{
    return m_playback.state().positionMs;
}

qint64 PlaybackViewModel::durationMs() const
{
    return m_playback.state().durationMs;
}

QString PlaybackViewModel::positionLabel() const
{
    return formatTime(positionMs());
}

QString PlaybackViewModel::durationLabel() const
{
    return formatTime(durationMs());
}

float PlaybackViewModel::outputVolume() const
{
    return m_playback.outputVolume();
}

void PlaybackViewModel::togglePlaying()
{
    m_playback.togglePlaying();
}

void PlaybackViewModel::previous()
{
    m_playback.previous();
}

void PlaybackViewModel::next()
{
    m_playback.next();
}

void PlaybackViewModel::seek(qint64 positionMs)
{
    m_playback.seek(positionMs);
}

void PlaybackViewModel::setOutputVolume(float value)
{
    m_playback.setOutputVolume(value);
}

void PlaybackViewModel::toggleRepeatCurrent()
{
    m_playback.toggleRepeatCurrent();
}

QString PlaybackViewModel::formatTime(qint64 positionMs)
{
    if (positionMs <= 0) {
        return QStringLiteral("0:00");
    }

    const qint64 totalSeconds = positionMs / 1000;
    const qint64 minutes = totalSeconds / 60;
    const qint64 seconds = totalSeconds % 60;
    return QStringLiteral("%1:%2").arg(minutes).arg(seconds, 2, 10, QLatin1Char('0'));
}

}  // namespace MyFin::Presentation
