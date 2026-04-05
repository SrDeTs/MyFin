#pragma once

#include "../models/tracklistmodel.h"

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QObject>

class PlaybackController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TrackListModel* queueModel READ queueModel CONSTANT)
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playbackStateChanged)
    Q_PROPERTY(bool paused READ isPaused NOTIFY playbackStateChanged)
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool shuffle READ shuffle WRITE setShuffle NOTIFY playbackModesChanged)
    Q_PROPERTY(RepeatMode repeatMode READ repeatMode WRITE setRepeatMode NOTIFY playbackModesChanged)
    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentTrackChanged)
    Q_PROPERTY(QVariantMap currentTrack READ currentTrack NOTIFY currentTrackChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)

public:
    enum RepeatMode {
        NoRepeat,
        RepeatTrack,
        RepeatQueue
    };
    Q_ENUM(RepeatMode)

    explicit PlaybackController(QObject *parent = nullptr);

    TrackListModel *queueModel();
    bool isPlaying() const;
    bool isPaused() const;
    qint64 position() const;
    qint64 duration() const;
    int volume() const;
    bool shuffle() const;
    RepeatMode repeatMode() const;
    int currentIndex() const;
    QVariantMap currentTrack() const;
    QString errorString() const;

    void setVolume(int value);
    void setShuffle(bool value);
    void setRepeatMode(RepeatMode value);

    Q_INVOKABLE void setQueue(const QVector<Track> &tracks);
    Q_INVOKABLE void enqueue(const QVariantMap &trackMap);
    void enqueue(const Track &track);
    Q_INVOKABLE void removeFromQueue(int index);
    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void next();
    Q_INVOKABLE void previous();
    Q_INVOKABLE void seek(qint64 value);
    Q_INVOKABLE void playIndex(int index);

signals:
    void playbackStateChanged();
    void positionChanged();
    void durationChanged();
    void volumeChanged();
    void playbackModesChanged();
    void currentTrackChanged();
    void errorChanged();

private:
    Track trackFromMap(const QVariantMap &trackMap) const;
    void loadTrack(int index, bool autoplay);
    void handleEndOfTrack();

    QMediaPlayer m_player;
    QAudioOutput m_audioOutput;
    TrackListModel m_queueModel;
    int m_currentIndex = -1;
    bool m_shuffle = false;
    RepeatMode m_repeatMode = RepeatQueue;
    QString m_errorString;
};
