#ifndef HOOKCOMPORT_H
#define HOOKCOMPORT_H

#include <QObject>
#include <QMap>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>


#include <QByteArray>


#include "../Global.h"

class HookCOMPort : public QObject
{
    Q_OBJECT

public:
    explicit HookCOMPort(QObject *parent = nullptr);
    ~HookCOMPort();


public slots:

    //Connect to COM Port
    void Connect(const quint8 &comPortNum, const QString &comPortName, const qint32 &comPortBaud, const quint8 &comPortData, const quint8 &comPortParity, const quint8 &comPortStop, const quint8 &comPortFlow);

    //Disconnect to COM Port
    void Disconnect(const quint8 &comPortNum);

    //Write Data to COM Port
    void WriteData(const quint8 &comPortNum, const QByteArray &writeData);

    //Read Data from COM Port
    void ReadData();

    //Disconnect All Open COM Ports
    void DisconnectAll();

signals:

    //Signal Used to Move Read Data to Hooker Engine
    void ReadDataSig(const quint8 &comPortNum, const QByteArray &readData);

    //Signal Used to Display Error Message from COM Port
    void ErrorMessage(const QString &title, const QString &errorMsg);

private slots:



private:

    ///////////////////////////////////////////////////////////////////////////

    //How Many COM Ports Open
    quint8                          numPortOpen;

    //If 1 or More COM Ports Open
    bool                            isPortOpen;

    //Bool List to Keep Track on What COM Ports that are Open
    bool                            comPortOpen[MAXCOMPORTS];

    //Pointer Array of Serial COM Ports
    QSerialPort                     *p_ComPortArray[MAXCOMPORTS];


};

#endif // HOOKCOMPORT_H
