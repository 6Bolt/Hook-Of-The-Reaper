#ifndef COMPORTDEVICE_H
#define COMPORTDEVICE_H


#include <QSerialPortInfo>


#include <QObject>
//#include <QDebug>



class ComPortDevice
{
public:
    //Constructor
    ComPortDevice(QString cpdString, quint8 cpdNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow);
    ComPortDevice(ComPortDevice const &cpdMember);


    //Set Functions
    void SetComPortDeviceName(QString cpdString);
    void SetComPortDeviceNumber(quint8 cpdNumber);
    void SetComPortNumber(quint8 cpNumber);
    void SetComPortString(QString cpString);
    void SetComPortInfo(QSerialPortInfo cpInfo);
    void SetComPortBaud(qint32 cpBaud);
    void SetComPortDataBits(quint8 cpDataBits);
    void SetComPortParity(quint8 cpParity);
    void SetComPortStopBits(quint8 cpStopBits);
    void SetComPortFlow(quint8 cpFlow);


    //Get Functions
    QString GetComPortDeviceName();
    quint8 GetComPortDeviceNumber();
    quint8 GetComPortNumber();
    QString GetComPortString();
    QSerialPortInfo GetComPortInfo();
    qint32 GetComPortBaud();
    quint8 GetComPortDataBits();
    quint8 GetComPortParity();
    quint8 GetComPortStopBits();
    quint8 GetComPortFlow();


    //Copies a COM Device - Used when Deleting a COM Device
    //To move higher up COM Devices Down 1
    void CopyComDevice(ComPortDevice const &lgMember);



private:


    ///////////////////////////////////////////////////////////////////////////

    //COM Device Info for the Serial COM Port
    QString             comPortDeviceName;
    quint8              comPortDeviceNum;
    quint8              comPortNum;
    QString             comPortString;
    QSerialPortInfo     comPortInfo;
    qint32              comPortBaud;
    quint8              comPortDataBits;
    quint8              comPortParity;
    quint8              comPortStopBits;
    quint8              comPortFlow;



};

#endif // COMPORTDEVICE_H
