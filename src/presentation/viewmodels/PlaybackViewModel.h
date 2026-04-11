#pragma once

#include <QObject>

namespace MyFin::Infrastructure::Playback {
class PlaybackController;
}

namespace MyFin::Presentation {

class PlaybackViewModel final : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY stateChanged)
    Q_PROPERTY(QString artist READ artist NOTIFY stateChanged)
    Q_PROPERTY(QString album READ album NOTIFY stateChanged)
    Q_PROPERTY(QString coverSource READ coverSource NOTIFY stateChanged)
    Q_PROPERTY(QString signalPath READ signalPath NOTIFY stateChanged)
    Q_PROPERTY(bool playing READ playing NOTIFY stateChanged)
    Q_PROPERTY(bool hasTrack READ hasTrack NOTIFY stateChanged)
    Q_PROPERTY(bool canGoNext READ canGoNext NOTIFY stateChanged)
    Q_PROPERTY(int queueLength READ queueLength NOTIFY stateChanged)
    Q_PROPERTY(qint64 positionMs READ positionMs NOTIFY stateChanged)
    Q_PROPERTY(qint64 durationMs READ durationMs NOTIFY stateChanged)
    Q_PROPERTY(QString positionLabel READ positionLabel NOTIFY stateChanged)
    Q_PROPERTY(QString durationLabel READ durationLabel NOTIFY stateChanged)

public:
    explicit PlaybackViewModel(Infrastructure::Playback::PlaybackController& playback,
                               QObject* parent = nullptr);

    QString title() const;
    QString artist() const;
    QString album() const;
    QString coverSource() const;
    QString signalPath() const;
    bool playing() const;
    bool hasTrack() const;
    bool canGoNext() const;
    int queueLength() const;
    qint64 positionMs() const;
    qint64 durationMs() const;
    QString positionLabel() const;
    QString durationLabel() const;

    Q_INVOKABLE void togglePlaying();
    Q_INVOKABLE void next();
    Q_INVOKABLE void seek(qint64 positionMs);

signals:
    void stateChanged();

private:
    static QString formatTime(qint64 positionMs);

    Infrastructure::Playback::PlaybackController& m_playback;
};

}  // namespace MyFin::Presentation
