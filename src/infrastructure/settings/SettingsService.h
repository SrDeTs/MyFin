#pragma once

#include <QString>
#include <QSize>
#include <QUrl>

class QSettings;

namespace MyFin::Infrastructure::Settings {

class AppPaths;

class SettingsService final {
public:
    explicit SettingsService(const AppPaths& paths);
    ~SettingsService();

    QString serverName() const;
    void setServerName(const QString& value);

    QUrl serverUrl() const;
    void setServerUrl(const QUrl& value);

    QString username() const;
    void setUsername(const QString& value);

    QString userId() const;
    void setUserId(const QString& value);

    QString deviceId() const;
    void setDeviceId(const QString& value);

    float outputVolume() const;
    void setOutputVolume(float value);

    QSize windowSize() const;
    void setWindowSize(const QSize& value);

private:
    QSettings& settings();
    const QSettings& settings() const;

    QString key(const QString& suffix) const;

    QSettings* m_settings = nullptr;
};

}  // namespace MyFin::Infrastructure::Settings
