#pragma once

#include "infrastructure/images/CoverArtImageProvider.h"
#include "infrastructure/jellyfin/JellyfinApiClient.h"
#include "infrastructure/playback/PlaybackController.h"
#include "infrastructure/settings/AppPaths.h"
#include "infrastructure/settings/SettingsService.h"
#include "infrastructure/storage/LibraryCache.h"
#include "infrastructure/storage/StateStore.h"
#include "linux/MprisService.h"
#include "linux/NotificationService.h"
#include "linux/SecretStore.h"
#include "linux/SingleInstanceService.h"

namespace MyFin::App {

class AppServices final {
public:
    AppServices();

    Infrastructure::Settings::AppPaths& paths();
    Infrastructure::Settings::SettingsService& settings();
    Infrastructure::Storage::StateStore& stateStore();
    Infrastructure::Storage::LibraryCache& libraryCache();
    Infrastructure::Jellyfin::JellyfinApiClient& jellyfin();
    Infrastructure::Playback::PlaybackController& playback();
    Linux::SecretStore& secrets();
    Linux::NotificationService& notifications();
    Linux::MprisService& mpris();
    Linux::SingleInstanceService& singleInstance();

private:
    Infrastructure::Settings::AppPaths m_paths;
    Infrastructure::Settings::SettingsService m_settings;
    Infrastructure::Storage::StateStore m_stateStore;
    Infrastructure::Storage::LibraryCache m_libraryCache;
    Linux::SecretStore m_secrets;
    Infrastructure::Jellyfin::JellyfinApiClient m_jellyfin;
    Infrastructure::Playback::PlaybackController m_playback;
    Linux::NotificationService m_notifications;
    Linux::MprisService m_mpris;
    Linux::SingleInstanceService m_singleInstance;
};

}  // namespace MyFin::App
