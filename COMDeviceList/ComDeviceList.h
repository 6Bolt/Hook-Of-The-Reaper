#ifndef COMDEVICELIST_H
#define COMDEVICELIST_H

#include "../Global.h"

#include <QDir>
#include <QMessageBox>
#include <QApplication>

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QSerialPortInfo>
#include <QList>
#include <QMap>




#include "LightGun.h"
#include "ComPortDevice.h"

class ComDeviceList
{

public:
    ComDeviceList();
    ~ComDeviceList();

    //Adds a Light Gun In the List
    //Copy Light Gun
    void            AddLightGun(LightGun const &lgMember);
    //For RS3 Reaper
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, quint16 maNumber, quint16 rvNumber, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);
    //Normal Light Gun & Fusion & Blamcon
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);
    //For MX24 Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, bool dipSwitchSet, quint8 dipSwitchNumber, quint8 hcpNum, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);
    //For JB Gun4IR & OpenFire Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, quint8 analStrength, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);
    //For Alien USB Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);
    //For USB Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, quint16 rcDelay, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis);


    //Adds a COM Device in the List
    void            AddComPortDevice(ComPortDevice const &cpdMember);
    void            AddComPortDevice(QString cpdString, quint8 cpdNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow);

    //Copies Available COM Ports
    void            CopyAvailableComPortsArray(bool *targetArray, quint8 size);

    //Get the Number of Certain Devices
    quint8          GetNumberLightGuns();
    quint8          GetNumberComPortDevices();

    //Switch COM Ports, Used in Edit Window
    void            SwitchComPortsInList(quint8 oldPort, quint8 newPort);

    //Enable/Disable A COM Port in the availableCOMPorts Array
    void            ModifyComPortArray(quint8 index, bool valueBool);

    //Delete a Light Gun or COM device
    void            DeleteLightGun(quint8 lgNumber);
    void            DeleteComDevice(quint8 cdNumber);

    //Player's Light Gun Assignment, Deassignment, or Get Assignment
    bool            AssignPlayerLightGun(quint8 playerNum, quint8 lgNum);
    void            DeassignPlayerLightGun(quint8 playerNum);
    quint8          GetPlayerLightGunAssignment(quint8 playerNum);

    //Save or Load Light Guns to/from a File
    void            SaveLightGunList();
    void            LoadLightGunList();

    //Save or Load Players Assignment to/from File
    void            SavePlayersAss();
    void            LoadPlayersAss();

    //Save or Load COM Device to/from a File
    void            SaveComDeviceList();
    void            LoadComDeviceList();

    //Save or Load Settings to/from a File
    void            SaveSettings();
    void            LoadSettings();

    //Settings Get & Set Functions
    //Use Default Light Gun Files before INI Files
    bool            GetUseDefaultLGFirst();
    void            SetUseDefaultLGFirst(bool udlgFirst);

    //Use Multi-Threading
    bool            GetUseMultiThreading();
    void            SetUseMultiThreading(bool umThreading);

    //Time to Refresh Display Data
    quint32         GetRefreshTimeDisplay();
    void            SetRefreshTimeDisplay(quint32 rtDisplay);

    //Close the COM Port Connect on When Game Ends or Leave Open Setting
    bool            GetCloseComPortGameExit();
    void            SetCloseComPortGameExit(bool ccpGameExit);

    //Ignore Warning for Useless Default LG game files
    bool            GetIgnoreUselessDFLGGF();
    void            SetIgnoreUselessDFLGGF(bool ignoreUDFLGGF);

    //Ignore Serial Port Checks for Debug
    bool            GetSerialPortWriteCheckBypass();
    void            SetSerialPortWriteCheckBypass(bool spwCB);

    //Reaper - Disable Reaper's 5 LEDs
    bool            GetDisableReaperLEDs();
    void            SetDisableReaperLEDs(bool drLED);

    //Normal Display Priority
    void            GetDisplayPriority(bool *ammo, bool *life);
    void            SetDisplayPriority(bool ammo, bool life);

    //Other Display Priority
    bool            GetDisplayOtherPriority();
    void            SetDisplayOtherPriority(bool other);

    //OpenFire - Enable OpenFire Display Ammo & Life
    bool            GetDisplayAmmoAndLife(bool *displayLG, bool *displayLB, bool *displayLN);
    void            SetDisplayAmmoAndLife(bool displayAAL, bool displayLG, bool displayLB, bool displayLN);

    //Enable New Game File Creation
    bool            GetEnableNewGameFileCreation();
    void            SetEnableNewGameFileCreation(bool enableNGFC);

    //Enable and Delay for Reamper Ammo 0 Delay
    quint8          GetReaperAmmo0Delay(bool *isAmmo0DelayEnabled, quint16 *reaperHST);
    void            SetReaperAmmo0Delay(bool isAmmo0DelayEnabled, quint8 delayTime, quint16 reaperHST);

    //Update Light Gun Settings
    void            UpdateLightGunWithSettings();

    //Copies Used Dip Players Array
    void            CopyUsedDipPlayersArray(bool *targetArray, quint8 size, quint8 hubComPort);
    void            ChangeUsedDipPlayersArray(quint8 hubComPort, quint8 dipPN, bool value);
    bool            IsComDipPlayer(quint8 comNum);

    //Resets Light Gun, when Game Has Ended
    void            ResetLightgun();
    void            ResetLightGun(quint8 lgNeedReset);

    //Check is the USB Light Gun Already Exsits
    bool            CheckUSBPath(QString lgPath);
    bool            CheckUSBPath(QString lgPath, quint8 lgNumbeer);

    QList<HIDInfo>  GetLightGunHIDInfo();

    //Processes usage & usagePage from USB HID Data
    QString         ProcessHIDUsage(quint16 usagePage, quint16 usage);

    //Get Recoil Priority
    quint8*         GetRecoilPriority();

    ///////////////////////////////////////////////////////////////////////////


    //Light Guns List
    LightGun*           p_lightGunList[MAXCOMPORTS];
    //COM Devices List
    ComPortDevice*      p_comPortDeviceList[MAXCOMPORTS];

    //Available Dip Switch Players for MX24
    QMap<quint8,QList<bool>>    usedHubComDipPlayers;


private:



    //Available Serial COM Ports
    bool                availableComPorts[MAXCOMPORTS];



    //Number of Light Guns & COM Devices
    quint8              numberLightGuns;
    quint8              numberComPortDevices;

    //Player's Light Gun Assignment
    quint8              playersLightGun[MAXPLAYERLIGHTGUNS];

    //File and Dir
    QDir                currentPathDir;
    QString             currentPath;
    QString             dataPath;
    QString             iniPath;
    QString             iniMAMEPath;
    bool                dataDirExists;
    bool                iniDirExists;
    bool                iniMAMEDirExists;
    QString             settingsSaveFile;
    QString             lightGunsSaveFile;
    QString             comDevicesSaveFile;
    QString             playersAssSaveFile;


    //Settings
    bool                useDefaultLGFirst;
    bool                useMultiThreading;
    quint32             refreshTimeDisplay;
    bool                closeComPortGameExit;
    bool                ignoreUselessDLGGF;
    bool                bypassSerialWriteChecks;
    bool                disbleReaperLEDs;
    bool                displayAmmoPriority;
    bool                displayLifePriority;
    bool                displayOtherPriority;
    bool                enableNewGameFileCreation;
    bool                enableReaperAmmo0Delay;
    quint8              repearAmmo0Delay;
    quint16             reaperHoldSlideTime;

    quint8              userRecoilPriority[NUMBEROFRECOILS];

    //OpenFire Settings
    bool                displayAmmoLife;
    bool                displayAmmoLifeGlyphs;
    bool                displayAmmoLifeBar;
    bool                displayAmmoLifeNumber;


};

#endif // COMDEVICELIST_H
