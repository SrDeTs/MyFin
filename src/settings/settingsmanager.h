#pragma once

#include <QObject>
#include <QSettings>
#include <QStringList>

class SettingsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString jellyfinUrl READ jellyfinUrl WRITE setJellyfinUrl NOTIFY jellyfinChanged)
    Q_PROPERTY(QString jellyfinUsername READ jellyfinUsername WRITE setJellyfinUsername NOTIFY jellyfinChanged)
    Q_PROPERTY(QString jellyfinPassword READ jellyfinPassword WRITE setJellyfinPassword NOTIFY jellyfinChanged)
    Q_PROPERTY(QStringList musicDirectories READ musicDirectories WRITE setMusicDirectories NOTIFY musicDirectoriesChanged)
    Q_PROPERTY(int defaultVolume READ defaultVolume WRITE setDefaultVolume NOTIFY generalSettingsChanged)
    Q_PROPERTY(bool animationsEnabled READ animationsEnabled WRITE setAnimationsEnabled NOTIFY generalSettingsChanged)
    Q_PROPERTY(QString startupBehavior READ startupBehavior WRITE setStartupBehavior NOTIFY generalSettingsChanged)
    Q_PROPERTY(QString queueBehavior READ queueBehavior WRITE setQueueBehavior NOTIFY generalSettingsChanged)
    Q_PROPERTY(QString visualTheme READ visualTheme WRITE setVisualTheme NOTIFY generalSettingsChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);

    QString jellyfinUrl() const;
    QString jellyfinUsername() const;
    QString jellyfinPassword() const;
    QStringList musicDirectories() const;
    int defaultVolume() const;
    bool animationsEnabled() const;
    QString startupBehavior() const;
    QString queueBehavior() const;
    QString visualTheme() const;

    void setJellyfinUrl(const QString &value);
    void setJellyfinUsername(const QString &value);
    void setJellyfinPassword(const QString &value);
    void setMusicDirectories(const QStringList &value);
    void setDefaultVolume(int value);
    void setAnimationsEnabled(bool value);
    void setStartupBehavior(const QString &value);
    void setQueueBehavior(const QString &value);
    void setVisualTheme(const QString &value);

    Q_INVOKABLE void save();
    Q_INVOKABLE void restoreDefaults();
    Q_INVOKABLE QString musicDirectoriesDisplay() const;
    Q_INVOKABLE void setMusicDirectoriesFromString(const QString &value);

signals:
    void jellyfinChanged();
    void musicDirectoriesChanged();
    void generalSettingsChanged();

private:
    void load();

    QSettings m_settings;
    QString m_jellyfinUrl;
    QString m_jellyfinUsername;
    QString m_jellyfinPassword;
    QStringList m_musicDirectories;
    int m_defaultVolume = 75;
    bool m_animationsEnabled = true;
    QString m_startupBehavior = QStringLiteral("home");
    QString m_queueBehavior = QStringLiteral("append");
    QString m_visualTheme = QStringLiteral("dynamic");
};
