#ifndef HOOKCOMPORTWIN_H
#define HOOKCOMPORTWIN_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QHostAddress>

#include <QByteArray>


#include "../Global.h"

#include <Windows.h>

#include <Windows.h>
#include "hidapi_winapi.h"


class HookCOMPortWin : public QObject
{
    Q_OBJECT

public:
    explicit HookCOMPortWin(QObject *parent = nullptr);
    ~HookCOMPortWin();

    //Says if the TCP is Connected or Not
    bool IsTCPConnected(quint16 port);
    //Says if the TCP is Trying to Connect to a Server
    bool IsTCPConnecting(quint16 port);
    //Send the TCP Port Number
    quint16 TCPPortNumber();

    bool IsCOMConnected(quint8 comPortNum);

    bool IsUSBHIDConnected(quint8 playerNum);

    //Reconnect to Exsiting COM Port
    bool Reconnect(quint8 comPortNum);

public slots:

    //Connect to COM Port
    void Connect(const quint8 &playerNum, const quint8 &comPortNum, const QString &comPortName, const qint32 &comPortBaud, const quint8 &comPortData, const quint8 &comPortParity, const quint8 &comPortStop, const quint8 &comPortFlow, const QString &comPortPath, const bool &isWriteOnly);

    //Disconnect to COM Port
    void Disconnect(const quint8 &playerNum, const quint8 &comPortNum);

    //Write Data to COM Port
    void WriteData(const quint8 &comPortNum, const QByteArray &writeData);

    //Read Data from COM Port
    //void ReadData();

    //Disconnect All Open COM Ports & USB HIDs
    void DisconnectAll();

    //Connect to an USB HID
    void ConnectHID(const quint8 &playerNum, const HIDInfo &lgHIDInfo);

    //Disconnect the USB HID Connection
    void DisconnectHID(const quint8 &playerNum);

    //Write Data to USB HID
    void WriteDataHID(const quint8 &playerNum, const QByteArray &writeData);

    //Slot to get setting for to Bypass Serial Port Write Checks or Not
    void SetBypassSerialWriteChecks(const bool &bypassSPWC);

    //Slot to for setting the Bypass COM Port Connection Fail Warning Pop-Up
    void SetBypassCOMPortConnectFailWarning(const bool &bypassCPCFW);

    //Connect to a TCP Server
    void ConnectTCP(const quint16 &port);
    void DisconnectTCP();

    void FoundTCPServer();
    void LostTCPServer();
    void TCPServerRead();

    void FoundTCPServer1();
    void LostTCPServer1();
    void TCPServerRead1();

    void WriteTCP(const QByteArray &writeData);

    void WriteTCP1(const QByteArray &writeData);


signals:

    //Signal Used to Display Error Message from COM Port
    void ErrorMessage(const QString &title, const QString &errorMsg);

    //When a Light Gun Connects to Output Interface
    void LightGunConnected(quint8 lgNum);

    //When a Light Gun Disconnects from a Output Interface
    void LightGunDisconnected(quint8 lgNum);

private:

    ///////////////////////////////////////////////////////////////////////////

    //How Many COM Ports Open
    quint8                          numPortOpen;

    //If 1 or More COM Ports Open
    bool                            isPortOpen;

    //Bool List to Keep Track on What COM Ports that are Open
    bool                            comPortOpen[MAXCOMPORTS];

    //Pointer Array of Serial COM Ports
    HANDLE                          comPortArray[MAXCOMPORTS];

    //COM Port Data
    QList<DCB>                      comPortDCBList;
    QList<COMMTIMEOUTS>             comPortTOList;
    QList<quint8>                   comPortPlayerList;
    QList<LPCWSTR>                  comPortLPCList;

    //Error char Array
    LPWSTR                          messageBuffer = nullptr;

    //Pointer Array of USB HIDs
    hid_device                      *p_hidConnection[MAXGAMEPLAYERS];

    //Bool List to Keep Track on What USB HID that are Open
    bool                            hidOpen[MAXGAMEPLAYERS];

    //HID Info Array of the HIDs being Used
    HIDInfo                         hidInfoArray[MAXGAMEPLAYERS];

    //Debug Setting to Bypass Serial Port Write Checks
    bool                            bypassSerialWriteChecks;

    //Bypass Pop-up Error, when Serial Port Cannot Connect on INI Side
    bool                            bypassCOMPortConnectFailWarning;


    bool                            noLightGunWarning[MAXCOMPORTS];

    // TCP Server 0
    quint16                         connectedTCPPort;
    bool                            isTCPConnected;
    bool                            isTCPConnecting;
    QTcpSocket                      *p_tcpServer;
    QByteArray                      readDataTCP;

    // TCP Server 1
    quint16                         connectedTCPPort1;
    bool                            isTCPConnected1;
    bool                            isTCPConnecting1;
    QTcpSocket                      *p_tcpServer1;
    QByteArray                      readDataTCP1;



};

#endif // HOOKCOMPORTWIN_H
