#include "LightGun.h"
#include "../Global.h"

//Constructors

LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, quint16 maNumber, quint16 rvNumber)
{
    defaultLightGun = lgDefault;
    defaultLightGunNum = dlgNum;

    lightGunName = lgName;
    lightGunNum = lgNumber;

    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

    maxAmmo = maNumber;
    reloadValue = rvNumber;

    maxAmmoSet = true;
    reloadValueSet = true;

}

LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow)
{
    defaultLightGun = lgDefault;
    defaultLightGunNum = dlgNum;

    lightGunName = lgName;
    lightGunNum = lgNumber;

    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

    if(defaultLightGun)
    {
        if(defaultLightGunNum == RS3_REAPER)
        {
            maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
            reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
            maxAmmoSet = true;
            reloadValueSet = true;
        }
        else
        {
            maxAmmoSet = false;
            reloadValueSet = false;
        }
    }
    else
    {
        maxAmmoSet = false;
        reloadValueSet = false;
    }
}

LightGun::LightGun(LightGun const &lgMember)
{
    defaultLightGun = lgMember.defaultLightGun;
    defaultLightGunNum = lgMember.defaultLightGunNum;

    lightGunName = lgMember.lightGunName;
    lightGunNum = lgMember.lightGunNum;

    comPortNum = lgMember.comPortNum;
    comPortString = lgMember.comPortString;
    comPortInfo = lgMember.comPortInfo;
    comPortBaud = lgMember.comPortBaud;
    comPortDataBits = lgMember.comPortDataBits;
    comPortParity = lgMember.comPortParity;
    comPortStopBits = lgMember.comPortStopBits;
    comPortFlow = lgMember.comPortFlow;

    if(lgMember.maxAmmoSet)
    {
        maxAmmo = lgMember.maxAmmo;
        maxAmmoSet = true;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
        maxAmmoSet = true;
    }
    else
    {
        maxAmmoSet = false;
    }

    if(lgMember.reloadValueSet)
    {
        reloadValue = lgMember.reloadValue;
        reloadValueSet = true;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
        reloadValueSet = true;
    }
    else
    {
        reloadValueSet = false;
    }

}

//public member functions


//Set Member Functions

void LightGun::SetDefaultLightGun(bool lgDefault)
{
    defaultLightGun = lgDefault;
}

void LightGun::SetDefaultLightGunNumer(quint8 dlgNum)
{
    defaultLightGunNum = dlgNum;
}

void LightGun::SetLightGunName(QString lgnString)
{
    lightGunName = lgnString;
}

void LightGun::SetLightGunNumber(quint8 lgNumber)
{
    lightGunNum = lgNumber;
}

void LightGun::SetComPortNumber(quint8 cpNumber)
{
    comPortNum = cpNumber;
}

void LightGun::SetComPortString(QString cpString)
{
    comPortString = cpString;
}

void LightGun::SetComPortInfo(QSerialPortInfo cpInfo)
{
    comPortInfo = cpInfo;
}

void LightGun::SetComPortBaud(qint32 cpBaud)
{
    comPortBaud = cpBaud;
}

void LightGun::SetComPortDataBits(quint8 cpDataBits)
{
    comPortDataBits = cpDataBits;
}

void LightGun::SetComPortParity(quint8 cpParity)
{
    comPortParity = cpParity;
}

void LightGun::SetComPortStopBits(quint8 cpStopBits)
{
    comPortStopBits = cpStopBits;
}

void LightGun::SetComPortFlow(quint8 cpFlow)
{
    comPortFlow = cpFlow;
}

void LightGun::SetMaxAmmo(quint8 maNumber)
{
    maxAmmo = maNumber;
    maxAmmoSet = true;
}

void LightGun::SetReloadValue(quint8 rvNumber)
{
    reloadValue = rvNumber;
    reloadValueSet = true;
}


//Get Member Functions

bool LightGun::GetDefaultLightGun()
{
    return defaultLightGun;
}

quint8 LightGun::GetDefaultLightGunNumber()
{
    return defaultLightGunNum;
}

QString LightGun::GetLightGunName()
{
    return lightGunName;
}

quint8 LightGun::GetLightGunNumber()
{
    return lightGunNum;
}

quint8 LightGun::GetComPortNumber()
{
    return comPortNum;
}

QString LightGun::GetComPortString()
{
    return comPortString;
}

QSerialPortInfo LightGun::GetComPortInfo()
{
    return comPortInfo;
}

qint32 LightGun::GetComPortBaud()
{
    return comPortBaud;
}

quint8 LightGun::GetComPortDataBits()
{
    return comPortDataBits;
}

quint8 LightGun::GetComPortParity()
{
    return comPortParity;
}

quint8 LightGun::GetComPortStopBits()
{
    return comPortStopBits;
}

quint8 LightGun::GetComPortFlow()
{
    return comPortFlow;
}

quint8 LightGun::GetMaxAmmo()
{
    return maxAmmo;
}

quint8 LightGun::GetReloadValue()
{
    return reloadValue;
}



bool LightGun::IsMaxAmmoSet()
{
    return maxAmmoSet;
}

bool LightGun::IsReloadValueSet()
{
    return reloadValueSet;
}




void LightGun::CopyLightGun(LightGun const &lgMember)
{
    defaultLightGun = lgMember.defaultLightGun;
    defaultLightGunNum = lgMember.defaultLightGunNum;

    lightGunName = lgMember.lightGunName;
    lightGunNum = lgMember.lightGunNum;

    comPortNum = lgMember.comPortNum;
    comPortString = lgMember.comPortString;
    comPortInfo = lgMember.comPortInfo;
    comPortBaud = lgMember.comPortBaud;
    comPortDataBits = lgMember.comPortDataBits;
    comPortParity = lgMember.comPortParity;
    comPortStopBits = lgMember.comPortStopBits;
    comPortFlow = lgMember.comPortFlow;

    maxAmmoSet = lgMember.maxAmmoSet;
    reloadValueSet = lgMember.reloadValueSet;

    if(maxAmmoSet)
    {
        maxAmmo = lgMember.maxAmmo;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
        maxAmmoSet = true;
    }

    if(reloadValueSet)
    {
        reloadValue = lgMember.reloadValue;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
        reloadValueSet = true;
    }
}




//Default Light Gun Commands for Certain Signals
QStringList LightGun::OpenComPortCommands()
{
    QStringList tempSL;
    tempSL << "ZS";
    tempSL << "ZZ";
    return tempSL;
}

QStringList LightGun::CloseComPortCommands()
{
    QStringList tempSL;
    tempSL << "ZX";
    return tempSL;
}

QStringList LightGun::DamageCommands()
{
    QStringList tempSL;
    tempSL << "ZZ";
    return tempSL;
}

QStringList LightGun::RecoilCommands()
{
    QStringList tempSL;
    tempSL << "Z5";
    return tempSL;
}

QStringList LightGun::ReloadCommands()
{
    QStringList tempSL;
    tempSL << "Z6";
    return tempSL;
}

QStringList LightGun::AmmoCommands()
{
    QStringList tempSL;
    tempSL << "Z5";
    return tempSL;
}

QStringList LightGun::AmmoValueCommands(quint16 ammoValue)
{
    quint16 tempAV;
    QString tempAVS;
    QStringList tempSL;

    if(ammoValue > maxAmmo)
        tempAV = maxAmmo;
    else
        tempAV = ammoValue;

    tempAVS = "Z"+QString::number (tempAV);
    tempSL << tempAVS;
    return tempSL;
}

QStringList LightGun::ShakeCommands()
{
    QStringList tempSL;
    tempSL << "ZZ";
    return tempSL;
}

QStringList LightGun::AutoLEDCommands()
{
    QStringList tempSL;
    tempSL << "ZR";
    return tempSL;
}

QStringList LightGun::AspectRatio16b9Commands()
{
    QStringList tempSL;
    tempSL << "ZW";
    return tempSL;
}

QStringList LightGun::AspectRatio4b3Commands()
{
    QStringList tempSL;
    tempSL << "ZN";
    return tempSL;
}

QStringList LightGun::JoystickModeCommands()
{
    QStringList tempSL;
    tempSL << "ZJ";
    return tempSL;
}


QStringList LightGun::MouseAndKeyboardModeCommands()
{
    QStringList tempSL;
    tempSL << "ZM";
    return tempSL;
}













