#pragma once

#include "infrastructure/playback/IPlaybackBackend.h"

#include <QObject>
#include <QVector>

class QAudioOutput;
class QMediaDevices;
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
    float volume() const;
    void setPosition(qint64 positionMs);
    bool isPlaying() const;
    qint64 position() const;
    qint64 duration() const;
    QVector<AudioOutputInfo> outputDevices() const;
    bool setOutputDeviceById(const QString& deviceId);
    QString currentOutputDeviceId() const;
    QString currentOutputDeviceName() const;
    int currentOutputSampleRate() const;
    int currentOutputChannelCount() const;
    QString currentOutputSampleFormat() const;
    QString backendName() const;

signals:
    void playingChanged();
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void mediaFinished();
    void errorOccurred(const QString& message);
    void outputDeviceChanged();
    void outputDevicesChanged();

private:
    QAudioOutput* m_audioOutput = nullptr;
    QMediaDevices* m_mediaDevices = nullptr;
    QMediaPlayer* m_player = nullptr;
};

}  // namespace MyFin::Infrastructure::Playback
