#pragma once

#include "domain/entities/Track.h"
#include "presentation/models/TrackListModel.h"

#include <QObject>
#include <QVector>

namespace MyFin::Infrastructure::Jellyfin {
class JellyfinApiClient;
}

namespace MyFin::Infrastructure::Playback {
class PlaybackController;
}

namespace MyFin::Presentation {

class LibraryViewModel final : public QObject {
    Q_OBJECT
    Q_PROPERTY(Models::TrackListModel* tracks READ tracks CONSTANT FINAL)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString serverLabel READ serverLabel NOTIFY serverLabelChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool hasTracks READ hasTracks NOTIFY contentChanged)
    Q_PROPERTY(int trackCount READ trackCount NOTIFY contentChanged)
    Q_PROPERTY(QString headlineTitle READ headlineTitle NOTIFY contentChanged)
    Q_PROPERTY(QString headlineSubtitle READ headlineSubtitle NOTIFY contentChanged)

public:
    LibraryViewModel(Infrastructure::Jellyfin::JellyfinApiClient& jellyfin,
                     Infrastructure::Playback::PlaybackController& playback,
                     QObject* parent = nullptr);

    Models::TrackListModel* tracks();
    bool loading() const;
    QString statusMessage() const;
    QString serverLabel() const;
    bool connected() const;
    bool hasTracks() const;
    int trackCount() const;
    QString headlineTitle() const;
    QString headlineSubtitle() const;

    Q_INVOKABLE void reload();
    Q_INVOKABLE void playTrack(const QString& trackId);

signals:
    void loadingChanged();
    void statusMessageChanged();
    void serverLabelChanged();
    void connectedChanged();
    void contentChanged();

private:
    QString formatDuration(qint64 durationMs) const;
    void rebuildRows();

    Infrastructure::Jellyfin::JellyfinApiClient& m_jellyfin;
    Infrastructure::Playback::PlaybackController& m_playback;
    Models::TrackListModel m_tracks;
    QVector<Domain::Track> m_cachedTracks;
    bool m_loading = false;
    QString m_statusMessage;
};

}  // namespace MyFin::Presentation
