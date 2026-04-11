#include "app/AppBootstrap.h"
#include "app/AppServices.h"
#include "infrastructure/logging/Logging.h"
#include "infrastructure/settings/AppPaths.h"
#include "presentation/viewmodels/AppRootViewModel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickStyle>
#include <QTimer>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    MyFin::App::AppBootstrap::configureApplicationMetadata(app);

    MyFin::Infrastructure::Settings::AppPaths bootstrapPaths;
    bootstrapPaths.ensureDirectories();
    MyFin::Infrastructure::Logging::initialize(bootstrapPaths.logFilePath());

    MyFin::App::AppServices services;
    if (!services.singleInstance().tryAcquire()) {
        qInfo(lcApp) << services.singleInstance().errorString();
        return 0;
    }

    MyFin::Presentation::AppRootViewModel appViewModel(services);
    appViewModel.bootstrap();

    QQmlApplicationEngine engine;
    engine.setOutputWarningsToStandardError(true);
    QObject::connect(&engine, &QQmlApplicationEngine::warnings, &app, [](const QList<QQmlError>& warnings) {
        for (const QQmlError& warning : warnings) {
            qWarning(lcUi).noquote() << "QML warning:" << warning.toString();
        }
    });

    MyFin::App::AppBootstrap::configureQmlEngine(engine, services, appViewModel);
    engine.loadFromModule("MyFin", "Main");

    if (engine.rootObjects().isEmpty()) {
        qCritical(lcApp) << "Failed to load QML root object";
        return 1;
    }

    qInfo(lcUi) << "Loaded QML root object count:" << engine.rootObjects().size();
    for (QObject* rootObject : engine.rootObjects()) {
        qInfo(lcUi) << "Root object:" << rootObject->metaObject()->className()
                    << "objectName=" << rootObject->objectName();
    }

    if (auto* rootWindow = qobject_cast<QQuickWindow*>(engine.rootObjects().constFirst())) {
        rootWindow->setVisibility(QWindow::Windowed);
        rootWindow->show();
        rootWindow->raise();
        rootWindow->requestActivate();

        QObject::connect(rootWindow, &QWindow::visibilityChanged, rootWindow, [rootWindow] {
            qInfo(lcUi) << "Root window visibility changed to" << rootWindow->visibility()
                        << "visible=" << rootWindow->isVisible()
                        << "size=" << rootWindow->size();
        });

        QTimer::singleShot(250, rootWindow, [rootWindow] {
            qInfo(lcUi) << "Root window post-show state:"
                        << "visible=" << rootWindow->isVisible()
                        << "visibility=" << rootWindow->visibility()
                        << "size=" << rootWindow->size()
                        << "position=" << rootWindow->position();
        });
    } else {
        qWarning(lcUi) << "Root QML object is not a QQuickWindow";
    }

    return app.exec();
}
