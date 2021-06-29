#include <bluetooth.h>

Bluetooth::Bluetooth(QListWidget *devicesListView)
{
    m_listWidget = devicesListView;
    //create device
    localDevice = new QBluetoothLocalDevice;
    agent = new QBluetoothDeviceDiscoveryAgent(this);
    addressList = new QList<QBluetoothAddress>; //create address list;
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    checkDeviceBluetooth();
    qRegisterMetaType<ConnectionStatus>("ConnectionStatus");

    connect(m_listWidget, &QListWidget::itemClicked, this, &Bluetooth::listWidgetClicked);

    //create discovery agent and connect to it's signals
    connect(agent,&QBluetoothDeviceDiscoveryAgent::deviceDiscovered,this,&Bluetooth::addDevice);
    connect(agent,&QBluetoothDeviceDiscoveryAgent::finished,this,&Bluetooth::scanFinished);
    connect(localDevice,&QBluetoothLocalDevice::pairingFinished,this,&Bluetooth::pairingDone);
    connect(localDevice,&QBluetoothLocalDevice::error,this,&Bluetooth::enableList);

    //socket signals
    connect(socket,&QBluetoothSocket::connected,this,&Bluetooth::socketConnected);
    connect(socket,&QBluetoothSocket::readyRead,this,&Bluetooth::readData);
    connect(socket,QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error), this,&Bluetooth::connectionLost);
    connect(socket, &QBluetoothSocket::disconnected, this, [=]
    {
        dropParameters();
        emit sendToTextBrowser("");
        emit setConnectionStatus(DISCONNECTED);
    });
}

Bluetooth::~Bluetooth()
{

}

void Bluetooth::checkDeviceBluetooth()
{
    //check if Bluetooth is available on this device
    if(!localDevice->isValid())
    {
        QMessageBox::critical(nullptr,"Info","Bluetooth is not available on this device");
        //update MainWindow
        emit sendToTextBrowser("Bluetooth: no device found");
    }
    else
        emit sendToTextBrowser("Bluetooth: ready"); //update MainWindow

    //check for bluetooth power ON
    if(localDevice->HostPoweredOff == 0)
    {
        localDevice->powerOn();
        QThread::sleep(3); // delay for uploading Bluetooth module
    }
}

void Bluetooth::agentStart()
{
    //scan for bluetooth devices
    socket->blockSignals(false); // enable socket disconnet signal
    localDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable); // make localDevice visible
    addressList->clear();
    agent->start();

    //update MainWindow
    m_listWidget->clear();
    emit sendToTextBrowser("Scanning...");
    emit setConnectionStatus(SCANNING);
}

void Bluetooth::agentStop()
{
    agent->stop();
    //update MainWindow
    emit sendToTextBrowser("");
    emit setConnectionStatus(NONE);
}

void Bluetooth::addDevice(const QBluetoothDeviceInfo &info)
{
    //find devices
    QString label = QString("%1").arg(info.name());

    if(label == "")
        label = QString("%1").arg(info.address().toString());

    QList<QListWidgetItem *> items = m_listWidget->findItems(label,Qt::MatchExactly);

    if(items.empty())
    {
        QListWidgetItem *item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus(info.address());
        if(pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired)
            item->QListWidgetItem::setForeground(QColor(Qt::blue));
        else
            item->QListWidgetItem::setForeground(QColor(Qt::black));
        // check for low energy bluetooth device
        if(info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
        {
            QString type = "  LE";
            item->setText(label + type);
        }
        addressList->push_back(info.address()); // save device address

        //update MainWindow
        m_listWidget->addItem(item);
    }
}

void Bluetooth::scanFinished()
{
    //show found devices
    if(m_listWidget->count() == 0)
        QMessageBox::critical(nullptr,"Scan Failure","No devices found!");
    else
    {
        //update MainWindow
        emit sendToTextBrowser("Devices found");
    }
    //update MainWindow
    emit enableObjectsMainWindow();
    emit setConnectionStatus(NONE);
}

void Bluetooth::pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing)
{
    //change device color status
    int number = addressList->indexOf(address);
    QListWidgetItem *item = m_listWidget->item(number);

    if(pairing == QBluetoothLocalDevice::Paired || pairing == QBluetoothLocalDevice::AuthorizedPaired)
        item->QListWidgetItem::setForeground(QColor(Qt::blue));
    else
        item->QListWidgetItem::setForeground(QColor(Qt::black));

    //update MainWindow
    emit sendToTextBrowser("Devices found");
    m_listWidget->setEnabled(true); // enable listWidget
}

void Bluetooth::listWidgetClicked(QListWidgetItem *item)
{
    m_listWidget->setEnabled(false); // disable listWidget
    //pair or unpair device by click
    if(agent->isActive())
    {
        agent->stop(); // stop for scanning
        scanFinished();
    }
    if(item->text() != "P-Climate Sensor")
    {
        QMessageBox::warning(nullptr,"Attention","Connection can be set only to P-Climate Sensor");
        m_listWidget->setEnabled(true);
        return;
    }
    QBluetoothAddress address = addressList->at(m_listWidget->row(item)); // get address from list
    QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus(address);
    QMessageBox msgBox; //connect/disconnect
    QString text; // text for connecting/disconnecting window

    if(pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired)
    {
        //Disconnect to device
        msgBox.setText("Connection Options");
        QPushButton *unpair = new QPushButton("Unpair");
        msgBox.addButton(unpair,QMessageBox::YesRole);
        QPushButton *connectButton = new QPushButton("Connect");
        msgBox.addButton(connectButton,QMessageBox::NoRole);
        msgBox.setStandardButtons(QMessageBox::Cancel);
        int choice = msgBox.exec();

        switch (choice)
        {
            case QMessageBox::YesRole:
            {
                localDevice->requestPairing(address,QBluetoothLocalDevice::Unpaired);
                emit sendToTextBrowser("Disconnecting...");
                break;
            }
            case QMessageBox::NoRole:
            {
                socket->connectToService(address,QBluetoothUuid(QBluetoothUuid::SerialPort));
                socket->open(QIODevice::ReadWrite);

                //update MainWindow
                emit sendToTextBrowser("Connecting...");
                emit setConnectionStatus(CONNECTING);
                break;
            }
            case QMessageBox::Cancel:
            m_listWidget->setEnabled(true);
            break;
            default:
            break;
        }
    }
    else
    {
        //Connect to device
        msgBox.setText("Pair Device");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int choice = msgBox.exec();

        switch (choice)
        {
            case QMessageBox::Yes:
            {
                localDevice->requestPairing(address,QBluetoothLocalDevice::Paired);

                //update MainWindow
                emit sendToTextBrowser("Pairing...");
                break;
            }
            case QMessageBox::No:
            m_listWidget->setEnabled(true);
            break;
            default:
            break;
        }
    }
    m_listWidget->clearSelection(); //deselect listWidget
}

void Bluetooth::socketConnected()
{
    m_listWidget->clear();

    //refresh display elements
    if(socket->isOpen())
    {
        emit sendToTextBrowser("Transmission set. Updating...");
        emit setConnectionStatus(CONNECTED); //update MainWindow and show pushButton_Disconnect
    }
    else
        connectionLost(QBluetoothSocket::ServiceNotFoundError);
}

void Bluetooth::readData()
{
    stringToAdd += socket->readAll();

    if(!stringToAdd.contains("\n") || !stringToAdd.contains("\r"))
        return;

    QStringList stringList = stringToAdd.split(QRegExp("\\r\\n"),Qt::KeepEmptyParts);
    stringList.size() > 1 ? stringToAdd = stringList[1] : stringToAdd = "";
    printData(stringList[0]);
}

void Bluetooth::printData(QString &data)
{
    //data income format example: "sensor16 40 760 95sensor450 150sensor15"
    //data map Temperature Humidity Pressure Battery eCO2 TCOV μSv/hr
    //proccess data
    if(data.isEmpty())
        return;

    if(!isConnected)
    {
        isConnected = true;
        soundSignal("intro");
        return;
    }

    QString output;
    output.append(socket->peerName() + "\n");
    QStringList list = data.split(QRegExp("/"),Qt::KeepEmptyParts);

    if(list.size() == 4)
    {
        QStringList bmeList = list[0].split(QRegExp(" "),Qt::KeepEmptyParts);

        if(bmeList.size() == 3)
        {
            output.append("Temperature: " + bmeList[0] + "°C\n");
            output.append("Humidity:  " + bmeList[1] + "%\n");
            output.append("Pressure: " + bmeList[2] + " mm\n");
            checkVitalSigns(bmeList[0]);
        }
        else
            output.append("BME280: error\n");

        chargeLevel = list[1].toInt();

        if(chargeLevel > 100)
            chargeLevel = 100;
        else if(chargeLevel < 0)
            chargeLevel = 0;

        output.append("Battery: " + QString::number(chargeLevel) + "%\n");
        checkPowerCritical(list[1]);

        QStringList ccsList = list[2].split(QRegExp(" "),Qt::KeepEmptyParts);

        if(ccsList.size() == 2)
        {
            output.append("eCO2: " + ccsList[0] + " ppm\n");
            output.append("TVOC:  " + ccsList[1] + " ppm\n");
            checkChemical(ccsList[0]);
        }
        else
            output.append("CCS811: error\n");

        output.append("Radiation: " + list[3] + " μSv/hr\n");
        checkRadiation(list[3]);
    }
    else
        output.append("Sensor error\n");

    updateText(output);
}

void Bluetooth::checkVitalSigns(const QString &data)
{
    bool ok = false;
    float number = data.toFloat(&ok);

    if(ok)
    {
        if(number > tempWarningLow || number < tempWarningHigh)
            isVitalSigns = false;

        if(number <= tempWarningLow || number >= tempWarningHigh)
        {
            isVitalSigns = true;
            soundSignal("health_critical");
        }
    }
}

void Bluetooth::checkChemical(const QString &data)
{
    bool ok = false;
    float number = data.toFloat(&ok);

    if(ok)
    {
        if(number < chemicalWarningLvl)
            isChemical = false;

        if(number < chemicalCriticalLvl)
            isDanger = false;

        if(number > chemicalWarningLvl && number < chemicalCriticalLvl && !isChemical)
        {
            soundSignal("chemical");
            isChemical = true;
        }
        else if(number > chemicalCriticalLvl && !isDanger)
        {
            soundSignal("danger");
            isDanger = true;
        }
    }
}

void Bluetooth::checkRadiation(const QString &data)
{
    bool ok = false;
    float number = data.toFloat(&ok);

    if(ok)
    {
        if(number < radiationWarningLvl)
            isRadiation = false;

        if(number < radiationCriticalLvl)
            isDanger = false;

        if(number > radiationWarningLvl && number < radiationCriticalLvl && !isRadiation)
        {
            soundSignal("radiation");
            isRadiation = true;
        }
        else if(number > radiationCriticalLvl && !isDanger)
        {
            soundSignal("danger");
            isDanger = true;
        }
    }
}

void Bluetooth::checkPowerCritical(const QString &data)
{
    bool ok = false;
    int number = data.toInt(&ok);

    if(ok)
    {
        if(number > 21)
            isPowerCritical = false;
        else if(number < 21 && !isPowerCritical)
        {
            soundSignal("power_critical");
            isPowerCritical = true;
        }
    }
}

void Bluetooth::disconnectDevice()
{
    //disconnect from device
    socket->disconnectFromService();
}

void Bluetooth::connectionLost(QBluetoothSocket::SocketError error)
{
    Q_UNUSED(error);
    //disconnect from device on lost connection
    disconnectDevice();
    QMessageBox::critical(nullptr,"Connection lost","Check sensor");
}

void Bluetooth::enableList()
{
    //listWidget enable
    m_listWidget->setEnabled(true);
    for(int k = 0; k < addressList->size(); k++)
    {
        QListWidgetItem *item = m_listWidget->item(k);
        QBluetoothLocalDevice::Pairing pairing = localDevice->pairingStatus(addressList->at(k));

        if(pairing == QBluetoothLocalDevice::Paired || pairing == QBluetoothLocalDevice::AuthorizedPaired)
            item->QListWidgetItem::setForeground(QColor(Qt::blue));
        else
            item->QListWidgetItem::setForeground(QColor(Qt::black));

        //update MainWindow
        emit sendToTextBrowser("Devices found");
    }
    QMessageBox::critical(nullptr,"Error","Can't pair device");
}

void Bluetooth::dropParameters()
{
    isConnected = false;
    isDanger = false;
    isChemical = false;
    isRadiation = false;
    isPowerCritical = false;
}
