#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>

namespace MyFin::Presentation::Models {

struct TrackRow {
    QString id;
    QString title;
    QString artist;
    QString album;
    QString durationText;
    QString artworkKey;
    QString coverSource;
    QString signalPath;
    qint64 durationMs = 0;
    bool favorite = false;
};

class TrackListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistRole,
        AlbumRole,
        DurationTextRole,
        DurationMsRole,
        ArtworkKeyRole,
        CoverSourceRole,
        SignalPathRole,
        FavoriteRole
    };

    explicit TrackListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void replaceRows(QVector<TrackRow>&& rows);
    bool trackForId(const QString& id, TrackRow* out) const;

private:
    QVector<TrackRow> m_rows;
};

}  // namespace MyFin::Presentation::Models
