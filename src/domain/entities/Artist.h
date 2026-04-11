#pragma once

#include <QString>

namespace MyFin::Domain {

struct Artist {
    QString id;
    QString name;
    QString coverKey;
    int albumCount = 0;
};

}  // namespace MyFin::Domain
