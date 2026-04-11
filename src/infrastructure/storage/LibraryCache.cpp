#include "infrastructure/storage/LibraryCache.h"

#include "infrastructure/logging/Logging.h"
#include "infrastructure/settings/AppPaths.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <atomic>

namespace MyFin::Infrastructure::Storage {

namespace {

constexpr auto kGenerationMetaKey = "home_tracks_generation";

std::atomic_uint s_connectionCounter = 0;

QString makeConnectionName()
{
    return QStringLiteral("LibraryCache-%1").arg(s_connectionCounter.fetch_add(1, std::memory_order_relaxed));
}

Domain::Track deserializeTrack(const QJsonObject& object)
{
    Domain::Track track;
    track.id = object.value(QStringLiteral("id")).toString();
    track.title = object.value(QStringLiteral("title")).toString();
    track.artist = object.value(QStringLiteral("artist")).toString();
    track.album = object.value(QStringLiteral("album")).toString();
    track.artworkKey = object.value(QStringLiteral("artworkKey")).toString();
    track.streamProfile = object.value(QStringLiteral("streamProfile")).toString();
    track.durationMs = object.value(QStringLiteral("durationMs")).toString().toLongLong();
    track.favorite = object.value(QStringLiteral("favorite")).toBool(false);
    return track;
}

class ScopedDatabaseConnection final {
public:
    explicit ScopedDatabaseConnection(const QString& databasePath)
        : m_connectionName(makeConnectionName())
        , m_database(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName))
    {
        m_database.setConnectOptions(QStringLiteral("QSQLITE_BUSY_TIMEOUT=3000"));
        m_database.setDatabaseName(databasePath);
    }

    ~ScopedDatabaseConnection()
    {
        if (m_database.isValid()) {
            m_database.close();
        }
        m_database = {};
        QSqlDatabase::removeDatabase(m_connectionName);
    }

    bool open()
    {
        if (m_database.isOpen()) {
            return true;
        }

        if (!m_database.open()) {
            qWarning(lcApp) << "Failed to open library cache database:" << m_database.lastError().text();
            return false;
        }

        return true;
    }

    QSqlDatabase& database()
    {
        return m_database;
    }

private:
    QString m_connectionName;
    QSqlDatabase m_database;
};

bool execStatement(QSqlDatabase& database, const QString& statement)
{
    QSqlQuery query(database);
    if (query.exec(statement)) {
        return true;
    }

    qWarning(lcApp) << "Library cache statement failed:" << statement << query.lastError().text();
    return false;
}

bool initializeSchema(QSqlDatabase& database)
{
    if (!execStatement(database, QStringLiteral("PRAGMA journal_mode=WAL"))) {
        return false;
    }
    if (!execStatement(database, QStringLiteral("PRAGMA synchronous=NORMAL"))) {
        return false;
    }
    if (!execStatement(database, QStringLiteral("PRAGMA temp_store=MEMORY"))) {
        return false;
    }
    if (!execStatement(database,
                       QStringLiteral("CREATE TABLE IF NOT EXISTS meta ("
                                      "key TEXT PRIMARY KEY,"
                                      "value TEXT NOT NULL)"))) {
        return false;
    }
    if (!execStatement(database,
                       QStringLiteral("CREATE TABLE IF NOT EXISTS tracks ("
                                      "id TEXT PRIMARY KEY,"
                                      "title TEXT NOT NULL,"
                                      "artist TEXT NOT NULL,"
                                      "album TEXT NOT NULL,"
                                      "artwork_key TEXT NOT NULL,"
                                      "stream_profile TEXT NOT NULL,"
                                      "duration_ms INTEGER NOT NULL DEFAULT 0,"
                                      "favorite INTEGER NOT NULL DEFAULT 0,"
                                      "position INTEGER NOT NULL DEFAULT 0,"
                                      "sync_generation INTEGER NOT NULL DEFAULT 0)"))) {
        return false;
    }
    if (!execStatement(database,
                       QStringLiteral("CREATE INDEX IF NOT EXISTS idx_tracks_generation "
                                      "ON tracks(sync_generation)"))) {
        return false;
    }
    if (!execStatement(database,
                       QStringLiteral("CREATE INDEX IF NOT EXISTS idx_tracks_position "
                                      "ON tracks(position)"))) {
        return false;
    }
    return true;
}

int currentGeneration(QSqlDatabase& database)
{
    QSqlQuery query(database);
    query.prepare(QStringLiteral("SELECT value FROM meta WHERE key = ?"));
    query.addBindValue(QString::fromLatin1(kGenerationMetaKey));
    if (!query.exec()) {
        qWarning(lcApp) << "Failed to load library cache generation:" << query.lastError().text();
        return 0;
    }

    if (!query.next()) {
        return 0;
    }

    return query.value(0).toString().toInt();
}

bool storeGeneration(QSqlDatabase& database, int generation)
{
    QSqlQuery query(database);
    query.prepare(QStringLiteral("INSERT INTO meta(key, value) VALUES(?, ?) "
                                 "ON CONFLICT(key) DO UPDATE SET value = excluded.value"));
    query.addBindValue(QString::fromLatin1(kGenerationMetaKey));
    query.addBindValue(QString::number(generation));
    if (query.exec()) {
        return true;
    }

    qWarning(lcApp) << "Failed to persist library cache generation:" << query.lastError().text();
    return false;
}

bool writeTracks(QSqlDatabase& database, const QVector<Domain::Track>& tracks)
{
    const int generation = currentGeneration(database) + 1;

    if (!database.transaction()) {
        qWarning(lcApp) << "Failed to begin library cache transaction:" << database.lastError().text();
        return false;
    }

    QSqlQuery upsert(database);
    upsert.prepare(QStringLiteral(
        "INSERT INTO tracks("
        "id, title, artist, album, artwork_key, stream_profile, duration_ms, favorite, position, sync_generation"
        ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
        "ON CONFLICT(id) DO UPDATE SET "
        "title = excluded.title, "
        "artist = excluded.artist, "
        "album = excluded.album, "
        "artwork_key = excluded.artwork_key, "
        "stream_profile = excluded.stream_profile, "
        "duration_ms = excluded.duration_ms, "
        "favorite = excluded.favorite, "
        "position = excluded.position, "
        "sync_generation = excluded.sync_generation"));

    for (qsizetype index = 0; index < tracks.size(); ++index) {
        const Domain::Track& track = tracks.at(index);
        upsert.addBindValue(track.id);
        upsert.addBindValue(track.title);
        upsert.addBindValue(track.artist);
        upsert.addBindValue(track.album);
        upsert.addBindValue(track.artworkKey);
        upsert.addBindValue(track.streamProfile);
        upsert.addBindValue(track.durationMs);
        upsert.addBindValue(track.favorite ? 1 : 0);
        upsert.addBindValue(index);
        upsert.addBindValue(generation);

        if (!upsert.exec()) {
            qWarning(lcApp) << "Failed to upsert cached track" << track.id << upsert.lastError().text();
            database.rollback();
            return false;
        }

        upsert.finish();
    }

    if (!storeGeneration(database, generation)) {
        database.rollback();
        return false;
    }

    QSqlQuery prune(database);
    prune.prepare(QStringLiteral("DELETE FROM tracks WHERE sync_generation <> ?"));
    prune.addBindValue(generation);
    if (!prune.exec()) {
        qWarning(lcApp) << "Failed to prune stale cached tracks:" << prune.lastError().text();
        database.rollback();
        return false;
    }

    if (!database.commit()) {
        qWarning(lcApp) << "Failed to commit library cache transaction:" << database.lastError().text();
        database.rollback();
        return false;
    }

    return true;
}

QVector<Domain::Track> loadTracks(QSqlDatabase& database)
{
    QSqlQuery query(database);
    query.prepare(QStringLiteral(
        "SELECT id, title, artist, album, artwork_key, stream_profile, duration_ms, favorite "
        "FROM tracks ORDER BY position ASC"));
    if (!query.exec()) {
        qWarning(lcApp) << "Failed to load cached tracks:" << query.lastError().text();
        return {};
    }

    QVector<Domain::Track> tracks;
    while (query.next()) {
        Domain::Track track;
        track.id = query.value(0).toString();
        track.title = query.value(1).toString();
        track.artist = query.value(2).toString();
        track.album = query.value(3).toString();
        track.artworkKey = query.value(4).toString();
        track.streamProfile = query.value(5).toString();
        track.durationMs = query.value(6).toLongLong();
        track.favorite = query.value(7).toBool();
        tracks.push_back(track);
    }

    return tracks;
}

QVector<Domain::Track> loadLegacyTracks(const QString& legacyJsonPath)
{
    QFile file(legacyJsonPath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    const QJsonArray items = document.object().value(QStringLiteral("tracks")).toArray();

    QVector<Domain::Track> tracks;
    tracks.reserve(items.size());
    for (const QJsonValue& value : items) {
        const Domain::Track track = deserializeTrack(value.toObject());
        if (!track.id.isEmpty()) {
            tracks.push_back(track);
        }
    }

    return tracks;
}

}  // namespace

LibraryCache::LibraryCache(const Settings::AppPaths& paths)
    : m_databasePath(paths.libraryCacheFilePath())
    , m_legacyJsonPath(QFileInfo(paths.libraryCacheFilePath()).dir().filePath(QStringLiteral("home_tracks.json")))
{
    QFileInfo info(m_databasePath);
    QDir().mkpath(info.dir().absolutePath());
}

QVector<Domain::Track> LibraryCache::loadHomeTracks() const
{
    ScopedDatabaseConnection connection(m_databasePath);
    if (!connection.open()) {
        return {};
    }

    QSqlDatabase& database = connection.database();
    if (!initializeSchema(database)) {
        return {};
    }

    QVector<Domain::Track> tracks = loadTracks(database);
    if (tracks.isEmpty() && QFile::exists(m_legacyJsonPath)) {
        const QVector<Domain::Track> legacyTracks = loadLegacyTracks(m_legacyJsonPath);
        if (!legacyTracks.isEmpty() && writeTracks(database, legacyTracks)) {
            tracks = loadTracks(database);
            qInfo(lcApp) << "Migrated" << tracks.size() << "tracks from legacy JSON cache to SQLite";
        }
    }

    if (!tracks.isEmpty()) {
        qInfo(lcApp) << "Loaded" << tracks.size() << "tracks from local SQLite cache";
    }
    return tracks;
}

bool LibraryCache::saveHomeTracks(const QVector<Domain::Track>& tracks) const
{
    ScopedDatabaseConnection connection(m_databasePath);
    if (!connection.open()) {
        return false;
    }

    QSqlDatabase& database = connection.database();
    if (!initializeSchema(database)) {
        return false;
    }

    if (!writeTracks(database, tracks)) {
        return false;
    }

    qInfo(lcApp) << "Saved" << tracks.size() << "tracks to local SQLite cache";
    return true;
}

}  // namespace MyFin::Infrastructure::Storage
