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

int PlaybackViewModel::queueLength() const
{
    return m_playback.state().queueLength;
}

void PlaybackViewModel::togglePlaying()
{
    m_playback.togglePlaying();
}

void PlaybackViewModel::next()
{
    m_playback.next();
}

}  // namespace MyFin::Presentation
