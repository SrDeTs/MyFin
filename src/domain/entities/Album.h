#pragma once

#include <QString>

namespace MyFin::Domain {

struct Album {
    QString id;
    QString title;
    QString artist;
    QString coverKey;
    int year = 0;
    int trackCount = 0;
};

}  // namespace MyFin::Domain
