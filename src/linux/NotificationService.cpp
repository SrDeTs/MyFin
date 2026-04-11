#include "linux/NotificationService.h"

#include "infrastructure/logging/Logging.h"

namespace MyFin::Linux {

NotificationService::NotificationService(QObject* parent)
    : QObject(parent)
{
}

void NotificationService::bindPlaybackController(Infrastructure::Playback::PlaybackController* controller)
{
    m_controller = controller;
}

void NotificationService::start()
{
    qInfo(lcApp) << "Desktop notification service placeholder initialized";
}

}  // namespace MyFin::Linux
