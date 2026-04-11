#pragma once

#include <QQuickImageProvider>

namespace MyFin::Infrastructure::Jellyfin {
class JellyfinApiClient;
}

namespace MyFin::Infrastructure::Images {

class CoverArtImageProvider final : public QQuickAsyncImageProvider {
public:
    CoverArtImageProvider(Infrastructure::Jellyfin::JellyfinApiClient& jellyfin,
                          QString cacheDir);

    QQuickImageResponse* requestImageResponse(const QString& id, const QSize& requestedSize) override;

private:
    Infrastructure::Jellyfin::JellyfinApiClient& m_jellyfin;
    QString m_cacheDir;
};

}  // namespace MyFin::Infrastructure::Images
