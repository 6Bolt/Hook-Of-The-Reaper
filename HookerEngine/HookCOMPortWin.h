#ifndef HOOKCOMPORTWIN_H
#define HOOKCOMPORTWIN_H

#include <QObject>
#include <QDebug>


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

public slots:

    //Connect to COM Port
    void Connect(const quint8 &comPortNum, const QString &comPortName, const qint32 &comPortBaud, const quint8 &comPortData, const quint8 &comPortParity, const quint8 &comPortStop, const quint8 &comPortFlow, const QString &comPortPath, const bool &isWriteOnly);

    //Disconnect to COM Port
    void Disconnect(const quint8 &comPortNum);

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

signals:

    //Signal Used to Display Error Message from COM Port
    void ErrorMessage(const QString &title, const QString &errorMsg);

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

};

#endif // HOOKCOMPORTWIN_H
