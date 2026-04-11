#include "infrastructure/playback/QtMediaPlaybackBackend.h"

#include "infrastructure/logging/Logging.h"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QMediaPlayer>

namespace MyFin::Infrastructure::Playback {

namespace {

QString deviceIdString(const QAudioDevice& device)
{
    return QString::fromLatin1(device.id().toHex());
}

QString sampleFormatLabel(QAudioFormat::SampleFormat format)
{
    switch (format) {
    case QAudioFormat::UInt8:
        return QStringLiteral("U8");
    case QAudioFormat::Int16:
        return QStringLiteral("S16");
    case QAudioFormat::Int32:
        return QStringLiteral("S32");
    case QAudioFormat::Float:
        return QStringLiteral("Float");
    case QAudioFormat::Unknown:
    default:
        return QStringLiteral("Desconhecido");
    }
}

}  // namespace

QtMediaPlaybackBackend::QtMediaPlaybackBackend(QObject* parent)
    : QObject(parent)
    , m_audioOutput(new QAudioOutput(this))
    , m_mediaDevices(new QMediaDevices(this))
    , m_player(new QMediaPlayer(this))
{
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setDevice(QMediaDevices::defaultAudioOutput());

    connect(m_player, &QMediaPlayer::playbackStateChanged, this, [this] {
        emit playingChanged();
    });

    connect(m_player, &QMediaPlayer::positionChanged, this, &QtMediaPlaybackBackend::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &QtMediaPlaybackBackend::durationChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            emit mediaFinished();
        }
    });

    connect(m_player, &QMediaPlayer::errorOccurred, this,
            [this](QMediaPlayer::Error error, const QString& errorString) {
                if (error == QMediaPlayer::NoError) {
                    return;
                }

                qWarning(lcPlayback) << "Playback backend error:" << errorString;
                emit errorOccurred(errorString);
            });

    connect(m_mediaDevices, &QMediaDevices::audioOutputsChanged, this, [this] {
        emit outputDevicesChanged();
    });
}

QtMediaPlaybackBackend::~QtMediaPlaybackBackend() = default;

void QtMediaPlaybackBackend::setSource(const QUrl& source)
{
    m_player->setSource(source);
}

void QtMediaPlaybackBackend::play()
{
    m_player->play();
}

void QtMediaPlaybackBackend::pause()
{
    m_player->pause();
}

void QtMediaPlaybackBackend::stop()
{
    m_player->stop();
}

void QtMediaPlaybackBackend::setVolume(float value)
{
    m_audioOutput->setVolume(value);
}

float QtMediaPlaybackBackend::volume() const
{
    return m_audioOutput->volume();
}

void QtMediaPlaybackBackend::setPosition(qint64 positionMs)
{
    m_player->setPosition(positionMs);
}

bool QtMediaPlaybackBackend::isPlaying() const
{
    return m_player->playbackState() == QMediaPlayer::PlayingState;
}

qint64 QtMediaPlaybackBackend::position() const
{
    return m_player->position();
}

qint64 QtMediaPlaybackBackend::duration() const
{
    return m_player->duration();
}

QVector<AudioOutputInfo> QtMediaPlaybackBackend::outputDevices() const
{
    QVector<AudioOutputInfo> outputs;
    const auto devices = QMediaDevices::audioOutputs();
    const QAudioDevice defaultDevice = QMediaDevices::defaultAudioOutput();
    outputs.reserve(devices.size());

    for (const QAudioDevice& device : devices) {
        const QAudioFormat format = device.preferredFormat();
        outputs.push_back({
            deviceIdString(device),
            device.description(),
            format.sampleRate(),
            format.channelCount(),
            sampleFormatLabel(format.sampleFormat()),
            device.id() == defaultDevice.id(),
        });
    }

    return outputs;
}

bool QtMediaPlaybackBackend::setOutputDeviceById(const QString& deviceId)
{
    const auto devices = QMediaDevices::audioOutputs();
    for (const QAudioDevice& device : devices) {
        if (deviceIdString(device) == deviceId) {
            m_audioOutput->setDevice(device);
            emit outputDeviceChanged();
            return true;
        }
    }

    if (deviceId.isEmpty()) {
        m_audioOutput->setDevice(QMediaDevices::defaultAudioOutput());
        emit outputDeviceChanged();
        return true;
    }

    return false;
}

QString QtMediaPlaybackBackend::currentOutputDeviceId() const
{
    return deviceIdString(m_audioOutput->device());
}

QString QtMediaPlaybackBackend::currentOutputDeviceName() const
{
    const QString description = m_audioOutput->device().description();
    return description.isEmpty() ? QStringLiteral("Saida padrao") : description;
}

int QtMediaPlaybackBackend::currentOutputSampleRate() const
{
    return m_audioOutput->device().preferredFormat().sampleRate();
}

int QtMediaPlaybackBackend::currentOutputChannelCount() const
{
    return m_audioOutput->device().preferredFormat().channelCount();
}

QString QtMediaPlaybackBackend::currentOutputSampleFormat() const
{
    return sampleFormatLabel(m_audioOutput->device().preferredFormat().sampleFormat());
}

QString QtMediaPlaybackBackend::backendName() const
{
    return QStringLiteral("Qt Multimedia");
}

}  // namespace MyFin::Infrastructure::Playback
