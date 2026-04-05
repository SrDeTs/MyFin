#include "tracklistmodel.h"

TrackListModel::TrackListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int TrackListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_tracks.size();
}

QVariant TrackListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_tracks.size()) {
        return {};
    }

    const Track &track = m_tracks.at(index.row());
    switch (role) {
    case IdRole:
        return track.id;
    case TitleRole:
        return track.title;
    case ArtistRole:
        return track.artist;
    case AlbumRole:
        return track.album;
    case DurationRole:
        return track.durationMs;
    case SourceUrlRole:
        return track.sourceUrl;
    case ArtUrlRole:
        return track.artUrl;
    case RemoteRole:
        return track.remote;
    case SourceNameRole:
        return track.sourceName;
    default:
        return {};
    }
}

QHash<int, QByteArray> TrackListModel::roleNames() const
{
    return {
        {IdRole, "trackId"},
        {TitleRole, "title"},
        {ArtistRole, "artist"},
        {AlbumRole, "album"},
        {DurationRole, "durationMs"},
        {SourceUrlRole, "sourceUrl"},
        {ArtUrlRole, "artUrl"},
        {RemoteRole, "remote"},
        {SourceNameRole, "sourceName"}
    };
}

const QVector<Track> &TrackListModel::tracks() const
{
    return m_tracks;
}

Track TrackListModel::trackAt(int index) const
{
    if (index < 0 || index >= m_tracks.size()) {
        return {};
    }

    return m_tracks.at(index);
}

void TrackListModel::setTracks(QVector<Track> tracks)
{
    beginResetModel();
    m_tracks = std::move(tracks);
    endResetModel();
    emit countChanged();
}

void TrackListModel::appendTrack(const Track &track)
{
    const int insertRow = m_tracks.size();
    beginInsertRows(QModelIndex(), insertRow, insertRow);
    m_tracks.append(track);
    endInsertRows();
    emit countChanged();
}

void TrackListModel::removeTrack(int index)
{
    if (index < 0 || index >= m_tracks.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_tracks.removeAt(index);
    endRemoveRows();
    emit countChanged();
}

void TrackListModel::clear()
{
    if (m_tracks.isEmpty()) {
        return;
    }

    beginResetModel();
    m_tracks.clear();
    endResetModel();
    emit countChanged();
}

QVariantMap TrackListModel::get(int index) const
{
    return trackAt(index).toVariantMap();
}
