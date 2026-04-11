#pragma once

#include <QUrl>

namespace MyFin::Infrastructure::Playback {

class IPlaybackBackend {
public:
    virtual ~IPlaybackBackend() = default;

    virtual void setSource(const QUrl& source) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
};

}  // namespace MyFin::Infrastructure::Playback
