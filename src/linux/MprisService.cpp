#include "linux/MprisService.h"

#include "infrastructure/logging/Logging.h"

namespace MyFin::Linux {

MprisService::MprisService(QObject* parent)
    : QObject(parent)
{
}

void MprisService::bindPlaybackController(Infrastructure::Playback::PlaybackController* controller)
{
    m_controller = controller;
}

void MprisService::start()
{
    qInfo(lcApp) << "MPRIS service placeholder initialized";
}

}  // namespace MyFin::Linux
