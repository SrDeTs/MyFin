#pragma once

#include <QString>
#include <QUrl>

namespace MyFin::Domain {

struct ServerProfile {
    QString name;
    QUrl baseUrl;
    QString username;
    QString userId;
};

}  // namespace MyFin::Domain
