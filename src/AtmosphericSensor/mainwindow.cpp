#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "javarequest.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    devicesListView = new QListWidget;
    btnScan = new QPushButton("Scan");
    btnQuit = new QPushButton("Quit");
    btnNotifier = new QPushButton("Power level");
    statusView = new QTextBrowser;
    displayBrowser = new QTextBrowser;
    QTimer::singleShot(3000, this, &MainWindow::checkAccess);
    setElementsPosition();

    //start thread sounds
    sounds = new Sounds(btnNotifier);
    soundThread = new QThread;
    sounds->moveToThread(soundThread);
    soundThread->start();

    //start bluetooth thread
    bluetooth = new Bluetooth(devicesListView);
    bluetoothThread = new  QThread;
    bluetooth->moveToThread(bluetoothThread);
    bluetoothThread->start();

    connect(btnScan, &QPushButton::clicked, this, &MainWindow::btnScanClicked, Qt::UniqueConnection);
    connect(btnQuit, &QPushButton::clicked, this, &MainWindow::btnQuitClicked, Qt::UniqueConnection);
    connect(btnNotifier, &QPushButton::clicked, this, [=]
    {
        emit playSound("battery",bluetooth->getChargeLevel());
    }, Qt::UniqueConnection);
    connect(bluetooth, &Bluetooth::sendToTextBrowser, this, &MainWindow::updateStatusView);
    connect(bluetooth, &Bluetooth::enableObjectsMainWindow, this, &MainWindow::enableObjects);
    connect(bluetooth, &Bluetooth::setConnectionStatus, this, &MainWindow::setConnectionStatus);
    connect(bluetooth, &Bluetooth::updateText, this, [&](const QString &text) { displayBrowser->setText(text); });
    connect(bluetooth, &Bluetooth::soundSignal, this, [&](const QString &soundsName)
    {
        emit playSound(soundsName, 0);
    });
    connect(this, &MainWindow::clearSounds, sounds, &Sounds::clearSounds);
    connect(this, &MainWindow::startAgent, bluetooth, &Bluetooth::agentStart);
    connect(this, &MainWindow::stopAgent, bluetooth, &Bluetooth::agentStop);
    connect(this, &MainWindow::disconnectDevice, bluetooth, &Bluetooth::disconnectDevice);
    connect(this, &MainWindow::playSound, sounds, &Sounds::play);
    //prevent GUI freeze on suspend
    //not 100% working solution
    //works better with android:launchMode="standard"
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [=]/*(Qt::ApplicationState state)*/
    {
        QApplication::screens().at(0)->refreshRate();
        this->repaint();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setElementsPosition()
{
    //screen size
    QScreen *screen = QApplication::screens().at(0);
    int height = screen->availableSize().height();
    int width = screen->availableSize().width();
    this->setGeometry(0,0,width,height);

    QVBoxLayout* verticalLyaout = new QVBoxLayout;
    QHBoxLayout* headerLayout = new QHBoxLayout;
    headerLayout->addWidget(btnScan);
    headerLayout->addWidget(statusView);
    verticalLyaout->addLayout(headerLayout);
    verticalLyaout->addWidget(devicesListView);
    verticalLyaout->addWidget(displayBrowser);
    verticalLyaout->addWidget(btnNotifier);
    verticalLyaout->addWidget(btnQuit);
    QWidget* widget = new QWidget;
    widget->setLayout(verticalLyaout);
    this->setCentralWidget(widget);

    //position elements
    int headerElementWidth = width / 2;
    int headerElementHeight = height * 0.1;
    QSize elementSize = QSize(headerElementWidth,headerElementHeight);
    btnScan->setFixedSize(elementSize);
    statusView->setFixedSize(elementSize);
    btnNotifier->setFixedHeight(headerElementHeight);
    btnQuit->setFixedHeight(headerElementHeight);

    //style
    statusView->setStyleSheet("border:none; font-size: 30px;");
    devicesListView->setStyleSheet("font-size: 50px");
    btnScan->setStyleSheet("background-color: rgb(115, 210, 22);");
    btnNotifier->setStyleSheet("background-color: rgb(255, 250, 22);");
    btnNotifier->hide();
    displayBrowser->setStyleSheet("font-size: 50px");
    displayBrowser->hide();
}

void MainWindow::btnScanClicked()
{
    switch(connectionStatus)
    {
        case(NONE):
        {
            //check location access
            if(!getAccess())
            {
                QMessageBox::critical(nullptr,"Access Denied!","To proceed allow app to access GPS");
                checkAccess();
                return;
            }

            devicesListView->setEnabled(false);
            emit startAgent();
            break;
        }

        case(SCANNING):
        {
            emit stopAgent();
            break;
        }

            case(CONNECTING): case(CONNECTED):
        {
            emit disconnectDevice();
            break;
        }
        default: break;
    }
}

void MainWindow::btnQuitClicked()
{
    QMessageBox* messageBox = new QMessageBox();
    messageBox->setIcon(QMessageBox::Warning);
    messageBox->setWindowTitle("Quit");
    messageBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox->setDefaultButton(QMessageBox::No);
    messageBox->setText("Are you sure you want to Quit?");

    if(messageBox->exec() == QMessageBox::Yes)
    {
        sounds->~Sounds();
        setEnabled(false);
        QMediaPlayer* player = new QMediaPlayer;
        player->setMedia(QUrl("assets:/safe_day.wav"));
        connect(player, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State state)
        {
            if(state == QMediaPlayer::StoppedState)
                close();
        });
        player->play();
    }
}

void MainWindow::checkAccess()
{
    checkPermissions();

    if(!getAccess())
        turnGpsOn();
    else
        statusView->setText("Ready");
}

void MainWindow::updateStatusView(const QString &text)
{
    statusView->clear();
    statusView->insertHtml(text);
}

void MainWindow::enableObjects()
{
    connectionStatus = NONE;
    btnScan->setText("Scan");
    devicesListView->setEnabled(true); // enable listWidget
}

void MainWindow::setConnectionStatus(ConnectionStatus status)
{
    connectionStatus = status;

    switch(status)
    {
        case(NONE):
        {
            checkAccess();
            btnScan->setText("Scan");
            btnScan->setStyleSheet("background-color: rgb(115, 210, 22);");
            devicesListView->setEnabled(true); // enable listWidget
            displayBrowser->hide();
            devicesListView->show();
            btnNotifier->hide();
            break;
        }

        case(SCANNING):
        {
            btnScan->setText("Stop");
            btnScan->setStyleSheet("background-color: rgb(255, 250, 22);");
            break;
        }

        case(CONNECTING):
        {
            btnScan->setText("Cancel");
            btnScan->setStyleSheet("background-color: rgb(255, 250, 22);");
            break;
        }

        case(CONNECTED):
        {
            btnScan->setText("Disconnect");
            btnScan->setStyleSheet("background-color: rgb(239, 41, 41);");
            btnNotifier->show();
            devicesListView->hide();
            displayBrowser->show();
            displayBrowser->clear();
            break;
        }

        case(DISCONNECTED):
        {
            setConnectionStatus(NONE);
            emit clearSounds();
            break;
        }
        default: break;
    }
}
