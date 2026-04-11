#include "app/AppServices.h"

namespace MyFin::App {

AppServices::AppServices()
    : m_paths()
    , m_settings(m_paths)
    , m_stateStore(m_settings)
    , m_libraryCache(m_paths)
    , m_secrets()
    , m_jellyfin(m_settings, m_libraryCache, m_secrets)
    , m_playback(m_jellyfin, m_settings)
    , m_notifications()
    , m_mpris()
    , m_singleInstance(m_paths)
{
    m_paths.ensureDirectories();
    m_notifications.bindPlaybackController(&m_playback);
    m_mpris.bindPlaybackController(&m_playback);
    m_notifications.start();
    m_mpris.start();
}

Infrastructure::Settings::AppPaths& AppServices::paths()
{
    return m_paths;
}

Infrastructure::Settings::SettingsService& AppServices::settings()
{
    return m_settings;
}

Infrastructure::Storage::StateStore& AppServices::stateStore()
{
    return m_stateStore;
}

Infrastructure::Storage::LibraryCache& AppServices::libraryCache()
{
    return m_libraryCache;
}

Infrastructure::Jellyfin::JellyfinApiClient& AppServices::jellyfin()
{
    return m_jellyfin;
}

Infrastructure::Playback::PlaybackController& AppServices::playback()
{
    return m_playback;
}

Linux::SecretStore& AppServices::secrets()
{
    return m_secrets;
}

Linux::NotificationService& AppServices::notifications()
{
    return m_notifications;
}

Linux::MprisService& AppServices::mpris()
{
    return m_mpris;
}

Linux::SingleInstanceService& AppServices::singleInstance()
{
    return m_singleInstance;
}

}  // namespace MyFin::App
