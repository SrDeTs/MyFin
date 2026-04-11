#pragma once

#include <QString>

class QLockFile;

namespace MyFin::Infrastructure::Settings {
class AppPaths;
}

namespace MyFin::Linux {

class SingleInstanceService final {
public:
    explicit SingleInstanceService(const Infrastructure::Settings::AppPaths& paths);
    ~SingleInstanceService();

    bool tryAcquire();
    QString errorString() const;

private:
    QLockFile* m_lockFile = nullptr;
};

}  // namespace MyFin::Linux
