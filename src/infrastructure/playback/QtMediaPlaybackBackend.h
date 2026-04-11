#pragma once

#include "infrastructure/playback/IPlaybackBackend.h"

#include <QObject>

class QAudioOutput;
class QMediaPlayer;

namespace MyFin::Infrastructure::Playback {

class QtMediaPlaybackBackend final : public QObject, public IPlaybackBackend {
    Q_OBJECT

public:
    explicit QtMediaPlaybackBackend(QObject* parent = nullptr);
    ~QtMediaPlaybackBackend() override;

    void setSource(const QUrl& source) override;
    void play() override;
    void pause() override;
    void stop() override;

    void setVolume(float value);
    bool isPlaying() const;
    qint64 position() const;
    qint64 duration() const;

signals:
    void playingChanged();
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void mediaFinished();
    void errorOccurred(const QString& message);

private:
    QAudioOutput* m_audioOutput = nullptr;
    QMediaPlayer* m_player = nullptr;
};

}  // namespace MyFin::Infrastructure::Playback
