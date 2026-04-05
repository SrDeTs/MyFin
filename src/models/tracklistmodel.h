#pragma once

#include "../core/track.h"

#include <QAbstractListModel>
#include <QVector>

class TrackListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistRole,
        AlbumRole,
        DurationRole,
        SourceUrlRole,
        ArtUrlRole,
        RemoteRole,
        SourceNameRole
    };
    Q_ENUM(Roles)

    explicit TrackListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    const QVector<Track> &tracks() const;
    Track trackAt(int index) const;

    void setTracks(QVector<Track> tracks);
    void appendTrack(const Track &track);
    void removeTrack(int index);
    void clear();

    Q_INVOKABLE QVariantMap get(int index) const;

signals:
    void countChanged();

private:
    QVector<Track> m_tracks;
};
