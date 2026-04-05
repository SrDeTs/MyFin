#include "app/applicationcontroller.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("MyFin"));
    app.setApplicationName(QStringLiteral("MyFin Player"));

    qRegisterMetaType<Track>("Track");
    qRegisterMetaType<QVector<Track>>("QVector<Track>");

    ApplicationController controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("AppController"), &controller);

    const QUrl mainUrl(QStringLiteral("qrc:/qt/qml/MyFin/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.load(mainUrl);

    controller.initialize();

    return app.exec();
}
