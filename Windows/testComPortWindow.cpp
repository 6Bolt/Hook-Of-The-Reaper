#include "testComPortWindow.h"
#include "ui_testComPortWindow.h"

testComPortWindow::testComPortWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::testComPortWindow)
{
    QString tempQS;
    quint8 i;

    tcpConnected = false;
    tcpInit = false;

    //Set Up This Windows
    ui->setupUi(this);

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports
    p_comDeviceList = cdList;

    //Load First Combo Box with the Saved Light Guns; Number, Name, and COM Port
    numberLightGuns = p_comDeviceList->GetNumberLightGuns();
    for(i = 0; i<numberLightGuns; i++)
    {
        quint8 outputConnection = p_comDeviceList->p_lightGunList[i]->GetOutputConnection();
        tempQS = QString::number(i+1);
        tempQS.append (": ");
        tempQS.append (p_comDeviceList->p_lightGunList[i]->GetLightGunName());
        tempQS.append (" on ");
        if(outputConnection == SERIALPORT)
            tempQS.append (p_comDeviceList->p_lightGunList[i]->GetComPortString());
        else if(outputConnection == USBHID)
            tempQS.append ("USB");
        else if(outputConnection == BTLE)
            tempQS.append ("BTLE");
        else if(outputConnection == TCP)
        {
            tempQS.append ("TCP(");
            quint16 tempTCP = p_comDeviceList->p_lightGunList[i]->GetTCPPort ();
            tempQS.append(QString::number (tempTCP));
            tempQS.append (")");
        }

        ui->savedLightGunsComboBox->insertItem(i,tempQS);
    }

    ui->savedLightGunsComboBox->setCurrentIndex(0);
    currentLG = 0;
    outputConnection = p_comDeviceList->p_lightGunList[currentLG]->GetOutputConnection();

    if(outputConnection == SERIALPORT)
    {
        comPortName = p_comDeviceList->p_lightGunList[0]->GetComPortString();
        comPortNum = p_comDeviceList->p_lightGunList[0]->GetComPortNumber();
        comPortBaud = p_comDeviceList->p_lightGunList[0]->GetComPortBaud ();
        comPortDataBits = p_comDeviceList->p_lightGunList[0]->GetComPortDataBits ();
        comPortParity = p_comDeviceList->p_lightGunList[0]->GetComPortParity ();
        comPortStopBits = p_comDeviceList->p_lightGunList[0]->GetComPortStopBits ();
        comPortFlow = p_comDeviceList->p_lightGunList[0]->GetComPortFlow ();
        isUSB = false;
    }
    else if(outputConnection == USBHID)
    {
        lgHIDInfo = p_comDeviceList->p_lightGunList[0]->GetUSBHIDInfo ();
        isUSB = true;
        ui->comPortLineEdit->setInputMask (TESTHIDMASK);
    }

    if(hid_init ())
    {
        const wchar_t* errorWChar = hid_error(p_hidConnection);
        QString errorMSG = QString::fromWCharArray(errorWChar);
        QMessageBox::critical (this, "HID API Failed to Init", "The USB HID API failed to init. Something is really wrong.\nError Message: "+errorMSG);
        return;
    }
}

testComPortWindow::~testComPortWindow()
{
    if(isPortOpen)
        serialComPort.close ();

    if(tcpConnected)
        p_testSocket->close ();

    if(tcpInit)
        delete p_testSocket;

    delete ui;
}



void testComPortWindow::on_savedLightGunsComboBox_currentIndexChanged(int index)
{
    if(index != -1)
        currentLG = index;

    outputConnection = p_comDeviceList->p_lightGunList[index]->GetOutputConnection();

    if(outputConnection == SERIALPORT)
    {
        comPortName = p_comDeviceList->p_lightGunList[index]->GetComPortString();
        comPortNum = p_comDeviceList->p_lightGunList[index]->GetComPortNumber();
        comPortBaud = p_comDeviceList->p_lightGunList[index]->GetComPortBaud ();
        comPortDataBits = p_comDeviceList->p_lightGunList[index]->GetComPortDataBits ();
        comPortParity = p_comDeviceList->p_lightGunList[index]->GetComPortParity ();
        comPortStopBits = p_comDeviceList->p_lightGunList[index]->GetComPortStopBits ();
        comPortFlow = p_comDeviceList->p_lightGunList[index]->GetComPortFlow ();

        //If Going From USB HID to Serial Port, then Clear Line Edit
        if(isUSB)
            ui->comPortLineEdit->clear ();

        isUSB = false;

        //Clear USB HID Hex Mask
        ui->comPortLineEdit->clearMask ();
    }
    else if(outputConnection == USBHID)
    {
        //If Going From Serial Port to USB HID, then Clear Line Edit
        if(!isUSB)
            ui->comPortLineEdit->clear ();

        lgHIDInfo = p_comDeviceList->p_lightGunList[index]->GetUSBHIDInfo ();
        isUSB = true;

        //Set Hex Mask
        ui->comPortLineEdit->setInputMask (TESTHIDMASK);
    }
    else if(outputConnection == TCP)
    {
        //If Going From USB HID to Serial Port, then Clear Line Edit
        if(isUSB)
            ui->comPortLineEdit->clear ();

        isUSB = false;

        //Clear USB HID Hex Mask
        ui->comPortLineEdit->clearMask ();
    }
}




void testComPortWindow::on_connectPushButton_clicked()
{
    if(outputConnection == SERIALPORT)
    {
        isPortOpen = ConnectComPort();

        if(isPortOpen)
        {
            ui->connectPushButton->setEnabled (false);
            ui->disconnectPushButton->setEnabled (true);
        }
        else
        {
            ui->connectPushButton->setEnabled (true);
            ui->disconnectPushButton->setEnabled (false);
            QSerialPort::SerialPortError portError = serialComPort.error();
            QString critMessage = "Can not open the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". Make sure a light gun is on that port number. Serial Port Error: "+QString::number(portError);
            QMessageBox::critical (nullptr, "Serial Port Connection Failed", critMessage, QMessageBox::Ok);
        }
    }
    else if(outputConnection == USBHID)
    {
        isUSBConnected = ConnectUSBHID();

        if(isUSBConnected)
        {
            ui->connectPushButton->setEnabled (false);
            ui->disconnectPushButton->setEnabled (true);
        }
        else
        {
            ui->connectPushButton->setEnabled (true);
            ui->disconnectPushButton->setEnabled (false);
        }
    }
    else if(outputConnection == TCP)
    {
        tcpPort = p_comDeviceList->p_lightGunList[currentLG]->GetTCPPort ();

        ConnectTCP();
    }

}


void testComPortWindow::on_disconnectPushButton_clicked()
{
    if(outputConnection == SERIALPORT)
    {
        serialComPort.close ();
        isPortOpen = false;
    }
    else if(outputConnection == USBHID)
    {
        hid_close(p_hidConnection);
        isUSBConnected = false;
    }
    else if(outputConnection == TCP)
        p_testSocket->close ();

    ui->connectPushButton->setEnabled (true);
    ui->disconnectPushButton->setEnabled (false);
}


void testComPortWindow::on_sendDataPushButton_clicked()
{
    qint64 bytesWritten;
    QString portData = ui->comPortLineEdit->text();

    if(isPortOpen && outputConnection == SERIALPORT)
    {
        QByteArray portDataBA = portData.toUtf8 ();

        //qDebug() << portDataBA;
        //qDebug() << portDataBA.toHex(' ');

        bytesWritten = serialComPort.write (portDataBA);

        if(bytesWritten != portDataBA.size())
        {
            QString size = QString::number (portDataBA.size());
            QString byteWrit = QString::number (bytesWritten);
            QString critMessage = "Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". Failed to write the data.\nBytes Written: "+byteWrit+"\nData Size: "+size;
            QMessageBox::critical (nullptr, "Serial Port Failed Write", critMessage, QMessageBox::Ok);
        }
    }
    else if(isUSBConnected && outputConnection == USBHID)
    {
        if (portData.length() % 2 != 0)
        {
            QString critMessage = "The hex values, need to be an even number. As two hex values equal one byte. Need to write in full bytes.";
            QMessageBox::critical (nullptr, "Odd Number of Hex Values", critMessage, QMessageBox::Ok);
            return;
        }

        //Convert String to Hex QByteArray
        QByteArray cpBA = QByteArray::fromHex(portData.toUtf8());
        const unsigned char *constDataPtr = reinterpret_cast<const unsigned char*>(cpBA.constData());
        std::size_t size = cpBA.size();
        qint16 bytesWritten = hid_write(p_hidConnection, constDataPtr, size);

        if(bytesWritten == -1)
        {
            const wchar_t* errorWChar = hid_error(p_hidConnection);
            QString errorMSG = QString::fromWCharArray(errorWChar);
            QString critMessage = "The USB HID write failed to the device.\nError Message: "+errorMSG;
            QMessageBox::critical (nullptr, "USB HID Write Failed", critMessage, QMessageBox::Ok);
        }
    }
    else if(tcpConnected && outputConnection == TCP)
    {
        portData.append ("\n");
        QByteArray cpBA = portData.toUtf8 ();
        qint32 byteWrite = p_testSocket->write(cpBA);

        qDebug() << "TCP Write Data:" << cpBA.toStdString () << "byteWrite" << byteWrite;

        if(byteWrite == -1)
            qDebug() << "TCP Server Write Total Fail, got -1";
        else if(byteWrite != cpBA.size())
        {
            if (!p_testSocket->waitForBytesWritten(500))
                qDebug() << "Failed to write bytes to TCP server after wait:" << p_testSocket->errorString();
        }
    }
}

void testComPortWindow::SocketConnected()
{
    tcpConnected = true;
    ui->connectPushButton->setEnabled (false);
    ui->disconnectPushButton->setEnabled (true);
}

void testComPortWindow::SocketDisconnected()
{
    tcpConnected = false;
    ui->connectPushButton->setEnabled (true);
    ui->disconnectPushButton->setEnabled (false);
}

void testComPortWindow::TCPReadData()
{
    //Read the TCP Socket Data
    readData = p_testSocket->readAll ();

    qDebug() << "TCP Read Data:" << readData.toStdString ();
}


bool testComPortWindow::ConnectComPort()
{
    serialComPort.setPortName (comPortName);
    serialComPort.setBaudRate ((QSerialPort::BaudRate) comPortBaud, QSerialPort::Output);
    serialComPort.setDataBits ((QSerialPort::DataBits) comPortDataBits);
    serialComPort.setParity ((QSerialPort::Parity) comPortParity);
    serialComPort.setStopBits ((QSerialPort::StopBits) comPortStopBits);
    serialComPort.setFlowControl ((QSerialPort::FlowControl) comPortFlow);

    bool isOpen = serialComPort.open (QIODevice::WriteOnly);

    return isOpen;
}

bool testComPortWindow::ConnectUSBHID()
{
    lgHIDInfo = p_comDeviceList->p_lightGunList[currentLG]->GetUSBHIDInfo ();

    QByteArray pathBA = lgHIDInfo.path.toUtf8();

    char* pathPtr = pathBA.data();

    p_hidConnection = hid_open_path(pathPtr);

    if(!p_hidConnection)
    {
        //Failed to Open USB HID
        const wchar_t* errorWChar = hid_error(p_hidConnection);
        QString errorMSG = QString::fromWCharArray(errorWChar);
        QString critMessage = "The USB HID failed to connect for Light Gun: "+QString::number(currentLG)+"\nError Message: "+errorMSG;
        QMessageBox::critical (nullptr, "USB HID Failed to Open", critMessage, QMessageBox::Ok);
        return false;
    }
    else
        return true;

    return false;
}

void testComPortWindow::testComPortWindow::ConnectTCP()
{
    if(!tcpInit)
    {
        p_testSocket = new QTcpSocket(this);
        tcpInit = true;

        connect(p_testSocket,SIGNAL(connected()), this, SLOT(SocketConnected()));
        connect(p_testSocket,SIGNAL(disconnected()), this, SLOT(SocketDisconnected()));
        connect(p_testSocket,SIGNAL(readyRead()), this, SLOT(TCPReadData()));
    }

    //Set the Address for the TCP Socket
    p_testSocket->connectToHost (TCPHOSTNAME, tcpPort);

    //Wait for Connection
    p_testSocket->waitForConnected (TIMETOWAIT);
}

