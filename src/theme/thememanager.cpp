#include "thememanager.h"

#include <algorithm>
#include <QImage>
#include <QImageReader>
#include <QNetworkReply>

namespace {
qreal luminance(const QColor &color)
{
    return 0.2126 * color.redF() + 0.7152 * color.greenF() + 0.0722 * color.blueF();
}

QColor mix(const QColor &left, const QColor &right, qreal weight)
{
    const qreal clamped = std::clamp(weight, 0.0, 1.0);
    return QColor::fromRgbF(left.redF() * (1.0 - clamped) + right.redF() * clamped,
                            left.greenF() * (1.0 - clamped) + right.greenF() * clamped,
                            left.blueF() * (1.0 - clamped) + right.blueF() * clamped,
                            1.0);
}
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
{
    applyFallbackTheme();
}

QColor ThemeManager::primaryColor() const { return m_primaryColor; }
QColor ThemeManager::secondaryColor() const { return m_secondaryColor; }
QColor ThemeManager::accentColor() const { return m_accentColor; }
QColor ThemeManager::textColor() const { return m_textColor; }

void ThemeManager::applyFallbackTheme()
{
    setTheme(QColor(QStringLiteral("#172033")),
             QColor(QStringLiteral("#26334f")),
             QColor(QStringLiteral("#d99f5d")));
}

void ThemeManager::updateFromArt(const QUrl &artUrl)
{
    if (!artUrl.isValid()) {
        applyFallbackTheme();
        return;
    }

    if (artUrl.isLocalFile()) {
        QImageReader reader(artUrl.toLocalFile());
        reader.setAutoTransform(true);
        const QImage image = reader.read();
        if (image.isNull()) {
            applyFallbackTheme();
            return;
        }

        extractFromImage(image);
        return;
    }

    QNetworkReply *reply = m_networkAccessManager.get(QNetworkRequest(artUrl));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray bytes = reply->readAll();
        reply->deleteLater();

        QImage image;
        image.loadFromData(bytes);
        if (image.isNull()) {
            applyFallbackTheme();
            return;
        }

        extractFromImage(image);
    });
}

void ThemeManager::setTheme(const QColor &primary, const QColor &secondary, const QColor &accent)
{
    m_primaryColor = primary;
    m_secondaryColor = secondary;
    m_accentColor = accent;
    m_textColor = luminance(m_primaryColor) > 0.55 ? QColor(QStringLiteral("#101317"))
                                                    : QColor(QStringLiteral("#f5f7fb"));
    emit themeChanged();
}

void ThemeManager::extractFromImage(const QImage &image)
{
    const QImage scaled = image.convertToFormat(QImage::Format_ARGB32).scaled(72, 72, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    if (scaled.isNull()) {
        applyFallbackTheme();
        return;
    }

    qreal red = 0.0;
    qreal green = 0.0;
    qreal blue = 0.0;
    int count = 0;
    QColor vibrant(QStringLiteral("#d99f5d"));
    qreal bestSaturation = 0.0;
    QColor bright(QStringLiteral("#526b9a"));
    qreal bestBrightness = 0.0;

    for (int y = 0; y < scaled.height(); y += 2) {
        for (int x = 0; x < scaled.width(); x += 2) {
            const QColor color = QColor::fromRgba(scaled.pixel(x, y));
            if (color.alpha() < 32) {
                continue;
            }

            red += color.redF();
            green += color.greenF();
            blue += color.blueF();
            ++count;

            const qreal sat = color.hslSaturationF();
            const qreal light = color.lightnessF();
            if (sat > bestSaturation && light > 0.18 && light < 0.82) {
                bestSaturation = sat;
                vibrant = color;
            }
            if (light > bestBrightness) {
                bestBrightness = light;
                bright = color;
            }
        }
    }

    if (count == 0) {
        applyFallbackTheme();
        return;
    }

    const QColor average = QColor::fromRgbF(red / count, green / count, blue / count, 1.0);
    const QColor primary = mix(average, QColor(QStringLiteral("#0c121d")), 0.48);
    const QColor secondary = mix(bright, QColor(QStringLiteral("#223455")), 0.42);
    const QColor accent = mix(vibrant, bright, 0.2);
    setTheme(primary, secondary, accent);
}
