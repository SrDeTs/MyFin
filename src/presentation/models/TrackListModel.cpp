#include "presentation/models/TrackListModel.h"

namespace MyFin::Presentation::Models {

TrackListModel::TrackListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int TrackListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_rows.size();
}

QVariant TrackListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_rows.size()) {
        return {};
    }

    const TrackRow& row = m_rows.at(index.row());
    switch (role) {
    case IdRole:
        return row.id;
    case TitleRole:
        return row.title;
    case ArtistRole:
        return row.artist;
    case AlbumRole:
        return row.album;
    case DurationTextRole:
        return row.durationText;
    case DurationMsRole:
        return row.durationMs;
    case ArtworkKeyRole:
        return row.artworkKey;
    case CoverSourceRole:
        return row.coverSource;
    case SignalPathRole:
        return row.signalPath;
    case FavoriteRole:
        return row.favorite;
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
        {DurationTextRole, "durationText"},
        {DurationMsRole, "durationMs"},
        {ArtworkKeyRole, "artworkKey"},
        {CoverSourceRole, "coverSource"},
        {SignalPathRole, "signalPath"},
        {FavoriteRole, "favorite"},
    };
}

void TrackListModel::replaceRows(QVector<TrackRow>&& rows)
{
    beginResetModel();
    m_rows = std::move(rows);
    endResetModel();
}

bool TrackListModel::trackForId(const QString& id, TrackRow* out) const
{
    for (const TrackRow& row : m_rows) {
        if (row.id == id) {
            if (out) {
                *out = row;
            }
            return true;
        }
    }

    return false;
}

}  // namespace MyFin::Presentation::Models
