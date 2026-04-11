#pragma once

#include <QString>

namespace MyFin::Domain {

struct Playlist {
    QString id;
    QString name;
    QString coverKey;
    int trackCount = 0;
    bool favorite = false;
};

}  // namespace MyFin::Domain
