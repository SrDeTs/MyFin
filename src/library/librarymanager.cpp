#include "librarymanager.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSet>

namespace {
QString displayFromBaseName(const QString &baseName)
{
    QString title = baseName;
    title.replace('_', ' ');
    return title.trimmed();
}
}

LibraryManager::LibraryManager(QObject *parent)
    : QObject(parent)
{
    setStatusMessage(QStringLiteral("Adicione uma pasta local ou conecte ao Jellyfin para carregar sua biblioteca."));
}

TrackListModel *LibraryManager::tracksModel() { return &m_tracksModel; }
bool LibraryManager::isScanning() const { return m_scanning; }
QString LibraryManager::statusMessage() const { return statusMessageValue; }

void LibraryManager::scanLocalDirectories(const QStringList &directories)
{
    setScanning(true);

    QVector<Track> discovered;
    const QSet<QString> extensions{
        QStringLiteral("mp3"),
        QStringLiteral("flac"),
        QStringLiteral("ogg"),
        QStringLiteral("oga"),
        QStringLiteral("wav"),
        QStringLiteral("m4a"),
        QStringLiteral("aac")
    };

    for (const QString &directory : directories) {
        QDirIterator it(directory,
                        QDir::Files,
                        QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString filePath = it.next();
            const QFileInfo info(filePath);
            if (!extensions.contains(info.suffix().toLower())) {
                continue;
            }
            discovered.append(buildLocalTrack(filePath));
        }
    }

    QVector<Track> remoteTracks;
    for (const Track &track : m_tracksModel.tracks()) {
        if (track.remote) {
            remoteTracks.append(track);
        }
    }
    discovered += remoteTracks;
    m_tracksModel.setTracks(discovered);

    setScanning(false);
    if (discovered.isEmpty()) {
        setStatusMessage(QStringLiteral("Nenhuma musica encontrada. Verifique suas pastas ou a conexao Jellyfin."));
    } else {
        setStatusMessage(QStringLiteral("%1 faixas carregadas.").arg(discovered.size()));
    }
}

void LibraryManager::importJellyfinTracks(const QVector<Track> &tracks)
{
    QVector<Track> merged;
    merged.reserve(m_tracksModel.tracks().size() + tracks.size());

    for (const Track &track : m_tracksModel.tracks()) {
        if (!track.remote) {
            merged.append(track);
        }
    }
    for (const Track &track : tracks) {
        merged.append(track);
    }

    m_tracksModel.setTracks(merged);

    if (merged.isEmpty()) {
        setStatusMessage(QStringLiteral("A biblioteca Jellyfin retornou vazia."));
    } else {
        setStatusMessage(QStringLiteral("Biblioteca atualizada com %1 faixas.").arg(merged.size()));
    }
}

void LibraryManager::clearRemoteTracks()
{
    QVector<Track> filtered;
    for (const Track &track : m_tracksModel.tracks()) {
        if (!track.remote) {
            filtered.append(track);
        }
    }
    m_tracksModel.setTracks(filtered);
}

Track LibraryManager::buildLocalTrack(const QString &filePath) const
{
    const QFileInfo info(filePath);
    const QDir dir = info.dir();

    return Track{
        info.absoluteFilePath(),
        displayFromBaseName(info.completeBaseName()),
        dir.dirName(),
        dir.absolutePath().section('/', -2, -2),
        0,
        QUrl::fromLocalFile(info.absoluteFilePath()),
        findCoverForFile(info),
        false,
        QStringLiteral("Local")
    };
}

QUrl LibraryManager::findCoverForFile(const QFileInfo &info) const
{
    const QStringList candidates{
        QStringLiteral("cover.jpg"),
        QStringLiteral("cover.png"),
        QStringLiteral("folder.jpg"),
        QStringLiteral("folder.png"),
        QStringLiteral("front.jpg"),
        QStringLiteral("front.png")
    };

    for (const QString &candidate : candidates) {
        const QString path = info.dir().absoluteFilePath(candidate);
        if (QFileInfo::exists(path)) {
            return QUrl::fromLocalFile(path);
        }
    }

    return {};
}

void LibraryManager::setScanning(bool value)
{
    if (m_scanning == value) {
        return;
    }
    m_scanning = value;
    emit scanningChanged();
}

void LibraryManager::setStatusMessage(const QString &value)
{
    if (statusMessageValue == value) {
        return;
    }
    statusMessageValue = value;
    emit statusMessageChanged();
}
