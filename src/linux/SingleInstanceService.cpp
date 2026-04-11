#include "linux/SingleInstanceService.h"

#include "infrastructure/settings/AppPaths.h"

#include <QLockFile>

namespace MyFin::Linux {

SingleInstanceService::SingleInstanceService(const Infrastructure::Settings::AppPaths& paths)
    : m_lockFile(new QLockFile(paths.runtimeDir() + QStringLiteral("/myfin.lock")))
{
    m_lockFile->setStaleLockTime(30000);
}

SingleInstanceService::~SingleInstanceService()
{
    delete m_lockFile;
}

bool SingleInstanceService::tryAcquire()
{
    if (m_lockFile->tryLock()) {
        return true;
    }

    if (m_lockFile->removeStaleLockFile() && m_lockFile->tryLock()) {
        return true;
    }

    return false;
}

QString SingleInstanceService::errorString() const
{
    switch (m_lockFile->error()) {
    case QLockFile::NoError:
        return {};
    case QLockFile::LockFailedError:
        return QStringLiteral("Another MyFin instance is already running.");
    case QLockFile::PermissionError:
        return QStringLiteral("MyFin could not create its runtime lock file due to a permission error.");
    case QLockFile::UnknownError:
        return QStringLiteral("MyFin failed to acquire the runtime lock for an unknown reason.");
    }

    return QStringLiteral("MyFin failed to acquire the runtime lock.");
}

}  // namespace MyFin::Linux
