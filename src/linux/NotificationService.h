#pragma once

#include <QObject>

namespace MyFin::Infrastructure::Playback {
class PlaybackController;
}

namespace MyFin::Linux {

class NotificationService final : public QObject {
    Q_OBJECT

public:
    explicit NotificationService(QObject* parent = nullptr);

    void bindPlaybackController(Infrastructure::Playback::PlaybackController* controller);
    void start();

private:
    Infrastructure::Playback::PlaybackController* m_controller = nullptr;
};

}  // namespace MyFin::Linux
