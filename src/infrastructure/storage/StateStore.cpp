#include "infrastructure/storage/StateStore.h"

#include "infrastructure/settings/SettingsService.h"

namespace MyFin::Infrastructure::Storage {

StateStore::StateStore(Settings::SettingsService& settings)
    : m_settings(settings)
    , m_snapshot{QStringLiteral("home"), QString(), false}
{
}

SessionSnapshot StateStore::restoreSession() const
{
    return m_snapshot;
}

void StateStore::saveSession(const SessionSnapshot& snapshot)
{
    m_snapshot = snapshot;
}

}  // namespace MyFin::Infrastructure::Storage
