#ifndef HOOKERENGINE_H
#define HOOKERENGINE_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QRegularExpression>


#include <QByteArray>
#include <QStringList>
#include <QTextStream>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QQueue>


#include "HookTCPSocket.h"
#include "GamePlayer.h"
#include "HookLight.h"

#ifdef Q_OS_WIN
#include "HookCOMPortWin.h"
#else
#include "HookCOMPort.h"
#endif

//For USB HID
#include <Windows.h>
#include "hidapi_winapi.h"

#include "../COMDeviceList/ComDeviceList.h"

class HookerEngine : public QObject
{
    Q_OBJECT

    //Threads for Multi-Threads
    QThread threadForTCPSocket;
    QThread threadForCOMPort;
    QThread* p_threadForLight;

public:
    explicit HookerEngine(ComDeviceList *cdList, bool displayGUI, QWidget *guiConnect, QObject *parent = nullptr);
    //HookerEngine(ComDeviceList *cdList, bool displayGUI, QWidget *guiConnect, QObject *parent = nullptr);
    ~HookerEngine();

    //Start & Stop the Hooker Engine
    void Start();
    void Stop();

    //Loads INI Game file for Checks
    bool LoadINIFileTest(QString fileNamePath);

    //Loads defaultLG Game file for Checks
    bool LoadLGFileTest(QString fileNamePath);

    //Closes All COM Port & USB HID Light Gun Connections
    void CloseAllLightGunConnections() { emit StopAllConnections(); }

    //Load Settings From COMDeviceList
    void LoadSettingsFromList();

    //Set-up Light Guns
    void SetUpLightGuns();

    //When Hook of the Reaper in Minimized in the tray, or on the Desktop
    void HOTRWindowState(bool isMin);

    //Load and Update Player Assignments Array
    void LoadUpdatePlayerAssignment();

public slots:

    //Read Data From The Serial Com Port (different thread)
    void ReadComPortSig(const quint8 &comPortNum, const QByteArray &readData);

    //Error Messages from the Serial COM Ports (different thread)
    void ErrorMessageCom(const QString &title, const QString &errorMsg);

    //Write to COM Port Based on DefaultLG
    void WriteLGComPortSlot(quint8 cpNum, QString cpData);

    //Handle Error Message Box from a different Thread
    void ErrorMessage(const QString &title, const QString &message);

signals:

    //Starts and Stops the TCP Socket (different thread)
    void StartTCPSocket();
    void StopTCPSocket();

    void TCPStatus(bool tcpConStatus);

    //Connects & Disconnects a Certain Serial COM Port (different thread)
    void StartComPort(const quint8 &playerNum, const quint8 &comPortNum, const QString &comPortName, const qint32 &comPortBaud, const quint8 &comPortData, const quint8 &comPortParity, const quint8 &comPortStop, const quint8 &comPortFlow, const QString &comPortPath, const bool &isWriteOnly);
    void StopComPort(const quint8 &playerNum, const quint8 &comPortNum);

    //Write Data on a Certain Serail COM Port (different thread)
    void WriteComPortSig(const quint8 &comPortNum, const QByteArray &writeData);

    //Set the Bypass of the Serial Port Write Checks
    void SetComPortBypassWriteChecks(const bool &cpBWC);

    //Set the Bypass of COM Port Connection Fail Warning Pop-Up
    void SetBypassComPortConnectFailWarning(const bool &cpBCPCFW);

    //Connects & Disconnects USB HID Device (different thread)
    void StartUSBHID(const quint8 &playerNum, const HIDInfo &lgHIDInfo);
    void StopUSBHID(const quint8 &playerNum);

    //Write data to USB HID Device (different thread)
    void WriteUSBHID(const quint8 &playerNum, const QByteArray &writeData);


    //Closes All Open Serial COM Ports and Open USB HIDs (different thread)
    void StopAllConnections();

    //Update the Display Data
    void MameConnectedNoGame();
    void MameConnectedGame(QString gName, bool iniGame, bool noGameFound);
    void AddSignalFromGame(const QString &sig, const QString &dat);
    void UpdateSignalFromGame(const QString &sig, const QString &dat);
    void UpdatePauseFromGame(QString dat);
    void UpdateOrientationFromGame(QString sig, QString dat);

    //TCP Server Signals
    void ConnectTCPServer(const quint16 &port);
    void DisconnectTCPServer();
    void WriteTCPServer(const QByteArray &writeData);

    void WriteTCPServer1(const QByteArray &writeData);

    //Tells TCP Socket when a Game Starts and what Output Signals to watch for
    void TCPGameStart(const QStringList &outputSignals);

    //Tells TCP Socket that the Game has Stopped, and not to filter data
    //void TCPGameStop();

    //Send Window State to TCP
    void WindowStateToTCP(const bool &isMin);

    //Internal Signals from Hooker Engine to Hook Light
    void InternalSignal2Light(const QString &signal, const QString &data);


private slots:

    //Used to Control the TCP Socket When Looking for a Hook Up
    //Why this program is Multi-Threaded. When Waiting it Locks
    //Up the GUI Window. Like the Boost thread Better, but Stuck
    //with Qt, to make compiling easier for others. Used a Timer
    //To Know When to restart the Waiting
    void TCPConnected();
    void TCPDisconnected();

    //When Timer Runs Out, Calls this Function to Update the Display
    void UpdateDisplayTimeOut();

    //For the Recoil_R2S Command
    void P1RecoilR2S();
    void P2RecoilR2S();
    void P3RecoilR2S();
    void P4RecoilR2S();
    void PXRecoilR2S(quint8 player);

    //For Light Gun Display Delay
    void P1LGDisplayDelay();
    void P2LGDisplayDelay();
    void P3LGDisplayDelay();
    void P4LGDisplayDelay();
    void PXLGDisplayDelay(quint8 player);

    void WriteDisplayDelayCMD(quint8 player, QString command);

    //For Open Solenoid Safety Timer
    void P1CloseSolenoidOrRecoilDelay();
    void P2CloseSolenoidOrRecoilDelay();
    void P3CloseSolenoidOrRecoilDelay();
    void P4CloseSolenoidOrRecoilDelay();
    void PXCloseSolenoid(quint8 player);
    void PXRecoilDelay(quint8 player);

    //Serial Port Connection Open/Close/Write Slots
    void OpenSerialPortSlot(quint8 playerNum, bool noInit);
    void CloseSerialPortSlot(quint8 playerNum, bool noInit, bool initOnly);
    void WriteSerialPortSlot(quint8 playerNum, QString cpData);

    //USB HID Connection Open/Close/Write Slots
    void OpenUSBHIDSlot(quint8 playerNum, bool noInit);
    void CloseUSBHIDSlot(quint8 playerNum, bool noInit, bool initOnly);
    void WriteUSBHIDSlot(quint8 playerNum, QString cpData);


    //TCP Server Connection Open/Close/Write
    void OpenTCPServerSlot(quint8 playerNum, bool noInit);
    void CloseTCPServerSlot(quint8 playerNum, bool noInit, bool initOnly);
    void WriteTCPServerSlot(quint8 playerNum, QString cpData);

    //Processes the TCP Socket Data
    //void ProcessTCPData(const QStringList &tcpReadData);
    void ProcessTCPData(const QString &signal, const QString &data);

    //Process Filtered TCP Data when not Minimized
    //void ProcessFilterTCPData(const QStringList &tcpReadData);
    void ProcessFilterTCPData(const QString &signal, const QString &data);

    //When Game has Started
    void GameStart(const QString &data);

    //When Game is an Empty Start
    void GameStartEmpty();

    //When the Game Stops
    void GameStopped();

    //Check is State or Signal
    void PreINICommandProcess(const QString &signal, const QString &data);

    //Light Gun has Connected to it's Output Interface
    void ConnectedLightGun(const quint8 lgNum);

    //Light Gun has Disconnected from it's Output Interface
    void DisconnectedLightGun(const quint8 lgNum);



private:


    //Clears Things out on a TCP Diconnect, if a Game has Run
    void ClearOnDisconnect();

    //Game Found, Starts things Off
    void GameFound();

    //Processes Commands - Picks INI or DefaultLG
    void ProcessCommands(QString signalName, QString value, bool isState);

    ///////////////////////////////////////////////////////////////////////////
    //MAMEHooker INI Side, to make it Compatible
    ///////////////////////////////////////////////////////////////////////////

    //Checks if an INI file Exists for Game
    bool IsINIFile();

    //Checks if an INI Deafult file Exists in ./ini
    bool IsDefaultINIFile();

    //Loads INI file for Game
    void LoadINIFile();

    //Checks the Commands Loaded in from INI File
    bool CheckINICommands(QStringList commadsNotChk, quint16 lineNumber, QString filePathName);

    //Checks a Signle Command Loaded in from INI File
    bool CheckINICommand(QString commndNotChk, quint16 lineNumber, QString filePathName);

    //Find USB HID Device HIDInfo Struct
    bool FindUSBHIDDeviceINI(quint16 vendorID, quint16 productID, quint8 deviceNumber);

    //If No Game File Exists for INI & Default LG, then make New INI File with all Signals
    void NewINIFile();

    //Gets the Signal and its Value, then Process it Based on INI file
    void ProcessINICommands(QString signalName, QString value, bool isState);

    //Open COM Port Based on INI
    void OpenINIComPort(quint8 cpNum);

    //Close COM Port Based on INI
    void CloseINIComPort(quint8 cpNum);

    //Read from COM Port Based on INI
    void ReadINIComPort(quint8 cpNum, QByteArray readData, quint8 bfNum, quint16 lengthNum);

    //Write to COM Port Based on INI
    void WriteINIComPort(quint8 cpNum, QString cpData);

    //Close All USB HID INI Connections
    void CloseINIUSBHID();

    ///////////////////////////////////////////////////////////////////////////
    //Default LG Side, New Way of Doing Light Guns
    ///////////////////////////////////////////////////////////////////////////

    //Checks if DefaultLG Game file Exists
    bool IsDefaultLGFile();

    //Checks if DefaultLG Default file Exists default.txt
    bool IsDefaultDefaultLGFile();

    //Loads DefaultLG file for Game
    void LoadLGFile();

    //Checks Commands with the Light Gun
    bool CheckCommandsWithLightGun(qint8 player, QString command);

    //Check if Loaded Command is Good or Not
    bool CheckLGCommand(QString commndNotChk);

    //If No Game File Exists for INI & DefaultLG, then make New DefaultLG File with all Signals
    void NewLGFile();

 private slots:

    //Gets the Signal and its Value, then Process it Based on DefaultLG file
    void ProcessLGCommands(const QString &signalName, const QString &value);

private:

    ///////////////////////////////////////////////////////////////////////////
    //Saves Signal and Data, Until the Display Timer Runs Out
    ///////////////////////////////////////////////////////////////////////////

    //New Output Signal to Be Added to the Display Data
    void AddSignalForDisplay(QString sig, QString dat);

    //Current Display Signal with Updated Data
    void UpdateSignalForDisplay(QString sig, QString dat);



    //Disconnects the Game Player Connections, before connecting them again.
    void DisconnectGamePlayers();



    ///////////////////////////////////////////////////////////////////////////
    //Variables Start - End of Member Functions
    ///////////////////////////////////////////////////////////////////////////

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList                   *p_comDeviceList;

    //TCP Socket that Connects to MAME and Demulshooter
    HookTCPSocket                   *p_hookSocket;

#ifdef Q_OS_WIN

    //Serial COM Port(s) for Writting to Light Guns & Other COM Devices
    HookCOMPortWin                  *p_hookComPortWin;

#else
    //Serial COM Port(s) for Writting to Light Guns & Other COM Devices
    HookCOMPort                     *p_hookComPort;

 #endif

    //Load Light Game Files and Processes Light Commands
    HookLight                       *p_hookLight;


    //Needed to Display QMessages Boxes for Warnings & Errors
    QWidget                         *p_guiConnect;

    ///////////////////////////////////////////////////////////////////////////

    //Is there Light Controllers
    bool                            isLightCntlrs;

    //Number of Saved Light Controllers
    quint8                          numberLightCntlrs;

    ///////////////////////////////////////////////////////////////////////////

    //Game Name from TCP Socket
    QString                         gameName;
    //When a Game is Found
    bool                            isGameFound;
    bool                            firstTimeGame;
    bool                            gameHasRun;
    bool                            isEmptyGame;

    ///////////////////////////////////////////////////////////////////////////

    //Files and Their Paths
    QString                         currentPath;

    //Game INI File & Path
    //Paths, if Exists, and Can Make Dir
    QString                         iniPath;
    QString                         iniMAMEPath;
    QString                         iniDefaultPath;
    bool                            iniDirExists;
    bool                            canMKDIR;
    bool                            iniMAMEDirExists;
    QDir                            iniMAMEPathDir;

    ///////////////////////////////////////////////////////////////////////////

    //Game Default LG File & Path
    QString                         defaultLGPath;
    QString                         lgDefaultPath;
    QDir                            defaultLGDir;

    ///////////////////////////////////////////////////////////////////////////

    //INI & Default LG Game File & Path
    QString                         gameINIFilePath;
    QString                         gameLGFilePath;
    //Knows if a INI or Default LG File is Loaded
    bool                            iniFileLoaded;
    bool                            lgFileLoaded;
    //Knows if a New INI or Default LG File is Made
    bool                            newINIFileMade;
    bool                            newLGFileMade;
    //QFile Used When Making a New File
    //A Pointer, Since Used in Multiple Functions
    QFile                           *p_newFile;
    //If Failed Loading the File
    bool                            iniFileLoadFail;
    bool                            lgFileLoadFail;
    bool                            isDefaultLGGame;
    bool                            isINIGame;

    ///////////////////////////////////////////////////////////////////////////

    //Check Commands & Command for INI & Default LG
    QList<quint8>                   openComPortCheck;
    QStringList                     commandLGList;


    ///////////////////////////////////////////////////////////////////////////

    //After Loading an INI or Default LG File, there will be
    //Signals and their Commands. In the INI files, Not All
    //Signal Have Commands
    QMap<QString,QStringList>       signalsAndCommands;
    QStringList                     signalsNoCommands;
    QStringList                     outputSignalsStates;


    //Also there will be States and Commands. States are a Pause,
    //Orientation, and so on.
    QMap<QString,QStringList>       stateAndCommands;
    QStringList                     statesNoCommands;

    ///////////////////////////////////////////////////////////////////////////

    //Used On the INI Side
    //Keeps Track of the Serial Port Data
    QMap<quint8,INIPortStruct>      iniPortMap;
    //Buffer Number for Reads - Not Implamented Yet
    quint8                          bufferNum;

    ///////////////////////////////////////////////////////////////////////////

    //Used On the Default LG Side
    //Number of Players Loaded from Default LG Game File
    quint8                          numberLGPlayers;
    //Player Numbers Loaded from Default LG Game File
    //IE P1 & P2 = 0,1 and P3 & P4 & P1 = 2,3,0
    quint8                          lgPlayerOrder[MAXGAMEPLAYERS];
    //Player's Light Gun Assignment from the COM Device List.
    //Used for the 2 Things Above
    quint8                          playersLGAssignment[MAXGAMEPLAYERS];
    //Corresponding Light Gun List Number to the Player
    //list above. So you Know What Player has What Light
    //Gun, based on the Player's Light Gun Assignment Data
    quint8                          loadedLGNumbers[MAXGAMEPLAYERS];
    //Array of the Loaded Light guns COM Port Number
    quint8                          loadedLGComPortNumber[MAXGAMEPLAYERS];
    //Array of Loaded Light Guns are USB HID or Serial
    bool                            isLoadedLGUSB[MAXGAMEPLAYERS];
    //Loaded Light Guns Support Reload
    bool                            loadedLGSupportReload[MAXGAMEPLAYERS];
    //Loaded Light Guns Support Recoil_Value
    bool                            loadedLGSupportRecoilValue[MAXGAMEPLAYERS];
    //Loaded Light Gun Recoil Priority
    quint8                          *p_loadedLGRecoilPriority[MAXGAMEPLAYERS];
    //Loaded Light Gun Recoil Option
    quint8                          loadRecoilForPLayer[MAXGAMEPLAYERS];
    //Loaded Light Gun Found Recoil Option or Not
    bool                            foundRecoilForPlayer[MAXGAMEPLAYERS];
    //If Light Gun is in Slow Mode
    bool                            loadedLGSlowMode[MAXGAMEPLAYERS];
    //If Recoil is even, for slow mode
    bool                            skipRecoilSlowMode[MAXGAMEPLAYERS];
    //If Light Gun Output Connection Closed
    bool                            lgConnectionClosed[MAXGAMEPLAYERS];
    //Light Gun's Output Connection - Serial Port, USB HID, BTLE, TCP
    qint8                           lgOutputConnection[MAXGAMEPLAYERS];
    //Light Gun's TCP Port & Player
    quint16                         lgTCPPort[MAXGAMEPLAYERS];
    quint8                          lgTCPPlayer[MAXGAMEPLAYERS];
    bool                            isTCPConnected;
    bool                            isTCPConnecting;
    quint8                          lgTCPConnections;
    bool                            isMultipleTCPPorts;
    quint16                         firstTCPPort;
    quint16                         secondTCPPort;

    //Game Players
    GamePlayer                      lgGamePlayers[MAXGAMEPLAYERS];

    //Options
    //Block Shake
    //Block Shake with Player's Life Value
    bool                            blockShake[MAXGAMEPLAYERS];
    //Is Block Shake Equal or Not Equal
    bool                            isBlockShakeEqual;
    // Block Shake Signal Value
    quint16                         blockShakeValue;
    //Is Block Shake Active for Player
    bool                            blockShakeActive[MAXGAMEPLAYERS];
    //Block Recoil_R2S
    //Block Recoil_R2S with Player's Life Value
    bool                            blockRecoil_R2S[MAXGAMEPLAYERS];
    //Is Block Recoil_R2S Equal or Not Equal
    bool                            isBlockRecoil_R2SEqual;
    // Block Recoil_R2S Signal Value
    quint16                         blockRecoil_R2SValue;
    //Is Block Shake Active for Player
    bool                            blockRecoil_R2SActive[MAXGAMEPLAYERS];

    //Is Light Guns Connected to it's Output Interface
    bool                            isLGConnected[MAXGAMEPLAYERS];

    bool                            isFilteredConnected;


    ///////////////////////////////////////////////////////////////////////////

    //TCP Socket Related
    //If Socket is Connected
    bool                            isTCPSocketConnected;
    //IF Engine is Running, to Start/Stop Socket & Timer
    bool                            isEngineStarted;
    //Timer for Waiting for TCP Connection
    QTimer                          *p_waitingForConnection;
    //Signals & Data and States & Data Collected from TCP Socket
    QMap<QString,QString>           signalsAndData;
    QMap<QString,QString>           statesAndData;

    ///////////////////////////////////////////////////////////////////////////

    //Settings used in Hooker Engine
    bool                            useDefaultLGFirst;
    bool                            useMultiThreading;
    //Refresh time in msec
    quint32                         refreshTimeDisplay;
    bool                            closeComPortGameExit;
    bool                            newGameFileOrDefaultFile;
    bool                            ignoreUselessDLGGF;
    bool                            bypassSerialWriteChecks;
    bool                            enableNewGameFileCreation;
    bool                            isAmmo0DelayEnabled;
    quint8                          ammo0DelayTime;
    quint16                         holdSlideBackTime;

    ///////////////////////////////////////////////////////////////////////////

    //Used to Display Data on Main Window
    //Timer to Refresh the Data
    QTimer                          *p_refreshDisplayTimer;
    //QMaps to Hold the Signals and Data
    QMap<QString,QString>           addSignalDataDisplay;
    QMap<QString,QString>           updateSignalDataDisplay;

    bool                            displayMB;

    //If HOTR is Minimized and in the Tray Icon
    bool                            isHOTRMinimized;

    ///////////////////////////////////////////////////////////////////////////

    //For Recoil_R2S
    QTimer                          pRecoilR2STimer[MAXGAMEPLAYERS];
    bool                            isPRecoilR2SFirstTime[MAXGAMEPLAYERS];
    quint32                         recoilR2SSkewPrec[MAXGAMEPLAYERS];
    QStringList                     recoilR2SInternalSignalName;

    ///////////////////////////////////////////////////////////////////////////

    //USB HID

    //If the USB HID Info Has Been Init Or Not
    bool                            isUSBHIDInit;
    QMap<QString,quint8>            hidPlayerMap;

    ///////////////////////////////////////////////////////////////////////////

    //Display & Display Refresh

    QTimer                          lgDisplayDelayTimer[MAXGAMEPLAYERS];
    bool                            isLGDisplayOnDelay[MAXGAMEPLAYERS];
    QString                         lgDisplayDelayAmmoCMDs[MAXGAMEPLAYERS];
    QString                         lgDisplayDelayLifeCMDs[MAXGAMEPLAYERS];
    QString                         lgDisplayDelayOtherCMDs[MAXGAMEPLAYERS];
    bool                            didDisplayWrite[MAXGAMEPLAYERS];
    quint16                         displayRefresh[MAXGAMEPLAYERS];

    ///////////////////////////////////////////////////////////////////////////

    //For Recoil_Value and Safety Timer

    QTimer                          openSolenoidOrRecoilDelay[MAXGAMEPLAYERS];
    QStringList                     closeSolenoidCMDs[MAXGAMEPLAYERS];
    bool                            isLGSolenoidOpen[MAXGAMEPLAYERS];
    bool                            blockRecoilValue[MAXGAMEPLAYERS];
    quint8                          timesStuckOpenSolenoid[MAXGAMEPLAYERS];


    ///////////////////////////////////////////////////////////////////////////


    //For Delay Recoil - Will reuse Timer from Recoil_Value

    quint16                         delayRecoilTime[MAXGAMEPLAYERS];
    bool                            isRecoilDelaySet[MAXGAMEPLAYERS];
    bool                            blockRecoil[MAXGAMEPLAYERS];
    bool                            doRecoilDelayEnds[MAXGAMEPLAYERS];

    ///////////////////////////////////////////////////////////////////////////

    // For Reaper Ammo 0 (Z0) Delay Buffer
    QList<quint8>                   connectedReaperLG;


};

#endif // HOOKERENGINE_H
