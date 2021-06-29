#ifndef SOUNDS_H
#define SOUNDS_H

#include "declaration.h"

class Sounds : public QObject
{
    Q_OBJECT

public:
    Sounds(QPushButton *btnNotifier);
    ~Sounds();

public slots:
    void play(const QString &soundName, int number);
    void clearSounds();

private:
    QList<QUrl>* introPlayList;
    QMediaPlayer* player;
    QMediaPlaylist* playlist;
    QList<QUrl>* soundsList;
    QList<QUrl>* dangerPlayList;
    QList<QUrl>* radiationPlayList;
    QList<QUrl>* powerCriticalPlayList;
    QList<QUrl>* chemicalPlayList;
    QList<QUrl>* vitalSignsPlayList;
    QVector<QString> playBack;
    bool isPlay = false;
    bool isStop = false;
    const QVector<QString> commandsList = {"intro","safe_day","battery","power_critical","radiation","chemical","danger","health_critical"};
    void setBatteryNotifier(int number);
    void simpleNotifier(int number);
    void complexNotifier(int number);
    void addPlayList(QList<QUrl>* soundList);
    void initPlaylists();
    bool checkPlayback(const QString &soundName);
};

#endif // SOUNDS_H
