#ifndef COMDEVICELIST_H
#define COMDEVICELIST_H

#include "../Global.h"

#include <QDir>
#include <QMessageBox>


#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QSerialPortInfo>


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
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, quint16 maNumber, quint16 rvNumber);
    //Normal Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow);
    //For MX24 Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, bool dipSwitchSet, quint8 dipSwitchNumber);
    //For JB Gun4IR Light Gun
    void            AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, quint8 analStrength);

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

    //Close the COM Port Connect on When Game Ends or Leave Open
    bool            GetCloseComPortGameExit();
    void            SetCloseComPortGameExit(bool ccpGameExit);

    //Copies Used Dip Players Array
    void            CopyUsedDipPlayersArray(bool *targetArray, quint8 size);

    ///////////////////////////////////////////////////////////////////////////


    //Light Guns List
    LightGun*           p_lightGunList[MAXCOMPORTS];
    //COM Devices List
    ComPortDevice*      p_comPortDeviceList[MAXCOMPORTS];

    //Available Dip Switch Players for MX24
    bool                usedDipPlayers[DIPSWITCH_NUMBER];


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


    //Settings
    bool                useDefaultLGFirst;
    bool                useMultiThreading;
    quint32             refreshTimeDisplay;
    bool                closeComPortGameExit;




};

#endif // COMDEVICELIST_H
