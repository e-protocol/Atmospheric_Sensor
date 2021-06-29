#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "declaration.h"
#include "sounds.h"

enum ConnectionStatus
{
    NONE,
    SCANNING,
    CONNECTING,
    CONNECTED,
    DISCONNECTED
};

class Bluetooth : public QObject
{
    Q_OBJECT

public:
    Bluetooth(QListWidget* devicesListView);
    ~Bluetooth();
    void listWidgetClicked(QListWidgetItem *item);
    int getChargeLevel() { return chargeLevel; }

private:
    Sounds* m_sound;
    QListWidget* m_listWidget;
    QBluetoothLocalDevice *localDevice;
    QBluetoothDeviceDiscoveryAgent *agent;
    QList<QBluetoothAddress> *addressList; //create address list;
    QBluetoothSocket *socket;
    QList<QListWidgetItem *> *list_items;
    QString stringToAdd;
    int chargeLevel = 0;
    bool isConnected = false;
    bool isDanger = false;
    bool isRadiation = false;
    bool isPowerCritical = false;
    bool isChemical = false;
    bool isVitalSigns = false;
    float chemicalWarningLvl = 1500.0;
    float chemicalCriticalLvl = 3000.0;
    float radiationWarningLvl = 1.0;
    float radiationCriticalLvl = 2.0;
    float tempWarningHigh = 30.0;
    float tempWarningLow = 0.0;
    void checkDeviceBluetooth();
    void checkRadiation(const QString &data);
    void checkPowerCritical(const QString &data);
    void checkChemical(const QString &data);
    void checkVitalSigns(const QString &data);
    void dropParameters();

private slots:
    void addDevice(const QBluetoothDeviceInfo &info);
    void scanFinished();
    void pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);
    void socketConnected(); //refresh display elements
    void readData(); //read Data from device
    void printData(QString &data); //view data from sensor in real time
    void enableList(); //listWidget enable and clear connection status on pairing error
    void connectionLost(QBluetoothSocket::SocketError error);

public slots:
    void agentStart();
    void agentStop();
    void disconnectDevice(); //disconnect from device if connection lost

signals:
    void sendToTextBrowser(const QString &text);
    void sendToListWidget(QListWidgetItem &item);
    void enableObjectsMainWindow();
    void setConnectionStatus(ConnectionStatus status);
    void updateText(const QString &text);
    void soundSignal(const QString &soundName);
};

#endif // BLUETOOTH_H
