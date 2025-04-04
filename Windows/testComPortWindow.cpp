#include "testComPortWindow.h"
#include "ui_testComPortWindow.h"

testComPortWindow::testComPortWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::testComPortWindow)
{
    QString tempQS;
    quint8 i;

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
        tempQS = QString::number(i+1);
        tempQS.append (": ");
        tempQS.append (p_comDeviceList->p_lightGunList[i]->GetLightGunName());
        tempQS.append (" on ");
        if(!p_comDeviceList->p_lightGunList[i]->IsLightGunUSB ())
            tempQS.append (p_comDeviceList->p_lightGunList[i]->GetComPortString());
        else
            tempQS.append ("USB");

        ui->savedLightGunsComboBox->insertItem(i,tempQS);
    }

    ui->savedLightGunsComboBox->setCurrentIndex(0);
    currentLG = 0;

    if(!p_comDeviceList->p_lightGunList[0]->IsLightGunUSB ())
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
    else
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
    delete ui;
}



void testComPortWindow::on_savedLightGunsComboBox_currentIndexChanged(int index)
{
    if(index != -1)
        currentLG = index;

    if(!p_comDeviceList->p_lightGunList[index]->IsLightGunUSB ())
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
    else
    {
        //If Going From Serial Port to USB HID, then Clear Line Edit
        if(!isUSB)
            ui->comPortLineEdit->clear ();

        lgHIDInfo = p_comDeviceList->p_lightGunList[index]->GetUSBHIDInfo ();
        isUSB = true;

        //Set Hex Mask
        ui->comPortLineEdit->setInputMask (TESTHIDMASK);
    }
}




void testComPortWindow::on_connectPushButton_clicked()
{
    if(!isUSB)
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
    else
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

}


void testComPortWindow::on_disconnectPushButton_clicked()
{
    if(!isUSB)
    {
        serialComPort.close ();
        isPortOpen = false;
    }
    else
    {
        hid_close(p_hidConnection);
        isUSBConnected = false;
    }

    ui->connectPushButton->setEnabled (true);
    ui->disconnectPushButton->setEnabled (false);
}


void testComPortWindow::on_sendDataPushButton_clicked()
{
    qint64 bytesWritten;
    QString portData = ui->comPortLineEdit->text();

    if(isPortOpen && !isUSB)
    {
        QByteArray portDataBA = portData.toUtf8 ();

        bytesWritten = serialComPort.write (portDataBA);

        if(bytesWritten != portDataBA.size())
        {
            QString size = QString::number (portDataBA.size());
            QString byteWrit = QString::number (bytesWritten);
            QString critMessage = "Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". Failed to write the data.\nBytes Written: "+byteWrit+"\nData Size: "+size;
            QMessageBox::critical (nullptr, "Serial Port Failed Write", critMessage, QMessageBox::Ok);
        }
    }
    else if(isUSBConnected && isUSB)
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



