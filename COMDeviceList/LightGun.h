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
#include <QThread>
#include <QtMath>
//#include <QDebug>

#include "../Global.h"



class LightGun : public QObject
{
    Q_OBJECT


public:
    //Constructors

    //RS3 Reaper
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, quint16 maNumber, quint16 rvNumber, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);
    //Normal Light Gun & Fusion & Blamcon
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);
    //Copy Light Gun
    explicit LightGun(LightGun const &lgMember);
    //MX24
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, bool dipSwitchSet, quint8 dipSwitchNumber, quint8 hubcpNumber, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);
    //JB Gun4IR & OpenFire
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, quint8 analStrength, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);
    //For Alien USB Light Gun
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);
    //For Ultimarc AimTrak USB Light Gun
    explicit LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, quint16 rcDelay, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);

    ~LightGun();

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
    void SetDisableReaperLEDs(bool disableRLED);
    void SetDisplayPriority(bool ammo, bool life);
    void SetDisplayOtherPriority(bool other);
    void SetDisplayAmmoAndLife(bool displayAAL, bool displayLG, bool displayLB, bool displayLN);
    void SetRecoilPriority(SupportedRecoils lgRecoils);
    void SetReloadOptions(bool reloadNR, bool reloadDis);
    void SetReaperAmmo0Delay(bool isAmmo0DelayEnabled, quint8 delayTime, quint16 reaperHST);


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
    bool IsRecoilDelay();
    QString GetComPortPath();
    bool GetDisableReaperLEDs();
    void GetDisplayPriority(bool *ammo, bool *life);
    bool GetDisplayOtherPriority();
    bool GetDisplayAmmoAndLife(bool *displayLG, bool *displayLB, bool *displayLN);
    quint16 GetDisplayRefresh(bool *isRDS);
    bool GetReloadNoRumble();
    bool GetReloadDisabled();
    quint8 GetReaperAmmo0Delay(bool *isAmmo0DelayEnabled, quint16 *reaperHST);


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
    QStringList RecoilValueCommands(bool *isSet, quint16 recoilValue);
    QStringList ReloadCommands(bool *isSet);
    QStringList ReloadValueCommands(bool *isSet, quint16 ammoValue);
    QStringList AmmoValueCommands(bool *isSet, quint16 ammoValue);
    QStringList ShakeCommands(bool *isSet);
    QStringList AutoLEDCommands(bool *isSet);
    QStringList AspectRatio16b9Commands(bool *isSet);
    QStringList AspectRatio4b3Commands(bool *isSet);
    QStringList JoystickModeCommands(bool *isSet);
    QStringList MouseAndKeyboardModeCommands(bool *isSet);
    //Rumble Recoil to Solenoid Recoil Command
    QStringList RecoilR2SCommands(bool *isSet);
    QStringList DisplayAmmoCommands(bool *isSet, quint16 ammoValue);
    QStringList DisplayLifeCommands(bool *isSet, quint16 lifeValue);
    QStringList DisplayOtherCommands(bool *isSet, quint16 otherValue);
    QStringList OffscreenButtonCommands(bool *isSet);
    QStringList OffscreenNormalShotCommands(bool *isSet);

    //Resets Light Gun Back to Starting Start when Game Ends
    void ResetLightGun();

    //Check USB Light Guns against Display Path
    //To make sure the Same USB is not used twice
    bool CheckUSBPath(QString lgPath);

    //Does Light Gun Support Recoil_Value Command
    bool IsRecoilValueSupported();

    //Does Light Gun Support Reload
    bool GetSupportedReload();

    //Get Recoil Priority
    quint8* GetRecoilPriority();

    //Get Recoil Priority for Hooker EngineFormat
    quint8* GetRecoilPriorityHE();

private:

    //Private Member Functions

    //Fills in Serial Port Info Struct with Qt Values
    void FillSerialPortInfo();

    //Init Reaper Ammo 0 (Z0) Timer
    void InitReaperAmmo0Timer();

private slots:

    //Time Out Function for the Reaper Ammo 0 (Z0) Timer
    void ReaperAmmo0TimeOut();


signals:

    void WriteCOMPort(const quint8 &cpNum, const QString &cpData);


private:

    //Private Varibles


    ///////////////////////////////////////////////////////////////////////////

    //Default Light Gun
    bool                defaultLightGun;
    quint8              defaultLightGunNum;


    //RS3 Reaper
    quint16             maxAmmo;
    quint16             reloadValue;
    bool                maxAmmoSet;
    bool                reloadValueSet;
    //Reaper 5LEDs
    bool                disableReaperLEDs;
    bool                isReaper5LEDsInited;
    //Repaer Ammo 0 Z0 Delay Buffer
    bool                enableReaperAmmo0Delay;
    quint8              repearAmmo0Delay;
    QTimer              *p_reaperAmmo0Timer;
    bool                isReaperAmmo0TimerInited;
    bool                isReaperTimerRunning;
    bool                isReaperSlideHeldBack;
    quint16             reaperHoldSlideTime;
    //MX24
    quint8              dipSwitchPlayerNumber;
    bool                isDipSwitchPlayerNumberSet;
    quint8              hubComPortNumber;
    //JB Gun4IR & OpenFire
    quint8              analogStrength;
    bool                isAnalogStrengthSet;

    //OpenFire
    bool                displayAmmoLife;
    bool                displayAmmoLifeGlyphs;
    bool                displayAmmoLifeBar;
    bool                displayAmmoLifeNumber;
    quint16             lifeBarMaxLife;

    //Light Gun Name & Number
    quint8              lightGunNum;
    QString             lightGunName;

    //Recoil Priority
    quint8              lgRecoilPriority[NUMBEROFRECOILS];
    quint8              recoilPriorityHE[NUMBEROFRECOILS];

    //Reload Options
    bool                reloadNoRumble;
    bool                reloadDisable;



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

    QString             currentPath;
    QString             dataPath;
    QString             defaultLGFilePath;

    //Commands QStringLists
    QStringList         openComPortCmds;
    QStringList         closeComPortCmds;
    QStringList         damageCmds;
    QStringList         recoilCmds;
    QStringList         recoilR2SCmds;
    QStringList         recoilValueCmds;
    QStringList         reloadCmds;
    QStringList         ammoValueCmds;
    QStringList         shakeCmds;
    QStringList         autoLedCmds;
    QStringList         aspect16x9Cmds;
    QStringList         aspect4x3Cmds;
    QStringList         joystickCmds;
    QStringList         keyMouseCmds;
    QStringList         displayAmmoCmds;
    QStringList         displayAmmoInitCmds;
    QStringList         displayLifeCmds;
    QStringList         displayLifeInitCmds;
    QStringList         displayOtherCmds;
    QStringList         displayOtherInitCmds;
    QStringList         offscreenButtonCmds;
    QStringList         offscreenNormalShotCmds;

    //Commands Bools
    bool                openComPortCmdsSet;
    bool                closeComPortCmdsSet;
    bool                damageCmdsSet;
    bool                recoilCmdsSet;
    bool                recoilR2SCmdsSet;
    bool                recoilValueCmdsSet;
    bool                reloadCmdsSet;
    bool                ammoValueCmdsSet;
    bool                shakeCmdsSet;
    bool                autoLedCmdsSet;
    bool                aspect16x9CmdsSet;
    bool                aspect4x3CmdsSet;
    bool                joystickCmdsSet;
    bool                keyMouseCmdsSet;
    bool                displayAmmoCmdsSet;
    bool                displayAmmoInitCmdsSet;
    bool                displayLifeCmdsSet;
    bool                displayLifeInitCmdsSet;
    bool                displayOtherCmdsSet;
    bool                displayOtherInitCmdsSet;
    bool                displayRefreshSet;
    bool                offscreenButtonCmdsSet;
    bool                offscreenNormalShotCmdsSet;

    //Ammo
    quint16             lastAmmoValue;
    quint16             reloadAmmoValue;
    quint8              reaper5LEDNumber;
    quint8              reaperBullets1LED;
    quint8              reaperBulletCount;
    bool                reaperLargeAmmo;
    quint8              reaperNumLEDOn;

    //Display
    bool                hasDisplayAmmoInited;
    bool                hasDisplayLifeInited;
    bool                hasDisplayOtherInited;
    bool                hasDisplayAmmoAndLifeInited;
    qint16              ammoDisplayValue;
    qint16              lifeDisplayValue;
    qint16              otherDisplayValue;

    quint16             displayRefresh;

    bool                displayAmmoPriority;
    bool                displayLifePriority;
    bool                displayOtherPriority;


};

#endif // LIGHTGUN_H
