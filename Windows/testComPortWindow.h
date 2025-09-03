#ifndef TESTCOMPORTWINDOW_H
#define TESTCOMPORTWINDOW_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QByteArray>
#include <QTcpSocket>

#include "../COMDeviceList/ComDeviceList.h"

#include <Windows.h>
#include "hidapi_winapi.h"

namespace Ui {
class testComPortWindow;
}

class testComPortWindow : public QDialog
{
    Q_OBJECT

public:
    explicit testComPortWindow(ComDeviceList *cdList, QWidget *parent = nullptr);
    ~testComPortWindow();

private slots:

    void on_savedLightGunsComboBox_currentIndexChanged(int index);

    void on_connectPushButton_clicked();

    void on_disconnectPushButton_clicked();

    void on_sendDataPushButton_clicked();

    void SocketConnected();
    void SocketDisconnected();
    void TCPReadData();


private:

    bool ConnectComPort();

    bool ConnectUSBHID();

    void ConnectTCP();

    Ui::testComPortWindow *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList *p_comDeviceList;

    QSerialPort serialComPort;

    bool isPortOpen;

    //Number of Light Gun in the List
    quint8 numberLightGuns;

    //Current Light Gun Number
    quint8 currentLG;

    //Current LG Connection Method
    quint8 outputConnection;

    //Light Gun Data
    QString lightGunName;
    quint8 lightGunNum;
    quint8 comPortNum;
    QString comPortName;
    qint32 comPortBaud;
    quint8 comPortDataBits;
    quint8 comPortParity;
    quint8 comPortStopBits;
    quint8 comPortFlow;

    //Light Gun USB HID Data
    HIDInfo lgHIDInfo;
    bool isUSB;
    hid_device *p_hidConnection;
    bool isUSBConnected;

    //Light Gun TCP Socket Data
    quint16 tcpPort;
    bool tcpConnected;
    bool tcpInit;
    QTcpSocket *p_testSocket;
    QByteArray readData;

};

#endif // TESTCOMPORTWINDOW_H
