#include "app/AppBootstrap.h"

#include "app/AppServices.h"
#include "infrastructure/images/CoverArtImageProvider.h"
#include "presentation/viewmodels/AppRootViewModel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

namespace MyFin::App {

void AppBootstrap::configureApplicationMetadata(QGuiApplication& app)
{
    app.setApplicationName(QStringLiteral("MyFin"));
    app.setApplicationDisplayName(QStringLiteral("MyFin"));
    app.setDesktopFileName(QStringLiteral("myfin"));
    app.setOrganizationName(QStringLiteral("MyFin"));
    app.setOrganizationDomain(QStringLiteral("io.github.myfin"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));
}

void AppBootstrap::configureQmlEngine(QQmlApplicationEngine& engine,
                                      AppServices& services,
                                      Presentation::AppRootViewModel& viewModel)
{
    engine.rootContext()->setContextProperty(QStringLiteral("AppVm"), &viewModel);
    engine.addImageProvider(QStringLiteral("cover"),
                            new Infrastructure::Images::CoverArtImageProvider(
                                services.jellyfin(),
                                services.paths().coverCacheDir()));
}

}  // namespace MyFin::App
