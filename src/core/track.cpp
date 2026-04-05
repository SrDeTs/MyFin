#include "track.h"

QVariantMap Track::toVariantMap() const
{
    return {
        {QStringLiteral("id"), id},
        {QStringLiteral("title"), title},
        {QStringLiteral("artist"), artist},
        {QStringLiteral("album"), album},
        {QStringLiteral("durationMs"), durationMs},
        {QStringLiteral("sourceUrl"), sourceUrl},
        {QStringLiteral("artUrl"), artUrl},
        {QStringLiteral("remote"), remote},
        {QStringLiteral("sourceName"), sourceName}
    };
}

bool Track::isValid() const
{
    return sourceUrl.isValid() && !title.trimmed().isEmpty();
}
