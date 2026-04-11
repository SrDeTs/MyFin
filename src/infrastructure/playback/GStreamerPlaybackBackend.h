#pragma once

#include "infrastructure/playback/IPlaybackBackend.h"

#include <QHash>
#include <QObject>
#include <QString>
#include <QVector>

class QTimer;

typedef struct _GstBus GstBus;
typedef struct _GstDevice GstDevice;
typedef struct _GstDeviceMonitor GstDeviceMonitor;
typedef struct _GstElement GstElement;

namespace MyFin::Infrastructure::Playback {

class GStreamerPlaybackBackend final : public QObject, public IPlaybackBackend {
    Q_OBJECT

public:
    explicit GStreamerPlaybackBackend(QObject* parent = nullptr);
    ~GStreamerPlaybackBackend() override;

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
    struct DeviceEntry {
        AudioOutputInfo info;
        GstDevice* device = nullptr;
    };

    static void ensureGStreamerInitialized();

    void processBusMessages();
    void refreshOutputDevices();
    bool applyOutputDevice(const QString& deviceId);
    QString defaultDeviceId() const;
    void setCurrentDeviceFromInfo(const AudioOutputInfo& info);
    void updateNegotiatedFormat();
    void emitPositionIfChanged();
    void emitDurationIfChanged();

    GstElement* m_playbin = nullptr;
    GstBus* m_bus = nullptr;
    GstDeviceMonitor* m_deviceMonitor = nullptr;
    QTimer* m_busTimer = nullptr;
    QTimer* m_positionTimer = nullptr;
    QTimer* m_deviceRefreshTimer = nullptr;

    QVector<AudioOutputInfo> m_outputDevices;
    QHash<QString, DeviceEntry> m_deviceEntries;

    QString m_currentOutputDeviceId;
    QString m_currentOutputDeviceName;
    int m_currentOutputSampleRate = 0;
    int m_currentOutputChannelCount = 0;
    QString m_currentOutputSampleFormat = QStringLiteral("Desconhecido");

    qint64 m_lastPositionMs = 0;
    qint64 m_lastDurationMs = 0;
    bool m_isPlaying = false;
    bool m_hasSource = false;
};

}  // namespace MyFin::Infrastructure::Playback
