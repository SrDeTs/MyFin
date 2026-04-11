#pragma once

#include "domain/entities/Track.h"

#include <QObject>
#include <QVector>

class QTimer;

namespace MyFin::Infrastructure::Jellyfin {
class JellyfinApiClient;
}

namespace MyFin::Infrastructure::Settings {
class SettingsService;
}

namespace MyFin::Infrastructure::Playback {

class QtMediaPlaybackBackend;

class PlaybackController final : public QObject {
    Q_OBJECT

public:
    struct State {
        QString trackId;
        QString title;
        QString artist;
        QString album;
        QString coverSource;
        QString signalPath;
        QString errorText;
        bool playing = false;
        int queueLength = 0;
        qint64 positionMs = 0;
        qint64 durationMs = 0;
    };

    PlaybackController(Infrastructure::Jellyfin::JellyfinApiClient& jellyfin,
                       Infrastructure::Settings::SettingsService& settings,
                       QObject* parent = nullptr);

    const State& state() const;
    const QVector<Domain::Track>& queue() const;

    void playNow(const Domain::Track& track);

    Q_INVOKABLE void togglePlaying();
    Q_INVOKABLE void next();

signals:
    void stateChanged();

private:
    void playCurrent();
    void syncStateFromCurrent();
    void flushPlaybackProgress(bool paused);
    void stopCurrentTrack(bool markPlayed);
    bool completionReached() const;

    Infrastructure::Jellyfin::JellyfinApiClient& m_jellyfin;
    Infrastructure::Settings::SettingsService& m_settings;
    QtMediaPlaybackBackend* m_backend = nullptr;
    QTimer* m_progressTimer = nullptr;
    QVector<Domain::Track> m_queue;
    int m_currentIndex = -1;
    QString m_playSessionId;
    State m_state;
};

}  // namespace MyFin::Infrastructure::Playback
