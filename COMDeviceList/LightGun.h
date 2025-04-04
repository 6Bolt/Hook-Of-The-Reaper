#ifndef LIGHTGUN_H
#define LIGHTGUN_H

#include <QObject>
#include <QSerialPortInfo>
#include <QApplication>
#include <QObject>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QTimer>
//#include <QDebug>

#include "../Global.h"

class LightGun : public QObject
{
    Q_OBJECT


public:
    //Constructors

    //RS3 Reaper
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, quint16 maNumber, quint16 rvNumber);
    //Normal Light Gun & Fusion & Blamcon
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow);
    //Copy Light Gun
    explicit LightGun(LightGun const &lgMember);
    //MX24
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, bool dipSwitchSet, quint8 dipSwitchNumber, quint8 hubcpNumber);
    //JB Gun4IR & OpenFire
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, quint8 analStrength);
    //For Alien USB Light Gun
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, quint16 rcDelay);

    //Set Functions that Sets the Stated Variable
    void SetDefaultLightGun(bool lgDefault);
    void SetDefaultLightGunNumer(quint8 dlgNum);
    void SetLightGunName(QString lgnString);
    void SetLightGunNumber(quint8 lgNumber);
    void SetComPortNumber(quint8 cpNumber);
    void SetComPortString(QString cpString);
    void SetComPortInfo(QSerialPortInfo cpInfo);
    void SetComPortBaud(qint32 cpBaud);
    void SetComPortDataBits(quint8 cpDataBits);
    void SetComPortParity(quint8 cpParity);
    void SetComPortStopBits(quint8 cpStopBits);
    void SetComPortFlow(quint8 cpFlow);
    void SetMaxAmmo(quint16 maNumber);
    void SetReloadValue(quint16 rvNumber);
    void SetDipSwitchPlayerNumber(quint8 dsNumber);
    void SetAnalogStrength(quint8 analStrength);
    void SetHubComPortNumber(quint8 hcpNumber);
    void SetHIDInfo(HIDInfo hidInfoStruct);
    void SetRecoilDelay(quint16 rcDelay);

    //Get Functions that Gets the Stated Variable
    bool GetDefaultLightGun();
    quint8 GetDefaultLightGunNumber();
    QString GetLightGunName();
    quint8 GetLightGunNumber();
    quint8 GetComPortNumber();
    quint8 GetComPortNumberBypass();
    QString GetComPortString();
    QSerialPortInfo GetComPortInfo();
    SerialPortInfo GetSerialProtInfo();
    qint32 GetComPortBaud();
    quint8 GetComPortDataBits();
    quint8 GetComPortParity();
    quint8 GetComPortStopBits();
    quint8 GetComPortFlow();
    quint16 GetMaxAmmo();
    quint16 GetReloadValue();
    quint8 GetDipSwitchPlayerNumber(bool *isSet);
    quint8 GetDipSwitchPlayerNumber();
    bool GetIsDipSwitchPlayerNumberSet();
    quint8 GetAnalogStrength(bool *isSet);
    quint8 GetAnalogStrength();
    bool GetIsAnalogStrengthSet();
    quint8 GetHubComPortNumber();
    HIDInfo GetUSBHIDInfo();
    bool IsLightGunUSB();
    quint16 GetRecoilDelay();
    QString GetComPortPath();


    //If a Default Light Gun, is Needed Varibles Set
    bool IsMaxAmmoSet();
    bool IsReloadValueSet();


    //Copies a Light Gun - Used when Deleting a Light Gun
    //To move higher up Light guns Down 1
    void CopyLightGun(LightGun const &lgMember);


    //Load Default Light Gun Commands
    void LoadDefaultLGCommands();

    QStringList SplitLoadedCommands(QString commandList);

    //Sends Out the Needed Commands for a Default Light Gun
    //Used In the Hooker Engine
    QStringList OpenComPortCommands(bool *isSet);
    QStringList CloseComPortCommands(bool *isSet);
    QStringList DamageCommands(bool *isSet);
    QStringList RecoilCommands(bool *isSet);
    QStringList ReloadCommands(bool *isSet);
    QStringList AmmoCommands(bool *isSet);
    QStringList AmmoValueCommands(bool *isSet, quint16 ammoValue);
    QStringList DisplayAmmoCommands(bool *isSet, quint16 ammoValue);
    QStringList ShakeCommands(bool *isSet);
    QStringList AutoLEDCommands(bool *isSet);
    QStringList AspectRatio16b9Commands(bool *isSet);
    QStringList AspectRatio4b3Commands(bool *isSet);
    QStringList JoystickModeCommands(bool *isSet);
    QStringList MouseAndKeyboardModeCommands(bool *isSet);
    //Rumble Recoil to Solenoid Recoil Command
    QStringList RecoilR2SCommands(bool *isSet);

    //Resets Light Gun Back to Starting Start when Game Ends
    void ResetLightGun();

    //Check USB Light Guns against Display Path
    //To make sure the Same USB is not used twice
    bool CheckUSBPath(QString lgPath);

private slots:

    void ClearRecoilDelayBlock();

private:

    //Fills in Serial Port Info Struct with Qt Values
    void FillSerialPortInfo();

    //Init The Recoil Delay Timer
    void InitRecoilDelayTimer();

    ///////////////////////////////////////////////////////////////////////////

    //Default Light Gun
    bool                defaultLightGun;
    quint8              defaultLightGunNum;

    //RS3 Reaper
    quint16             maxAmmo;
    quint16             reloadValue;
    bool                maxAmmoSet;
    bool                reloadValueSet;
    //MX24
    quint8              dipSwitchPlayerNumber;
    bool                isDipSwitchPlayerNumberSet;
    quint8              hubComPortNumber;
    //JB Gun4IR
    quint8              analogStrength;
    bool                isAnalogStrengthSet;


    //Light Gun Name & Number
    quint8              lightGunNum;
    QString             lightGunName;

    //COM Port Info
    quint8              comPortNum;
    QString             comPortString;
    QSerialPortInfo     comPortInfo;
    qint32              comPortBaud;
    quint8              comPortDataBits;
    quint8              comPortParity;
    quint8              comPortStopBits;
    quint8              comPortFlow;
    SerialPortInfo      serialPortInfo;

    //USB Info
    bool                isUSBLightGun;
    HIDInfo             usbHIDInfo;
    bool                isRecoilDelaySet;
    quint16             recoilDelay;
    QTimer              *p_recoilDelayTimer;
    bool                blockRecoil;

    QString             currentPath;
    QString             dataPath;
    QString             defaultLGFilePath;

    //Commands QStringLists
    QStringList         openComPortCmds;
    QStringList         closeComPortCmds;
    QStringList         damageCmds;
    QStringList         recoilCmds;
    QStringList         recoilR2SCmds;
    QStringList         reloadCmds;
    QStringList         ammoCmds;
    QStringList         ammoValueCmds;
    QStringList         displayAmmoCmds;
    QStringList         shakeCmds;
    QStringList         autoLedCmds;
    QStringList         aspect16x9Cmds;
    QStringList         aspect4x3Cmds;
    QStringList         joystickCmds;
    QStringList         keyMouseCmds;

    bool                openComPortCmdsSet;
    bool                closeComPortCmdsSet;
    bool                damageCmdsSet;
    bool                recoilCmdsSet;
    bool                recoilR2SCmdsSet;
    bool                reloadCmdsSet;
    bool                ammoCmdsSet;
    bool                ammoValueCmdsSet;
    bool                displayAmmoCmdsSet;
    bool                shakeCmdsSet;
    bool                autoLedCmdsSet;
    bool                aspect16x9CmdsSet;
    bool                aspect4x3CmdsSet;
    bool                joystickCmdsSet;
    bool                keyMouseCmdsSet;

    quint16             lastAmmoValue;



};

#endif // LIGHTGUN_H
