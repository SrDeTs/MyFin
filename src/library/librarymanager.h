#pragma once

#include "../models/tracklistmodel.h"

#include <QObject>
#include <QFileInfo>
#include <QStringList>

class LibraryManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TrackListModel* tracksModel READ tracksModel CONSTANT)
    Q_PROPERTY(bool scanning READ isScanning NOTIFY scanningChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

public:
    explicit LibraryManager(QObject *parent = nullptr);

    TrackListModel *tracksModel();
    bool isScanning() const;
    QString statusMessage() const;

    Q_INVOKABLE void scanLocalDirectories(const QStringList &directories);
    Q_INVOKABLE void importJellyfinTracks(const QVector<Track> &tracks);
    Q_INVOKABLE void clearRemoteTracks();

signals:
    void scanningChanged();
    void statusMessageChanged();

private:
    Track buildLocalTrack(const QString &filePath) const;
    QUrl findCoverForFile(const QFileInfo &info) const;
    void setScanning(bool value);
    void setStatusMessage(const QString &value);

    TrackListModel m_tracksModel;
    bool m_scanning = false;
    QString statusMessageValue;
};
