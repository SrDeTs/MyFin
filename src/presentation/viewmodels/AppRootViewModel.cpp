#include "presentation/viewmodels/AppRootViewModel.h"

#include "app/AppServices.h"
#include "presentation/viewmodels/AudioSettingsViewModel.h"
#include "presentation/viewmodels/LibraryViewModel.h"
#include "presentation/viewmodels/PlaybackViewModel.h"
#include "presentation/viewmodels/SessionViewModel.h"

namespace MyFin::Presentation {

AppRootViewModel::AppRootViewModel(App::AppServices& services, QObject* parent)
    : QObject(parent)
    , m_services(services)
    , m_library(new LibraryViewModel(m_services.jellyfin(), m_services.playback(), this))
    , m_playback(new PlaybackViewModel(m_services.playback(), this))
    , m_session(new SessionViewModel(m_services.jellyfin(), m_services.settings(), this))
    , m_audio(new AudioSettingsViewModel(m_services.playback(), m_services.paths(), *m_library, this))
{
    connect(m_playback, &PlaybackViewModel::stateChanged, this, [this] {
        if (m_playback->hasTrack()) {
            m_windowTitle = QStringLiteral("%1 • %2").arg(m_playback->title(), m_playback->artist());
        } else {
            m_windowTitle = QStringLiteral("MyFin");
        }
        emit windowTitleChanged();
    });
}

AppRootViewModel::~AppRootViewModel() = default;

LibraryViewModel* AppRootViewModel::library()
{
    return m_library;
}

PlaybackViewModel* AppRootViewModel::playback()
{
    return m_playback;
}

SessionViewModel* AppRootViewModel::session()
{
    return m_session;
}

AudioSettingsViewModel* AppRootViewModel::audio()
{
    return m_audio;
}

QString AppRootViewModel::windowTitle() const
{
    return m_windowTitle;
}

void AppRootViewModel::bootstrap()
{
}

void AppRootViewModel::reloadLibrary()
{
    m_library->reload();
}

}  // namespace MyFin::Presentation
