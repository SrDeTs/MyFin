#pragma once

#include <QString>

namespace MyFin::Domain {

struct Track {
    QString id;
    QString title;
    QString artist;
    QString album;
    QString artworkKey;
    QString streamProfile;
    qint64 durationMs = 0;
    bool favorite = false;
};

}  // namespace MyFin::Domain
