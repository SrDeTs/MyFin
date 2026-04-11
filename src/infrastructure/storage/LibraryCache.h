#pragma once

#include "domain/entities/Track.h"

#include <QVector>

namespace MyFin::Infrastructure::Settings {
class AppPaths;
}

namespace MyFin::Infrastructure::Storage {

class LibraryCache final {
public:
    explicit LibraryCache(const Settings::AppPaths& paths);

    QVector<Domain::Track> loadHomeTracks() const;
    bool saveHomeTracks(const QVector<Domain::Track>& tracks) const;

private:
    QString m_databasePath;
    QString m_legacyJsonPath;
};

}  // namespace MyFin::Infrastructure::Storage
