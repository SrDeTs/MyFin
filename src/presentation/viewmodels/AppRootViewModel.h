#pragma once

#include "presentation/viewmodels/LibraryViewModel.h"
#include "presentation/viewmodels/PlaybackViewModel.h"
#include "presentation/viewmodels/SessionViewModel.h"

#include <QObject>

namespace MyFin::App {
class AppServices;
}

namespace MyFin::Presentation {

class AppRootViewModel final : public QObject {
    Q_OBJECT
    Q_PROPERTY(MyFin::Presentation::LibraryViewModel* library READ library CONSTANT FINAL)
    Q_PROPERTY(MyFin::Presentation::PlaybackViewModel* playback READ playback CONSTANT FINAL)
    Q_PROPERTY(MyFin::Presentation::SessionViewModel* session READ session CONSTANT FINAL)
    Q_PROPERTY(QString windowTitle READ windowTitle NOTIFY windowTitleChanged)

public:
    explicit AppRootViewModel(App::AppServices& services, QObject* parent = nullptr);
    ~AppRootViewModel() override;

    LibraryViewModel* library();
    PlaybackViewModel* playback();
    SessionViewModel* session();
    QString windowTitle() const;

    Q_INVOKABLE void bootstrap();
    Q_INVOKABLE void reloadLibrary();

signals:
    void windowTitleChanged();

private:
    App::AppServices& m_services;
    LibraryViewModel* m_library = nullptr;
    PlaybackViewModel* m_playback = nullptr;
    SessionViewModel* m_session = nullptr;
    QString m_windowTitle = QStringLiteral("MyFin");
};

}  // namespace MyFin::Presentation
