#include "infrastructure/images/CoverArtImageProvider.h"

#include "infrastructure/jellyfin/JellyfinApiClient.h"

#include <QCryptographicHash>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QImage>
#include <QImageReader>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QQuickTextureFactory>
#include <QSaveFile>
#include <QtConcurrent>

namespace MyFin::Infrastructure::Images {

namespace {

struct CoverFetchResult {
    QImage image;
    QString error;
};

QString cachePathForRequest(const QString& cacheDir, const QString& artworkKey, const QSize& requestedSize)
{
    const QByteArray digest = QCryptographicHash::hash(
        QStringLiteral("%1|%2x%3").arg(artworkKey).arg(requestedSize.width()).arg(requestedSize.height()).toUtf8(),
        QCryptographicHash::Sha1);

    return cacheDir + QLatin1Char('/') + QString::fromLatin1(digest.toHex()) + QStringLiteral(".img");
}

QImage loadImageFromDisk(const QString& cachePath)
{
    QImageReader reader(cachePath);
    return reader.read();
}

QImage fallbackImage(const QSize& requestedSize)
{
    const QSize size = requestedSize.isValid() ? requestedSize : QSize(96, 96);
    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(QColor(QStringLiteral("#202a31")));
    return image;
}

class CoverArtResponse final : public QQuickImageResponse {
    Q_OBJECT

public:
    CoverArtResponse(QUrl remoteUrl, QString cachePath, QSize requestedSize)
        : m_cachePath(std::move(cachePath))
    {
        connect(&m_watcher, &QFutureWatcher<CoverFetchResult>::finished, this, [this] {
            const CoverFetchResult result = m_watcher.result();
            m_image = result.image.isNull() ? fallbackImage(QSize()) : result.image;
            m_error = result.error;
            emit finished();
        });

        m_watcher.setFuture(QtConcurrent::run([remoteUrl, cachePath = m_cachePath, requestedSize] {
            CoverFetchResult result;

            QFileInfo info(cachePath);
            QDir().mkpath(info.dir().absolutePath());

            if (QFile::exists(cachePath)) {
                result.image = loadImageFromDisk(cachePath);
                if (!result.image.isNull()) {
                    return result;
                }
            }

            if (!remoteUrl.isValid()) {
                result.image = fallbackImage(requestedSize);
                return result;
            }

            QNetworkAccessManager manager;
            QNetworkRequest request(remoteUrl);
            QEventLoop loop;
            QNetworkReply* reply = manager.get(request);
            QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            loop.exec();

            const QByteArray bytes = reply->readAll();
            const bool hasError = reply->error() != QNetworkReply::NoError;
            if (hasError) {
                result.image = fallbackImage(requestedSize);
                result.error = reply->errorString();
                reply->deleteLater();
                return result;
            }

            result.image.loadFromData(bytes);
            if (result.image.isNull()) {
                result.image = fallbackImage(requestedSize);
                result.error = QStringLiteral("Invalid cover image payload");
                reply->deleteLater();
                return result;
            }

            QSaveFile file(cachePath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(bytes);
                file.commit();
            }

            reply->deleteLater();
            return result;
        }));
    }

    QQuickTextureFactory* textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(m_image);
    }

    QString errorString() const override
    {
        return m_error;
    }

private:
    QString m_cachePath;
    QFutureWatcher<CoverFetchResult> m_watcher;
    QImage m_image;
    QString m_error;
};

}  // namespace

CoverArtImageProvider::CoverArtImageProvider(Infrastructure::Jellyfin::JellyfinApiClient& jellyfin,
                                             QString cacheDir)
    : m_jellyfin(jellyfin)
    , m_cacheDir(std::move(cacheDir))
{
    QDir().mkpath(m_cacheDir);
}

QQuickImageResponse* CoverArtImageProvider::requestImageResponse(const QString& id, const QSize& requestedSize)
{
    const QString cachePath = cachePathForRequest(m_cacheDir, id, requestedSize);
    const QUrl remoteUrl = m_jellyfin.buildArtworkUrl(id, requestedSize);
    return new CoverArtResponse(remoteUrl, cachePath, requestedSize);
}

}  // namespace MyFin::Infrastructure::Images

#include "CoverArtImageProvider.moc"
