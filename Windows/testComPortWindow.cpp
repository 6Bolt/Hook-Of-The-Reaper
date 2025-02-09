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
        tempQS.append (p_comDeviceList->p_lightGunList[i]->GetComPortString());
        ui->savedLightGunsComboBox->insertItem(i,tempQS);
    }

    ui->savedLightGunsComboBox->setCurrentIndex(0);

    comPortName = p_comDeviceList->p_lightGunList[0]->GetComPortString();
    comPortNum = p_comDeviceList->p_lightGunList[0]->GetComPortNumber();
    comPortBaud = p_comDeviceList->p_lightGunList[0]->GetComPortBaud ();
    comPortDataBits = p_comDeviceList->p_lightGunList[0]->GetComPortDataBits ();
    comPortParity = p_comDeviceList->p_lightGunList[0]->GetComPortParity ();
    comPortStopBits = p_comDeviceList->p_lightGunList[0]->GetComPortStopBits ();
    comPortFlow = p_comDeviceList->p_lightGunList[0]->GetComPortFlow ();



}

testComPortWindow::~testComPortWindow()
{
    if(isPortOpen)
        serialComPort.close ();
    delete ui;
}



void testComPortWindow::on_savedLightGunsComboBox_currentIndexChanged(int index)
{
    comPortName = p_comDeviceList->p_lightGunList[index]->GetComPortString();
    comPortNum = p_comDeviceList->p_lightGunList[index]->GetComPortNumber();
    comPortBaud = p_comDeviceList->p_lightGunList[index]->GetComPortBaud ();
    comPortDataBits = p_comDeviceList->p_lightGunList[index]->GetComPortDataBits ();
    comPortParity = p_comDeviceList->p_lightGunList[index]->GetComPortParity ();
    comPortStopBits = p_comDeviceList->p_lightGunList[index]->GetComPortStopBits ();
    comPortFlow = p_comDeviceList->p_lightGunList[index]->GetComPortFlow ();
}




void testComPortWindow::on_connectPushButton_clicked()
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


void testComPortWindow::on_disconnectPushButton_clicked()
{
    serialComPort.close ();
    ui->connectPushButton->setEnabled (true);
    ui->disconnectPushButton->setEnabled (false);
    isPortOpen = false;
}


void testComPortWindow::on_sendDataPushButton_clicked()
{
    if(isPortOpen)
    {
        qint64 bytesWritten;
        QString portData = ui->comPortLineEdit->text();
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





