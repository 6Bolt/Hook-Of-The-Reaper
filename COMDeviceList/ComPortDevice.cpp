#include "ComPortDevice.h"

//Constructors
ComPortDevice::ComPortDevice(QString cpdString, quint8 cpdNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow)
{
    comPortDeviceName = cpdString;
    comPortDeviceNum = cpdNumber;
    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

}

ComPortDevice::ComPortDevice(ComPortDevice const &cpdMember)
{
    comPortDeviceName = cpdMember.comPortDeviceName;
    comPortDeviceNum = cpdMember.comPortDeviceNum;
    comPortNum = cpdMember.comPortNum;
    comPortString = cpdMember.comPortString;
    comPortInfo = cpdMember.comPortInfo;
    comPortBaud = cpdMember.comPortBaud;
    comPortDataBits = cpdMember.comPortDataBits;
    comPortParity = cpdMember.comPortParity;
    comPortStopBits = cpdMember.comPortStopBits;
    comPortFlow = cpdMember.comPortFlow;

}


//Set Member Functions

void ComPortDevice::SetComPortDeviceName(QString cpdString)
{
    comPortDeviceName = cpdString;
}

void ComPortDevice::SetComPortDeviceNumber(quint8 cpdNumber)
{
    comPortDeviceNum = cpdNumber;
}

void ComPortDevice::SetComPortNumber(quint8 cpNumber)
{
    comPortNum = cpNumber;
}

void ComPortDevice::SetComPortString(QString cpString)
{
    comPortString = cpString;
}

void ComPortDevice::SetComPortInfo(QSerialPortInfo cpInfo)
{
    comPortInfo = cpInfo;
}

void ComPortDevice::SetComPortBaud(qint32 cpBaud)
{
    comPortBaud = cpBaud;
}

void ComPortDevice::SetComPortDataBits(quint8 cpDataBits)
{
    comPortDataBits = cpDataBits;
}

void ComPortDevice::SetComPortParity(quint8 cpParity)
{
    comPortParity = cpParity;
}

void ComPortDevice::SetComPortStopBits(quint8 cpStopBits)
{
    comPortStopBits = cpStopBits;
}

void ComPortDevice::SetComPortFlow(quint8 cpFlow)
{
    comPortFlow = cpFlow;
}



//Get Member Functions

QString ComPortDevice::GetComPortDeviceName()
{
    return comPortDeviceName;
}

quint8 ComPortDevice::GetComPortDeviceNumber()
{
    return comPortDeviceNum;
}

quint8 ComPortDevice::GetComPortNumber()
{
    return comPortNum;
}

QString ComPortDevice::GetComPortString()
{
    return comPortString;
}

QSerialPortInfo ComPortDevice::GetComPortInfo()
{
    return comPortInfo;
}

qint32 ComPortDevice::GetComPortBaud()
{
    return comPortBaud;
}

quint8 ComPortDevice::GetComPortDataBits()
{
    return comPortDataBits;
}

quint8 ComPortDevice::GetComPortParity()
{
    return comPortParity;
}

quint8 ComPortDevice::GetComPortStopBits()
{
    return comPortStopBits;
}

quint8 ComPortDevice::GetComPortFlow()
{
    return comPortFlow;
}



void ComPortDevice::CopyComDevice(ComPortDevice const &lgMember)
{

    comPortDeviceName = lgMember.comPortDeviceName;
    comPortDeviceNum = lgMember.comPortDeviceNum;

    comPortNum = lgMember.comPortNum;
    comPortString = lgMember.comPortString;
    comPortInfo = lgMember.comPortInfo;
    comPortBaud = lgMember.comPortBaud;
    comPortDataBits = lgMember.comPortDataBits;
    comPortParity = lgMember.comPortParity;
    comPortStopBits = lgMember.comPortStopBits;
    comPortFlow = lgMember.comPortFlow;
}




