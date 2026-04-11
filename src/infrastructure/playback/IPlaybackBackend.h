#pragma once

#include <QString>
#include <QUrl>

namespace MyFin::Infrastructure::Playback {

struct AudioOutputInfo {
    QString id;
    QString name;
    int preferredSampleRate = 0;
    int preferredChannelCount = 0;
    QString preferredSampleFormat;
    bool isDefault = false;
};

class IPlaybackBackend {
public:
    virtual ~IPlaybackBackend() = default;

    virtual void setSource(const QUrl& source) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
};

}  // namespace MyFin::Infrastructure::Playback
