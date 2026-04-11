#include "infrastructure/logging/Logging.h"

#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QTextStream>

#include <cstdio>
#include <memory>

Q_LOGGING_CATEGORY(lcApp, "myfin.app")
Q_LOGGING_CATEGORY(lcJellyfin, "myfin.jellyfin")
Q_LOGGING_CATEGORY(lcPlayback, "myfin.playback")
Q_LOGGING_CATEGORY(lcUi, "myfin.ui")

namespace {

std::unique_ptr<QFile> s_logFile;
QMutex s_logMutex;

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    const char* level = "DBG";
    switch (type) {
    case QtDebugMsg:
        level = "DBG";
        break;
    case QtInfoMsg:
        level = "INF";
        break;
    case QtWarningMsg:
        level = "WRN";
        break;
    case QtCriticalMsg:
        level = "CRT";
        break;
    case QtFatalMsg:
        level = "FTL";
        break;
    }

    const QString line = QStringLiteral("%1 [%2] [%3] %4\n")
                             .arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs),
                                  QString::fromLatin1(level),
                                  QString::fromLatin1(context.category ? context.category : "qt"),
                                  message);

    {
        QMutexLocker locker(&s_logMutex);
        if (s_logFile && s_logFile->isOpen()) {
            QTextStream stream(s_logFile.get());
            stream << line;
            stream.flush();
        }
    }

    std::fputs(line.toLocal8Bit().constData(), stderr);
    std::fflush(stderr);

    if (type == QtFatalMsg) {
        std::abort();
    }
}

}  // namespace

namespace MyFin::Infrastructure::Logging {

void initialize(const QString& logFilePath)
{
    bool logFileOpened = false;

    {
        QMutexLocker locker(&s_logMutex);

        s_logFile = std::make_unique<QFile>(logFilePath);
        if (s_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            logFileOpened = true;
        } else {
            s_logFile.reset();
        }
    }

    qInstallMessageHandler(messageHandler);
    if (logFileOpened) {
        qInfo(lcApp) << "Logging initialized at" << logFilePath;
        return;
    }

    qWarning(lcApp) << "Failed to open log file, using stderr only:" << logFilePath;
}

}  // namespace MyFin::Infrastructure::Logging
