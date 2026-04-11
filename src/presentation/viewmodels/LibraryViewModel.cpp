#include "presentation/viewmodels/LibraryViewModel.h"

#include "infrastructure/jellyfin/JellyfinApiClient.h"
#include "infrastructure/playback/PlaybackController.h"

#include <QtGlobal>

namespace MyFin::Presentation {

LibraryViewModel::LibraryViewModel(Infrastructure::Jellyfin::JellyfinApiClient& jellyfin,
                                   Infrastructure::Playback::PlaybackController& playback,
                                   QObject* parent)
    : QObject(parent)
    , m_jellyfin(jellyfin)
    , m_playback(playback)
    , m_tracks(this)
{
    connect(&m_jellyfin, &Infrastructure::Jellyfin::JellyfinApiClient::homeFeedReady, this, [this] {
        m_loading = false;
        emit loadingChanged();

        m_cachedTracks = m_jellyfin.currentHomeTracks();
        rebuildRows();

        m_statusMessage = QStringLiteral("%1 tracks synchronized from %2")
                              .arg(m_cachedTracks.size())
                              .arg(m_jellyfin.serverLabel());
        emit statusMessageChanged();
        emit contentChanged();
    });

    connect(&m_jellyfin, &Infrastructure::Jellyfin::JellyfinApiClient::loginSucceeded, this, [this] {
        m_loading = true;
        emit loadingChanged();
        m_statusMessage = QStringLiteral("Refreshing local snapshot from Jellyfin…");
        emit statusMessageChanged();
        emit connectedChanged();
        emit serverLabelChanged();
        emit contentChanged();
    });

    connect(&m_jellyfin, &Infrastructure::Jellyfin::JellyfinApiClient::loggedOut, this, [this] {
        m_loading = false;
        emit loadingChanged();

        m_cachedTracks = m_jellyfin.currentHomeTracks();
        rebuildRows();

        m_statusMessage = hasTracks()
                              ? QStringLiteral("Disconnected from Jellyfin. Showing the last cached snapshot.")
                              : QStringLiteral("Disconnected from Jellyfin.");
        emit statusMessageChanged();
        emit connectedChanged();
        emit serverLabelChanged();
        emit contentChanged();
    });

    connect(&m_jellyfin, &Infrastructure::Jellyfin::JellyfinApiClient::requestFailed, this,
            [this](const QString& message) {
                m_loading = false;
                emit loadingChanged();
                m_statusMessage = message;
                emit statusMessageChanged();
            });

    connect(&m_jellyfin, &Infrastructure::Jellyfin::JellyfinApiClient::sessionChanged, this, [this] {
        emit serverLabelChanged();
        emit connectedChanged();
        emit contentChanged();
    });

    if (!m_jellyfin.currentHomeTracks().isEmpty()) {
        m_cachedTracks = m_jellyfin.currentHomeTracks();
        rebuildRows();
        m_statusMessage = QStringLiteral("Loaded %1 cached tracks from the local snapshot.")
                              .arg(m_cachedTracks.size());
    }
}

Models::TrackListModel* LibraryViewModel::tracks()
{
    return &m_tracks;
}

bool LibraryViewModel::loading() const
{
    return m_loading;
}

QString LibraryViewModel::statusMessage() const
{
    return m_statusMessage;
}

QString LibraryViewModel::serverLabel() const
{
    return m_jellyfin.serverLabel();
}

bool LibraryViewModel::connected() const
{
    return m_jellyfin.authenticated();
}

bool LibraryViewModel::hasTracks() const
{
    return !m_cachedTracks.isEmpty();
}

int LibraryViewModel::trackCount() const
{
    return m_cachedTracks.size();
}

QString LibraryViewModel::headlineTitle() const
{
    if (!connected()) {
        return hasTracks() ? QStringLiteral("Cached library snapshot")
                           : QStringLiteral("Connect your Jellyfin music library");
    }

    if (m_cachedTracks.isEmpty()) {
        return QStringLiteral("Library connected");
    }

    return m_cachedTracks.constFirst().album;
}

QString LibraryViewModel::headlineSubtitle() const
{
    if (!connected()) {
        return hasTracks()
                   ? QStringLiteral("Browsing the last cached snapshot. Sign in to stream or refresh.")
                   : QStringLiteral("Sign in with your server, username, and password to load music metadata.");
    }

    if (m_cachedTracks.isEmpty()) {
        return QStringLiteral("No tracks loaded yet. Refresh the library snapshot.");
    }

    const Domain::Track& firstTrack = m_cachedTracks.constFirst();
    return QStringLiteral("%1 • %2 curated tracks ready").arg(firstTrack.artist).arg(m_cachedTracks.size());
}

void LibraryViewModel::reload()
{
    if (m_loading) {
        return;
    }

    if (!connected()) {
        m_statusMessage = QStringLiteral("Sign in to Jellyfin before refreshing the library.");
        emit statusMessageChanged();
        emit contentChanged();
        return;
    }

    m_loading = true;
    emit loadingChanged();

    m_statusMessage = QStringLiteral("Refreshing local snapshot from Jellyfin…");
    emit statusMessageChanged();

    m_jellyfin.fetchHomeFeed();
}

void LibraryViewModel::playTrack(const QString& trackId)
{
    for (const Domain::Track& track : m_cachedTracks) {
        if (track.id == trackId) {
            m_playback.playNow(track);
            return;
        }
    }
}

QString LibraryViewModel::formatDuration(qint64 durationMs) const
{
    const qint64 totalSeconds = durationMs / 1000;
    const qint64 minutes = totalSeconds / 60;
    const qint64 seconds = totalSeconds % 60;
    return QStringLiteral("%1:%2")
        .arg(minutes)
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

void LibraryViewModel::rebuildRows()
{
    QVector<Models::TrackRow> rows;
    rows.reserve(m_cachedTracks.size());

    for (const Domain::Track& track : m_cachedTracks) {
        rows.push_back({
            track.id,
            track.title,
            track.artist,
            track.album,
            formatDuration(track.durationMs),
            track.artworkKey,
            m_jellyfin.coverSourceForArtworkKey(track.artworkKey),
            track.streamProfile,
            track.durationMs,
            track.favorite,
        });
    }

    m_tracks.replaceRows(std::move(rows));
}

}  // namespace MyFin::Presentation
