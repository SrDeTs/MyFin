#pragma once

#include <QString>

namespace MyFin::Infrastructure::Settings {

class AppPaths final {
public:
    AppPaths();

    void ensureDirectories() const;

    QString configDir() const;
    QString cacheDir() const;
    QString dataDir() const;
    QString stateDir() const;
    QString runtimeDir() const;
    QString logFilePath() const;
    QString settingsFilePath() const;
    QString libraryCacheFilePath() const;
    QString coverCacheDir() const;

private:
    QString m_configDir;
    QString m_cacheDir;
    QString m_dataDir;
    QString m_stateDir;
    QString m_runtimeDir;
};

}  // namespace MyFin::Infrastructure::Settings
