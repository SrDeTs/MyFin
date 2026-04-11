#pragma once

#include <QString>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(lcApp)
Q_DECLARE_LOGGING_CATEGORY(lcJellyfin)
Q_DECLARE_LOGGING_CATEGORY(lcPlayback)
Q_DECLARE_LOGGING_CATEGORY(lcUi)

namespace MyFin::Infrastructure::Logging {

void initialize(const QString& logFilePath);

}  // namespace MyFin::Infrastructure::Logging
