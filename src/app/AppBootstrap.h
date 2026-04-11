#pragma once

#include <QObject>

class QGuiApplication;
class QQmlApplicationEngine;

namespace MyFin::Presentation {
class AppRootViewModel;
}

namespace MyFin::App {

class AppServices;

class AppBootstrap final {
public:
    static void configureApplicationMetadata(QGuiApplication& app);
    static void configureQmlEngine(QQmlApplicationEngine& engine,
                                   AppServices& services,
                                   Presentation::AppRootViewModel& viewModel);
};

}  // namespace MyFin::App
