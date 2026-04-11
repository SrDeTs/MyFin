#pragma once

#include <QString>

namespace MyFin::Infrastructure::Settings {
class SettingsService;
}

namespace MyFin::Infrastructure::Storage {

struct SessionSnapshot {
    QString route;
    QString currentTrackId;
    bool playing = false;
};

class StateStore final {
public:
    explicit StateStore(Settings::SettingsService& settings);

    SessionSnapshot restoreSession() const;
    void saveSession(const SessionSnapshot& snapshot);

private:
    Settings::SettingsService& m_settings;
    SessionSnapshot m_snapshot;
};

}  // namespace MyFin::Infrastructure::Storage
