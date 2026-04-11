#include "infrastructure/settings/AppPaths.h"

#include <QDir>
#include <QStandardPaths>

namespace MyFin::Infrastructure::Settings {

namespace {

QString ensureFallback(const QString& path, const QString& fallback)
{
    return path.isEmpty() ? fallback : path;
}

}  // namespace

AppPaths::AppPaths()
    : m_configDir(ensureFallback(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation),
                                 QDir::homePath() + QStringLiteral("/.config/MyFin")))
    , m_cacheDir(ensureFallback(QStandardPaths::writableLocation(QStandardPaths::CacheLocation),
                                QDir::homePath() + QStringLiteral("/.cache/MyFin")))
    , m_dataDir(ensureFallback(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation),
                               QDir::homePath() + QStringLiteral("/.local/share/MyFin")))
    , m_stateDir(ensureFallback(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation),
                                QDir::homePath() + QStringLiteral("/.local/state/MyFin")))
    , m_runtimeDir(ensureFallback(QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation),
                                  QDir::tempPath() + QStringLiteral("/myfin-runtime")))
{
    ensureDirectories();
}

void AppPaths::ensureDirectories() const
{
    QDir().mkpath(m_configDir);
    QDir().mkpath(m_cacheDir);
    QDir().mkpath(m_dataDir);
    QDir().mkpath(m_stateDir);
    QDir().mkpath(m_runtimeDir);
    QDir().mkpath(m_stateDir + QStringLiteral("/logs"));
}

QString AppPaths::configDir() const
{
    return m_configDir;
}

QString AppPaths::cacheDir() const
{
    return m_cacheDir;
}

QString AppPaths::dataDir() const
{
    return m_dataDir;
}

QString AppPaths::stateDir() const
{
    return m_stateDir;
}

QString AppPaths::runtimeDir() const
{
    return m_runtimeDir;
}

QString AppPaths::logFilePath() const
{
    return m_stateDir + QStringLiteral("/logs/myfin.log");
}

QString AppPaths::settingsFilePath() const
{
    return m_configDir + QStringLiteral("/settings.ini");
}

QString AppPaths::libraryCacheFilePath() const
{
    return m_dataDir + QStringLiteral("/library/library_cache.sqlite3");
}

QString AppPaths::coverCacheDir() const
{
    return m_cacheDir + QStringLiteral("/covers");
}

}  // namespace MyFin::Infrastructure::Settings
