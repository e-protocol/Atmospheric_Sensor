#include "sounds.h"

Sounds::Sounds(QPushButton* btnNotifier)
{
    initPlaylists();
    player = new QMediaPlayer;
    playlist = new QMediaPlaylist;

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [&](QMediaPlayer::MediaStatus status)
    {
        if(status == QMediaPlayer::LoadedMedia && !isPlay)
            player->play();
    },Qt::AutoConnection);

    connect(player, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State state)
    {
        if(state == QMediaPlayer::StoppedState && !isStop)
        {
            btnNotifier->blockSignals(false);
            isPlay = false;

            if(!playBack.isEmpty())
            {
                QString soundName = playBack[0];
                playBack.remove(0);
                play(soundName,0);
            }
        }
        else if(state  == QMediaPlayer::PlayingState)
        {
            btnNotifier->blockSignals(true);
            isPlay = true;
        }

        isStop = false;

    },Qt::AutoConnection);
}

Sounds::~Sounds()
{
    delete player;
}

void Sounds::initPlaylists()
{
    soundsList = new QList<QUrl>;
    introPlayList = new QList<QUrl>;
    introPlayList->push_back(QUrl("assets:/fuzz.wav"));
    introPlayList->push_back(QUrl("assets:/fuzz.wav"));
    introPlayList->push_back(QUrl("assets:/atmospherics_on.wav"));
    introPlayList->push_back(QUrl("assets:/communications_on.wav"));
    dangerPlayList = new QList<QUrl>;
    dangerPlayList->push_back(QUrl("assets:/fuzz.wav"));
    dangerPlayList->push_back(QUrl("assets:/fuzz.wav"));
    dangerPlayList->push_back(QUrl("assets:/danger.wav"));
    dangerPlayList->push_back(QUrl("assets:/evacuate_area.wav"));
    dangerPlayList->push_back(QUrl("assets:/immediately.wav"));
    radiationPlayList = new QList<QUrl>;
    radiationPlayList->push_back(QUrl("assets:/fuzz.wav"));
    radiationPlayList->push_back(QUrl("assets:/fuzz.wav"));
    radiationPlayList->push_back(QUrl("assets:/radiation_detected.wav"));
    powerCriticalPlayList = new QList<QUrl>;
    powerCriticalPlayList->push_back(QUrl("assets:/fuzz.wav"));
    powerCriticalPlayList->push_back(QUrl("assets:/fuzz.wav"));
    powerCriticalPlayList->push_back(QUrl("assets:/warning.wav"));
    powerCriticalPlayList->push_back(QUrl("assets:/power_level_is.wav"));
    powerCriticalPlayList->push_back(QUrl("assets:/critical.wav"));
    chemicalPlayList = new QList<QUrl>;
    chemicalPlayList->push_back(QUrl("assets:/fuzz.wav"));
    chemicalPlayList->push_back(QUrl("assets:/fuzz.wav"));
    chemicalPlayList->push_back(QUrl("assets:/chemical_detected.wav"));
    vitalSignsPlayList = new QList<QUrl>;
    vitalSignsPlayList->push_back(QUrl("assets:/fuzz.wav"));
    vitalSignsPlayList->push_back(QUrl("assets:/fuzz.wav"));
    vitalSignsPlayList->push_back(QUrl("assets:/health_critical.wav"));
}

void Sounds::play(const QString &soundName, int number)
{ 
    if(!checkPlayback(soundName))
    {
        if(soundName != commandsList[2])
            playBack.push_back(soundName);

        return;
    }

    if(player->state() != QMediaPlayer::StoppedState)
        player->stop();

    int index = 0;

    for(index = 0; index < commandsList.size(); index++)
        if(commandsList[index] == soundName)
            break;

    isPlay = false;

    switch(index)
    {
        case(0): addPlayList(introPlayList); break;
        case(1): player->setMedia(QUrl("assets:/safe_day.wav")); break;
        case(2): setBatteryNotifier(number); break;
        case(3): addPlayList(powerCriticalPlayList); break;
        case(4): addPlayList(radiationPlayList); break;
        case(5): addPlayList(chemicalPlayList); break;
        case(6): addPlayList(dangerPlayList); break;
        case(7): addPlayList(vitalSignsPlayList); break;
        default: break;
    }
}

//check which playlist can be played
bool Sounds::checkPlayback(const QString &soundName)
{
    int index = 0;

    for(int k = 0; k < commandsList.size(); k++)
        if(commandsList[index] == soundName)
        {
            index = k;
            break;
        }

    if(player->state() != QMediaPlayer::StoppedState && index != 1)
        return false;

    return true;
}

void Sounds::addPlayList(QList<QUrl> *mediaList)
{
    playlist->clear();

    for(auto &elem : *mediaList)
        playlist->addMedia(elem);

    playlist->setCurrentIndex(0);
    player->setPlaylist(playlist);
}

void Sounds::setBatteryNotifier(int number)
{
    soundsList->clear();
    soundsList->push_back(QUrl("assets:/fuzz.wav"));
    soundsList->push_back(QUrl("assets:/fuzz.wav"));
    soundsList->push_back(QUrl("assets:/power_level_is.wav"));

    if(number < 20)
        simpleNotifier(number);
    else
    {
        int firstDigit = 0;

        for(int count = number; count >= 10; count -= 10)
            firstDigit++;

        int secondDigit = number % 10;
        complexNotifier(firstDigit * 10);
        simpleNotifier(secondDigit);
    }

    soundsList->push_back(QUrl("assets:/percent.wav"));
    addPlayList(soundsList);
}

void Sounds::simpleNotifier(int number)
{
    //----------------NOTE------------------
    //on android path is ":/assets/fuzz.wav"
    //QDir curDir(QDir::currentPath());

    switch(number)
    {
        case(1): soundsList->push_back(QUrl("assets:/one.wav")); break;
        case(2): soundsList->push_back(QUrl("assets:/two.wav")); break;
        case(3): soundsList->push_back(QUrl("assets:/three.wav")); break;
        case(4): soundsList->push_back(QUrl("assets:/four.wav")); break;
        case(5): soundsList->push_back(QUrl("assets:/five.wav")); break;
        case(6): soundsList->push_back(QUrl("assets:/six.wav")); break;
        case(7): soundsList->push_back(QUrl("assets:/seven.wav")); break;
        case(8): soundsList->push_back(QUrl("assets:/eight.wav")); break;
        case(9): soundsList->push_back(QUrl("assets:/nine.wav")); break;
        case(10): soundsList->push_back(QUrl("assets:/ten.wav")); break;
        case(11): soundsList->push_back(QUrl("assets:/eleven.wav")); break;
        case(12): soundsList->push_back(QUrl("assets:/twelve.wav")); break;
        case(13): soundsList->push_back(QUrl("assets:/thirteen.wav")); break;
        case(14): soundsList->push_back(QUrl("assets:/fourteen.wav")); break;
        case(15): soundsList->push_back(QUrl("assets:/fifteen.wav")); break;
        case(16): soundsList->push_back(QUrl("assets:/sixteen.wav")); break;
        case(17): soundsList->push_back(QUrl("assets:/seventeen.wav")); break;
        case(18): soundsList->push_back(QUrl("assets:/eighteen.wav")); break;
        case(19): soundsList->push_back(QUrl("assets:/nineteen.wav")); break;
        default:
            break;
    }
}

void Sounds::complexNotifier(int number)
{
    //----------------NOTE------------------
    //on android path is ":/assets/fuzz.wav"
    //QDir curDir(QDir::currentPath());

    switch(number)
    {
        case(20): soundsList->push_back(QUrl("assets:/twenty.wav")); break;
        case(30): soundsList->push_back(QUrl("assets:/thirty.wav")); break;
        case(40): soundsList->push_back(QUrl("assets:/fourty.wav")); break;
        case(50): soundsList->push_back(QUrl("assets:/fifty.wav")); break;
        case(60): soundsList->push_back(QUrl("assets:/sixty.wav")); break;
        case(70): soundsList->push_back(QUrl("assets:/seventy.wav")); break;
        case(80): soundsList->push_back(QUrl("assets:/eighty.wav")); break;
        case(90): soundsList->push_back(QUrl("assets:/ninety.wav")); break;
        case(100): soundsList->push_back(QUrl("assets:/onehundred.wav")); break;
        default:
            break;
    }
}

void Sounds::clearSounds()
{
    isStop = true; //prevent playing on disconnect
    player->stop();
}
