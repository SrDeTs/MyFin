#pragma once

#include "../integration/jellyfinclient.h"
#include "../library/librarymanager.h"
#include "../player/playbackcontroller.h"
#include "../settings/settingsmanager.h"
#include "../theme/thememanager.h"

#include <QObject>

class ApplicationController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SettingsManager* settings READ settings CONSTANT)
    Q_PROPERTY(LibraryManager* library READ library CONSTANT)
    Q_PROPERTY(PlaybackController* playback READ playback CONSTANT)
    Q_PROPERTY(JellyfinClient* jellyfin READ jellyfin CONSTANT)
    Q_PROPERTY(ThemeManager* theme READ theme CONSTANT)

public:
    explicit ApplicationController(QObject *parent = nullptr);

    SettingsManager *settings();
    LibraryManager *library();
    PlaybackController *playback();
    JellyfinClient *jellyfin();
    ThemeManager *theme();

    Q_INVOKABLE void initialize();
    Q_INVOKABLE void playLibraryTrack(int index);
    Q_INVOKABLE void enqueueLibraryTrack(int index);
    Q_INVOKABLE void playFromQueue(int index);
    Q_INVOKABLE void removeQueueTrack(int index);
    Q_INVOKABLE void reloadLocalLibrary();
    Q_INVOKABLE void connectJellyfin();
    Q_INVOKABLE QString formatTime(qint64 value) const;

private:
    SettingsManager m_settings;
    LibraryManager m_library;
    PlaybackController m_playback;
    JellyfinClient m_jellyfin;
    ThemeManager m_theme;
};
