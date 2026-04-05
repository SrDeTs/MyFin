#include "settingsmanager.h"

#include <algorithm>
#include <QDir>
#include <QStandardPaths>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_settings(QStringLiteral("MyFin"), QStringLiteral("MyFinPlayer"))
{
    load();
}

QString SettingsManager::jellyfinUrl() const { return m_jellyfinUrl; }
QString SettingsManager::jellyfinUsername() const { return m_jellyfinUsername; }
QString SettingsManager::jellyfinPassword() const { return m_jellyfinPassword; }
QStringList SettingsManager::musicDirectories() const { return m_musicDirectories; }
int SettingsManager::defaultVolume() const { return m_defaultVolume; }
bool SettingsManager::animationsEnabled() const { return m_animationsEnabled; }
QString SettingsManager::startupBehavior() const { return m_startupBehavior; }
QString SettingsManager::queueBehavior() const { return m_queueBehavior; }
QString SettingsManager::visualTheme() const { return m_visualTheme; }

void SettingsManager::setJellyfinUrl(const QString &value)
{
    if (m_jellyfinUrl == value) {
        return;
    }
    m_jellyfinUrl = value.trimmed();
    emit jellyfinChanged();
}

void SettingsManager::setJellyfinUsername(const QString &value)
{
    if (m_jellyfinUsername == value) {
        return;
    }
    m_jellyfinUsername = value.trimmed();
    emit jellyfinChanged();
}

void SettingsManager::setJellyfinPassword(const QString &value)
{
    if (m_jellyfinPassword == value) {
        return;
    }
    m_jellyfinPassword = value;
    emit jellyfinChanged();
}

void SettingsManager::setMusicDirectories(const QStringList &value)
{
    if (m_musicDirectories == value) {
        return;
    }
    m_musicDirectories = value;
    emit musicDirectoriesChanged();
}

void SettingsManager::setDefaultVolume(int value)
{
    value = std::clamp(value, 0, 100);
    if (m_defaultVolume == value) {
        return;
    }
    m_defaultVolume = value;
    emit generalSettingsChanged();
}

void SettingsManager::setAnimationsEnabled(bool value)
{
    if (m_animationsEnabled == value) {
        return;
    }
    m_animationsEnabled = value;
    emit generalSettingsChanged();
}

void SettingsManager::setStartupBehavior(const QString &value)
{
    if (m_startupBehavior == value) {
        return;
    }
    m_startupBehavior = value;
    emit generalSettingsChanged();
}

void SettingsManager::setQueueBehavior(const QString &value)
{
    if (m_queueBehavior == value) {
        return;
    }
    m_queueBehavior = value;
    emit generalSettingsChanged();
}

void SettingsManager::setVisualTheme(const QString &value)
{
    if (m_visualTheme == value) {
        return;
    }
    m_visualTheme = value;
    emit generalSettingsChanged();
}

void SettingsManager::save()
{
    m_settings.setValue(QStringLiteral("jellyfin/url"), m_jellyfinUrl);
    m_settings.setValue(QStringLiteral("jellyfin/username"), m_jellyfinUsername);
    m_settings.setValue(QStringLiteral("jellyfin/password"), m_jellyfinPassword);
    m_settings.setValue(QStringLiteral("library/directories"), m_musicDirectories);
    m_settings.setValue(QStringLiteral("player/defaultVolume"), m_defaultVolume);
    m_settings.setValue(QStringLiteral("ui/animationsEnabled"), m_animationsEnabled);
    m_settings.setValue(QStringLiteral("ui/startupBehavior"), m_startupBehavior);
    m_settings.setValue(QStringLiteral("player/queueBehavior"), m_queueBehavior);
    m_settings.setValue(QStringLiteral("ui/visualTheme"), m_visualTheme);
    m_settings.sync();
}

void SettingsManager::restoreDefaults()
{
    const QString musicLocation = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    m_jellyfinUrl.clear();
    m_jellyfinUsername.clear();
    m_jellyfinPassword.clear();
    m_musicDirectories = musicLocation.isEmpty() ? QStringList{} : QStringList{musicLocation};
    m_defaultVolume = 75;
    m_animationsEnabled = true;
    m_startupBehavior = QStringLiteral("home");
    m_queueBehavior = QStringLiteral("append");
    m_visualTheme = QStringLiteral("dynamic");

    emit jellyfinChanged();
    emit musicDirectoriesChanged();
    emit generalSettingsChanged();
    save();
}

QString SettingsManager::musicDirectoriesDisplay() const
{
    return m_musicDirectories.join(QStringLiteral("; "));
}

void SettingsManager::setMusicDirectoriesFromString(const QString &value)
{
    QStringList directories;
    for (const QString &entry : value.split(';', Qt::SkipEmptyParts)) {
        directories.append(entry.trimmed());
    }
    setMusicDirectories(directories);
}

void SettingsManager::load()
{
    const QString musicLocation = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    m_jellyfinUrl = m_settings.value(QStringLiteral("jellyfin/url")).toString();
    m_jellyfinUsername = m_settings.value(QStringLiteral("jellyfin/username")).toString();
    m_jellyfinPassword = m_settings.value(QStringLiteral("jellyfin/password")).toString();
    m_musicDirectories = m_settings.value(QStringLiteral("library/directories"),
                                          musicLocation.isEmpty() ? QStringList{} : QStringList{musicLocation}).toStringList();
    m_defaultVolume = m_settings.value(QStringLiteral("player/defaultVolume"), 75).toInt();
    m_animationsEnabled = m_settings.value(QStringLiteral("ui/animationsEnabled"), true).toBool();
    m_startupBehavior = m_settings.value(QStringLiteral("ui/startupBehavior"), QStringLiteral("home")).toString();
    m_queueBehavior = m_settings.value(QStringLiteral("player/queueBehavior"), QStringLiteral("append")).toString();
    m_visualTheme = m_settings.value(QStringLiteral("ui/visualTheme"), QStringLiteral("dynamic")).toString();
}
