#include "infrastructure/playback/GStreamerPlaybackBackend.h"

#include "infrastructure/logging/Logging.h"

#include <QMetaObject>
#include <QMutexLocker>
#include <QTimer>

#include <gst/gst.h>

#include <algorithm>
#include <mutex>

namespace MyFin::Infrastructure::Playback {

namespace {

constexpr auto kBusPollIntervalMs = 100;
constexpr auto kPositionPollIntervalMs = 250;
constexpr auto kDeviceRefreshIntervalMs = 3000;

QString fromGstString(const gchar* value)
{
    return value ? QString::fromUtf8(value) : QString();
}

bool boolFromStructure(const GstStructure* structure, const char* fieldName)
{
    if (structure == nullptr || !gst_structure_has_field(structure, fieldName)) {
        return false;
    }

    gboolean value = FALSE;
    return gst_structure_get_boolean(structure, fieldName, &value) && value;
}

QString stringFromStructure(const GstStructure* structure, const char* fieldName)
{
    if (structure == nullptr || !gst_structure_has_field(structure, fieldName)) {
        return {};
    }

    if (const gchar* value = gst_structure_get_string(structure, fieldName)) {
        return QString::fromUtf8(value);
    }

    return {};
}

int firstIntFromValue(const GValue* value)
{
    if (value == nullptr) {
        return 0;
    }

    if (G_VALUE_HOLDS_INT(value)) {
        return g_value_get_int(value);
    }

    if (GST_VALUE_HOLDS_INT_RANGE(value)) {
        return gst_value_get_int_range_min(value);
    }

    if (GST_VALUE_HOLDS_LIST(value) || GST_VALUE_HOLDS_ARRAY(value)) {
        const guint size = gst_value_list_get_size(value);
        for (guint index = 0; index < size; ++index) {
            if (const int nested = firstIntFromValue(gst_value_list_get_value(value, index)); nested > 0) {
                return nested;
            }
        }
    }

    return 0;
}

QString firstStringFromValue(const GValue* value)
{
    if (value == nullptr) {
        return {};
    }

    if (G_VALUE_HOLDS_STRING(value)) {
        return QString::fromUtf8(g_value_get_string(value));
    }

    if (GST_VALUE_HOLDS_LIST(value) || GST_VALUE_HOLDS_ARRAY(value)) {
        const guint size = gst_value_list_get_size(value);
        for (guint index = 0; index < size; ++index) {
            const QString nested = firstStringFromValue(gst_value_list_get_value(value, index));
            if (!nested.isEmpty()) {
                return nested;
            }
        }
    }

    return {};
}

int intFieldFromCaps(const GstCaps* caps, const char* fieldName)
{
    if (caps == nullptr || gst_caps_is_empty(caps)) {
        return 0;
    }

    const GstStructure* structure = gst_caps_get_structure(caps, 0);
    if (structure == nullptr) {
        return 0;
    }

    int value = 0;
    if (gst_structure_get_int(structure, fieldName, &value)) {
        return value;
    }

    return firstIntFromValue(gst_structure_get_value(structure, fieldName));
}

QString stringFieldFromCaps(const GstCaps* caps, const char* fieldName)
{
    if (caps == nullptr || gst_caps_is_empty(caps)) {
        return {};
    }

    const GstStructure* structure = gst_caps_get_structure(caps, 0);
    if (structure == nullptr) {
        return {};
    }

    if (const gchar* value = gst_structure_get_string(structure, fieldName)) {
        return QString::fromUtf8(value);
    }

    return firstStringFromValue(gst_structure_get_value(structure, fieldName));
}

QString deviceIdFor(GstDevice* device)
{
    if (device == nullptr) {
        return {};
    }

    GstStructure* properties = gst_device_get_properties(device);
    const QString nodeName = stringFromStructure(properties, "node.name");
    const QString objectPath = stringFromStructure(properties, "object.path");
    const QString targetObject = stringFromStructure(properties, "target-object");
    const QString objectSerial = stringFromStructure(properties, "object.serial");
    const QString objectId = stringFromStructure(properties, "object.id");
    const QString deviceId = stringFromStructure(properties, "device.id");

    if (properties != nullptr) {
        gst_structure_free(properties);
    }

    if (!nodeName.isEmpty()) return nodeName;
    if (!objectPath.isEmpty()) return objectPath;
    if (!targetObject.isEmpty()) return targetObject;
    if (!objectSerial.isEmpty()) return objectSerial;
    if (!objectId.isEmpty()) return objectId;
    if (!deviceId.isEmpty()) return deviceId;
    return fromGstString(gst_device_get_display_name(device));
}

AudioOutputInfo infoForDevice(GstDevice* device)
{
    AudioOutputInfo info;
    info.id = deviceIdFor(device);
    info.name = fromGstString(gst_device_get_display_name(device));
    info.isDefault = false;

    GstStructure* properties = gst_device_get_properties(device);
    if (properties != nullptr) {
        info.isDefault = boolFromStructure(properties, "is-default");

        if (info.name.isEmpty()) {
            const QString description = stringFromStructure(properties, "node.description");
            if (!description.isEmpty()) {
                info.name = description;
            }
        }

        gst_structure_free(properties);
    }

    GstCaps* caps = gst_device_get_caps(device);
    info.preferredSampleRate = intFieldFromCaps(caps, "rate");
    info.preferredChannelCount = intFieldFromCaps(caps, "channels");
    info.preferredSampleFormat = stringFieldFromCaps(caps, "format");
    if (caps != nullptr) {
        gst_caps_unref(caps);
    }

    if (info.name.isEmpty()) {
        info.name = QStringLiteral("Dispositivo de saida");
    }

    if (info.preferredSampleFormat.isEmpty()) {
        info.preferredSampleFormat = QStringLiteral("Desconhecido");
    }

    return info;
}

QString deviceSignature(const AudioOutputInfo& info)
{
    return QStringLiteral("%1|%2|%3|%4|%5|%6")
        .arg(info.id,
             info.name,
             QString::number(info.preferredSampleRate),
             QString::number(info.preferredChannelCount),
             info.preferredSampleFormat,
             info.isDefault ? QStringLiteral("1") : QStringLiteral("0"));
}

bool sameDeviceList(const QVector<AudioOutputInfo>& lhs, const QVector<AudioOutputInfo>& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (qsizetype index = 0; index < lhs.size(); ++index) {
        if (deviceSignature(lhs.at(index)) != deviceSignature(rhs.at(index))) {
            return false;
        }
    }

    return true;
}

}  // namespace

void GStreamerPlaybackBackend::ensureGStreamerInitialized()
{
    static std::once_flag once;
    std::call_once(once, [] {
        gst_init(nullptr, nullptr);
    });
}

void GStreamerPlaybackBackend::onAboutToFinish(GstElement* playbin, gpointer userData)
{
    Q_UNUSED(playbin)

    auto* self = static_cast<GStreamerPlaybackBackend*>(userData);
    if (self == nullptr) {
        return;
    }

    QString queuedUri;
    {
        QMutexLocker locker(&self->m_queueMutex);
        queuedUri = self->m_queuedSourceUri;
        self->m_queuedSourceUri.clear();
    }

    if (queuedUri.isEmpty() || self->m_playbin == nullptr) {
        return;
    }

    const QByteArray uriBytes = queuedUri.toUtf8();
    g_object_set(G_OBJECT(self->m_playbin), "uri", uriBytes.constData(), nullptr);

    QMetaObject::invokeMethod(self, [self] {
        self->m_lastPositionMs = 0;
        self->m_lastDurationMs = 0;
        emit self->positionChanged(0);
        emit self->queuedSourceActivated();
    }, Qt::QueuedConnection);
}

GStreamerPlaybackBackend::GStreamerPlaybackBackend(QObject* parent)
    : QObject(parent)
    , m_playbin(nullptr)
    , m_bus(nullptr)
    , m_deviceMonitor(nullptr)
    , m_busTimer(new QTimer(this))
    , m_positionTimer(new QTimer(this))
    , m_deviceRefreshTimer(new QTimer(this))
{
    ensureGStreamerInitialized();

    m_playbin = gst_element_factory_make("playbin3", "myfin-player");
    if (m_playbin == nullptr) {
        m_playbin = gst_element_factory_make("playbin", "myfin-player");
    }

    if (m_playbin == nullptr) {
        qCritical(lcPlayback) << "Failed to create GStreamer playbin";
        return;
    }

    g_signal_connect(m_playbin, "about-to-finish", G_CALLBACK(&GStreamerPlaybackBackend::onAboutToFinish), this);

    m_bus = gst_element_get_bus(m_playbin);
    m_deviceMonitor = gst_device_monitor_new();
    if (m_deviceMonitor != nullptr) {
        gst_device_monitor_add_filter(m_deviceMonitor, "Audio/Sink", nullptr);
        if (!gst_device_monitor_start(m_deviceMonitor)) {
            qWarning(lcPlayback) << "Failed to start GStreamer device monitor";
        }
    }

    m_busTimer->setInterval(kBusPollIntervalMs);
    connect(m_busTimer, &QTimer::timeout, this, &GStreamerPlaybackBackend::processBusMessages);
    m_busTimer->start();

    m_positionTimer->setInterval(kPositionPollIntervalMs);
    connect(m_positionTimer, &QTimer::timeout, this, [this] {
        emitPositionIfChanged();
        emitDurationIfChanged();
        updateNegotiatedFormat();
    });
    m_positionTimer->start();

    m_deviceRefreshTimer->setInterval(kDeviceRefreshIntervalMs);
    connect(m_deviceRefreshTimer, &QTimer::timeout, this, &GStreamerPlaybackBackend::refreshOutputDevices);
    m_deviceRefreshTimer->start();

    refreshOutputDevices();
    if (!m_outputDevices.isEmpty()) {
        applyOutputDevice(defaultDeviceId());
    }
}

GStreamerPlaybackBackend::~GStreamerPlaybackBackend()
{
    if (m_playbin != nullptr) {
        gst_element_set_state(m_playbin, GST_STATE_NULL);
    }

    for (const auto& entry : m_deviceEntries) {
        if (entry.device != nullptr) {
            gst_object_unref(entry.device);
        }
    }
    m_deviceEntries.clear();

    if (m_bus != nullptr) {
        gst_object_unref(m_bus);
        m_bus = nullptr;
    }

    if (m_deviceMonitor != nullptr) {
        gst_device_monitor_stop(m_deviceMonitor);
        gst_object_unref(m_deviceMonitor);
        m_deviceMonitor = nullptr;
    }

    if (m_playbin != nullptr) {
        gst_object_unref(m_playbin);
        m_playbin = nullptr;
    }
}

void GStreamerPlaybackBackend::setSource(const QUrl& source)
{
    if (m_playbin == nullptr) {
        return;
    }

    const QByteArray uri = source.toString(QUrl::FullyEncoded).toUtf8();
    g_object_set(G_OBJECT(m_playbin), "uri", uri.constData(), nullptr);
    m_hasSource = source.isValid() && !source.isEmpty();
    m_lastPositionMs = 0;
    emit positionChanged(0);
}

void GStreamerPlaybackBackend::play()
{
    if (m_playbin == nullptr) {
        return;
    }

    gst_element_set_state(m_playbin, GST_STATE_PLAYING);
}

void GStreamerPlaybackBackend::pause()
{
    if (m_playbin == nullptr) {
        return;
    }

    gst_element_set_state(m_playbin, GST_STATE_PAUSED);
}

void GStreamerPlaybackBackend::stop()
{
    if (m_playbin == nullptr) {
        return;
    }

    clearQueuedSource();
    gst_element_set_state(m_playbin, GST_STATE_NULL);
    m_hasSource = false;
    if (m_isPlaying) {
        m_isPlaying = false;
        emit playingChanged();
    }
}

void GStreamerPlaybackBackend::setVolume(float value)
{
    if (m_playbin == nullptr) {
        return;
    }

    g_object_set(G_OBJECT(m_playbin), "volume", qBound(0.0, static_cast<double>(value), 1.0), nullptr);
}

void GStreamerPlaybackBackend::setQueuedSource(const QUrl& source)
{
    QMutexLocker locker(&m_queueMutex);
    m_queuedSourceUri = source.isValid() && !source.isEmpty()
                            ? source.toString(QUrl::FullyEncoded)
                            : QString();
}

void GStreamerPlaybackBackend::clearQueuedSource()
{
    QMutexLocker locker(&m_queueMutex);
    m_queuedSourceUri.clear();
}

float GStreamerPlaybackBackend::volume() const
{
    if (m_playbin == nullptr) {
        return 0.0F;
    }

    gdouble value = 0.0;
    g_object_get(G_OBJECT(m_playbin), "volume", &value, nullptr);
    return static_cast<float>(value);
}

void GStreamerPlaybackBackend::setPosition(qint64 positionMs)
{
    if (m_playbin == nullptr) {
        return;
    }

    gst_element_seek_simple(m_playbin,
                            GST_FORMAT_TIME,
                            static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                            static_cast<gint64>(positionMs) * GST_MSECOND);
}

bool GStreamerPlaybackBackend::isPlaying() const
{
    return m_isPlaying;
}

qint64 GStreamerPlaybackBackend::position() const
{
    if (m_playbin == nullptr) {
        return 0;
    }

    gint64 value = 0;
    if (!gst_element_query_position(m_playbin, GST_FORMAT_TIME, &value) || value < 0) {
        return m_lastPositionMs;
    }

    return value / GST_MSECOND;
}

qint64 GStreamerPlaybackBackend::duration() const
{
    if (m_playbin == nullptr) {
        return 0;
    }

    gint64 value = 0;
    if (!gst_element_query_duration(m_playbin, GST_FORMAT_TIME, &value) || value <= 0) {
        return m_lastDurationMs;
    }

    return value / GST_MSECOND;
}

QVector<AudioOutputInfo> GStreamerPlaybackBackend::outputDevices() const
{
    return m_outputDevices;
}

bool GStreamerPlaybackBackend::setOutputDeviceById(const QString& deviceId)
{
    return applyOutputDevice(deviceId);
}

QString GStreamerPlaybackBackend::currentOutputDeviceId() const
{
    return m_currentOutputDeviceId;
}

QString GStreamerPlaybackBackend::currentOutputDeviceName() const
{
    return m_currentOutputDeviceName.isEmpty()
               ? QStringLiteral("Saida padrao do sistema")
               : m_currentOutputDeviceName;
}

int GStreamerPlaybackBackend::currentOutputSampleRate() const
{
    return m_currentOutputSampleRate;
}

int GStreamerPlaybackBackend::currentOutputChannelCount() const
{
    return m_currentOutputChannelCount;
}

QString GStreamerPlaybackBackend::currentOutputSampleFormat() const
{
    return m_currentOutputSampleFormat.isEmpty()
               ? QStringLiteral("Desconhecido")
               : m_currentOutputSampleFormat;
}

QString GStreamerPlaybackBackend::backendName() const
{
    return QStringLiteral("GStreamer");
}

void GStreamerPlaybackBackend::processBusMessages()
{
    if (m_bus == nullptr) {
        return;
    }

    while (GstMessage* message = gst_bus_pop(m_bus)) {
        switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_EOS:
            m_isPlaying = false;
            emit playingChanged();
            emit mediaFinished();
            break;

        case GST_MESSAGE_ERROR: {
            GError* error = nullptr;
            gchar* debug = nullptr;
            gst_message_parse_error(message, &error, &debug);
            const QString errorText = error != nullptr && error->message != nullptr
                                          ? QString::fromUtf8(error->message)
                                          : QStringLiteral("Playback error");
            qWarning(lcPlayback) << "GStreamer playback error:" << errorText;
            if (debug != nullptr) {
                qWarning(lcPlayback) << "GStreamer debug:" << debug;
            }
            gst_element_set_state(m_playbin, GST_STATE_READY);
            if (m_isPlaying) {
                m_isPlaying = false;
                emit playingChanged();
            }
            emit errorOccurred(errorText);
            if (error != nullptr) {
                g_error_free(error);
            }
            if (debug != nullptr) {
                g_free(debug);
            }
            break;
        }

        case GST_MESSAGE_STATE_CHANGED:
            if (GST_MESSAGE_SRC(message) == GST_OBJECT(m_playbin)) {
                GstState oldState = GST_STATE_NULL;
                GstState newState = GST_STATE_NULL;
                GstState pendingState = GST_STATE_NULL;
                gst_message_parse_state_changed(message, &oldState, &newState, &pendingState);
                Q_UNUSED(oldState)
                Q_UNUSED(pendingState)

                const bool nowPlaying = newState == GST_STATE_PLAYING;
                if (m_isPlaying != nowPlaying) {
                    m_isPlaying = nowPlaying;
                    emit playingChanged();
                }

                if (newState >= GST_STATE_PAUSED) {
                    emitDurationIfChanged();
                    updateNegotiatedFormat();
                }
            }
            break;

        case GST_MESSAGE_ASYNC_DONE:
        case GST_MESSAGE_STREAM_START:
        case GST_MESSAGE_DURATION_CHANGED:
            emitDurationIfChanged();
            updateNegotiatedFormat();
            break;

        default:
            break;
        }

        gst_message_unref(message);
    }
}

void GStreamerPlaybackBackend::refreshOutputDevices()
{
    QVector<AudioOutputInfo> newOutputs;
    QHash<QString, DeviceEntry> newEntries;

    if (m_deviceMonitor != nullptr) {
        GList* devices = gst_device_monitor_get_devices(m_deviceMonitor);
        for (GList* item = devices; item != nullptr; item = item->next) {
            GstDevice* device = GST_DEVICE(item->data);
            if (device == nullptr) {
                continue;
            }

            DeviceEntry entry;
            entry.info = infoForDevice(device);
            if (entry.info.id.isEmpty()) {
                continue;
            }

            entry.device = GST_DEVICE(gst_object_ref(device));
            newOutputs.push_back(entry.info);
            newEntries.insert(entry.info.id, entry);
        }

        g_list_free_full(devices, reinterpret_cast<GDestroyNotify>(gst_object_unref));
    }

    std::sort(newOutputs.begin(), newOutputs.end(), [](const AudioOutputInfo& lhs, const AudioOutputInfo& rhs) {
        if (lhs.isDefault != rhs.isDefault) {
            return lhs.isDefault;
        }
        return lhs.name.localeAwareCompare(rhs.name) < 0;
    });

    const bool deviceListChanged = !sameDeviceList(m_outputDevices, newOutputs);

    const auto oldEntries = m_deviceEntries;
    m_outputDevices = newOutputs;
    m_deviceEntries = newEntries;

    for (const auto& entry : oldEntries) {
        if (entry.device != nullptr) {
            gst_object_unref(entry.device);
        }
    }

    if (m_outputDevices.isEmpty()) {
        m_currentOutputDeviceId.clear();
        m_currentOutputDeviceName = QStringLiteral("Saida padrao do sistema");
        m_currentOutputSampleRate = 0;
        m_currentOutputChannelCount = 0;
        m_currentOutputSampleFormat = QStringLiteral("Desconhecido");
    } else if (!m_currentOutputDeviceId.isEmpty() && !m_deviceEntries.contains(m_currentOutputDeviceId)) {
        applyOutputDevice(defaultDeviceId());
    } else if (m_currentOutputDeviceId.isEmpty()) {
        applyOutputDevice(defaultDeviceId());
    }

    if (deviceListChanged) {
        emit outputDevicesChanged();
    }
}

bool GStreamerPlaybackBackend::applyOutputDevice(const QString& deviceId)
{
    if (m_playbin == nullptr) {
        return false;
    }

    QString resolvedId = deviceId;
    if (resolvedId.isEmpty()) {
        resolvedId = defaultDeviceId();
    }

    GstElement* sink = nullptr;
    AudioOutputInfo selectedInfo;

    if (!resolvedId.isEmpty()) {
        const auto it = m_deviceEntries.constFind(resolvedId);
        if (it == m_deviceEntries.constEnd() || it->device == nullptr) {
            return false;
        }

        sink = gst_device_create_element(it->device, nullptr);
        if (sink == nullptr) {
            qWarning(lcPlayback) << "Failed to create sink for output device" << resolvedId;
            return false;
        }

        selectedInfo = it->info;
    } else {
        sink = gst_element_factory_make("autoaudiosink", nullptr);
        selectedInfo.id.clear();
        selectedInfo.name = QStringLiteral("Saida padrao do sistema");
        selectedInfo.preferredSampleRate = 0;
        selectedInfo.preferredChannelCount = 0;
        selectedInfo.preferredSampleFormat = QStringLiteral("Desconhecido");
        selectedInfo.isDefault = true;
    }

    const qint64 resumePositionMs = position();
    const bool hadPlaybackState = m_hasSource;
    const bool wasPlaying = m_isPlaying;

    gst_object_ref_sink(GST_OBJECT(sink));
    gst_element_set_state(m_playbin, GST_STATE_READY);
    g_object_set(G_OBJECT(m_playbin), "audio-sink", sink, nullptr);
    gst_object_unref(sink);

    setCurrentDeviceFromInfo(selectedInfo);

    if (hadPlaybackState) {
        gst_element_set_state(m_playbin, GST_STATE_PAUSED);
        if (resumePositionMs > 0) {
            gst_element_seek_simple(m_playbin,
                                    GST_FORMAT_TIME,
                                    static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                                    static_cast<gint64>(resumePositionMs) * GST_MSECOND);
        }
        gst_element_set_state(m_playbin, wasPlaying ? GST_STATE_PLAYING : GST_STATE_PAUSED);
    } else {
        gst_element_set_state(m_playbin, GST_STATE_NULL);
    }

    emit outputDeviceChanged();
    return true;
}

QString GStreamerPlaybackBackend::defaultDeviceId() const
{
    for (const auto& output : m_outputDevices) {
        if (output.isDefault) {
            return output.id;
        }
    }

    return m_outputDevices.isEmpty() ? QString() : m_outputDevices.front().id;
}

void GStreamerPlaybackBackend::setCurrentDeviceFromInfo(const AudioOutputInfo& info)
{
    m_currentOutputDeviceId = info.id;
    m_currentOutputDeviceName = info.name;
    m_currentOutputSampleRate = info.preferredSampleRate;
    m_currentOutputChannelCount = info.preferredChannelCount;
    m_currentOutputSampleFormat = info.preferredSampleFormat.isEmpty()
                                      ? QStringLiteral("Desconhecido")
                                      : info.preferredSampleFormat;
}

void GStreamerPlaybackBackend::updateNegotiatedFormat()
{
    if (m_playbin == nullptr) {
        return;
    }

    GstElement* sink = nullptr;
    g_object_get(G_OBJECT(m_playbin), "audio-sink", &sink, nullptr);
    if (sink == nullptr) {
        return;
    }

    GstPad* pad = gst_element_get_static_pad(sink, "sink");
    if (pad == nullptr) {
        gst_object_unref(sink);
        return;
    }

    GstCaps* caps = gst_pad_get_current_caps(pad);
    if (caps == nullptr) {
        caps = gst_pad_query_caps(pad, nullptr);
    }

    const int sampleRate = intFieldFromCaps(caps, "rate");
    const int channelCount = intFieldFromCaps(caps, "channels");
    QString sampleFormat = stringFieldFromCaps(caps, "format");
    if (sampleFormat.isEmpty()) {
        sampleFormat = QStringLiteral("Desconhecido");
    }

    if (caps != nullptr) {
        gst_caps_unref(caps);
    }
    gst_object_unref(pad);
    gst_object_unref(sink);

    const bool changed = sampleRate > 0
                         && (sampleRate != m_currentOutputSampleRate
                             || channelCount != m_currentOutputChannelCount
                             || sampleFormat != m_currentOutputSampleFormat);

    if (sampleRate > 0) {
        m_currentOutputSampleRate = sampleRate;
    }
    if (channelCount > 0) {
        m_currentOutputChannelCount = channelCount;
    }
    m_currentOutputSampleFormat = sampleFormat;

    if (changed) {
        emit outputDeviceChanged();
    }
}

void GStreamerPlaybackBackend::emitPositionIfChanged()
{
    const qint64 currentPosition = position();
    if (currentPosition != m_lastPositionMs) {
        m_lastPositionMs = currentPosition;
        emit positionChanged(currentPosition);
    }
}

void GStreamerPlaybackBackend::emitDurationIfChanged()
{
    const qint64 currentDuration = duration();
    if (currentDuration > 0 && currentDuration != m_lastDurationMs) {
        m_lastDurationMs = currentDuration;
        emit durationChanged(currentDuration);
    }
}

}  // namespace MyFin::Infrastructure::Playback
