#pragma once

#include "../core/track.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>

class JellyfinClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

public:
    explicit JellyfinClient(QObject *parent = nullptr);

    bool isConnected() const;
    bool isBusy() const;
    QString errorString() const;

    Q_INVOKABLE void connectToServer(const QString &serverUrl,
                                     const QString &username,
                                     const QString &password);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE void refreshLibrary();

signals:
    void connectedChanged();
    void busyChanged();
    void errorStringChanged();
    void libraryReceived(const QVector<Track> &tracks);
    void authenticationSucceeded();

private:
    QNetworkRequest makeJsonRequest(const QUrl &url) const;
    QNetworkRequest makeAuthenticatedRequest(const QUrl &url) const;
    QUrl normalizedServerUrl(const QString &value) const;
    void setBusy(bool value);
    void setErrorString(const QString &value);
    void fetchMusicLibrary();

    QNetworkAccessManager m_networkAccessManager;
    QUrl m_serverUrl;
    QString m_username;
    QString m_password;
    QString m_accessToken;
    QString m_userId;
    QString m_deviceId;
    bool m_connected = false;
    bool m_busy = false;
    QString m_errorString;
};
