#include "applicationcontroller.h"

ApplicationController::ApplicationController(QObject *parent)
    : QObject(parent)
{
    connect(&m_jellyfin, &JellyfinClient::libraryReceived,
            &m_library, &LibraryManager::importJellyfinTracks);

    connect(&m_playback, &PlaybackController::currentTrackChanged, this, [this]() {
        if (m_settings.visualTheme() == QStringLiteral("fallback")) {
            m_theme.applyFallbackTheme();
            return;
        }
        const QUrl artUrl = m_playback.currentTrack().value(QStringLiteral("artUrl")).toUrl();
        m_theme.updateFromArt(artUrl);
    });
}

SettingsManager *ApplicationController::settings() { return &m_settings; }
LibraryManager *ApplicationController::library() { return &m_library; }
PlaybackController *ApplicationController::playback() { return &m_playback; }
JellyfinClient *ApplicationController::jellyfin() { return &m_jellyfin; }
ThemeManager *ApplicationController::theme() { return &m_theme; }

void ApplicationController::initialize()
{
    m_playback.setVolume(m_settings.defaultVolume());
    reloadLocalLibrary();

    if (!m_settings.jellyfinUrl().isEmpty() &&
        !m_settings.jellyfinUsername().isEmpty() &&
        !m_settings.jellyfinPassword().isEmpty()) {
        connectJellyfin();
    }
}

void ApplicationController::playLibraryTrack(int index)
{
    const QVector<Track> tracks = m_library.tracksModel()->tracks();
    if (index < 0 || index >= tracks.size()) {
        return;
    }

    m_playback.setQueue(tracks);
    m_playback.playIndex(index);
}

void ApplicationController::enqueueLibraryTrack(int index)
{
    const Track track = m_library.tracksModel()->trackAt(index);
    if (!track.isValid()) {
        return;
    }

    if (m_settings.queueBehavior() == QStringLiteral("replace")) {
        m_playback.setQueue({track});
        m_playback.playIndex(0);
        return;
    }

    m_playback.enqueue(track);
}

void ApplicationController::playFromQueue(int index)
{
    m_playback.playIndex(index);
}

void ApplicationController::removeQueueTrack(int index)
{
    m_playback.removeFromQueue(index);
}

void ApplicationController::reloadLocalLibrary()
{
    m_library.scanLocalDirectories(m_settings.musicDirectories());
}

void ApplicationController::connectJellyfin()
{
    m_settings.save();
    m_jellyfin.connectToServer(m_settings.jellyfinUrl(),
                               m_settings.jellyfinUsername(),
                               m_settings.jellyfinPassword());
}

QString ApplicationController::formatTime(qint64 value) const
{
    const qint64 totalSeconds = value / 1000;
    const qint64 minutes = totalSeconds / 60;
    const qint64 seconds = totalSeconds % 60;
    return QStringLiteral("%1:%2")
        .arg(minutes)
        .arg(seconds, 2, 10, QChar('0'));
}
