#ifndef COMDEVICELIST_H
#define COMDEVICELIST_H



#include <QObject>
#include <QDir>
#include <QMessageBox>
#include <QApplication>


#include <QFile>
#include <QTextStream>
#include <QString>
#include <QSerialPortInfo>
#include <QList>
#include <QMap>
#include <QlockFile>

#include "../Global.h"

//#include "Windows.h"

//Light Gun Class
#include "LightGun.h"

//Light Controller Class
#include "LightController.h"

//Ultimarc PacDrive SDK
#include "PacDriveControl.h"

class ComDeviceList : public QObject
{
    Q_OBJECT

public:

    explicit ComDeviceList(QObject *parent = nullptr);


    ~ComDeviceList();

    //Adds a Light Gun In the List
    //Copy Light Gun
    void            AddLightGun(LightGun const &lgMember);
    //For RS3 Reaper
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, SupportedRecoils lgRecoils, LightGunSettings lgSet, bool disableLEDs,  quint8 largeAmmo, ReaperSlideData slideData);
    //Normal Light Gun & Fusion & Xena & X-Gunner & JB Gun4IR
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, SupportedRecoils lgRecoils, LightGunSettings lgSet);
    //For MX24 Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, bool dipSwitchSet, quint8 dipSwitchNumber, quint8 hcpNum, SupportedRecoils lgRecoils);
    //For OpenFire Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, SupportedRecoils lgRecoils, LightGunSettings lgSet, bool noDis, DisplayPriority displayP, DisplayOpenFire displayOF);
    //For Alien USB Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, SupportedRecoils lgRecoils, bool n2DDisplay, DisplayPriority displayP);
    //For AimTrak USB Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, quint16 rcDelay, SupportedRecoils lgRecoils);
    // For Sinden Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint16 port, quint8 player, quint8 recVolt, SupportedRecoils lgRecoils, LightGunSettings lgSet);
    //For Blamcon
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, SupportedRecoils lgRecoils, LightGunSettings lgSet, bool has2DigitDiplay, DisplayPriority displayP);

    //Adds a Light Controller
    void            AddLightController(LightController const &other);
    void            AddLightController(UltimarcData dataU);

    //Copies Available COM Ports
    void            CopyAvailableComPortsArray(bool *targetArray, quint8 size);

    //Get the Number of Certain Devices
    quint8          GetNumberLightGuns();
    quint8          GetNumberLightControllers();

    //Switch COM Ports, Used in Edit Window
    void            SwitchComPortsInList(quint8 oldPort, quint8 newPort);

    //Enable/Disable A COM Port in the availableCOMPorts Array
    void            ModifyComPortArray(quint8 index, bool valueBool);

    //Delete a Light Gun, Light Controller, or COM device
    void            DeleteLightGun(quint8 lgNumber);
    void            DeleteLightController(quint8 lcNumber);

    //Player's Light Gun Assignment, Deassignment, or Get Assignment
    bool            AssignPlayerLightGun(quint8 playerNum, quint8 lgNum);
    void            DeassignPlayerLightGun(quint8 playerNum);
    quint8          GetPlayerLightGunAssignment(quint8 playerNum);


    //Save or Load Light Guns to/from a File
    void            SaveLightGunList();
    void            LoadLightGunList();

    //Load V3 Light Gun Saved Data
    void            LoadLightGunListV3();


    //Save or Load Players Assignment to/from File
    void            SavePlayersAss();
    void            LoadPlayersAss();

    //Save or Load Settings to/from a File
    void            SaveSettings();
    void            LoadSettings();

    void            LoadSettingsV2();

    //Save or Load Light Controllers
    void            SaveLightControllersList();
    void            LoadLightControllersList();

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

    //Enable New Game File Creation
    bool            GetEnableNewGameFileCreation();
    void            SetEnableNewGameFileCreation(bool enableNGFC);

    //Get TCP Port Player Info
    qint8           GetTCPPortPlayerInfo(quint16 portNumber);
    void            SetTCPPortPlayerInfo(quint16 portNumber, quint8 playerInfo);
    bool            CheckTCPPortPlayer(quint16 portNumber, quint8 playerInfo);
    void            RemoveTCPPortPlayer(quint16 portNumber, quint8 playerInfo);

    //Update Light Gun Settings New and Old
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


public slots:

    //Handle Error Message Box from a different Thread
    void ErrorMessage(const QString &title, const QString &message);



signals:

    //Show Error Message Box in Main Thread, from LightCommand
    void ShowErrorMessage(const QString &title, const QString &message);


public:


    QList<HIDInfo>  GetLightGunHIDInfo();

    //Processes usage & usagePage from USB HID Data
    QString         ProcessHIDUsage(quint16 usagePage, quint16 usage);

    //Get Recoil Priority
    quint8*         GetRecoilPriority();

    //Get Light Controller Thread
    QThread*        GetLightCntlrThread() { return p_threadForLight; }

    ///////////////////////////////////////////////////////////////////////////


    //Light Guns List
    LightGun*           p_lightGunList[MAXCOMPORTS];

    //Light Controllers List
    LightController*    p_lightCntlrList[MAXLIGHTCONTROLLERS];

    //PacDrive Controller
    PacDriveControl*    p_pacDrive;

    //Available Dip Switch Players for MX24
    QMap<quint8,QList<bool>>    usedHubComDipPlayers;

    //Thread for Light Controller
    QThread*            p_threadForLight;

private:



    //Available Serial COM Ports
    bool                availableComPorts[MAXCOMPORTS];



    //Number of Light Guns & COM Devices
    quint8              numberLightGuns;
    quint8              numberLightCntrls;

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
    QString             lightCntlrsSaveFile;


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

    bool                saveLGAfterSettings;

    //TCP Server Port and Players
    //0 - Both P1 and P2 Taken, 1 - P1 Taken, 2 - P2 Taken
    QMap<quint16,quint8>    tcpPortPlayersMap;

};

#endif // COMDEVICELIST_H
