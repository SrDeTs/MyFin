#pragma once

#include <QColor>
#include <QImage>
#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>

class ThemeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor primaryColor READ primaryColor NOTIFY themeChanged)
    Q_PROPERTY(QColor secondaryColor READ secondaryColor NOTIFY themeChanged)
    Q_PROPERTY(QColor accentColor READ accentColor NOTIFY themeChanged)
    Q_PROPERTY(QColor textColor READ textColor NOTIFY themeChanged)

public:
    explicit ThemeManager(QObject *parent = nullptr);

    QColor primaryColor() const;
    QColor secondaryColor() const;
    QColor accentColor() const;
    QColor textColor() const;

    Q_INVOKABLE void applyFallbackTheme();
    Q_INVOKABLE void updateFromArt(const QUrl &artUrl);

signals:
    void themeChanged();

private:
    void setTheme(const QColor &primary, const QColor &secondary, const QColor &accent);
    void extractFromImage(const QImage &image);

    QNetworkAccessManager m_networkAccessManager;
    QColor m_primaryColor;
    QColor m_secondaryColor;
    QColor m_accentColor;
    QColor m_textColor;
};
