#ifndef HOOKERENGINE_H
#define HOOKERENGINE_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>


#include <QByteArray>
#include <QStringList>
#include <QTextStream>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QMessageBox>


#include "HookTCPSocket.h"
#include "HookCOMPort.h"
#include "../COMDeviceList/ComDeviceList.h"

class HookerEngine : public QObject
{
    Q_OBJECT
    //Threads for Multi-Threads
    QThread threadForTCPSocket;
    QThread threadForCOMPort;

public:
    //explicit HookerEngine(ComDeviceList *cdList, bool displayGUI, QWidget *guiConnect, QObject *parent = nullptr);
    HookerEngine(ComDeviceList *cdList, bool displayGUI, QWidget *guiConnect, QObject *parent = nullptr);
    ~HookerEngine();

    //Start & Stop the Hooker Engine
    void Start();
    void Stop();

    //Loads INI Game file for Checks
    bool LoadINIFileTest(QString fileNamePath);

    //Loads defaultLG Game file for Checks
    bool LoadLGFileTest(QString fileNamePath);

public slots:

    //Read Data from the TCP Socket (different thread)
    void TCPReadyRead(const QByteArray &readBA);

    //Read Data From The Serial Com Port (different thread)
    void ReadComPortSig(const quint8 &comPortNum, const QByteArray &readData);

    //Error Messages from the Serial COM Ports (different thread)
    void ErrorMessageCom(const QString &title, const QString &errorMsg);


signals:

    //Starts and Stops the TCP Socket (different thread)
    void StartTCPSocket();
    void StopTCPSocket();

    //Starts and Stops a Certain Serial COM Port (different thread)
    void StartComPort(const quint8 &comPortNum, const QString &comPortName, const qint32 &comPortBaud, const quint8 &comPortData, const quint8 &comPortParity, const quint8 &comPortStop, const quint8 &comPortFlow, const bool &isWriteOnly);
    void StopComPort(const quint8 &comPortNum);

    //Write Data on a Certain Serail COM Port (different thread)
    void WriteComPortSig(const quint8 &comPortNum, const QByteArray &writeData);

    //Closes All Open Serial COM Ports (different thread)
    void StopAllComPorts();

    //Update the Display Data
    void MameConnectedNoGame();
    void MameConnectedGame(QString gName);
    void AddSignalFromGame(const QString &sig, const QString &dat);
    void UpdateSignalFromGame(const QString &sig, const QString &dat);
    void UpdatePauseFromGame(QString dat);
    void UpdateOrientationFromGame(QString sig, QString dat);

private slots:

    //Used to Control the TCP Socket When Looking for a Hook Up
    //Why this program is Multi-Threaded. When Waiting it Locks
    //Up the GUI Window. Like the Boost thread Better, but Stuck
    //with Qt, to make compiling easier for others. Used a Timer
    //To Know When to restart the Waiting
    void TCPConnectionTimeOut();
    void TCPConnected();
    void TCPDisconnected();

    //When Timer Runs Out, Calls this Function to Update the Display
    void UpdateDisplayTimeOut();


private:

    //Processes the TCP Socket Data
    void ProcessTCPData(QStringList tcpReadData);

    //Game Found, Starts things Off
    void GameFound();

    //Processes Commands - Picks INI or DefaultLG
    void ProcessCommands(QString signalName, QString value, bool isState);

    ///////////////////////////////////////////////////////////////////////////
    //MAMEHooker INI Side, to make it Compatible
    ///////////////////////////////////////////////////////////////////////////

    //Checks if an INI file Exists for Game
    bool IsINIFile();

    //Loads INI file for Game
    void LoadINIFile();

    //Checks the Commands Loaded in from INI File
    bool CheckINICommands(QStringList commadsNotChk, quint16 lineNumber, QString filePathName);

    //Checks a Signle Command Loaded in from INI File
    bool CheckINICommand(QString commndNotChk, quint16 lineNumber, QString filePathName);


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


    ///////////////////////////////////////////////////////////////////////////
    //Default LG Side, New Way of Doing Light Guns
    ///////////////////////////////////////////////////////////////////////////

    //Checks if Default LG file Exists for Game
    bool IsDefaultLGFile();

    //Loads INI file for Game
    void LoadLGFile();

    //Check if Loaded Command is Good or Not
    bool CheckLGCommand(QString commndNotChk);

    //If No Game File Exists for INI & Default LG, then make New INI File with all Signals
    void NewLGFile();

    //Gets the Signal and its Value, then Process it Based on INI file
    void ProcessLGCommands(QString signalName, QString value);

    //Open COM Port Based on INI
    void OpenLGComPort(bool allPlayers, quint8 playerNum);

    //Close COM Port Based on INI
    void CloseLGComPort(bool allPlayers, quint8 playerNum);

    //Write to COM Port Based on INI
    void WriteLGComPort(quint8 playerNum, QString cpData);


    ///////////////////////////////////////////////////////////////////////////
    //Saves Signal and Data, Until the Display Timer Runs Out
    ///////////////////////////////////////////////////////////////////////////

    //New Output Signal to Be Added to the Display Data
    void AddSignalForDisplay(QString sig, QString dat);

    //Current Display Signal with Updated Data
    void UpdateSignalForDisplay(QString sig, QString dat);







    ///////////////////////////////////////////////////////////////////////////
    //Variables Start - End of Member Functions
    ///////////////////////////////////////////////////////////////////////////

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList                   *p_comDeviceList;

    //TCP Socket that Connects to MAME and Demulshooter
    HookTCPSocket                   *p_hookSocket;

    //Serial COM Port(s) for Writting to Light Guns & Other COM Devices
    HookCOMPort                     *p_hookComPort;

    //Needed to Display QMessages Boxes for Warnings & Errors
    QWidget                         *p_guiConnect;

    ///////////////////////////////////////////////////////////////////////////

    //Game Name from TCP Socket
    QString                         gameName;
    //When a Game is Found
    bool                            isGameFound;
    bool                            firstTimeGame;

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
    quint8                          lgPlayerOrder[MAXPLAYERLIGHTGUNS];
    //Player's Light Gun Assignment from the COM Device List.
    //Used for the 2 Things Above
    quint8                          playersLGAssignment[MAXPLAYERLIGHTGUNS];
    //Corresponding Light Gun List Number to the Player
    //list above. So you Know What Player has What Light
    //Gun, based on the Player's Light Gun Assignment Data
    quint8                          loadedLGNumbers[MAXPLAYERLIGHTGUNS];
    //Default Light Gun Numbers Based on the Player Order Above
    QList<quint8>                   defaultLGNumbers;
    //Number of Different Default Light guns Used
    quint8                          uniqueDefaultLG;


    //Used to Test defaultLG Game Files
    quint8                          numberLGPlayersTest;
    quint8                          lgPlayerOrderTest[MAXPLAYERLIGHTGUNS];
    quint8                          loadedLGNumbersTest[MAXPLAYERLIGHTGUNS];
    QList<quint8>                   defaultLGNumbersTest;
    quint8                          uniqueDefaultLGTest;


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

    ///////////////////////////////////////////////////////////////////////////

    //Used to Display Data on Main Window
    //Timer to Refresh the Data
    QTimer                          *p_refreshDisplayTimer;
    //QMaps to Hold the Signals and Data
    QMap<QString,QString>           addSignalDataDisplay;
    QMap<QString,QString>           updateSignalDataDisplay;
    //Refresh time in msec
    quint32                         refreshTimeDisplay;


    bool                            displayMB;

};

#endif // HOOKERENGINE_H
