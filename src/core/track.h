#pragma once

#include <QMetaType>
#include <QUrl>
#include <QString>
#include <QVariantMap>

struct Track {
    QString id;
    QString title;
    QString artist;
    QString album;
    qint64 durationMs = 0;
    QUrl sourceUrl;
    QUrl artUrl;
    bool remote = false;
    QString sourceName;

    QVariantMap toVariantMap() const;
    bool isValid() const;
};

Q_DECLARE_METATYPE(Track)
