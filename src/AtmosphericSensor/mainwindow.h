#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "declaration.h"
#include "bluetooth.h"
#include "sounds.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPushButton* btnScan;
    QPushButton* btnQuit;
    QPushButton* btnNotifier;
    QListWidget* devicesListView;
    QTextBrowser* statusView;
    QTextBrowser* displayBrowser;
    Bluetooth* bluetooth;
    Sounds* sounds;
    QThread* bluetoothThread;
    QThread* soundThread;
    ConnectionStatus connectionStatus = NONE;
    void setElementsPosition();
    void checkAccess();

private slots:
    void btnScanClicked();
    void btnQuitClicked();
    void updateStatusView(const QString &text);
    void enableObjects();
    void setConnectionStatus(ConnectionStatus status);

signals:
    void startAgent();
    void stopAgent();
    void disconnectDevice();
    void playSound(const QString &soundName, int number);
    void clearSounds();
};
#endif // MAINWINDOW_H
