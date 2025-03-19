#ifndef HOOKCOMPORTWIN_H
#define HOOKCOMPORTWIN_H

#include <QObject>
#include <QDebug>


#include <QByteArray>


#include "../Global.h"

#include <Windows.h>

class HookCOMPortWin : public QObject
{
    Q_OBJECT
public:
    explicit HookCOMPortWin(QObject *parent = nullptr);
    ~HookCOMPortWin();

public slots:

    //Connect to COM Port
    void Connect(const quint8 &comPortNum, const QString &comPortName, const qint32 &comPortBaud, const quint8 &comPortData, const quint8 &comPortParity, const quint8 &comPortStop, const quint8 &comPortFlow, const bool &isWriteOnly);

    //Disconnect to COM Port
    void Disconnect(const quint8 &comPortNum);

    //Write Data to COM Port
    void WriteData(const quint8 &comPortNum, const QByteArray &writeData);

    //Read Data from COM Port
    //void ReadData();

    //Disconnect All Open COM Ports
    void DisconnectAll();


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


};

#endif // HOOKCOMPORTWIN_H
