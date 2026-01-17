#include "HookerEngine.h"
#include "../Global.h"

//Constructor
HookerEngine::HookerEngine(ComDeviceList *cdList, bool displayGUI, QWidget *guiConnect, QObject *parent)
   : QObject{parent}
{
    //Used IN TCP Socket
    isTCPSocketConnected = false;
    isEngineStarted = false;

    //When Game is Found
    isGameFound = false;
    //First time Game is Found
    firstTimeGame = true;
    //If a game has ran so far
    gameHasRun = false;
    //If No Game is Loaded ___empty
    isEmptyGame = false;

    //HOTR start Minimized in the Tray Icon
    isHOTRMinimized = true;

    //INI or Default LG Game File Loaded or failed loading
    iniFileLoaded = false;
    lgFileLoaded = false;
    iniFileLoadFail = false;
    lgFileLoadFail = false;
    isDefaultLGGame = false;
    isINIGame = false;

    //New INI or Default LG Game File Made
    newINIFileMade = false;
    newLGFileMade = false;
    //New File Pointer, set to Null
    p_newFile = nullptr;

    //bool to check if mkdir
    canMKDIR = true;

    //Used For INI COM Port Read
    bufferNum = 0;

    isUSBHIDInit = false;

    isFilteredConnected = false;

    commandLGList << OPENCOMPORT << CLOSECOMPORT << DAMAGECMD << RECOILCMD << RELOADCMD;
    commandLGList << AMMOVALUECMD << SHAKECMD << AUTOLEDCMD << ARATIO169CMD;
    commandLGList << ARATIO43CMD << JOYMODECMD << KANDMMODECMD << DLGNULLCMD << RECOIL_R2SCMD;
    commandLGList << OPENCOMPORTNOINIT << CLOSECOMPORTNOINIT << DISPLAYAMMOCMD << DISPLAYAMMOINITCMD;
    commandLGList << DISPLAYLIFECMD << DISPLAYLIFEINITCMD << DISPLAYOTHERCMD << DISPLAYOTHERINITCMD;
    commandLGList << CLOSECOMPORTINITONLYCMD << RECOILVALUECMD << RELOADVALUECMD << OFFSCREENBUTTONCMD;
    commandLGList << OFFSCREENNORMALSHOTCMD << BLOCKSIGNALCOMMAND << OFFSCREENLEFTCORNERCMD << OFFSCREENDISABLECMD;
    commandLGList << LIFEVALUECMD << DEATHVALUECMD;

    //qDebug() << "Hooker Engine Started";

    //Pass in QWidget, So Engine can Still Use QMessageBox
    p_guiConnect = guiConnect;

    displayMB = displayGUI;

    //Move Over the ComDevice List
    p_comDeviceList = cdList;

    //Get Number of Light Controllers
    numberLightCntlrs = p_comDeviceList->GetNumberLightControllers ();

    if(numberLightCntlrs > 0)
        isLightCntlrs = true;
    else
        isLightCntlrs = false;

    //Start of Directory and Path Checking. If Directory Doesn't Exists, It will Make it
    //currentPath = QDir::currentPath();
    currentPath = QApplication::applicationDirPath();

    //INI Stuff
    iniMAMEPathDir.setPath (currentPath);

    iniDirExists = iniMAMEPathDir.exists (INIFILEDIR);

    if(!iniDirExists)
        canMKDIR = iniMAMEPathDir.mkdir (INIFILEDIR);
    else
        canMKDIR = true;

    if(!canMKDIR)
    {
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "File Error", "Can not make the directory ini. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    iniMAMEPathDir.setPath (currentPath+"/"+INIFILEDIR);
    iniPath = currentPath+"/"+INIFILEDIR;

    iniMAMEDirExists = iniMAMEPathDir.exists (INIMAMEFILEDIR);

    if(!iniMAMEDirExists)
        iniMAMEPathDir.mkdir (INIMAMEFILEDIR);

    iniMAMEPathDir.setPath (currentPath+"/"+INIFILEDIR+"/"+INIMAMEFILEDIR);
    iniMAMEPath = currentPath+"/"+INIFILEDIR+"/"+INIMAMEFILEDIR;

    iniDefaultPath = currentPath+"/"+DATAFILEDIR+"/"+INIDEFAULTFILE;


    //Default LG Stuff
    defaultLGPath = currentPath+"/"+DEFAULTLGDIR;

    defaultLGDir.setPath (currentPath);

    bool defaultLGDirExists = defaultLGDir.exists (DEFAULTLGDIR);

    if(!defaultLGDirExists)
        defaultLGDir.mkdir(DEFAULTLGDIR);

    lgDefaultPath = currentPath+"/"+DATAFILEDIR+"/"+LGDEFAULTFILE;

    //Load Player's Light Gun Assignments
    for(quint8 i = 0; i < MAXPLAYERLIGHTGUNS; i++)
    {
        playersLGAssignment[i] = p_comDeviceList->GetPlayerLightGunAssignment(i);
    }

    //Needed Settings Values
    useMultiThreading = p_comDeviceList->GetUseMultiThreading ();
    refreshTimeDisplay = p_comDeviceList->GetRefreshTimeDisplay ();

    //Set-Up Refresh Display Timer
    p_refreshDisplayTimer = new QTimer(this);

    //Set Interval to the Setting of refresh Time Display
    p_refreshDisplayTimer->setInterval (refreshTimeDisplay);

    //Set Up the Signal and Slots for the timer
    connect(p_refreshDisplayTimer, SIGNAL(timeout()), this, SLOT(UpdateDisplayTimeOut()));

    //Set Up the TCP Socket
    p_hookSocket = new HookTCPSocket();

    //Make HookLight
    p_hookLight = new HookLight(cdList, currentPath);

    if(useMultiThreading)
    {
        //Move TCP Socket onto Different Thread
        p_hookSocket->moveToThread (&threadForTCPSocket);
        connect(&threadForTCPSocket, &QThread::finished, p_hookSocket, &QObject::deleteLater);

        p_threadForLight = p_comDeviceList->GetLightCntlrThread ();

        //Move Light onto Different Thread threadForLight
        p_hookLight->moveToThread (p_threadForLight);
        connect(p_threadForLight, &QThread::finished, p_hookLight, &QObject::deleteLater);
    }


    //Connect the Signals & Slots for Multi-Thread Communication for TCP Socket
    connect(this, &HookerEngine::StartTCPSocket, p_hookSocket, &HookTCPSocket::Connect);
    connect(this, &HookerEngine::StopTCPSocket, p_hookSocket, &HookTCPSocket::Disconnect);
    connect(p_hookSocket,&HookTCPSocket::DataRead, this, &HookerEngine::ProcessTCPData);
    connect(p_hookSocket,&HookTCPSocket::SocketConnectedSignal, this, &HookerEngine::TCPConnected);
    connect(p_hookSocket,&HookTCPSocket::SocketDisconnectedSignal, this, &HookerEngine::TCPDisconnected);

    //For TCP Socket to know when Game has Started or Stoppeds
    connect(this, &HookerEngine::TCPGameStart, p_hookSocket, &HookTCPSocket::GameStartSocket);
    //connect(this, &HookerEngine::TCPGameStop, p_hookSocket, &HookTCPSocket::GameStopSocket);

    //Window State to TCP
    connect(this, &HookerEngine::WindowStateToTCP, p_hookSocket, &HookTCPSocket::WindowStateTCP);

    //Process Filtered TCP Data
    connect(p_hookSocket,&HookTCPSocket::FilteredTCPData, this, &HookerEngine::ProcessFilterTCPData);

    //When Game Has Started or an Empty Game has Started
    connect(p_hookSocket,&HookTCPSocket::GameHasStarted, this, &HookerEngine::GameStart);
    connect(p_hookSocket,&HookTCPSocket::EmptyGameHasStarted, this, &HookerEngine::GameStartEmpty);

    //When Game Has Stopped
    connect(p_hookSocket,&HookTCPSocket::GameHasStopped, this, &HookerEngine::GameStopped);


    //Connections for HookLight GameHasStarted
    /////
    ///

    //Game Has Started from the TCP Socket. Sends Game Name
    connect(p_hookSocket,&HookTCPSocket::GameHasStarted, p_hookLight, &HookLight::GameStarted);

    //Game Has Stopped from the TCP Socket
    connect(p_hookSocket,&HookTCPSocket::GameHasStopped, p_hookLight, &HookLight::GameStopped);

    //Send Light Output Signals to TCP Socket
    connect(p_hookLight,&HookLight::GameStartSignals, p_hookSocket, &HookTCPSocket::GameStartLight);

    //Connect to TCP Filtered Data
    connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsLight, p_hookLight, &HookLight::ProcessSignal);
    connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, p_hookLight, &HookLight::ProcessSignal);

    //TCP Socket Disconnected
    connect(p_hookSocket,&HookTCPSocket::SocketDisconnectedSignal, p_hookLight, &HookLight::TCPSocketDisconnected);

    //Show Error Message Box
    connect(p_hookLight,&HookLight::ShowErrorMessage, this, &HookerEngine::ErrorMessage);

    if(useMultiThreading)
    {
        //Start TCP and Light Threads
        threadForTCPSocket.start(QThread::HighPriority);

        //Light Thread Alread Started with PacDrive
        //threadForLight.start(QThread::HighPriority);
    }



    //Set Up the Serial COM Port(s)
#ifdef Q_OS_WIN
    p_hookComPortWin = new HookCOMPortWin();

    if(useMultiThreading)
    {
        //Move Serial COM Port(s) onto Different Thread
        p_hookComPortWin->moveToThread (&threadForCOMPort);
        connect(&threadForCOMPort, &QThread::finished, p_hookComPortWin, &QObject::deleteLater);
    }

    //Connect the Signals & Slots for Serial COM Port
    connect(this, &HookerEngine::StartComPort, p_hookComPortWin, &HookCOMPortWin::Connect);
    connect(this, &HookerEngine::StopComPort, p_hookComPortWin, &HookCOMPortWin::Disconnect);
    connect(this, &HookerEngine::WriteComPortSig, p_hookComPortWin, &HookCOMPortWin::WriteData);
    connect(this, &HookerEngine::SetComPortBypassWriteChecks, p_hookComPortWin, &HookCOMPortWin::SetBypassSerialWriteChecks);
    connect(this, &HookerEngine::SetBypassComPortConnectFailWarning, p_hookComPortWin, &HookCOMPortWin::SetBypassCOMPortConnectFailWarning);

    //Connect the Signals & Slots for USB HID
    connect(this, &HookerEngine::StartUSBHID, p_hookComPortWin, &HookCOMPortWin::ConnectHID);
    connect(this, &HookerEngine::StopUSBHID, p_hookComPortWin, &HookCOMPortWin::DisconnectHID);
    connect(this, &HookerEngine::WriteUSBHID, p_hookComPortWin, &HookCOMPortWin::WriteDataHID);

    //Connect the Signals & Slots for TCP Server
    connect(this, &HookerEngine::ConnectTCPServer, p_hookComPortWin, &HookCOMPortWin::ConnectTCP);
    connect(this, &HookerEngine::DisconnectTCPServer, p_hookComPortWin, &HookCOMPortWin::DisconnectTCP);
    connect(this, &HookerEngine::WriteTCPServer, p_hookComPortWin, &HookCOMPortWin::WriteTCP);
    connect(this, &HookerEngine::WriteTCPServer1, p_hookComPortWin, &HookCOMPortWin::WriteTCP1);


    connect(this, &HookerEngine::StopAllConnections, p_hookComPortWin, &HookCOMPortWin::DisconnectAll);
    connect(p_hookComPortWin, &HookCOMPortWin::ErrorMessage, this, &HookerEngine::ErrorMessageCom);

    //Light Gun has Connected or Disconnected from it's Interface
    connect(p_hookComPortWin, &HookCOMPortWin::LightGunConnected, this, &HookerEngine::ConnectedLightGun);
    connect(p_hookComPortWin, &HookCOMPortWin::LightGunDisconnected, this, &HookerEngine::DisconnectedLightGun);

#else
    p_hookComPort = new HookCOMPort();

    if(useMultiThreading)
    {
        //Move Serial COM Port(s) onto Different Thread
        p_hookComPort->moveToThread (&threadForCOMPort);
        connect(&threadForCOMPort, &QThread::finished, p_hookComPort, &QObject::deleteLater);
    }

    //Connect the Signals & Slots for Multi-Thread Communication for Serial COM Port
    connect(this, &HookerEngine::StartComPort, p_hookComPort, &HookCOMPort::Connect);
    connect(this, &HookerEngine::StopComPort, p_hookComPort, &HookCOMPort::Disconnect);
    connect(this, &HookerEngine::WriteComPortSig, p_hookComPort, &HookCOMPort::WriteData);
    connect(p_hookComPort, &HookCOMPort::ReadDataSig, this, &HookerEngine::ReadComPortSig);
    connect(this, &HookerEngine::StopAllConnections, p_hookComPort, &HookCOMPort::DisconnectAll);
    connect(p_hookComPort, &HookCOMPort::ErrorMessage, this, &HookerEngine::ErrorMessageCom);

    //Connect the Signals & Slots for USB HID
    connect(this, &HookerEngine::StartUSBHID, p_hookComPortWin, &HookCOMPortWin::ConnectHID);
    connect(this, &HookerEngine::StopUSBHID, p_hookComPortWin, &HookCOMPortWin::DisconnectHID);
    connect(this, &HookerEngine::WriteUSBHID, p_hookComPortWin, &HookCOMPortWin::WriteDataHID);

    //Connect the Signals & Slots for TCP Server
    connect(this, &HookerEngine::ConnectTCPServer, p_hookComPortWin, &HookCOMPortWin::ConnectTCP);
    connect(this, &HookerEngine::DisconnectTCPServer, p_hookComPortWin, &HookCOMPortWin::DisconnectTCP);
    connect(this, &HookerEngine::WriteTCPServer, p_hookComPortWin, &HookCOMPortWin::WriteTCP);


#endif

    if(useMultiThreading)
    {
        threadForCOMPort.start(QThread::HighPriority);
    }

    //Load Up Settings
    LoadSettingsFromList();

    //Connect Recoil_R2S Timers To Slots
    connect(&pRecoilR2STimer[0], SIGNAL(timeout()), this, SLOT(P1RecoilR2S()));
    connect(&pRecoilR2STimer[1], SIGNAL(timeout()), this, SLOT(P2RecoilR2S()));
    connect(&pRecoilR2STimer[2], SIGNAL(timeout()), this, SLOT(P3RecoilR2S()));
    connect(&pRecoilR2STimer[3], SIGNAL(timeout()), this, SLOT(P4RecoilR2S()));

    //Connect Display Delay Timers
    connect(&lgDisplayDelayTimer[0], SIGNAL(timeout()), this, SLOT(P1LGDisplayDelay()));
    connect(&lgDisplayDelayTimer[1], SIGNAL(timeout()), this, SLOT(P2LGDisplayDelay()));
    connect(&lgDisplayDelayTimer[2], SIGNAL(timeout()), this, SLOT(P3LGDisplayDelay()));
    connect(&lgDisplayDelayTimer[3], SIGNAL(timeout()), this, SLOT(P4LGDisplayDelay()));

    //Connect Open Solenoid Safety Timers
    connect(&openSolenoidOrRecoilDelay[0], SIGNAL(timeout()), this, SLOT(P1CloseSolenoidOrRecoilDelay()));
    connect(&openSolenoidOrRecoilDelay[1], SIGNAL(timeout()), this, SLOT(P2CloseSolenoidOrRecoilDelay()));
    connect(&openSolenoidOrRecoilDelay[2], SIGNAL(timeout()), this, SLOT(P3CloseSolenoidOrRecoilDelay()));
    connect(&openSolenoidOrRecoilDelay[3], SIGNAL(timeout()), this, SLOT(P4CloseSolenoidOrRecoilDelay()));



    for(quint8 i = 0; i < MAXGAMEPLAYERS; i++)
    {
        //Set Up Game Players
        lgGamePlayers[i].SetPlayer (i);

        isPRecoilR2SFirstTime[i] = true;
        recoilR2SSkewPrec[i] = 100;
        isLGDisplayOnDelay[i] = false;
        lgDisplayDelayTimer[i].setInterval (DISPLAYREFRESHDEFAULT);
        lgDisplayDelayTimer[i].setSingleShot(true);
        //lgDisplayDelayTimer[i].setTimerType (Qt::PreciseTimer);
        didDisplayWrite[i] = false;
        isLGSolenoidOpen[i] = false;
        openSolenoidOrRecoilDelay[i].setInterval (OPENSOLENOIDDEFAULTTIME);
        openSolenoidOrRecoilDelay[i].setSingleShot(true);
        //openSolenoidOrRecoilDelay[i].setTimerType (Qt::PreciseTimer);
        blockRecoilValue[i] = false;
        timesStuckOpenSolenoid[i] = 0;
        isRecoilDelaySet[i] = false;
        blockRecoil[i] = false;
        delayRecoilTime[i] = ALIENUSBDELAYDFLT;
        skipRecoilSlowMode[i] = false;
        blockShake[i] = false;
        blockRecoil_R2S[i] = false;
        blockShakeActive[i] = false;
        blockRecoil_R2SActive[i] = false;
        lgConnectionClosed[i] = true;
        lgOutputConnection[i] = -1;
        lgTCPPort[i] = 0;
        lgTCPPlayer[i] = UNASSIGN;
        isLGConnected[i] = false;
    }

    //Status of the LG TCP Server
    isTCPConnected = false;
    isTCPConnecting = false;
    lgTCPConnections = 0;

}

//Deconstructor
HookerEngine::~HookerEngine()
{
    //Clean Up Pointers
    if(p_newFile != nullptr)
        delete p_newFile;

    //Close all COM Port & USB HID Connections
    emit StopAllConnections();

    //Clean Up threads
    if(useMultiThreading)
    {
        threadForTCPSocket.quit();
        threadForCOMPort.quit();
        //threadForLight.quit();
        threadForTCPSocket.wait();
        threadForCOMPort.wait();
        //threadForLight.wait();
    }
    else
    {
        delete p_hookSocket;

#ifdef Q_OS_WIN
        delete p_hookComPortWin;
#else
        delete p_hookComPort;
#endif
    }

}

//Public Member Functions

void HookerEngine::HookerEngine::Start()
{
    //Display TCP Connection
    emit TCPStatus(isTCPSocketConnected);

    //Load Player's Light Gun Assignments, In Case Something Changed
    LoadUpdatePlayerAssignment();

    //Load Settings
    LoadSettingsFromList();

    //Set-Up Reapers if they are Holding Slide Back
    SetUpLightGuns();

    isEngineStarted = true;

    //Start the TCP Connection
    emit StartTCPSocket();
}

void HookerEngine::HookerEngine::Stop()
{
    isEngineStarted = false;

    if(p_refreshDisplayTimer->isActive ())
        p_refreshDisplayTimer->stop ();

    //Disconnect the TCP Socket, Which Will Shut Things Down.
    emit StopTCPSocket();
}


bool HookerEngine::LoadINIFileTest(QString fileNamePath)
{
    QString line;
    QString signal;
    QString commands;
    QStringList tempSplit;
    quint16 indexEqual;
    bool isOutput = false;
    bool goodCommand;
    quint16 lineNumber = 0;

    openComPortCheck.clear();

    //Open File. If Failed to Open, so Critical Message Box
    QFile iniFile(fileNamePath);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool openFile = iniFile.open (QIODeviceBase::ReadOnly | QIODevice::Text);
#else
    bool openFile = iniFile.open (QIODevice::ReadOnly | QIODevice::Text);
#endif

    if(!openFile)
    {
        QString tempCrit = "Can not open INI data file to read. Please close program and solve file problem. Might be permissions problem.\nFile: "+fileNamePath;
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "File Error", tempCrit, QMessageBox::Ok);
        return false;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&iniFile);

    while(!in.atEnd ())
    {

        //Get a line of data from file
        line = in.readLine();
        lineNumber++;

        //All lines have an '=', except for the one with '[' and ']'
        if(!line.startsWith("["))
        {
            //Get the Index of the Equal
            indexEqual = line.indexOf('=',0);

            //If there is Nothing After '=', but in NoCommnds List
            //If something after '=' then Split it up into Signals and Command(s)
            if(line.length() != indexEqual+1)
            {

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                if(line[indexEqual-1] == ' ')
                    signal = line.first(indexEqual-1);
                else
                    signal = line.first(indexEqual);

                if(line[indexEqual+1] == ' ')
                    commands = line.sliced(indexEqual+2);
                else
                    commands = line.sliced(indexEqual+1);
#else
                if(line[indexEqual-1] == ' ')
                    signal = line.chopped(line.size()-(indexEqual-1));
                else
                    signal = line.chopped(line.size()-indexEqual);

                quint16 indexOverEq;
                if(line[indexEqual+1] == ' ')
                    indexOverEq = indexEqual + 2;
                else
                    indexOverEq = indexEqual + 1;

                for(quint16 k = indexOverEq; k < line.size(); k++)
                    signal.append (line[k]);
#endif

                commands.replace(", ", ",");
                commands.replace(" ,", ",");

                tempSplit = commands.split(',', Qt::SkipEmptyParts);

                if(!isOutput)
                {
                    if(signal.startsWith ("On"))
                        signal.remove(0,2);

                    if(signal.startsWith(JUSTMAME, Qt::CaseInsensitive))
                        signal.insert(4,'_');

                    signal = signal.toLower();
                }

                //qDebug() << "Loaded INI Signal or State: " << signal;

                goodCommand = CheckINICommands(tempSplit, lineNumber, fileNamePath);

                //If bad command file, then fail load
                if(!goodCommand)
                {
                    //Close File and Fail
                    iniFile.close();
                    return false;
                }

            }
            else
            {
                line.chop(1);
            }
        }
        else
        {
            //This is the lines with the '[' and ']'
            if(line.contains(SIGNALSTATE, Qt::CaseInsensitive))
                isOutput = true;
            else
                isOutput = false;
        }

    }

    //Close File
    iniFile.close();


    return true;
}


bool HookerEngine::LoadLGFileTest(QString fileNamePath)
{
    QString line;
    quint8 playerNumber;
    QString signal;
    QStringList commands;
    bool gotCommands = false;
    bool gotSignal = false;
    bool gotPlayer = false;
    bool isNumber;
    bool begin = true;
    quint8 lgNumber;
    QString tempLine;
    quint8 tempPlayer;
    bool isGoodCmd;
    QStringList subCommands;
    quint8 subCmdsCnt;
    quint16 lineNumber = 0;
    bool isStates = false;
    quint8 unassignLG = 0;
    quint8 numberLGPlayersTest;
    quint8 lgPlayerOrderTest[MAXPLAYERLIGHTGUNS];
    quint8 loadedLGNumbersTest[MAXPLAYERLIGHTGUNS];
    quint16 signalsAndCommandsCountTest = 0;

    bool gameRecoilsSupported[NUMBEROFRECOILS];
    bool gameReloadsSupported[NUMBEROFRECOILS];
    QStringList ammoValueCMDs;
    QStringList recoilCMDs;
    QStringList recoilR2SCMDs;
    QStringList recoilValueCMDs;
    QMap<quint8,QStringList> recoilCMDsMap;
    bool hasRecoilOption = false;
    qint16 playerForCMD;

    quint8 testLoadRecoilForPLayer[MAXGAMEPLAYERS];
    bool testFoundRecoilForPlayer[MAXGAMEPLAYERS];
    bool testIsLoadedLGUSB[MAXGAMEPLAYERS];
    quint8 testLoadedLGComPortNumber[MAXGAMEPLAYERS];
    bool testIsRecoilDelaySet[MAXGAMEPLAYERS];
    quint16 testDelayRecoilTime[MAXGAMEPLAYERS];
    bool testLoadedLGSupportRecoilValue[MAXGAMEPLAYERS];
    bool testLoadedLGSupportReload[MAXGAMEPLAYERS];
    quint8 *testp_loadedLGRecoilPriority[MAXGAMEPLAYERS];

    bool isGoodLG;

    for(quint8 x = 0; x < MAXGAMEPLAYERS; x++)
    {
        gameRecoilsSupported[x] = false;
        gameReloadsSupported[x] = false;
        testLoadRecoilForPLayer[x] = 1;
        testFoundRecoilForPlayer[x] = false;
    }


    QFile lgFile(fileNamePath);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool openFile = lgFile.open (QIODeviceBase::ReadOnly | QIODevice::Text);
#else
    bool openFile = lgFile.open (QIODevice::ReadOnly | QIODevice::Text);
#endif

    if(!openFile)
    {
        QString tempCrit = "Can not open default light gun game file to read. Please close program and solve file problem. Might be permissions problem.\nFile: "+fileNamePath;
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
        return false;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&lgFile);

    while(!in.atEnd ())
    {

        //Get a line of data from file
        line = in.readLine();
        lineNumber++;

        //For Faster Processing, search first char
        if(line[0] == 'P'  && begin)
        {
            //File should Start with "Players"
            if(line.startsWith(PLAYERSSTART))
            {
                //Next Line is the number of Players
                line = in.readLine();
                lineNumber++;
                numberLGPlayersTest = line.toUInt (&isNumber);

                if(!isNumber)
                {
                    lgFile.close();
                    QString tempCrit = "The number pf Players, is not a number. Please close program and fix.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number: "+line+"\nFile: "+fileNamePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return false;
                }

                //qDebug() << "Number of Players: " << numberLGPlayersTest;

                //Read in Players and there Order IE P1, P2 or P3, P1, P4
                for(quint8 i = 0; i < numberLGPlayersTest; i++)
                {
                    line = in.readLine();
                    lineNumber++;

                    if(line[0] != PLAYER2CHHAR)
                    {
                        lgFile.close();
                        QString tempCrit = "The player needs to be P[1-4]. Please close program and fix.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number: "+line+"\nFile: "+fileNamePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return false;
                    }

                    //Check for Slow Mode on PLayers IE 'P1 Slow'
                    if(line.contains(SLOWMODE))
                    {
                        QStringList playerSlowMode = line.split (' ', Qt::SkipEmptyParts);
                        playerSlowMode[0] = playerSlowMode[0].trimmed();
                        //Make line just the player number IE P1, P2, or P3
                        line = playerSlowMode[0];
                        playerSlowMode[1] = playerSlowMode[1].trimmed();
                        if(playerSlowMode[1] != SLOWMODE)
                        {
                            lgFile.close();
                            QString tempCrit = "Something is after the player, and is not Slow mode.\nLine Number: "+QString::number(lineNumber)+"\nAfter player: "+playerSlowMode[1]+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }
                    }
                    else
                        loadedLGSlowMode[i] = false;

                    //Remove 'P'
                    line.remove (0,1);
                    playerNumber = line.toUInt (&isNumber);
                    if(isNumber)
                    {
                        if(playerNumber == 0 || playerNumber > MAXPLAYERLIGHTGUNS)
                        {
                            lgFile.close();
                            QString tempCrit = "Player number under \"Players\" is out of range. Player number range is 1-4.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number: "+line+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }
                        lgPlayerOrderTest[i] = playerNumber-1;
                    }
                    else
                    {
                        lgFile.close();
                        QString tempCrit = "Player number was not a number in game file.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number: "+line+"\nFile: "+fileNamePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return false;
                    }
                    //qDebug() << "Player " << i << " connected to: " << line;

                    //Check if Light Gun is a Default Light Gun
                    lgNumber = playersLGAssignment[lgPlayerOrderTest[i]];


                    //Load Light Gun Order, based on Player Order and Player's Assignment
                    loadedLGNumbersTest[i] = lgNumber;

                    //If Player Assignment is Unassign, increament unassignLG
                    if(lgNumber == UNASSIGN)
                        unassignLG++;
                    else
                    {
                        //Check if Serial Port or USB HID. If Serial Port, get Serial Port Number
                        //if(p_comDeviceList->p_lightGunList[lgNumber]->IsLightGunUSB ())
                        if(p_comDeviceList->p_lightGunList[lgNumber]->GetOutputConnection() == USBHID)
                        {
                            testIsLoadedLGUSB[i] = true;
                            testLoadedLGComPortNumber[i] = UNASSIGN;

                            if(p_comDeviceList->p_lightGunList[lgNumber]->IsRecoilDelay())
                            {
                                testIsRecoilDelaySet[i] = true;
                                testDelayRecoilTime[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetRecoilDelay ();
                                //openSolenoidOrRecoilDelay[i].setInterval (delayRecoilTime[i]);
                            }
                            else
                                testIsRecoilDelaySet[i] = false;
                        }
                        else
                        {
                            testIsLoadedLGUSB[i] = false;
                            testLoadedLGComPortNumber[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetComPortNumber();
                            testIsRecoilDelaySet[i] = false;
                            //openSolenoidOrRecoilDelay[i].setInterval (OPENSOLENOIDDEFAULTTIME);
                        }

                        //Does Light Gun Support Recoil_Value Command
                        testLoadedLGSupportRecoilValue[i] = p_comDeviceList->p_lightGunList[lgNumber]->IsRecoilValueSupported ();
                        //Does Light Gun Support Reload
                        testLoadedLGSupportReload[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetSupportedReload ();
                        //Light Gun Recoil Priority
                        testp_loadedLGRecoilPriority[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetRecoilPriorityHE();

                    }

                }

                if(unassignLG == numberLGPlayersTest)
                {
                    lgFile.close();
                    QString tempCrit = "No Player(s) have assign light gun. Atleast one player needs to have an assign light gun. Please close game, and then assign a light gun to the player.\nFile: "+fileNamePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return false;
                }


                begin = false;
            }
        }
        else if(line == RECOILRELOAD)
        {
            //Loads Up Support Recoil & Reload Options in Game File
            quint8 index = 0;

            //Get Ammo_Value: Recoil and Reload Info from Game File Header
            line = in.readLine();
            lineNumber++;
            line.replace("  ", " ");

            if(line.startsWith (AMMOVALUECMDONLY))
            {
                QStringList ammoValueSL = line.split (' ', Qt::SkipEmptyParts);
                if(ammoValueSL[1] == "1")
                {
                    gameRecoilsSupported[0] = true;
                    gameReloadsSupported[0] = true;
                    ammoValueSL[0].prepend('#');
                    ammoValueCMDs << ammoValueSL[0];


                    if(ammoValueSL.length() > 2)
                    {
                        ammoValueSL[2].prepend('#');
                        ammoValueCMDs << ammoValueSL[2];
                    }
                    else
                        ammoValueCMDs << ammoValueSL[0];
                }
                else
                    ammoValueCMDs << "" << "";
            }
            else
            {
                lgFile.close();
                QString tempCrit = "The line didn't match 'Ammo_Value' Recoil Options\nLine"+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            recoilCMDsMap.insert (0, ammoValueCMDs);

            //Get Recoil: Recoil and Reload Info from Game File Header
            line = in.readLine();
            lineNumber++;
            line.replace("  ", " ");

            if(line.startsWith (RECOILCMDONLY))
            {
                QStringList recoilSL = line.split (' ', Qt::SkipEmptyParts);
                if(recoilSL[1] == "1")
                {
                    gameRecoilsSupported[1] = true;
                    recoilSL[0].prepend('#');
                    recoilCMDs << recoilSL[0];

                    if(recoilSL.length() > 2)
                    {
                        recoilSL[2].prepend('#');
                        recoilCMDs << recoilSL[2];
                        gameReloadsSupported[1] = true;
                    }
                    else
                        recoilCMDs << "";
                }
                else
                    recoilCMDs << "" << "";
            }
            else
            {
                lgFile.close();
                QString tempCrit = "The line didn't match 'Recoil' Recoil Options\nLine"+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            recoilCMDsMap.insert (1, recoilCMDs);

            //Get Recoil_R2S: Recoil and Reload Info from Game File Header
            line = in.readLine();
            lineNumber++;
            line.replace("  ", " ");

            if(line.startsWith (RECOIL_R2SONLY))
            {
                QStringList recoilR2SSL = line.split (' ', Qt::SkipEmptyParts);
                if(recoilR2SSL[1] == "1")
                {
                    gameRecoilsSupported[2] = true;
                    recoilR2SSL[0].prepend('#');
                    recoilR2SCMDs << recoilR2SSL[0];

                    if(recoilR2SSL.length() > 2)
                    {
                        recoilR2SSL[2].prepend('#');
                        recoilR2SCMDs << recoilR2SSL[2];
                        gameReloadsSupported[2] = true;
                    }
                    else
                        recoilR2SCMDs << "";
                }
                else
                    recoilR2SCMDs << "" << "";
            }
            else
            {
                lgFile.close();
                QString tempCrit = "The line didn't match 'Recoil_R2S' Recoil Options\nLine"+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            recoilCMDsMap.insert (2, recoilR2SCMDs);

            //Get Recoil_Value: Recoil and Reload Info from Game File Header
            line = in.readLine();
            lineNumber++;
            line.replace("  ", " ");

            if(line.startsWith (RECOILVALUEONLY))
            {
                QStringList recoilValueSL = line.split (' ', Qt::SkipEmptyParts);
                if(recoilValueSL[1] == "1")
                {
                    gameRecoilsSupported[3] = true;
                    recoilValueSL[0].prepend('#');
                    recoilValueCMDs << recoilValueSL[0];

                    if(recoilValueSL.length() > 2)
                    {
                        recoilValueSL[2].prepend('#');
                        recoilValueCMDs << recoilValueSL[2];
                        gameReloadsSupported[3] = true;
                    }
                    else
                        recoilValueCMDs << "";
                }
                else
                    recoilValueCMDs << "" << "";
            }
            else
            {
                lgFile.close();
                QString tempCrit = "The line didn't match 'Recoil_Value' Recoil Options\nLine"+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            recoilCMDsMap.insert (3, recoilValueCMDs);

            //Light Gun Recoil & Priority Matching with the Game File Supported
            for(quint8 i = 0; i < numberLGPlayersTest; i++)
            {
                if(loadedLGNumbersTest[i] != UNASSIGN)
                {
                    while(!testFoundRecoilForPlayer[i] && index < NUMBEROFRECOILS)
                    {
                        if(testp_loadedLGRecoilPriority[i][index] == 0 && gameRecoilsSupported[0])
                        {
                            testLoadRecoilForPLayer[i] = 0;
                            testFoundRecoilForPlayer[i] = true;
                        }
                        else if(testp_loadedLGRecoilPriority[i][index] == 1 && gameRecoilsSupported[1])
                        {
                            testLoadRecoilForPLayer[i] = 1;
                            testFoundRecoilForPlayer[i] = true;
                        }
                        else if(testp_loadedLGRecoilPriority[i][index] == 2 && gameRecoilsSupported[2])
                        {
                            testLoadRecoilForPLayer[i] = 2;
                            testFoundRecoilForPlayer[i] = true;
                        }
                        else if(testp_loadedLGRecoilPriority[i][index] == 3 && testLoadedLGSupportRecoilValue[i] && gameRecoilsSupported[3])
                        {
                            testLoadRecoilForPLayer[i] = 3;
                            testFoundRecoilForPlayer[i] = true;
                        }

                        index++;
                    }

                    //qDebug() << "Player" << i+1 << "testFoundRecoilForPlayer" << testFoundRecoilForPlayer[i] << "testLoadRecoilForPLayer" << testLoadRecoilForPLayer[i];
                }
                index = 0;
            }

            //Check to Make Sure Recoil Option Found for All Loaded Light Gun Players
            for(quint8 i = 0; i < numberLGPlayersTest; i++)
            {
                //qDebug() << "Player" << i+1 << "testFoundRecoilForPlayer" << testFoundRecoilForPlayer[i] << "loadedLGNumbersTest" << loadedLGNumbersTest[i];

                if(!testFoundRecoilForPlayer[i] && loadedLGNumbersTest[i] != UNASSIGN)
                {

                    lgFile.close();
                    QString tempCrit = "Cannot find a supported recoil option.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return false;
                }
            }

            hasRecoilOption = true;

        }  //else if(line == RECOILRELOAD)
        else if(line[0] == '[')
        {
            //Nothing In Here Yet, But Might Be Used Later
            if(line.startsWith("[Sig"))
            {
                isStates = true;
            }
            else if(line.startsWith(OPTIONSSECTION) && isStates)
            {
                lgFile.close();
                QString tempCrit = "Options need to come before the States.\nLine"+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }
            else if(line.startsWith(OPTIONSSECTION))
            {
                //Only Options Supported is Block for Shake & Recoil_R2S command, using the player's life value.
                //Go through the Lines, until "End Options" happens

                //Get a line of data from file
                line = in.readLine();
                lineNumber++;

                while(line.startsWith(ENDOPTIONS) == false)
                {
                    if(line.startsWith(BLOCKSHAKEOPTION))
                    {
                        QStringList blockShakeSL = line.split (' ', Qt::SkipEmptyParts);
                        quint8 blockShakeLength = blockShakeSL.length();

                        if(blockShakeLength < BLOCKSHAKELENGTH || blockShakeLength > BLOCKSHAKESTARTLENGTH)
                        {
                            lgFile.close();
                            QString tempCrit = "Block Shake needs E or NE next, and then the value.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }

                        if(blockShakeSL[2] != "NE" && blockShakeSL[2] != "E")
                        {
                            lgFile.close();
                            QString tempCrit = "Block Shake needs E or NE next, and then the value.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }

                        quint8 tempBlockShakeValue = blockShakeSL[3].toUShort(&isNumber);

                        if(!isNumber)
                        {
                            lgFile.close();
                            QString tempCrit = "Block Shake needs a number at the end.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }

                        //If the Block from Start Option is Set
                        if(blockShakeLength == BLOCKSHAKESTARTLENGTH)
                        {
                            if(blockShakeSL[4] != BLOCKSTART)
                            {
                                lgFile.close();
                                QString tempCrit = "Block Shake Start option is not correct.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                                if(displayMB)
                                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                                return false;
                            }
                        }
                    }
                    else if(line.startsWith(BLOCKRECOIL_R2SOPTION))
                    {
                        QStringList blockRecoil_R2SSL = line.split (' ', Qt::SkipEmptyParts);
                        quint8 blockRecoil_R2SLength = blockRecoil_R2SSL.length();

                        if(blockRecoil_R2SLength < BLOCKRECOIL_R2SLENGTH || blockRecoil_R2SLength > BLOCKRECOIL_R2STRLENGTH)
                        {
                            lgFile.close();
                            QString tempCrit = "Block Recoil_R2S needs E or NE next, and then the value.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }

                        if(blockRecoil_R2SSL[2] != "NE" && blockRecoil_R2SSL[2] != "E")
                        {
                            lgFile.close();
                            QString tempCrit = "Block Recoil_R2S needs E or NE next, and then the value.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }

                        quint8 tempBlockRecoil_R2SValue = blockRecoil_R2SSL[3].toUShort(&isNumber);

                        if(!isNumber)
                        {
                            lgFile.close();
                            QString tempCrit = "Block Recoil_R2S needs a number at the end.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }

                        //If the Block from Start Option is Set
                        if(blockRecoil_R2SLength == BLOCKSHAKESTARTLENGTH)
                        {
                            if(blockRecoil_R2SSL[4] != BLOCKSTART)
                            {
                                lgFile.close();
                                QString tempCrit = "Block Recoil_R2S Start option is not correct.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                                if(displayMB)
                                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                                return false;
                            }
                        }
                    }
                    else if(line.startsWith(OVERRIDERECOILVOLT))
                    {
                        QStringList overrideRecoilVoltSL = line.split (' ', Qt::SkipEmptyParts);

                        if(overrideRecoilVoltSL.length() != OVERRIDERECOILLENGTH)
                        {
                            lgFile.close();
                            QString tempCrit = "The Override_Recoil_Voltage should have 2 parts. The Override_Recoil_Voltage and a number from 0-10.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }

                        quint8 orRecoilVolt = overrideRecoilVoltSL[OVERRIDERECOILVOLTNUM].toUShort(&isNumber);

                        if(!isNumber || orRecoilVolt > RECOILVOLTAGEMAX)
                        {
                            lgFile.close();
                            QString tempCrit = "The Override Recoil Voltage number is not a number or greater than 10.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }
                    }
                    else if(line.startsWith(SINDENTRIGGERRECOIL))
                    {
                        QStringList sindenTriggerRecoilSL = line.split (' ', Qt::SkipEmptyParts);

                        if(sindenTriggerRecoilSL.length() != SINDENTRIGGERRECOILLEN)
                        {
                            lgFile.close();
                            QString tempCrit = "The Sinden_Trigger_Recoil should have 2 parts. The Sinden_Trigger_Recoil and a number from 0-3.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }

                        quint8 sinTrigRec = sindenTriggerRecoilSL[SINDENTRIGGERRECOILNUM].toUShort(&isNumber);

                        if(!isNumber || sinTrigRec > SINDENTRIGGERRECOILMAX)
                        {
                            lgFile.close();
                            QString tempCrit = "The Sinden Trigger Recoil number is not a number or greater than 3.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return false;
                        }
                    }
                    else if(line.startsWith(AMMOCHECKOPTION))
                    {
                        //Nothing to test for on Ammo_Check
                    }
                    else
                    {
                        lgFile.close();
                        QString tempCrit = "This Option doesn't exist in the DefaultLG protocol.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return false;
                    }

                    //Get Next Line and Increment the Line Count
                    line = in.readLine();
                    lineNumber++;
                }
            }
        }
        else if(line[0] == SIGNALSTARTCHAR)
        {
            //Got a Signal, order is Signal, Player Commands, where there could be multiple Players, But a Command is needed after Player
            //But first process older data

            //If Got a Signal & Player(s) and Command(s), then Load into QMap
            if(gotSignal && gotPlayer && gotCommands)
            {
                isGoodLG = false;

                if(playerForCMD == -1)
                    isGoodLG = true;
                else if(playerForCMD < MAXGAMEPLAYERS && loadedLGNumbersTest[playerForCMD] != UNASSIGN)
                    isGoodLG = true;

                if(isGoodLG)
                {
                    //qDebug() << "Signal: " << signal << " Commands: " << commands;

                    if(!signal.startsWith (MAMESTAFTER) || (signal.startsWith (MAMESTAFTER) && commands.count() > 2))
                        signalsAndCommandsCountTest++;
                }

                //qDebug() << "Signal: " << signal << " Commands: " << commands;

                gotPlayer = false;
                gotCommands = false;   

                commands.clear ();
            }
            else if(gotSignal && !gotPlayer && !gotCommands)
            {
                //Signal that has no players and no commands
            }
            else if(gotSignal && gotPlayer && !gotCommands)
            {
                //Not Using that Recoil/Reload Option
                gotPlayer = false;
                commands.clear ();
            }

            //Process Current Line

            if(line.contains ('='))
            {
                lgFile.close();
                QString tempCrit = "A signal cannot have a = for the Default Light Gun game format. You might be confussed with MAMEHooker INI format, which uses the = sign.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            //Remove the ':' from the Front
            line.remove (0,1);

            signal = line;

            gotSignal = true;

        }
        else if(line[0] == PLAYERSTARTCHAR)
        {
            //Got a Player

            if(gotPlayer && !gotCommands)
            {
                lgFile.close();
                QString tempCrit = "There are two or more Player lines back to back.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            //If Got a Signal, Add The Player. If Not then Sometrhing Went Wrong
            if(gotSignal)
            {
                //Check If it is a Player Number, like *P1 or *P3
                if(line[1] == PLAYER2CHHAR)
                {

                    tempLine = line;

                    //Remove the '*P' from the Front of Players
                    tempLine.remove (0,2);

                    tempPlayer = tempLine.toUInt (&isNumber);

                    //Check if Number
                    if(!isNumber)
                    {
                        lgFile.close();
                        QString tempCrit = "Player Number is not a number in defaultLG game file.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number:"+tempLine+"\nFile: "+fileNamePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return false;
                    }

                    playerForCMD = tempPlayer - 1;

                    //Check if Player Number Matches Loaded Players Above
                    bool playerMatch = false;

                    for(quint8 i = 0; i < numberLGPlayersTest; i++)
                    {
                        if(tempPlayer-1 == i)
                            playerMatch = true;
                    }

                    if(!playerMatch)
                    {
                        lgFile.close();
                        QString tempCrit = "Player Number does not match loaded Players in defaultLG game file.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number:"+tempLine+"\nFile: "+fileNamePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return false;
                    }

                } //If not *P[1-4], then must be *All. If not those 2, then something is wrong.
                else if(line[1] == ALL2CHAR)
                {
                    //All Players Been Selected
                    playerForCMD = -1;

                }
                else if(line != ALLPLAYERS)
                {
                    lgFile.close();
                    QString tempCrit = "Something is wrong with Player Number in defaultLG game file. Can be *All or *P[1-4].\nLine Number: "+QString::number(lineNumber)+"\nLine :"+line+"\nFile: "+fileNamePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return false;
                }

                //First thing of the Command is the Player(s)
                if(!gotPlayer)
                    commands << line;

                gotPlayer = true;
            }
            else if(in.atEnd ())
            {
                lgFile.close();
                QString tempCrit = "File cannot end on a player(*). Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }
            else
            {
                lgFile.close();
                QString tempCrit = "Player(*) came before a signal(:). Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }
        }
        else if(line[0] == COMMANDSTARTCHAR)
        {

            //Got a Command - Check if Good Command

            if(line.contains ('|'))
            {
                if(line.startsWith (RECOIL_R2SCMD))
                {
                    lgFileLoadFail = true;
                    lgFile.close();
                    QString tempCrit = "Recoil_R2S command doesn't support the '|'.\nLine Number: "+QString::number(lineNumber)+"\nCMD: Recoil_R2S"+"\nFile: "+fileNamePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return false;
                }

                line.replace(" |","|");
                line.replace("| ","|");

                subCommands = line.split('|', Qt::SkipEmptyParts);
                subCmdsCnt = subCommands.size ();

                for(quint8 i = 0; i < subCmdsCnt; i++)
                {
                    isGoodCmd = CheckLGCommand(subCommands[i]);

                    if(!isGoodCmd)
                    {
                        lgFile.close();
                        QString tempCrit = "Command(>) is not found in the command list. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+subCommands[i]+"\nFile: "+fileNamePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return false;
                    }
                }

            }
            else
            {

                if(line.startsWith (RECOIL_R2SCMD) && line.size() > RECOIL_R2SCMDCNT)
                {
                    subCommands = line.split(' ', Qt::SkipEmptyParts);

                    if(subCommands.size() > 2)
                    {
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "Too many variables after Recoil_R2S command. Can only have 0-1 variable. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+subCommands[2]+"\nFile: "+fileNamePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return false;
                    }


                    subCommands[1].toULong (&isNumber);

                    if(!isNumber)
                    {
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "Recoil_R2S Skew is not a number.\nLine Number: "+QString::number(lineNumber)+"\nSkew Number:"+subCommands[1]+"\nFile: "+fileNamePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return false;
                    }

                    line = subCommands[0];
                }

                //Check Command to to see if it is good
                isGoodCmd = CheckLGCommand(line);

                if(!isGoodCmd)
                {
                    lgFile.close();
                    QString tempCrit = "Command(>) is not found in the command list. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+fileNamePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return false;
                }
            }

            //If Got a Signal and Player, then add Command
            if(gotSignal && gotPlayer)
            {

                //Check to See iff at Last Line
                if(!in.atEnd ())
                {
                    //Last Data Must Be Commands
                    commands << line;

                    gotCommands = true;
                }
                else
                {
                    isGoodLG = false;

                    if(playerForCMD == -1)
                        isGoodLG = true;
                    else if(playerForCMD < MAXGAMEPLAYERS && loadedLGNumbersTest[playerForCMD] != UNASSIGN)
                        isGoodLG = true;

                    if(isGoodLG)
                    {

                        commands << line;

                        if(!signal.startsWith (MAMESTAFTER) || (signal.startsWith (MAMESTAFTER) && commands.count() > 2))
                            signalsAndCommandsCountTest++;

                        //At Last Line of the File
                        //qDebug() << "Last1 Signal: " << signal << " Commands: " << commands;
                    }

                    gotPlayer = false;
                    gotCommands = false;
                    gotSignal = false;

                    commands.clear ();
                }
            }
            else if(gotSignal && !gotPlayer && gotCommands)
            {
                //If Got a Signal and Command, with No Player
                lgFile.close();
                QString tempCrit = "No player(*) between a signal(:) and command(>). Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nSignal: "+signal+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

        }
        else if(line[0] == RECOMMANDSTARTCHAR)
        {
            //Got a Optional Recoil/Reload Command - Check if Choosen Option and if Good Command

            //Do 4 Checks before Start Processing
            //If No Recoil Option Data Loaded - hasRecoilOption
            if(!hasRecoilOption)
            {
                lgFile.close();
                QString tempCrit = "Recoil\\Reload Option command data is missing in game file.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            //Check for |
            if(line.contains ('|'))
            {
                lgFile.close();
                QString tempCrit = "Recoil\\Reload Option Commands command doesn't support the '|'.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            //Check for All Players
            if(playerForCMD < 0)
            {
                lgFile.close();
                QString tempCrit = "Recoil\\Reload Option Commands command doesn't support the *All players.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            //Check for Got Output Signal, but No Player
            if(gotSignal && !gotPlayer)
            {
                lgFile.close();
                QString tempCrit = "Recoil\\Reload Option Commands command has Output Signal, but no Player.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+fileNamePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            QString tmpCMD;
            bool skipReloadCMD = false;
            bool isReloadCMD = false;
            QString chkLine = line;

            //Get Recoil/Reload Option for Loaded Light Gun Player
            quint8 reOption = testLoadRecoilForPLayer[playerForCMD];

            //If Loaded Light Gun Doesn't Support Reload & Is a Reload Option Command, then Skip
            if(line.startsWith(OPTIONRELOADCMD))
                isReloadCMD = true;

            if(isReloadCMD && !testLoadedLGSupportReload[playerForCMD])
                skipReloadCMD = true;

            //If Recoil_R2S with Skew Percentage, then Need to Remove Skew Percentage for Check
            if(line.startsWith (OPTIONRECOIL_R2SCMD) && line.size() > RECOIL_R2SCMDCNT)
            {
                subCommands = line.split(' ', Qt::SkipEmptyParts);
                chkLine = subCommands[0];
            }

            //Check if Command is in Loaded Light Gun Recoil/Reload String List and Not to Skip Reload Command
            if(recoilCMDsMap[reOption].contains(chkLine) && !skipReloadCMD)
            {
                //If Command Matches Loaded Light Gun, then Replace # with >
                tmpCMD = line;
                tmpCMD[0] = COMMANDSTARTCHAR;

                //If Recoil_R2S, then Set-up and Check
                if(tmpCMD.startsWith (RECOIL_R2SCMD) && tmpCMD.size() > RECOIL_R2SCMDCNT)
                {
                    if(subCommands.size() > 2)
                    {
                        lgFile.close();
                        QString tempCrit = "Too many variables after Recoil_R2S command. Can only have 0-1 variable. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+subCommands[2]+"\nFile: "+fileNamePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return false;
                    }

                    //recoilR2SSkewPrec[playerForCMD] = subCommands[1].toULong (&isNumber);

                    if(!isNumber)
                    {
                        lgFile.close();
                        QString tempCrit = "Recoil_R2S Skew is not a number.\nLine Number: "+QString::number(lineNumber)+"\nSkew Number:"+subCommands[1]+"\nFile: "+fileNamePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return false;
                    }

                    tmpCMD = subCommands[0];
                    tmpCMD[0] = COMMANDSTARTCHAR;
                }


                //Check Command to to see if it is good
                isGoodCmd = CheckLGCommand(tmpCMD);

                if(!isGoodCmd)
                {
                    lgFile.close();
                    QString tempCrit = "Command(>) is not found in the command list. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+fileNamePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return false;
                }


                //If Got a Signal and Player, then add Command
                if(gotSignal && gotPlayer)
                {

                    //Check to See if Not at Last Line
                    if(!in.atEnd ())
                    {
                        //Last Data Must Be Commands
                        commands << tmpCMD;

                        gotCommands = true;
                    }
                    else
                    {
                        isGoodLG = false;

                        if(playerForCMD == -1)
                            isGoodLG = true;
                        else if(playerForCMD < MAXGAMEPLAYERS && loadedLGNumbersTest[playerForCMD] != UNASSIGN)
                            isGoodLG = true;

                        if(isGoodLG)
                        {
                            //If at End of Game File, then Add Signal & Commands to QMap
                            commands << tmpCMD;
                            signalsAndCommands.insert(signal, commands);

                            if(!signal.startsWith (MAMESTAFTER) || (signal.startsWith (MAMESTAFTER) && commands.count() > 2))
                                signalsAndCommandsCountTest++;
                        }

                        gotPlayer = false;
                        gotCommands = false;
                        gotSignal = false;

                        commands.clear ();
                    }
                }
                else if(gotSignal && !gotPlayer && gotCommands)
                {
                    //If Got a Signal and Command, with No Player
                    lgFile.close();
                    QString tempCrit = "No player(*) between a signal(:) and command(>). Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nSignal: "+signal+"\nFile: "+fileNamePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return false;
                }
            }  //if(recoilCMDsMap[reOption].contains(line))
        }  //else if(line[0] == RECOMMANDSTARTCHAR)
        else
        {
            //Something went wrong. Extra Line
            lgFile.close();
            QString tempCrit = "This Line has no place in DefaultLG game file. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nLine: "+line+"\nFile: "+fileNamePath;
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
            return false;
        }

    }

    //If there is a blank line at the end, then need to enter the last data
    if(gotSignal && gotPlayer && gotCommands)
    {
        isGoodLG = false;

        if(playerForCMD == -1)
            isGoodLG = true;
        else if(playerForCMD < MAXGAMEPLAYERS && loadedLGNumbersTest[playerForCMD] != UNASSIGN)
            isGoodLG = true;

        //qDebug() << "Last2 Signal: " << signal << " Commands: " << commands;
        if(isGoodLG)
        {
            if(!signal.startsWith (MAMESTAFTER) || (signal.startsWith (MAMESTAFTER) && commands.count() > 2))
                signalsAndCommandsCountTest++;
        }
    }

    if(signalsAndCommandsCountTest == 0 && ignoreUselessDLGGF == false)
    {
        //No Signal, Player, & Command was Loaded
        lgFile.close();
        QString tempCrit = "No Signal, Player, and Command (not counting mame_start/mame_stop) was loaded into the QMap. If No signals to watch out for, then nothing to do, but to connect and disconnect.\nFile: "+fileNamePath;
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
        return false;
    }


    lgFile.close();

    return true;
}


void HookerEngine::LoadSettingsFromList()
{
    bool timerRunning = false;

    //Settings Values
    useDefaultLGFirst = p_comDeviceList->GetUseDefaultLGFirst ();
    //Don't Get Multi-Threading, as it needs reset
    //useMultiThreading = p_comDeviceList->GetUseMultiThreading ();
    quint32 newRefreshTimeDisplay = p_comDeviceList->GetRefreshTimeDisplay ();
    closeComPortGameExit = p_comDeviceList->GetCloseComPortGameExit ();
    ignoreUselessDLGGF = p_comDeviceList->GetIgnoreUselessDFLGGF ();
    bypassSerialWriteChecks = p_comDeviceList->GetSerialPortWriteCheckBypass ();
    enableNewGameFileCreation = p_comDeviceList->GetEnableNewGameFileCreation ();

    if(newRefreshTimeDisplay != refreshTimeDisplay)
    {
        if(p_refreshDisplayTimer->isActive ())
        {
            p_refreshDisplayTimer->stop ();
            timerRunning = true;
        }

        //Set Interval to the Setting of refresh Time Display
        p_refreshDisplayTimer->setInterval (refreshTimeDisplay);
    }

    emit SetComPortBypassWriteChecks(bypassSerialWriteChecks);

    if(timerRunning)
        p_refreshDisplayTimer->start (); 
}

void HookerEngine::SetUpLightGuns()
{
    quint8 numberLG = p_comDeviceList->GetNumberLightGuns();
    bool isLGDefaultLG;
    quint8 defaultLGNumber, i;
    ReaperSlideData slideData;

    //Disconnect old connections, if any
    if(!connectedReaperLG.isEmpty())
    {
        for(i = 0; i < connectedReaperLG.size (); i++)
            disconnect(p_comDeviceList->p_lightGunList[connectedReaperLG[i]], &LightGun::WriteCOMPort, this, &HookerEngine::WriteLGComPortSlot);

        connectedReaperLG.clear ();
    }

    //Connect new connections
    for(i = 0; i < numberLG; i++)
    {
        isLGDefaultLG = p_comDeviceList->p_lightGunList[i]->GetDefaultLightGun();
        defaultLGNumber = p_comDeviceList->p_lightGunList[i]->GetDefaultLightGunNumber();

        if(isLGDefaultLG && defaultLGNumber == RS3_REAPER)
        {
            slideData = p_comDeviceList->p_lightGunList[i]->GetReaperSlideData ();

            if(!slideData.disableHoldBack && slideData.enableHoldDelay)
            {
                connect(p_comDeviceList->p_lightGunList[i], &LightGun::WriteCOMPort, this, &HookerEngine::WriteLGComPortSlot);
                connectedReaperLG << i;
            }
        }
    }
}

void HookerEngine::HOTRWindowState(bool isMin)
{
    //When HOTR is Minimized, or on the Desktop. When Minimized, Stop the Display Timer

    isHOTRMinimized = isMin;

    if(isHOTRMinimized)
    {
        //Stop Display Timer
        if(p_refreshDisplayTimer->isActive ())
            p_refreshDisplayTimer->stop ();
    }
    else
    {
        //If HOTR is now on the Desktop and Game Running, Update Display Signals & Restart Timer
        if(isGameFound)
        {
            UpdateDisplayTimeOut();
            p_refreshDisplayTimer->start ();
        }
    }

    //Tell TCP of the Update
    emit WindowStateToTCP(isHOTRMinimized);
}


void HookerEngine::LoadUpdatePlayerAssignment()
{
    //ReLoad the PlayerAss.hor file
    p_comDeviceList->LoadPlayersAss();


    //Update Local Player Assignment Array
    for(quint8 i = 0; i < MAXPLAYERLIGHTGUNS; i++)
    {
        playersLGAssignment[i] = p_comDeviceList->GetPlayerLightGunAssignment(i);
    }
}


//Public Slots

void HookerEngine::ReadComPortSig(const quint8 &comPortNum, const QByteArray &readData)
{
    //It is Here, but not Implamented Yet
    quint8 tempPortNum = comPortNum;
    QByteArray tempPortRead = readData;

    //qDebug() << "Read Data from COM Port: " << tempPortNum << " Read Data: " << tempPortRead.toStdString ();

    ReadINIComPort(tempPortNum, tempPortRead, bufferNum, tempPortRead.length ());

    bufferNum++;
}

void HookerEngine::ErrorMessageCom(const QString &title, const QString &errorMsg)
{
    if(displayMB)
        QMessageBox::warning (p_guiConnect, title, errorMsg);
}

void HookerEngine::WriteLGComPortSlot(quint8 cpNum, QString cpData)
{
    QByteArray cpBA = cpData.toUtf8 ();

    //qDebug() << "COM Port: " << cpNum << " Data: " << cpData;

    //Send Data to COM Port
    emit WriteComPortSig(cpNum, cpBA);
}


void HookerEngine::ErrorMessage(const QString &title, const QString &message)
{
    if(displayMB)
        QMessageBox::critical (p_guiConnect, title, message, QMessageBox::Ok);
}


//Private Slots

void HookerEngine::TCPConnected()
{
    //qDebug() << "TCP Socket is Connected - Hooker Engine TCP Connected";

    isTCPSocketConnected = true;

    emit TCPStatus(isTCPSocketConnected);
}

void HookerEngine::TCPDisconnected()
{
    //qDebug() << "TCP Socket is Disconnected - Hooker Engine TCP Disconnected. Trying to Reconnect";

    isTCPSocketConnected = false;

    emit TCPStatus(isTCPSocketConnected);

    //If in Game Mode & TCP Socket Diconnects, then Clear Things Out
    if(isGameFound)
        ClearOnDisconnect();

    if(!p_hookSocket->isConnected && !p_hookSocket->isConnecting)
        emit StartTCPSocket();
}


void HookerEngine::UpdateDisplayTimeOut()
{
    if(!addSignalDataDisplay.isEmpty ())
    {
        QMapIterator<QString, QString> x(addSignalDataDisplay);
        while (x.hasNext())
        {
            x.next();
            emit AddSignalFromGame(x.key(), x.value());
        }
        addSignalDataDisplay.clear ();
    }

    if(!updateSignalDataDisplay.isEmpty ())
    {
        QMapIterator<QString, QString> x(updateSignalDataDisplay);
        while (x.hasNext())
        {
            x.next();
            emit UpdateSignalFromGame(x.key(), x.value());
        }
        updateSignalDataDisplay.clear ();
    }
}


void HookerEngine::P1RecoilR2S()
{
    PXRecoilR2S(0);
}

void HookerEngine::P2RecoilR2S()
{
    PXRecoilR2S(1);
}

void HookerEngine::P3RecoilR2S()
{
    PXRecoilR2S(2);
}

void HookerEngine::P4RecoilR2S()
{
    PXRecoilR2S(3);
}

void HookerEngine::PXRecoilR2S(quint8 player)
{
    if(isRecoilDelaySet[player] && blockRecoil[player])
        doRecoilDelayEnds[player] = true;
    else
    {
        bool dlgCMDFound;
        QStringList dlgCommands;

        //Get Recoil Commands
        dlgCommands = p_comDeviceList->p_lightGunList[loadedLGNumbers[player]]->RecoilCommands(&dlgCMDFound);

        for(quint8 k = 0; k < dlgCommands.count(); k++)
            lgGamePlayers[player].Write (dlgCommands[k]);

        if(isRecoilDelaySet[player])
        {
            blockRecoil[player] = true;
            doRecoilDelayEnds[player] = false;
            openSolenoidOrRecoilDelay[player].start();
        }
    }
}


void HookerEngine::P1LGDisplayDelay()
{
    PXLGDisplayDelay(0);
}

void HookerEngine::P2LGDisplayDelay()
{
    PXLGDisplayDelay(1);
}

void HookerEngine::P3LGDisplayDelay()
{
    PXLGDisplayDelay(2);
}

void HookerEngine::P4LGDisplayDelay()
{
    PXLGDisplayDelay(3);
}

void HookerEngine::PXLGDisplayDelay(quint8 player)
{
    if(didDisplayWrite[player])
    {
        didDisplayWrite[player] = false;
    }
    else
    {
        if(!lgDisplayDelayOtherCMDs[player].isEmpty())
        {
            WriteDisplayDelayCMD(player, lgDisplayDelayOtherCMDs[player]);
            lgDisplayDelayOtherCMDs[player].clear();
        }
        else if(!lgDisplayDelayAmmoCMDs[player].isEmpty())
        {
            //qDebug() << "Ammo Player" << player << lgDisplayDelayAmmoCMDs[player];
            WriteDisplayDelayCMD(player, lgDisplayDelayAmmoCMDs[player]);
            lgDisplayDelayAmmoCMDs[player].clear();
        }
        else if(!lgDisplayDelayLifeCMDs[player].isEmpty())
        {
            //qDebug() << "Life Player" << player << lgDisplayDelayLifeCMDs[player];
            WriteDisplayDelayCMD(player, lgDisplayDelayLifeCMDs[player]);
            lgDisplayDelayLifeCMDs[player].clear();
        }
        else
            isLGDisplayOnDelay[player] = false;
    }

    //If Queue is not Empty, re-start Delay Timer
    if(isLGDisplayOnDelay[player])
        lgDisplayDelayTimer[player].start();
}


void HookerEngine::WriteDisplayDelayCMD(quint8 player, QString command)
{
    lgGamePlayers[player].Write (command);
}


void HookerEngine::P1CloseSolenoidOrRecoilDelay()
{
    //qDebug() << "Recoil Delay Timer Ended" << isRecoilDelaySet[0];

    if(isRecoilDelaySet[0])
        PXRecoilDelay(0);
    else
        PXCloseSolenoid(0);
}


void HookerEngine::P2CloseSolenoidOrRecoilDelay()
{
    if(isRecoilDelaySet[1])
        PXRecoilDelay(1);
    else
        PXCloseSolenoid(1);
}

void HookerEngine::P3CloseSolenoidOrRecoilDelay()
{
    if(isRecoilDelaySet[2])
        PXRecoilDelay(2);
    else
        PXCloseSolenoid(2);
}


void HookerEngine::P4CloseSolenoidOrRecoilDelay()
{
    if(isRecoilDelaySet[3])
        PXRecoilDelay(3);
    else
        PXCloseSolenoid(3);
}

void HookerEngine::PXCloseSolenoid(quint8 player)
{
    if(closeSolenoidCMDs[player].isEmpty ())
    {
        QStringList dlgCommands;
        bool dlgCMDFound;

        dlgCommands = p_comDeviceList->p_lightGunList[loadedLGNumbers[player]]->RecoilValueCommands(&dlgCMDFound, 0);

        if(dlgCMDFound)
            closeSolenoidCMDs[player] = dlgCommands;
    }


    for(quint8 i = 0; i < closeSolenoidCMDs[player].length(); i++)
        lgGamePlayers[player].Write (closeSolenoidCMDs[player][i]);

    isLGSolenoidOpen[player] = false;
    blockRecoilValue[player] = true;
    timesStuckOpenSolenoid[player]++;

    //Display Critical Warning that the Solenoid was Open Too long
    QString tempCrit = "P"+QString::number(player+1)+" light gun's solenoid was open for longer than 200ms. Please fix this critical error urgently, as it can damage the light gun's solenoid. ";
    tempCrit.append ("The affected player's Recoil_Value command will be blocked for the rest of the game, and reset when a new game starts.");

    if(timesStuckOpenSolenoid[player] > 2)
        tempCrit.append ("\n\nThis is the third time or more now! Don't be the reason why every shirt has \"Don't Iron while Wearing\" printed on it's tag");

    if(displayMB)
        QMessageBox::critical (p_guiConnect, "Solenoid Open Too Long", tempCrit, QMessageBox::Ok);

}


void HookerEngine::PXRecoilDelay(quint8 player)
{
    //qDebug() << "Recoil Delay for P" << player+1 << " doRecoilDelayEnds[player]: " << doRecoilDelayEnds[player];

    //If Do a Recoil on delay ends, then do recoil and re-start timer
    if(doRecoilDelayEnds[player])
    {
        bool dlgCMDFound;
        QStringList dlgCommands;

        //Get Recoil Commands
        dlgCommands = p_comDeviceList->p_lightGunList[loadedLGNumbers[player]]->RecoilCommands(&dlgCMDFound);

        for(quint8 k = 0; k < dlgCommands.count(); k++)
            lgGamePlayers[player].Write (dlgCommands[k]);

            //WriteLGUSBHID(player, dlgCommands[k]);

        openSolenoidOrRecoilDelay[player].start();
        doRecoilDelayEnds[player] = false;
        blockRecoil[player] = true;
    }
    else
        blockRecoil[player] = false;
}


void HookerEngine::OpenSerialPortSlot(quint8 playerNum, bool noInit)
{
    quint8 lightGun, j;

    quint8 tempCPNum;
    QString tempCPName;
    qint32 tempBaud;
    quint8 tempData;
    quint8 tempParity;
    quint8 tempStop;
    quint8 tempFlow;
    QString tempPath;
    QStringList commands;
    QByteArray cpBA;
    bool isCommands;
    quint8 count = 0;

    //Get Light Gun Number
    lightGun = loadedLGNumbers[playerNum];


    //Gets COM Port Settings
    tempCPNum = loadedLGComPortNumber[playerNum];
    tempCPName = p_comDeviceList->p_lightGunList[lightGun]->GetComPortString();
    tempBaud = p_comDeviceList->p_lightGunList[lightGun]->GetComPortBaud();
    tempData = p_comDeviceList->p_lightGunList[lightGun]->GetComPortDataBits();
    tempParity = p_comDeviceList->p_lightGunList[lightGun]->GetComPortParity();
    tempStop = p_comDeviceList->p_lightGunList[lightGun]->GetComPortStopBits();
    tempFlow = p_comDeviceList->p_lightGunList[lightGun]->GetComPortFlow();
    tempPath = p_comDeviceList->p_lightGunList[lightGun]->GetComPortPath();

    //Opens the COM Port
    emit StartComPort(playerNum, tempCPNum, tempCPName, tempBaud, tempData, tempParity, tempStop, tempFlow, tempPath, true);

    lgConnectionClosed[playerNum] = false;

    if(!noInit)
    {
        //Get the Commnds for Open COM Port
        commands = p_comDeviceList->p_lightGunList[lightGun]->OpenComPortCommands(&isCommands);

        //qDebug() << "Command Count: " << cmdCount << " Commands: " << commands;

        //Write Commands to the COM Port
        if(isCommands)
        {
            for(j = 0; j < commands.count(); j++)
            {
                cpBA = commands[j].toUtf8 ();
                emit WriteComPortSig(tempCPNum, cpBA);
            }
        }
    }
}


void HookerEngine::CloseSerialPortSlot(quint8 playerNum, bool noInit, bool initOnly)
{
    quint8 j, lightGun;
    quint8 tempCPNum;
    QStringList commands;
    QByteArray cpBA;
    bool isCommands;

    //Get Light Gun Number
    lightGun = loadedLGNumbers[playerNum];

    //Get COM Port For Light Gun
    tempCPNum = loadedLGComPortNumber[playerNum];

    if(!noInit || initOnly)
    {
        //Get Close COM Port Commands for Light Gun
        commands = p_comDeviceList->p_lightGunList[lightGun]->CloseComPortCommands(&isCommands);

        //Write Commnds to COM Port
        if(isCommands)
        {
            for(j = 0; j < commands.count(); j++)
            {
                cpBA = commands[j].toUtf8 ();
                emit WriteComPortSig(tempCPNum, cpBA);
            }
        }
    }

    //Closes The COM Port
    if(closeComPortGameExit && !initOnly)
        emit StopComPort(playerNum, tempCPNum);

    lgConnectionClosed[playerNum] = true;
}

void HookerEngine::WriteSerialPortSlot(quint8 cpNum, QString cpData)
{
    QByteArray cpBA = cpData.toUtf8 ();

    //qDebug() << "COM Port: " << cpNum << " Data: " << cpData;

    //Send Data to COM Port
    emit WriteComPortSig(cpNum, cpBA);
}


void HookerEngine::OpenUSBHIDSlot(quint8 playerNum, bool noInit)
{
    quint8 lightGun, j;
    QByteArray cpBA;
    QStringList commands;
    bool isCommands;
    quint8 count = 0;

    //Get Light Gun Number
    lightGun = loadedLGNumbers[playerNum];

    //For USB HID Connection
    HIDInfo lgHIDInfo = p_comDeviceList->p_lightGunList[lightGun]->GetUSBHIDInfo ();
    emit StartUSBHID(playerNum, lgHIDInfo);

    lgConnectionClosed[playerNum] = false;

    if(!noInit)
    {
        //Get the Commnds for Open USB HID
        commands = p_comDeviceList->p_lightGunList[lightGun]->OpenComPortCommands(&isCommands);

        //qDebug() << "Command Count: " << cmdCount << " Commands: " << commands;

        //Write Commands to the USB HID
        if(isCommands)
        {
            for(j = 0; j < commands.count(); j++)
            {
                cpBA = QByteArray::fromHex(commands[j].toUtf8 ());
                emit WriteUSBHID(playerNum, cpBA);
            }
        }
    }
}

void HookerEngine::CloseUSBHIDSlot(quint8 playerNum, bool noInit, bool initOnly)
{
    quint8 j, lightGun;
    QStringList commands;
    bool isCommands;
    QByteArray cpBA;

    //Get Light Gun Number
    lightGun = loadedLGNumbers[playerNum];

    if(!noInit || initOnly)
    {
        //Get Close USB HID Commands for Light Gun
        commands = p_comDeviceList->p_lightGunList[lightGun]->CloseComPortCommands(&isCommands);

        //Write Commnds to USB HID
        if(isCommands)
        {
            for(j = 0; j < commands.count(); j++)
            {
                cpBA = QByteArray::fromHex(commands[j].toUtf8 ());
                emit WriteUSBHID(playerNum, cpBA);
            }
        }
    }

    //Closes The USB HID
    if(closeComPortGameExit && !initOnly)
        emit StopUSBHID(playerNum);

    lgConnectionClosed[playerNum] = true;
}

void HookerEngine::WriteUSBHIDSlot(quint8 playerNum, QString cpData)
{
    /*
    //Check to make Sure Even Number of Hex Values as 2 Hex Values = 1 Byte
    if (cpData.length() % 2 != 0)
    {
        //cpData.prepend ('0');
        QString tempCrit = "The USB HID data needs to have even number of hex values. As two hex values is one byte. Cannot transfer a half of a byte. Please correct the data";
        if(displayMB)
            QMessageBox::warning (p_guiConnect, "USB HID Data Not Correct", tempCrit, QMessageBox::Ok);
        return;
    }
    */

    //Convert String to Hex QByteArray
    QByteArray cpBA = QByteArray::fromHex(cpData.toUtf8 ());

    //Send Data to USB HID
    emit WriteUSBHID(playerNum, cpBA);
}


void HookerEngine::OpenTCPServerSlot(quint8 playerNum, bool noInit)
{
    quint8 lightGun, j;
    QByteArray cpBA;
    QStringList commands;
    bool isCommands;

    //Get Light Gun Number
    lightGun = loadedLGNumbers[playerNum];

    isTCPConnected = p_hookComPortWin->IsTCPConnected (lgTCPPort[playerNum]);
    isTCPConnecting = p_hookComPortWin->IsTCPConnecting (lgTCPPort[playerNum]);

    if(!isTCPConnected && !isTCPConnecting)
    {
        //Connect to TCP Server
        emit ConnectTCPServer(lgTCPPort[playerNum]);
    }

    isTCPConnected = p_hookComPortWin->IsTCPConnected (lgTCPPort[playerNum]);

    //Wait until connected, can take 1 seconds. Connecting started when loading DefaultLG game file
    while(!isTCPConnected)
    {
        QThread::msleep(TCPSLEEPTIME);
        isTCPConnected = p_hookComPortWin->IsTCPConnected (lgTCPPort[playerNum]);
        //qDebug() << "Waiting for TCP Server connected" << isTCPConnected;
    }

    ConnectedLightGun(playerNum);

    if(!noInit)
    {
        //Get the Commnds for Open COM Port
        commands = p_comDeviceList->p_lightGunList[lightGun]->OpenComPortCommands(&isCommands);

        //qDebug() << "Command Count: " << commands.count() << " Commands: " << commands;

        //Write Commands to the TCP
        if(isCommands)
        {
            for(j = 0; j < commands.count(); j++)
            {
                QString tempCMD = commands[j] + '\n';
                cpBA = tempCMD.toUtf8 ();
                if(firstTCPPort == lgTCPPort[playerNum])
                    emit WriteTCPServer(cpBA);
                else
                    emit WriteTCPServer1(cpBA);
            }
        }
    }

    //Says LG Has Open and Init
    lgConnectionClosed[playerNum] = false;
    lgTCPConnections++;
}

void HookerEngine::CloseTCPServerSlot(quint8 playerNum, bool noInit, bool initOnly)
{
    quint8 j, lightGun;
    QStringList commands;
    bool isCommands;
    QByteArray cpBA;

    //Get Light Gun Number
    lightGun = loadedLGNumbers[playerNum];

    if(!noInit || initOnly)
    {
        //Get Close TCP Commands for Light Gun
        commands = p_comDeviceList->p_lightGunList[lightGun]->CloseComPortCommands(&isCommands);

        //Write Commnds to TCP
        if(isCommands)
        {
            for(j = 0; j < commands.count(); j++)
            {
                QString tempCMD = commands[j] + '\n';
                cpBA = tempCMD.toUtf8 ();
                if(firstTCPPort == lgTCPPort[playerNum])
                    emit WriteTCPServer(cpBA);
                else
                    emit WriteTCPServer1(cpBA);
            }
        }
    }

    lgTCPConnections--;
    DisconnectedLightGun(playerNum);

    if(closeComPortGameExit && !initOnly)
    {
        if(lgTCPConnections == 0)
            emit DisconnectTCPServer();
    }

    //Says LG is Init & Closed if closeComPortGameExit is true
    lgConnectionClosed[playerNum] = true;
}

void HookerEngine::WriteTCPServerSlot(quint8 playerNum, QString cpData)
{
    //QString tempData = cpData + "\n";
    cpData.append ('\n');
    QByteArray cpBA = cpData.toUtf8 ();

    //qDebug() << "TCP Player: " << playerNum << " Data: " << cpData;
    //Send Data to COM Port
    if(playerNum == 0)
        emit WriteTCPServer(cpBA);
    else
        emit WriteTCPServer1(cpBA);
}


//Process the Read Data from The TCP Socket. This is MAME or Demulshooter
void HookerEngine::ProcessTCPData(const QString &signal, const QString &data)
{
    //qDebug() << "Processing TCP Data, signal:" << signal << "data:" << data;
    //qDebug() << "Processing TCP Data, String is:" << tcpReadData;

    //qDebug() << "signal:" << signal << "data:" << data;
        //qDebug() << tcpSocketData[i];

    //Check if No Game is Found
    if(!isGameFound)
    {
        if(isEmptyGame)
        {
            if(signal == PAUSE)
                emit UpdatePauseFromGame(data);
            else if(signal.startsWith (ORIENTATION))
                emit UpdateOrientationFromGame(signal,data);
        }
    }
    else
    {
        //qDebug() << "0: " << signal << " 1: " << data;

        //Check if it is a State, else it is a Signal
        if(signal == PAUSE || signal.startsWith (ORIENTATION))
        {
            //qDebug() << "State: " << signal << " Data: " << data;

            //Process the Command(s) attached to the State, only on MH INI Side
            if(isINIGame)
            {
                if(stateAndCommands.contains(signal))
                    ProcessINICommands(signal, data, true);
            }

            //Update Pause and Orientation to the Display
            if(signal == PAUSE)
                emit UpdatePauseFromGame(data);
            else
                emit UpdateOrientationFromGame(signal,data);

        }
        else
        {
            //Signal Side
            //If not In the QMap, then Add it. Also Add it to
            //One of the Display QMaps, when Timer runs out.
            if(!isHOTRMinimized)
            {
                if(signalsAndData.contains(signal) == false)
                {
                    signalsAndData.insert(signal,data);
                    AddSignalForDisplay(signal, data);
                }
                else
                {
                    signalsAndData[signal] = data;
                    UpdateSignalForDisplay(signal, data);
                }
            }

            //qDebug() << "Processing Signal:" << signal << "and Data:" << data;

            if(signalsAndCommands.contains(signal))
            {
                if(lgFileLoaded)
                    ProcessLGCommands(signal, data);
                else if(iniFileLoaded)
                    ProcessINICommands(signal, data, false);
            }
        }
    } //isGameFound
}

//Process Filtered Read Data from The TCP Socket, Mostly for the Display Data when not Minimized
void HookerEngine::ProcessFilterTCPData(const QString &signal, const QString &data)
{
    //qDebug() << "signal:" << signal << "data:" << data;

    //if(isGameFound && !isHOTRMinimized)
    if(!isHOTRMinimized)
    {
        QString newSignal = signal;

        if(newSignal[0] == 'M' && newSignal[1] == 'a' && newSignal[2] == 'm')
        {
            if(newSignal[4] == 'P' && newSignal.size() == 9)
                newSignal = PAUSE;
            else if(newSignal[4] == 'O' && newSignal.size() == 15)
                newSignal.replace(MAMEORIENTATION, ORIENTATION);
        }

        if(newSignal == PAUSE)
            emit UpdatePauseFromGame(data);
        else if(newSignal.startsWith (ORIENTATION))
            emit UpdateOrientationFromGame(newSignal,data);
        //Signal Side
        //If not In the QMap, then Add it. Also Add it to
        //One of the Display QMaps, when Timer runs out.
        else if(signalsAndData.contains(newSignal) == false)
        {
            signalsAndData.insert(newSignal,data);
            AddSignalForDisplay(newSignal, data);
        }
        else
        {
            signalsAndData[newSignal] = data;
            UpdateSignalForDisplay(newSignal, data);
        }
    }
}


//qDebug() << "Processing Signal:" << signal << "and Data:" << data;


void HookerEngine::GameStart(const QString &data)
{
    gameName = data;

    isGameFound = true;
    gameHasRun = true;
    isEmptyGame = false;

    //Re-Load and Update Player Assignment
    LoadUpdatePlayerAssignment();

    //Start Refresh Time for Display Timer
    if(!isHOTRMinimized)
        p_refreshDisplayTimer->start ();

    //if(isDefaultLGGame)
    //    connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::ProcessLGCommands);
    //else if(isINIGame)
    //    connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::PreINICommandProcess);

    //Start Looking For Game Files to Load
    GameFound();
}


void HookerEngine::GameStartEmpty()
{
    emit MameConnectedNoGame();
    isEmptyGame = true;
}



void HookerEngine::GameStopped()
{
    //qDebug() << "GameStopped in Hooker Engine";

    quint8 j;

    if(isDefaultLGGame)
    {
        disconnect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::ProcessLGCommands);
        disconnect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, this, &HookerEngine::ProcessLGCommands);
        isFilteredConnected = false;
    }
    else if(isINIGame)
    {
        disconnect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::PreINICommandProcess);
        disconnect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, this, &HookerEngine::PreINICommandProcess);
        isFilteredConnected = false;
    }

    //Game Has Stopped
    isGameFound = false;
    isEmptyGame = false;

    //qDebug() << "Game Has Stopped!!!!!!!!!!!";

    //Tell TCP Socket that Game Stopped
    //Not Needed Anymore, as TCP Socket looks for Game Stop now
    //emit TCPGameStop();

    //Stop Refresh Time for Display Timer
    if(p_refreshDisplayTimer->isActive ())
        p_refreshDisplayTimer->stop ();

    //Clear Refresh Display QMaps
    addSignalDataDisplay.clear();
    updateSignalDataDisplay.clear();

    //Run the commands attached to 'mame_stop'
    if(lgFileLoaded)
        ProcessLGCommands(MAMESTOPFRONT, "");
    else if(iniFileLoaded)
        ProcessINICommands(MAMESTOPFRONT, "", false);

    if(newINIFileMade || newLGFileMade)
    {
        //qDebug() << "newINIFileMade";

        //If New Files where Made, then Print all Signals Collected to the File

        QTextStream out(p_newFile);

        //Print out the Signals to the new file and close
        QMapIterator<QString, QString> x(signalsAndData);
        while (x.hasNext())
        {
            x.next();
            if(newINIFileMade)
                out << x.key() << "=\n";
            else
                out << ":" << x.key() << "\n";
        }

        //Close the File
        p_newFile->close ();
        delete p_newFile;
        //Make bools false, since closed now
        newINIFileMade = false;
        newLGFileMade = false;
    }
    else if(iniFileLoaded)
    {
        //qDebug() << "Opening INI File";

        //If INI file Loaded, Must Search for Any New Signal(s) not in the File
        //These Signals are Appended to the INI Game File and Closed

        quint8 foundCount = 0;
        bool foundNewSignal = false;
        QStringList nemSignalList;

        //Searching the 2 QMaps for Any New Signal(s)

        QMapIterator<QString, QString> x(signalsAndData);
        while (x.hasNext())
        {
            x.next();
            //qDebug() << "Searching for: " << x.key();
            if(!signalsAndCommands.contains(x.key()) && !signalsNoCommands.contains(x.key()))
            {
                nemSignalList << x.key();
                foundCount++;
                foundNewSignal = true;
                //qDebug() << "New Signal Found for Existing INI File: " << x.key();
            }
        }

        //If Any New Signal(s) Found, then Append to the INI Game File
        if(foundNewSignal)
        {
            //Open INI File to Append
            QFile iniFileTemp(gameINIFilePath);
            iniFileTemp.open(QIODevice::Append | QIODevice::Text);
            QTextStream out(&iniFileTemp);

            for(j = 0; j < foundCount; j++)
                out << nemSignalList[j] << "=\n";

            iniFileTemp.close ();
        }

        iniFileLoaded = false;
    }

    //Reset Light guns
    p_comDeviceList->ResetLightgun ();

    //Clear out Old Games Signal & Data and States & Data
    signalsAndData.clear ();
    statesAndData.clear ();

    //Clear out Signal & Commands QMap & Signal & No Commands QLists
    signalsAndCommands.clear ();
    stateAndCommands.clear ();
    signalsNoCommands.clear ();
    statesNoCommands.clear ();

    //The other 2 File Loaded bools where checked before, so Clear the Last 2
    lgFileLoaded = false;
    iniFileLoaded = false;


    for(quint8 i = 0; i < MAXGAMEPLAYERS; i++)
    {
        isPRecoilR2SFirstTime[i] = true;
        recoilR2SSkewPrec[i] = 100;
        isLGDisplayOnDelay[i] = false;
        lgDisplayDelayAmmoCMDs[i].clear();
        lgDisplayDelayLifeCMDs[i].clear();
        lgDisplayDelayOtherCMDs[i].clear();
        if(lgDisplayDelayTimer[i].isActive())
            lgDisplayDelayTimer[i].stop();
        didDisplayWrite[i] = false;
        isLGSolenoidOpen[i] = false;
        closeSolenoidCMDs[i].clear ();
        blockRecoilValue[i] = false;
        blockRecoil[i] = false;
        doRecoilDelayEnds[i] = false;
        skipRecoilSlowMode[i] = false;
        blockShake[i] = false;
        blockRecoil_R2S[i] = false;
        blockShakeActive[i] = false;
        blockRecoil_R2SActive[i] = false;
        //lgOutputConnection[i] = -1;
        lgTCPPort[i] = 0;
        lgTCPPlayer[i] = UNASSIGN;
    }
}


void HookerEngine::PreINICommandProcess(const QString &signal, const QString &data)
{
    ProcessINICommands(signal, data, stateAndCommands.contains(signal));
}


void HookerEngine::ConnectedLightGun(const quint8 lgNum)
{
    isLGConnected[lgNum] = true;
    //qDebug() << "Light Gun:" << lgNum << "Has been Connected";
}

void HookerEngine::DisconnectedLightGun(const quint8 lgNum)
{
    isLGConnected[lgNum] = false;
    //qDebug() << "Light Gun:" << lgNum << "Has been Disconnected";
}





//Private Member Functions


void HookerEngine::ClearOnDisconnect()
{
    quint8 j;
    bool stillOpen = false;

    //When TCP Socket Disconnects, Then Clear Out Game Data

    //Check if the Light Gun connection Closed or Not
    for(quint8 i = 0; i < MAXGAMEPLAYERS; i++)
    {
        if(lgConnectionClosed[i] == false)
            stillOpen = true;
    }

    //If Connections are not closed, then Run mame_stop command
    if(stillOpen)
    {
        if(lgFileLoaded)
            ProcessLGCommands(MAMESTOPFRONT, "");
        else if(iniFileLoaded)
            ProcessINICommands(MAMESTOPFRONT, "", false);
    }

    //Output Says No Game
    emit MameConnectedNoGame();

    //Check if Still Connected to TCP Socket
    if(isFilteredConnected)
    {
        if(isDefaultLGGame)
        {
            disconnect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::ProcessLGCommands);
            disconnect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, this, &HookerEngine::ProcessLGCommands);
            isFilteredConnected = false;
        }
        else if(isINIGame)
        {
            disconnect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::PreINICommandProcess);
            disconnect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, this, &HookerEngine::PreINICommandProcess);
            isFilteredConnected = false;
        }
    }

    //If Game Files Still Open, close them out
    if(newINIFileMade || newLGFileMade)
    {
        //qDebug() << "newINIFileMade";

        //If New Files where Made, then Print all Signals Collected to the File

        QTextStream out(p_newFile);

        //Print out the Signals to the new file and close
        QMapIterator<QString, QString> x(signalsAndData);
        while (x.hasNext())
        {
            x.next();
            if(newINIFileMade)
                out << x.key() << "=\n";
            else
                out << ":" << x.key() << "\n";
        }

        //Close the File
        p_newFile->close ();
        delete p_newFile;
        //Make bools false, since closed now
        newINIFileMade = false;
        newLGFileMade = false;
    }
    else if(iniFileLoaded)
    {
        //qDebug() << "Opening INI File";

        //If INI file Loaded, Must Search for Any New Signal(s) not in the File
        //These Signals are Appended to the INI Game File and Closed

        quint8 foundCount = 0;
        bool foundNewSignal = false;
        QStringList nemSignalList;

        //Searching the 2 QMaps for Any New Signal(s)

        QMapIterator<QString, QString> x(signalsAndData);
        while (x.hasNext())
        {
            x.next();
            //qDebug() << "Searching for: " << x.key();
            if(!signalsAndCommands.contains(x.key()) && !signalsNoCommands.contains(x.key()))
            {
                nemSignalList << x.key();
                foundCount++;
                foundNewSignal = true;
                //qDebug() << "New Signal Found for Existing INI File: " << x.key();
            }
        }

        //If Any New Signal(s) Found, then Append to the INI Game File
        if(foundNewSignal)
        {
            //Open INI File to Append
            QFile iniFileTemp(gameINIFilePath);
            iniFileTemp.open(QIODevice::Append | QIODevice::Text);
            QTextStream out(&iniFileTemp);

            for(j = 0; j < foundCount; j++)
                out << nemSignalList[j] << "=\n";

            iniFileTemp.close ();
        }


    }

    //Reset Light guns
    p_comDeviceList->ResetLightgun ();

    //Stop Refresh Time for Display Timer
    if(p_refreshDisplayTimer->isActive ())
        p_refreshDisplayTimer->stop ();


    isGameFound = false;
    gameHasRun = false;
    isEmptyGame = false;
    signalsAndCommands.clear ();
    stateAndCommands.clear ();
    signalsNoCommands.clear ();
    statesNoCommands.clear ();
    signalsAndData.clear ();
    statesAndData.clear ();
    addSignalDataDisplay.clear();
    updateSignalDataDisplay.clear();
    iniFileLoaded = false;
    lgFileLoaded = false;


    for(quint8 i = 0; i < MAXGAMEPLAYERS; i++)
    {
        isPRecoilR2SFirstTime[i] = true;
        recoilR2SSkewPrec[i] = 100;
        isLGDisplayOnDelay[i] = false;
        lgDisplayDelayAmmoCMDs[i].clear();
        lgDisplayDelayLifeCMDs[i].clear();
        lgDisplayDelayOtherCMDs[i].clear();
        if(lgDisplayDelayTimer[i].isActive())
            lgDisplayDelayTimer[i].stop();
        didDisplayWrite[i] = false;
        isLGSolenoidOpen[i] = false;
        closeSolenoidCMDs[i].clear ();
        blockRecoilValue[i] = false;
        blockRecoil[i] = false;
        skipRecoilSlowMode[i] = false;
        blockShake[i] = false;
        blockRecoil_R2S[i] = false;
        blockShakeActive[i] = false;
        blockRecoil_R2SActive[i] = false;
        //lgOutputConnection[i] = -1;
        lgTCPPort[i] = 0;
        lgTCPPlayer[i] = UNASSIGN;
    }


}




void HookerEngine::GameFound()
{
    //If Using Default LG Files First
    if(useDefaultLGFirst)
    {
        //Check if File Exists
        isDefaultLGGame = IsDefaultLGFile();

        if(isDefaultLGGame)
        {
            emit MameConnectedGame(gameName, false, false);
            connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::ProcessLGCommands);
            connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, this, &HookerEngine::ProcessLGCommands);
            isFilteredConnected = true;
            LoadLGFile();
        }
        else
        {
            //Check if there is an INI file when no Default LG Game File
            isINIGame = IsINIFile();

            if(isINIGame)
            {
                emit MameConnectedGame(gameName, true, false);
                connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::PreINICommandProcess);
                connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, this, &HookerEngine::PreINICommandProcess);
                isFilteredConnected = true;
                //Load and Process the INI File
                LoadINIFile();
            }
            else
            {
                //Checks if a Default File Exists
                isDefaultLGGame = IsDefaultDefaultLGFile();

                //If there is a Default File, then Load it
                if(isDefaultLGGame)
                {
                    emit MameConnectedGame("default", false, false);
                    connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::ProcessLGCommands);
                    connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, this, &HookerEngine::ProcessLGCommands);
                    isFilteredConnected = true;
                    LoadLGFile();
                }
                else
                {
                    //Create New Game File, if Settings is Set
                    if(enableNewGameFileCreation)
                    {
                        //Creates a New Deafualt LG Game File
                        NewLGFile();
                        QString tempGF = "Making New Game File ";
                        tempGF.append (gameName);
                        emit MameConnectedGame(tempGF, false, false);
                    }
                    else
                        emit MameConnectedGame(gameName, false, true);
                    //If enableNewGameFileCreation false, then do nothing for this game
                }
            }
        }
    }
    else
    {
        //Check if there is an INI file
        isINIGame = IsINIFile();

        if(isINIGame)
        {
            emit MameConnectedGame(gameName, true, false);
            connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::PreINICommandProcess);
            connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, this, &HookerEngine::PreINICommandProcess);
            isFilteredConnected = true;
            //Load and Process the INI File
            LoadINIFile();
        }
        else
        {
            //Check is a Default LG File
            isDefaultLGGame = IsDefaultLGFile();

            if(isDefaultLGGame)
            {
                emit MameConnectedGame(gameName, false, false);
                connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::ProcessLGCommands);
                connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, this, &HookerEngine::ProcessLGCommands);
                isFilteredConnected = true;
                LoadLGFile();
            }
            else
            {
                //Checks if a Default File Exists
                isINIGame = IsDefaultINIFile();

                //If there is a Default File, then Load it
                if(isINIGame)
                {
                    emit MameConnectedGame("default", true, false);
                    connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignals, this, &HookerEngine::PreINICommandProcess);
                    connect(p_hookSocket,&HookTCPSocket::FilteredOutputSignalsBoth, this, &HookerEngine::PreINICommandProcess);
                    isFilteredConnected = true;
                    LoadINIFile();
                }
                else
                {
                    //Create New Game File, if Settings is Set
                    if(enableNewGameFileCreation)
                    {
                        //Creates a New INI Game File
                        NewINIFile();
                        QString tempGF = "Making New Game File ";
                        tempGF.append (gameName);
                        emit MameConnectedGame(tempGF, true, false);
                    }
                    else
                        emit MameConnectedGame(gameName, true, true);
                    //If enableNewGameFileCreation false, then do nothing for this game
                }
            }
        }
    }  // else of if(useDefaultLGFirst)

}

void HookerEngine::ProcessCommands(QString signalName, QString value, bool isState)
{
    //qDebug() << "iniFileLoaded: " << iniFileLoaded << " iniFileLoaded: " << iniFileLoaded;
    //If INI Game File Loaded, then Use INI Side, If Default LG Game File Loaded, then Use Default LG Side
    if(iniFileLoaded)
        ProcessINICommands(signalName, value, isState);
    else if(lgFileLoaded)
        ProcessLGCommands(signalName, value);

}


///////////////////////////////////////////////////////////////////////////
//MAMEHooker INI Side, to make it Compatible
///////////////////////////////////////////////////////////////////////////

bool HookerEngine::IsINIFile()
{
    bool fileFound;

    gameINIFilePath = iniMAMEPath+"/"+gameName+ENDOFINIFILE;

    fileFound = QFile::exists (gameINIFilePath);

    //qDebug() << gameINIFilePath << " is found: " << fileFound;

    return fileFound;

}

bool HookerEngine::IsDefaultINIFile()
{
    bool fileFound;

    QString tempFilePath = iniPath+"/"+DEFAULTFILE+ENDOFINIFILE;

    fileFound = QFile::exists (tempFilePath);

    //qDebug() << gameINIFilePath << " is found: " << fileFound;

    if(fileFound)
        gameINIFilePath = tempFilePath;

    return fileFound;
}


void HookerEngine::LoadINIFile()
{
    QString line;
    QString signal;
    QString commands;
    QStringList tempSplit;
    quint16 indexEqual;
    bool isOutput = false;
    bool goodCommand;
    quint16 lineNumber = 0;

    iniFileLoadFail = false;
    openComPortCheck.clear();

    //Clear Out TCP Output Signal List
    outputSignalsStates.clear();

    //Close Any Old USB HIDs from INI Side
    CloseINIUSBHID();
    hidPlayerMap.clear ();

    //Open File. If Failed to Open, so Critical Message Box
    QFile iniFile(gameINIFilePath);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool openFile = iniFile.open (QIODeviceBase::ReadOnly | QIODevice::Text);
#else
    bool openFile = iniFile.open (QIODevice::ReadOnly | QIODevice::Text);
#endif

    if(!openFile)
    {
        QString tempCrit = "Can not open INI data file to read. Please close program and solve file problem. Might be permissions problem.\nFile: "+gameINIFilePath;
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "File Error", tempCrit, QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&iniFile);

    while(!in.atEnd ())
    {

        //Get a line of data from file
        line = in.readLine();
        lineNumber++;

        //All lines have an '=', except for the one with '[' and ']'
        if(!line.startsWith("["))
        {
            //Get the Index of the Equal
            indexEqual = line.indexOf('=',0);

            //If there is Nothing After '=', put in NoCommnds List
            //If something after '=' then Split it up into Signals and Command(s)
            if(line.length() != indexEqual+1)
            {

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                if(line[indexEqual-1] == ' ')
                    signal = line.first(indexEqual-1);
                else
                    signal = line.first(indexEqual);

                if(line[indexEqual+1] == ' ')
                    commands = line.sliced(indexEqual+2);
                else
                    commands = line.sliced(indexEqual+1);
#else
                if(line[indexEqual-1] == ' ')
                    signal = line.chopped(line.size()-(indexEqual-1));
                else
                    signal = line.chopped(line.size()-indexEqual);

                quint16 indexOverEq;
                if(line[indexEqual+1] == ' ')
                    indexOverEq = indexEqual + 2;
                else
                    indexOverEq = indexEqual + 1;

                for(quint16 k = indexOverEq; k < line.size(); k++)
                    signal.append (line[k]);
#endif

                commands.replace(", ", ",");
                commands.replace(" ,", ",");

                tempSplit = commands.split(',', Qt::SkipEmptyParts);

                if(!isOutput)
                {
                    if(signal.startsWith ("On"))
                        signal.remove(0,2);

                    if(signal.startsWith(JUSTMAME, Qt::CaseInsensitive))
                        signal.insert(4,'_');

                    signal = signal.toLower();
                }

                //qDebug() << "Loaded INI Signal or State: " << signal;

                goodCommand = CheckINICommands(tempSplit, lineNumber, gameINIFilePath);

                //If bad command file, then fail load
                if(!goodCommand)
                {
                    iniFileLoadFail = true;
                    iniFile.close();
                    return;
                }


                if(isOutput)
                    signalsAndCommands.insert(signal, tempSplit);
                else
                    stateAndCommands.insert(signal, tempSplit);

                outputSignalsStates << signal;
            }
            else
            {
                line.chop(1);
                if(isOutput)
                    signalsNoCommands << line;
                else
                    statesNoCommands << line;
            }
        }
        else
        {
            //This is the lines with the '[' and ']'
            if(line.contains(SIGNALSTATE, Qt::CaseInsensitive))
                isOutput = true;
            else
                isOutput = false;
        }

    }

    //Close File
    iniFile.close();
    iniFileLoaded = true;

    //Add mame_stop and States to outputSignalsStates
    //outputSignalsStates << PAUSE << ORIENTATION;

    //qDebug() << "MH - Output Signals & Commands QMap:" << signalsAndCommands;

    //qDebug() << "MH - Output Signals & States String List:" << outputSignalsStates;

    //Tell TCP Socket a Game has Started
    emit TCPGameStart(outputSignalsStates);

    //Bypass the COM Port Connection Fail Warning Pop-up, as MH does
    emit SetBypassComPortConnectFailWarning(true);

    //Process the "mame_start" Signal
    ProcessINICommands(MAMESTARTAFTER, "", true);


}

bool HookerEngine::CheckINICommands(QStringList commadsNotChk, quint16 lineNumber, QString filePathName)
{
    QString command;
    QStringList subCommands;
    quint8 subCmdCnt;
    QString subCmd;
    quint8 commandCount = commadsNotChk.length ();
    bool isCommandsGood = true;
    quint8 i, j;

    for(i = 0; i < commandCount; i++)
    {
        command = commadsNotChk[i];

        if(command.contains ('|'))
        {
            commadsNotChk[i].replace(" |","|");
            commadsNotChk[i].replace("| ","|");

            subCommands = commadsNotChk[i].split ('|');
            subCmdCnt = subCommands.length ();

            for(j = 0; j < subCmdCnt; j++)
            {
                subCmd = subCommands[j];

                //Check for %s%, if so replace with 0
                if(subCmd.contains (SIGNALDATAVARIBLE))
                    subCmd.replace(SIGNALDATAVARIBLE, "0");

                isCommandsGood = CheckINICommand(subCmd, lineNumber, filePathName);

                if(!isCommandsGood)
                {
                    QString tempCrit = "Loaded INI game file has a bad command. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+subCommands[j]+"\nFile: "+filePathName;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                    return false;
                }
            }
        }
        else
        {
            //Check for %s%, if so replace with 0
            if(command.contains (SIGNALDATAVARIBLE))
                command.replace(SIGNALDATAVARIBLE, "0");

            isCommandsGood = CheckINICommand(command, lineNumber, filePathName);

            if(!isCommandsGood)
            {
                QString tempCrit = "Loaded INI game file has a bad command. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+command+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                return false;
            }
        }

    }

    //Ran the gaunlet
    return isCommandsGood;
}

bool HookerEngine::CheckINICommand(QString commndNotChk, quint16 lineNumber, QString filePathName)
{
    QStringList temp1, temp2;
    INIPortStruct portTemp;
    quint8 comPortNumber;
    bool isCommandsGood = true;
    bool chkSetting;
    quint8 i;


    //COM Port Commands, Starts with "cm" or "cs"
    if(commndNotChk.startsWith(PORTCMDSTART1, Qt::CaseInsensitive) == true || commndNotChk.startsWith(PORTCMDSTART2, Qt::CaseInsensitive) == true)
    {
        //qDebug() << "Is a COM Port Command";

        //COM Port Open Command
        if(commndNotChk.startsWith(COMPORTOPEN, Qt::CaseInsensitive) || commndNotChk.startsWith(COMPORTSETTINGS, Qt::CaseInsensitive))
        {
            //This will give 3 Strings 1: cmo/css  2: Com Port #  3: Settings
            temp1 = commndNotChk.split(' ', Qt::SkipEmptyParts);

            bool isNumber;

            comPortNumber = temp1[1].toUInt(&isNumber);

            if(!isNumber)
            {
                QString tempCrit = "In COM port open command, port number is not a number.\nLine Number: "+QString::number(lineNumber)+"\nPort Number: "+temp1[1]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            //Add in COM Port number for checks
            openComPortCheck << comPortNumber;

            //qDebug() << "Open COM Port Command: " << temp1[0] << " " << temp1[1] << " " << temp1[2];

            //Split the Settings into 4 Strings  1: Baud  2: Parity  3: Data  4: Stop
            temp2 = temp1[2].split('_', Qt::SkipEmptyParts);
            temp2[0].remove(BAUDREMOVE);
            temp2[1].remove(PARITYREMOVE);
            temp2[2].remove(DATAREMOVE);
            temp2[3].remove(STOPREMOVE);

            portTemp.baud = temp2[0].toUInt(&isNumber);

            if(!isNumber)
            {
                QString tempCrit = "In COM port open command, baud rate is not a number.\nLine Number: "+QString::number(lineNumber)+"\nBaud Rate Number: "+temp2[0]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            chkSetting = false;

            for(i = 0; i < BAUD_NUMBER; i++)
            {
                if(portTemp.baud == BAUDDATA_ARRAY[i])
                    chkSetting = true;
            }

            if(!chkSetting)
            {
                QString tempCrit = "In COM port open command, baud rate is not a correct rate.\nLine Number: "+QString::number(lineNumber)+"\nBaud Rate Number: "+temp2[0]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                return false;
            }


            if(temp2[1] == "N")
                portTemp.parity = 0;
            else if(temp2[1] == "E")
                portTemp.parity = 2;
            else if(temp2[1] == "O")
                portTemp.parity = 3;
            else if(temp2[1] == "S")
                portTemp.parity = 4;
            else if(temp2[1] == "M")
                portTemp.parity = 5;
            else
            {
                QString tempCrit = "In COM port open command, parity is not a correct char[N,E,O,S,M].\nLine Number: "+QString::number(lineNumber)+"\nParity Char: "+temp2[1]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            portTemp.data = temp2[2].toUInt(&isNumber);

            if(!isNumber)
            {
                QString tempCrit = "In COM port open command, data bits is not a number.\nLine Number: "+QString::number(lineNumber)+"\nData Bits Number: "+temp2[2]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            //Can be 5-8
            if(portTemp.data < 5 || portTemp.data > 8)
            {
                QString tempCrit = "In COM port open command, data bits is not in range [5-8].\nLine Number: "+QString::number(lineNumber)+"\nData Bits Number: "+temp2[2]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                return false;
            }


            if(temp2[3] == "1.5")
                temp2[3] = "3";

            portTemp.stop = temp2[3].toUInt(&isNumber);

            if(!isNumber)
            {
                QString tempCrit = "In COM port open command, stop bits is not a number.\nLine Number: "+QString::number(lineNumber)+"\nStop Bits Number: "+temp2[3]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            //Can be 1-3
            if(portTemp.stop == 0 || portTemp.stop > 3)
            {
                QString tempCrit = "In COM port open command, stop bits is not in range [1,1.5,2].\nLine Number: "+QString::number(lineNumber)+"\nData Bits Number: "+temp2[3]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                return false;
            }


            //Good open or settings command
            return true;


        }
        else if(commndNotChk.startsWith(COMPORTCLOSE, Qt::CaseInsensitive))
        {
            //This will give 2 Strings 1: cmc  2: Com Port #
            temp1 = commndNotChk.split(' ', Qt::SkipEmptyParts);

            bool isNumber;
            quint8 comPortNumber = temp1[1].toUInt (&isNumber);

            if(!isNumber)
            {
                QString tempCrit = "In COM port close command, port number is not a number.\nLine Number: "+QString::number(lineNumber)+"\nPort Number: "+temp1[1]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            if(!openComPortCheck.isEmpty ())
            {
                if(!openComPortCheck.contains (comPortNumber))
                {
                    QString tempCrit = "In COM port close command, port number doesn't match any open port number(s).\nLine Number: "+QString::number(lineNumber)+"\nPort Number: "+temp1[1]+"\nFile: "+filePathName;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                    return false;
                }
            }

            //ran the gaunlet, good Close CMD
            return true;

        }
        else if(commndNotChk.startsWith(COMPORTREAD, Qt::CaseInsensitive))
        {
            //This will give 4 Strings 1: cmr  2: Com Port #  3: Buffer # 4: length
            //temp1 = commands[i].split(' ', Qt::SkipEmptyParts);

            QString tempCrit = "In COM port read, reads are not implamented yet. Please remove read.\nLine Number: "+QString::number(lineNumber)+"\nRead CMD: "+commndNotChk+"\nFile: "+filePathName;
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
            return false;


            //Not Implamented Yet
            //ReadINIComPort(temp1[1].toUInt(), temp1[2].toUInt(), temp1[3].toUInt());
        }
        else if(commndNotChk.startsWith(COMPORTWRITE, Qt::CaseInsensitive))
        {
            //This will give 3 Strings 1: cmw  2: Com Port #  3: Data
            temp1 = commndNotChk.split(' ', Qt::SkipEmptyParts);

            //qDebug() << "Write to COM Port on Number: " << temp1[1] << " with Data: " << temp1[2];

            bool isNumber;
            quint8 comPortNumber = temp1[1].toUInt (&isNumber);

            if(!isNumber)
            {
                QString tempCrit = "In COM port write command, port number is not a number.\nLine Number: "+QString::number(lineNumber)+"\nPort Number: "+temp1[1]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
                return false;
            }

            if(!openComPortCheck.isEmpty ())
            {
                if(!openComPortCheck.contains (comPortNumber))
                {
                    QString tempCrit = "In COM port write command, port number doesn't match any open port number(s).\nLine Number: "+QString::number(lineNumber)+"\nPort Number: "+temp1[1]+"\nFile: "+filePathName;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Loaded INI Game File Has Error", tempCrit, QMessageBox::Ok);
                    return false;
                }
            }

            //Ran the gaunlet, good Write CMD;
            return true;

        }
    }//COM Port Commands, Starts with "cm" or "cs"
    else if(commndNotChk.startsWith(USBHIDCMD, Qt::CaseSensitive) == true)
    {
        //USB HID Write Command
        //Sample command: ghd 1 &H04B4 &H6870 2 &h02:&h0%s%
        //ghd - Command, 1 - Device, &H04B4 - VendorID, &H6870 - ProductID, 2 - Number of Bytes, &h02 - Byte 0, &h0%s% - Byte 1

        //This will give 6 Strings 1: ghd  2: Device#  3: VendorID 4: ProductID 5: Number of Bytes 6: Bytes
        temp1 = commndNotChk.split(' ', Qt::SkipEmptyParts);

        if(temp1.count () != 6)
        {
            QString tempCrit = "In USB HID write command, doesn't have enough or too many variables. It should be ghd and 5 variables.\nLine Number: "+QString::number(lineNumber)+"\nCommand: "+commndNotChk+"\nFile: "+filePathName;
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
            return false;
        }


        bool isNumber;
        quint8 deviceNumber = temp1[1].toUInt (&isNumber);
        quint16 vendorID;
        quint16 productID;

        if(!isNumber)
        {
            QString tempCrit = "In USB HID write command, Device is not a number.\nLine Number: "+QString::number(lineNumber)+"\nDevice Number: "+temp1[1]+"\nFile: "+filePathName;
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
            return false;
        }

        if(temp1[2][0] == '&' && temp1[2][1] == 'H')
        {
            //Remove the '&H' So just have Hex Number Left
            temp1[2].remove(0,2);
            vendorID = temp1[2].toUShort (&isNumber, 16);

            if(!isNumber)
            {
                QString tempCrit = "In USB HID write command, VendorID is not a hex number.\nLine Number: "+QString::number(lineNumber)+"\nVendorID Number: "+temp1[2]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
                return false;
            }
        }
        else
        {
            QString tempCrit = "In USB HID write command, VendorID doesn't have the needed &H in front of number.\nLine Number: "+QString::number(lineNumber)+"\nVendorID: "+temp1[2]+"\nFile: "+filePathName;
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
            return false;
        }

        if(temp1[3][0] == '&' && temp1[3][1] == 'H')
        {
            //Remove the '&H' So just have Hex Number Left
            temp1[3].remove(0,2);
            productID = temp1[3].toUShort (&isNumber, 16);

            if(!isNumber)
            {
                QString tempCrit = "In USB HID write command, ProductID is not a hex number.\nLine Number: "+QString::number(lineNumber)+"\nProductID Number: "+temp1[3]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
                return false;
            }
        }
        else
        {
            QString tempCrit = "In USB HID write command, ProductID doesn't have the needed &H in front of number.\nLine Number: "+QString::number(lineNumber)+"\nProductID: "+temp1[3]+"\nFile: "+filePathName;
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
            return false;
        }

        bool foundHID = FindUSBHIDDeviceINI(vendorID, productID, deviceNumber);

        if(!foundHID)
        {
            QString tempCrit = "In USB HID write command, Cannot find the HID with the vendorID, productID, and device number.\nLine Number: "+QString::number(lineNumber)+"\nProductID: "+temp1[3]+"\nFile: "+filePathName;
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
            return false;
        }

        quint8 byteNumber = temp1[4].toUInt (&isNumber);

        if(!isNumber)
        {
            QString tempCrit = "In USB HID write command, Number of Bytes is not a number.\nLine Number: "+QString::number(lineNumber)+"\nNumber of Bytes: "+temp1[4]+"\nFile: "+filePathName;
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
            return false;
        }

        temp2 = temp1[5].split(':', Qt::SkipEmptyParts);

        if(temp2.count () != byteNumber)
        {
            QString tempCrit = "In USB HID write command, Number of Bytes doesn't equal number of data bytes.\nLine Number: "+QString::number(lineNumber)+"\nNumber of Bytes: "+temp1[4]+"\nData Bytes: "+temp1[5]+"\nFile: "+filePathName;
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
            return false;
        }

        for(quint8 i = 0; i < temp2.count (); i++)
        {
            if(temp2[i][0] == '&' && temp2[i][1] == 'h')
            {
                //Remove the '&H' So just have Hex Number Left
                temp2[i].remove(0,2);
                quint16 byteData = temp2[i].toUShort (&isNumber, 16);

                if(!isNumber)
                {
                    QString tempCrit = "In USB HID write command, Data Byte is not a hex number.\nLine Number: "+QString::number(lineNumber)+"\nData Byte: "+temp2[i]+"\nFile: "+filePathName;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
                    return false;
                }
            }
            else
            {
                QString tempCrit = "In USB HID write command, Data Byte doesn't have the needed &h in front of number.\nLine Number: "+QString::number(lineNumber)+"\nData Byte: "+temp2[i]+"\nFile: "+filePathName;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Processes INI Command Error", tempCrit, QMessageBox::Ok);
                return false;
            }
        }

        //Ran the gaunlet
        return true;
    }
    else if(commndNotChk.startsWith(ININULLCMD, Qt::CaseInsensitive))
    {
        //Null Command - Do Nothing, but return true;
        return true;
    }
    else
    {
        return false;
    }

    //Something Bad happen to get here
    return false;
}


bool HookerEngine::FindUSBHIDDeviceINI(quint16 vendorID, quint16 productID, quint8 deviceNumber)
{
    HIDInfo foundHIDInfo;
    QString path;
    QStringList pathList;
    quint8 numberOfDevices = 0;
    QString hidKey;

    //USB HID Init
    if(!isUSBHIDInit)
    {
        //Init the USB HID
        if (hid_init())
        {
            QString tempCrit = "The USB HID Init failed in the Hooker Engine. Something got fucked up.";
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Hooker Engine: USB HID Init", tempCrit, QMessageBox::Ok);
            return false;
        }
        else
            isUSBHIDInit = true;
    }

    //Key used for the QMap
    hidKey = QString::number(vendorID, 16);
    hidKey.append (QString::number(productID, 16));
    hidKey.append (QString::number(deviceNumber, 10));

    //Check to See If in QMap Already
    if(hidPlayerMap.contains (hidKey))
        return true;

    hid_device_info *devs;

    //Find USB HID Devices with the same vendorID & productID
    devs = hid_enumerate(vendorID, productID);

    //Go Through the USB HID Devices.
    //Multiple Devices can have Same vendorID, productID, and Serial Number
    for (; devs; devs = devs->next)
    {
        path = QString::fromLatin1(devs->path);

        if(numberOfDevices == 0)
        {
            pathList << path;
            numberOfDevices++;
        }
        else
        {
            if(!pathList.contains (path))
            {
                pathList << path;
                numberOfDevices++;
            }
        }


        if(numberOfDevices == deviceNumber)
        {

            foundHIDInfo.vendorID = devs->vendor_id;
            QString tempVID = QString::number(devs->vendor_id, 16).rightJustified(4, '0');
            tempVID = tempVID.toUpper ();
            tempVID.prepend ("0x");
            foundHIDInfo.vendorIDString = tempVID;

            foundHIDInfo.productID = devs->product_id;
            QString tempPID = QString::number(devs->product_id, 16).rightJustified(4, '0');
            tempPID = tempPID.toUpper ();
            tempPID.prepend ("0x");
            foundHIDInfo.productIDString = tempPID;

            foundHIDInfo.path = QString::fromLatin1(devs->path);
            QString tempDP = foundHIDInfo.path;
            tempDP.remove(0,ALIENUSBFRONTPATHREM);
            foundHIDInfo.displayPath = tempDP;

            foundHIDInfo.serialNumber = QString::fromWCharArray(devs->serial_number);
            foundHIDInfo.releaseNumber = devs->release_number;
            QString tempR = QString::number(devs->release_number, 16).rightJustified(4, '0');
            tempR = tempR.toUpper ();
            tempR.prepend ("0x");
            foundHIDInfo.releaseString = tempR;


            foundHIDInfo.manufacturer = QString::fromWCharArray(devs->manufacturer_string);
            foundHIDInfo.productDiscription = QString::fromWCharArray(devs->product_string);
            foundHIDInfo.interfaceNumber = devs->interface_number;
            foundHIDInfo.usagePage = devs->usage_page;
            foundHIDInfo.usage = devs->usage;
            foundHIDInfo.usageString = p_comDeviceList->ProcessHIDUsage(foundHIDInfo.usagePage, foundHIDInfo.usage);

            quint8 playerNum = hidPlayerMap.count ();

            if(playerNum >= 4)
            {
                QString tempCrit = "The USB HID can only have 4 HIDs open at one time for the light guns. This is for P1 to P4.\nVendorID: "+QString::number(vendorID, 16)+"\nProductID: "+QString::number(productID, 16)+"\nDevice Number: "+QString::number(deviceNumber);
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Hooker Engine: USB HID INI", tempCrit, QMessageBox::Ok);
                return false;
            }

            hidPlayerMap.insert(hidKey, playerNum);

            //Connect the USB HID
            emit StartUSBHID(playerNum, foundHIDInfo);

            //qDebug() << "Starting USB HID, playerNum: " << playerNum << " hidKey: " << hidKey;

            //Release HID enumeration
            hid_free_enumeration(devs);

            return true;
        }
    }

    //Release HID enumeration
    hid_free_enumeration(devs);

    QString tempCrit = "The USB HID search cannot find the device. Something got fucked up.\nVendorID: "+QString::number(vendorID, 16)+"\nProductID: "+QString::number(productID, 16)+"\nDevice Number: "+QString::number(deviceNumber);
    if(displayMB)
        QMessageBox::critical (p_guiConnect, "Hooker Engine: USB HID INI", tempCrit, QMessageBox::Ok);
    return false;

}




void HookerEngine::NewINIFile()
{

    //Copy default INI file over with game's name as the file's name
    bool copyFile = QFile::copy(iniDefaultPath, gameINIFilePath);

    if(!copyFile)
    {
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "File Error", "Can not copy default INI file to ini/MAME. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }


    //After copy, open file
    p_newFile = new QFile(gameINIFilePath);


    bool openFile = p_newFile->open (QIODevice::Append | QIODevice::Text);

    if(!openFile)
    {
        QString tempCrit = "Can not open INI data file to read. Please close program and solve file problem. Might be permissions problem.\nFile: "+gameINIFilePath;
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "File Error", tempCrit, QMessageBox::Ok);
        return;
    }


    //When Game Closes, will write out the signal to the INI file in ProcessTCPData

    newINIFileMade = true;

}


void HookerEngine::ProcessINICommands(QString signalName, QString value, bool isState)
{
    quint8 i;
    quint8 comPortNumber;
    INIPortStruct portTemp;
    QStringList temp1, temp2;
    QStringList commands;

    if(isState)
        commands = stateAndCommands[signalName];
    else
        commands = signalsAndCommands[signalName];


    //qDebug() << "Processing Commands from: " << signalName << "   Is Found in QMap: " << foundCommand;

    for(i = 0; i < commands.size (); i++)
    {
        //qDebug() << commands[i];

        //First Check if there are more values with '|', is so split.
        if(commands[i].contains ('|'))
        {
            commands[i].replace(" |","|");
            commands[i].replace("| ","|");

            temp1 = commands[i].split('|', Qt::SkipEmptyParts);

            //Pick the command to use, based on the value, as 0 Starts on the right and increases to the left. If value is blank, then use 0
            if(value != "")
            {
                quint16 dataNum = value.toUInt ();
                //It is -1 Since it Starts at 0
                qint16 mvSize = temp1.size () - 1;

                //If Value is Larger than the Number of Commands, then pick the highest command
                if(dataNum > mvSize)
                    dataNum = mvSize;

                commands[i] = temp1[dataNum];
            }
            else
            {
                commands[i] = temp1[0];
            }

        }

        //Second, Check if there is the %s% variable. If so, replace with value
        if(commands[i].contains(SIGNALDATAVARIBLE) && !commands[i].startsWith(USBHIDCMD))
        {
            commands[i].replace(SIGNALDATAVARIBLE, value);
        }

        //qDebug() << commands[i];

        //COM Port Commands, Starts with "cm" or "cs"
        if(commands[i].startsWith(PORTCMDSTART1, Qt::CaseInsensitive) == true || commands[i].startsWith(PORTCMDSTART2, Qt::CaseInsensitive) == true)
        {
            //qDebug() << "Is a COM Port Command";

            //COM Port Open Command
            if(commands[i].startsWith(COMPORTOPEN, Qt::CaseInsensitive) || commands[i].startsWith(COMPORTSETTINGS, Qt::CaseInsensitive))
            {
                //This will give 3 Strings 1: cmo/css  2: Com Port #  3: Settings
                temp1 = commands[i].split(' ', Qt::SkipEmptyParts);

                comPortNumber = temp1[1].toUInt();

                //qDebug() << "Open COM Port Command: " << temp1[0] << " " << temp1[1] << " " << temp1[2];

                //Split the Settings into 4 Strings  1: Baud  2: Parity  3: Data  4: Stop
                temp2 = temp1[2].split('_', Qt::SkipEmptyParts);
                temp2[0].remove(BAUDREMOVE);
                temp2[1].remove(PARITYREMOVE);
                temp2[2].remove(DATAREMOVE);
                temp2[3].remove(STOPREMOVE);

                portTemp.baud = temp2[0].toUInt();


                if(temp2[1] == "N")
                    portTemp.parity = 0;
                else if(temp2[1] == "E")
                    portTemp.parity = 2;
                else if(temp2[1] == "O")
                    portTemp.parity = 3;
                else if(temp2[1] == "S")
                    portTemp.parity = 4;
                else if(temp2[1] == "M")
                    portTemp.parity = 5;
                else
                    portTemp.parity = 1;

                portTemp.data = temp2[2].toUInt();

                if(temp2[3] == "1.5")
                    temp2[3] = "3";

                portTemp.stop = temp2[3].toUInt();


                //qDebug() << "COM Port Setting - Baud: " << temp2[0] << " Parity: " << temp2[1] << " Data: " << temp2[2] << " Stop: " << temp2[3];

                if(commands[i].startsWith(COMPORTOPEN, Qt::CaseInsensitive))
                {
                    //qDebug() << "Open COM Port on: " << comPortNumber;
                    iniPortMap.insert(comPortNumber,portTemp);
                    OpenINIComPort(comPortNumber);
                }
                else
                {
                    //qDebug() << "Change COM Port on: " << comPortNumber;
                    if(iniPortMap.contains(comPortNumber))
                        iniPortMap[comPortNumber] = portTemp;
                    else
                        iniPortMap.insert(comPortNumber,portTemp);
                }

            }
            else if(commands[i].startsWith(COMPORTCLOSE, Qt::CaseInsensitive))
            {
                //This will give 2 Strings 1: cmc  2: Com Port #
                temp1 = commands[i].split(' ', Qt::SkipEmptyParts);

                //qDebug() << "Close COM Port on Number: " << temp1[1];

                quint8 comPortNumber = temp1[1].toUInt ();

                CloseINIComPort(comPortNumber);
            }
            else if(commands[i].startsWith(COMPORTREAD, Qt::CaseInsensitive))
            {
                //This will give 4 Strings 1: cmr  2: Com Port #  3: Buffer # 4: length
                //temp1 = commands[i].split(' ', Qt::SkipEmptyParts);

                //Not Implamented Yet
                //ReadINIComPort(temp1[1].toUInt(), temp1[2].toUInt(), temp1[3].toUInt());
            }
            else if(commands[i].startsWith(COMPORTWRITE, Qt::CaseInsensitive))
            {
                //This will give 3 Strings 1: cmw  2: Com Port #  3: Data
                temp1 = commands[i].split(' ', Qt::SkipEmptyParts);

                //qDebug() << "Write to COM Port on Number: " << temp1[1] << " with Data: " << temp1[2];

                quint8 comPortNumber = temp1[1].toUInt ();

                WriteINIComPort(comPortNumber, temp1[2]);
            }
        }//COM Port Commands, Starts with "cm" or "cs"
        else if(commands[i].startsWith(ININULLCMD, Qt::CaseInsensitive))
        {
            //Null Command - Do Nothing
        }
        else if(commands[i].startsWith(USBHIDCMD, Qt::CaseSensitive) == true)
        {
            //USB HID Write Command
            //Sample command: ghd 1 &H04B4 &H6870 2 &h02:&h0%s%
            //ghd - Command, 1 - Device, &H04B4 - VendorID, &H6870 - ProductID, 2 - Number of Bytes, &h02 - Byte 0, &h0%s% - Byte 1

            //This will give 6 Strings 1: ghd  2: Device#  3: VendorID 4: ProductID 5: Number of Bytes 6: Bytes
            temp1 = commands[i].split(' ', Qt::SkipEmptyParts);

            QString hidKey;
            bool isNumber;
            quint8 deviceNumber = temp1[1].toUInt (&isNumber);

            //Remove the '&H' So just have Hex Number Left
            temp1[2].remove(0,2);
            quint16 vendorID = temp1[2].toUShort (&isNumber, 16);

            //Remove the '&H' So just have Hex Number Left
            temp1[3].remove(0,2);
            quint16 productID = temp1[3].toUShort (&isNumber, 16);

            //Not Needed, since can find size already
            //quint8 byteNumber = temp1[4].toUInt (&isNumber);

            quint8 valueMarkers = temp1[5].count (SIGNALDATAVARIBLE);
            quint16 valueNum = value.toUShort ();
            QString upperDigit, lowerDigit;

            if(valueMarkers > 1)
            {
                if(valueNum > 9)
                {
                    upperDigit = value[0];
                    lowerDigit = value[1];
                }
                else
                {
                    upperDigit= '0';
                    lowerDigit = value[0];
                }
            }
            else if(valueMarkers == 1)
                lowerDigit = value[0];

            //Split up the Data Bytes
            temp2 = temp1[5].split(':', Qt::SkipEmptyParts);

            QString dataBytes;

            for(quint8 j = 0; j < temp2.count (); j++)
            {
                //Remove the '&h' So just have Hex Number Left
                temp2[j].remove(0,2);

                if(temp2[j].contains(SIGNALDATAVARIBLE))
                {
                    if(valueMarkers > 1)
                    {
                        temp2[j].replace(SIGNALDATAVARIBLE, upperDigit);
                        valueMarkers--;
                    }
                    else if(valueMarkers == 1)
                        temp2[j].replace(SIGNALDATAVARIBLE, lowerDigit);
                }


                if(temp2[j].length () == 1)
                    temp2[j].prepend ('0');

                dataBytes.append (temp2[j]);
            }

            //qDebug() << "dataBytes: " << dataBytes;

            //Got All the Data Now

            //Find the USB HID Device
            //Key used for the QMap
            hidKey = QString::number(vendorID, 16);
            hidKey.append (QString::number(productID, 16));
            hidKey.append (QString::number(deviceNumber, 10));

            quint8 player = hidPlayerMap[hidKey];

            //Convert String to Hex QByteArray
            QByteArray cpBA = QByteArray::fromHex(dataBytes.toUtf8());

            //Send Data to USB HID
            emit WriteUSBHID(player, cpBA);
        }
    }//for(i
}




void HookerEngine::OpenINIComPort(quint8 cpNum)
{
    QString cpName = BEGINCOMPORTNAME+QString::number(cpNum);
    QString cpPath = cpName;
    cpPath.prepend (COMPORTPATHFRONT);


    //qDebug() << "OpenINIComPort with name: " << cpName << " Emitting StartComPort";

    emit StartComPort(0, cpNum, cpName, iniPortMap[cpNum].baud, iniPortMap[cpNum].data, iniPortMap[cpNum].parity, iniPortMap[cpNum].stop, 0, cpPath, true);
}

void HookerEngine::CloseINIComPort(quint8 cpNum)
{
    if(closeComPortGameExit)
        emit StopComPort(0, cpNum);
}

void HookerEngine::ReadINIComPort(quint8 cpNum, QByteArray readData, quint8 bfNum, quint16 lengthNum)
{
    //Not Implamented Yet
}

void HookerEngine::WriteINIComPort(quint8 cpNum, QString cpData)
{
    QByteArray cpBA = cpData.toUtf8 ();
    emit WriteComPortSig(cpNum, cpBA);
}

void HookerEngine::CloseINIUSBHID()
{
    quint8 numberPlayers = hidPlayerMap.count ();
    quint8 i;

    if(numberPlayers > 0)
    {
        for(i = 0; i < numberPlayers; i++)
            emit StopUSBHID(i);
    }
}


///////////////////////////////////////////////////////////////////////////
//Default LG Side, New Way of Doing Light Guns
///////////////////////////////////////////////////////////////////////////


bool HookerEngine::IsDefaultLGFile()
{
    bool fileFound;

    gameLGFilePath = defaultLGPath+"/"+gameName+ENDOFLGFILE;

    fileFound = QFile::exists (gameLGFilePath);

    //qDebug() << gameLGFilePath << " is found: " << fileFound;

    return fileFound;
}

bool HookerEngine::IsDefaultDefaultLGFile()
{
    bool fileFound;

    QString tempGameFilePath = defaultLGPath+"/"+DEFAULTFILE+ENDOFLGFILE;

    fileFound = QFile::exists (tempGameFilePath);

    if(fileFound)
        gameLGFilePath = tempGameFilePath;

    //qDebug() << gameLGFilePath << " is found: " << fileFound;

    return fileFound;
}

void HookerEngine::NewLGFile()
{
    //Copy default light gun file over with game's name as the file's name
    bool copyFile = QFile::copy(lgDefaultPath, gameLGFilePath);

    if(!copyFile)
    {
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "File Error", "Can not copy top of Default Light Gun game file to defaultLG directory. Please close program and solve file problem. Might be permissions problem.\nFile: "+gameLGFilePath, QMessageBox::Ok);
        return;
    }


    //After copy, open file
    p_newFile = new QFile(gameLGFilePath);


    bool openFile = p_newFile->open (QIODevice::Append | QIODevice::Text);

    if(!openFile)
    {
        QString tempCrit = "Can not open new default light gun game file to write. Please close program and solve file problem. Might be permissions problem.\nFile: "+gameLGFilePath;
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "File Error", tempCrit, QMessageBox::Ok);
        return;
    }


    //When Game Closes, will write out the signal to the INI file

    newLGFileMade = true;
}

void HookerEngine::LoadLGFile()
{
    QString line;
    quint8 playerNumber;
    QString signal;
    QStringList commands;
    bool gotCommands = false;
    bool gotSignal = false;
    bool gotPlayer = false;
    bool isNumber;
    bool begin = true;
    quint8 lgNumber;
    QString tempLine;
    quint8 tempPlayer;
    qint16 playerForCMD;
    bool isGoodCmd;
    QStringList subCommands;
    quint8 subCmdsCnt;
    quint16 lineNumber = 0;
    //bool isOutput = false;
    bool isStates = false;
    quint8 unassignLG = 0;
    quint16 signalsAndCommandsCountTest = 0;
    quint8 tcpServerCount = 0;

    bool gameRecoilsSupported[NUMBEROFRECOILS];
    bool gameReloadsSupported[NUMBEROFRECOILS];
    QStringList ammoValueCMDs;
    QStringList recoilCMDs;
    QStringList recoilR2SCMDs;
    QStringList recoilValueCMDs;
    QMap<quint8,QStringList> recoilCMDsMap;
    bool hasRecoilOption = false;

    bool isGoodLG;

    isMultipleTCPPorts = false;
    firstTCPPort = 0;
    secondTCPPort = 0;

    outputSignalsStates.clear();


    for(quint8 x = 0; x < MAXGAMEPLAYERS; x++)
    {
        gameRecoilsSupported[x] = false;
        gameReloadsSupported[x] = false;
        loadRecoilForPLayer[x] = 1;
        foundRecoilForPlayer[x] = false;
    }



    //Close Any Old USB HIDs from INI Side
    CloseINIUSBHID();

    lgFileLoadFail = false;

    QFile lgFile(gameLGFilePath);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool openFile = lgFile.open (QIODeviceBase::ReadOnly | QIODevice::Text);
#else
    bool openFile = lgFile.open (QIODevice::ReadOnly | QIODevice::Text);
#endif

    if(!openFile)
    {
        QString tempCrit = "Can not open default light gun game file to read. Please close program and solve file problem. Might be permissions problem.\nFile: "+gameLGFilePath;
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&lgFile);

    while(!in.atEnd ())
    {

        //Get a line of data from file
        line = in.readLine();
        lineNumber++;

        //For Faster Processing, search first char
        if(line[0] == 'P' && begin)
        {
            //File should Start with "Players"
            if(line.startsWith(PLAYERSSTART))
            {
                //Next Line is the number of Players
                line = in.readLine();
                lineNumber++;
                numberLGPlayers = line.toUInt (&isNumber);

                if(!isNumber)
                {
                    lgFileLoadFail = true;
                    lgFile.close();
                    QString tempCrit = "The number of Players is not a number. Please close program and fix.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number: "+line+"\nFile: "+gameLGFilePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return;
                }

                //Disconnect Game Players
                DisconnectGamePlayers();

                //qDebug() << "Number of Players: " << numberLGPlayers;

                //Read in Players and there Order IE P1, P2 or P3, P1, P4
                for(quint8 i = 0; i < numberLGPlayers; i++)
                {
                    line = in.readLine();
                    lineNumber++;

                    if(line[0] != PLAYER2CHHAR)
                    {
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "The player needs to be P[1-4]. Please close program and fix.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number: "+line+"\nFile: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }

                    //Check for Slow Mode on PLayers IE 'P1 Slow'
                    if(line.contains(SLOWMODE))
                    {
                        QStringList playerSlowMode = line.split (' ', Qt::SkipEmptyParts);
                        playerSlowMode[0] = playerSlowMode[0].trimmed();
                        //Make line just the player number IE P1, P2, or P3
                        line = playerSlowMode[0];
                        playerSlowMode[1] = playerSlowMode[1].trimmed();
                        if(playerSlowMode[1] == SLOWMODE)
                            loadedLGSlowMode[i] = true;
                        else
                        {
                            loadedLGSlowMode[i] = false;
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "Something is after the player, and is not Slow mode.\nLine Number: "+QString::number(lineNumber)+"\nAfter player: "+playerSlowMode[1]+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }
                    }
                    else
                        loadedLGSlowMode[i] = false;


                    //Remove 'P'
                    line.remove (0,1);
                    playerNumber = line.toUInt (&isNumber);
                    if(isNumber)
                    {
                        if(playerNumber == 0 || playerNumber > MAXPLAYERLIGHTGUNS)
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "Player number under \"Players\" is out of range. Player number range is 1-4.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number: "+line+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }
                        lgPlayerOrder[i] = playerNumber-1;
                    }
                    else
                    {
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "Player number was not a number in game file.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number: "+line+"\nFile: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }
                    //qDebug() << "Player " << i << " connected to: " << line;

                    //Check if Light Gun is a Default Light Gun
                    lgNumber = playersLGAssignment[lgPlayerOrder[i]];

                    //Load Light Gun Order, based on Player Order and Player's Assignment
                    loadedLGNumbers[i] = lgNumber;

                    //If Player Assignment is Unassign, increament unassignLG and set Output Connection to Unknown
                    if(lgNumber == UNASSIGN)
                    {
                        unassignLG++;
                        lgOutputConnection[i] = UNKNOWNCONNECT;
                    }
                    else
                    {
                        //Check if Slow mode enabled, if so then enable it in light gun
                        if(loadedLGSlowMode[i])
                            p_comDeviceList->p_lightGunList[lgNumber]->SlowModeEnabled ();

                        lgOutputConnection[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetOutputConnection();

                        //qDebug() << "Game Player" << i << "Output Connection:" << lgOutputConnection[i];

                        //Check if Serial Port or USB HID. If Serial Port, get Serial Port Number
                        if(lgOutputConnection[i] == USBHID)
                        {
                            //qDebug() << "Game Player" << i << "Setting up USB HID";

                            isLoadedLGUSB[i] = true;
                            loadedLGComPortNumber[i] = UNASSIGN;

                            if(p_comDeviceList->p_lightGunList[lgNumber]->IsRecoilDelay())
                            {
                                isRecoilDelaySet[i] = true;
                                delayRecoilTime[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetRecoilDelay ();
                                openSolenoidOrRecoilDelay[i].setInterval (delayRecoilTime[i]);
                                //qDebug() << "Setting Up Recoil Delay to" << delayRecoilTime[i] << "ms";
                            }
                            else
                                isRecoilDelaySet[i] = false;

                            connect(&lgGamePlayers[i], &GamePlayer::ConnectToLG, this, &HookerEngine::OpenUSBHIDSlot);
                            connect(&lgGamePlayers[i], &GamePlayer::DisconnectFromLG, this, &HookerEngine::CloseUSBHIDSlot);
                            connect(&lgGamePlayers[i], &GamePlayer::WriteToLG, this, &HookerEngine::WriteUSBHIDSlot);

                            lgGamePlayers[i].SetWriteNumber (i);
                        }
                        else if(lgOutputConnection[i] == SERIALPORT)
                        {
                            //qDebug() << "Game Player" << i << "Setting up USB HID";

                            isLoadedLGUSB[i] = false;
                            loadedLGComPortNumber[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetComPortNumber();
                            isRecoilDelaySet[i] = false;
                            openSolenoidOrRecoilDelay[i].setInterval (OPENSOLENOIDDEFAULTTIME);

                            connect(&lgGamePlayers[i], &GamePlayer::ConnectToLG, this, &HookerEngine::OpenSerialPortSlot);
                            connect(&lgGamePlayers[i], &GamePlayer::DisconnectFromLG, this, &HookerEngine::CloseSerialPortSlot);
                            connect(&lgGamePlayers[i], &GamePlayer::WriteToLG, this, &HookerEngine::WriteSerialPortSlot);

                            lgGamePlayers[i].SetWriteNumber (loadedLGComPortNumber[i]);
                        }
                        else if(lgOutputConnection[i] == TCP)
                        {
                            isLoadedLGUSB[i] = false;
                            isRecoilDelaySet[i] = false;
                            loadedLGComPortNumber[i] = UNASSIGN;
                            //quint16 tcpServerPort = p_hookComPortWin->TCPPortNumber();

                            //Get TCP LG Port and Player Number
                            lgTCPPort[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetTCPPort ();
                            lgTCPPlayer[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetTCPPlayer ();

                            //Makes Sure that the Players TCP Port number is the same
                            if(tcpServerCount == 0)
                            {
                                firstTCPPort = lgTCPPort[i];
                                tcpServerCount++;
                            }
                            else
                            {
                                //Checks if the Ports are the same
                                if(firstTCPPort != lgTCPPort[i])
                                {
                                    tcpServerCount++;
                                    isMultipleTCPPorts = true;

                                    if(tcpServerCount > MAXTCPSERVERS)
                                    {
                                        lgFileLoadFail = true;
                                        lgFile.close();
                                        QString tempCrit = "Three or more TCP Server ports cannot be used.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number: "+line+"\nFile: "+gameLGFilePath;
                                        if(displayMB)
                                            QMessageBox::critical (p_guiConnect, "DefaultLG Game File Load Error", tempCrit, QMessageBox::Ok);
                                        return;
                                    }
                                    else
                                        secondTCPPort = lgTCPPort[i];
                                }
                            }

                            connect(&lgGamePlayers[i], &GamePlayer::ConnectToLG, this, &HookerEngine::OpenTCPServerSlot);
                            connect(&lgGamePlayers[i], &GamePlayer::DisconnectFromLG, this, &HookerEngine::CloseTCPServerSlot);
                            connect(&lgGamePlayers[i], &GamePlayer::WriteToLG, this, &HookerEngine::WriteTCPServerSlot);

                            if(firstTCPPort == lgTCPPort[i])
                                lgGamePlayers[i].SetWriteNumber (0);
                            else
                                lgGamePlayers[i].SetWriteNumber (1);
                        }
                        else if(lgOutputConnection[i] == UNKNOWNCONNECT)
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "Game Player " + QString::number(i) + " light gun doesn't have output connection method.\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "DefaultLG Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        //Does Light Gun Support Recoil_Value Command
                        loadedLGSupportRecoilValue[i] = p_comDeviceList->p_lightGunList[lgNumber]->IsRecoilValueSupported ();
                        //Does Light Gun Support Reload
                        loadedLGSupportReload[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetSupportedReload ();
                        //Light Gun Recoil Priority
                        p_loadedLGRecoilPriority[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetRecoilPriorityHE();

                        //for(quint8 y = 0; y < 4; y++)
                        //    qDebug() << "Player" << i+1 << "recoil priority:" << p_loadedLGRecoilPriority[i][y];

                        //Get Display Refresh, If None is set, Use Default Display Refresh
                        bool isRefreshSet;
                        displayRefresh[i] = p_comDeviceList->p_lightGunList[lgNumber]->GetDisplayRefresh(&isRefreshSet);

                        if(!isRefreshSet || displayRefresh[i] < 1)
                            displayRefresh[i] = DISPLAYREFRESHDEFAULT;

                        //qDebug() << "For Player: " << i << " Setting Refresh Display: " << displayRefresh[i];
                        lgDisplayDelayTimer[i].setInterval (displayRefresh[i]);
                    }
                }  //for(quint8 i = 0; i < numberLGPlayers; i++)

                if(unassignLG == numberLGPlayers)
                {
                    lgFileLoadFail = true;
                    lgFile.close();
                    QString tempCrit = "No Player(s) have assign light gun. Atleast one player needs to have an assign light gun. Please close game, and then assign a light gun to the player.\nFile: "+gameLGFilePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return;
                }

                //Don't Run the Players Again
                begin = false;
            }
        }
        else if(line == RECOILRELOAD)
        {
            //Loads Up Support Recoil & Reload Options in Game File
            quint8 index = 0;

            //Get Ammo_Value: Recoil and Reload Info from Game File Header
            line = in.readLine();
            lineNumber++;
            line.replace("  ", " ");

            if(line.startsWith (AMMOVALUECMDONLY))
            {
                QStringList ammoValueSL = line.split (' ', Qt::SkipEmptyParts);
                if(ammoValueSL[1] == "1")
                {
                    gameRecoilsSupported[0] = true;
                    gameReloadsSupported[0] = true;
                    ammoValueSL[0].prepend('#');
                    ammoValueCMDs << ammoValueSL[0];


                    if(ammoValueSL.length() > 2)
                    {
                        ammoValueSL[2].prepend('#');
                        ammoValueCMDs << ammoValueSL[2];
                    }
                    else
                        ammoValueCMDs << ammoValueSL[0];
                }
                else
                    ammoValueCMDs << "" << "";
            }
            else
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "The line didn't match 'Ammo_Value' Recoil Options\nLine"+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }

            recoilCMDsMap.insert (0, ammoValueCMDs);

            //Get Recoil: Recoil and Reload Info from Game File Header
            line = in.readLine();
            lineNumber++;
            line.replace("  ", " ");

            if(line.startsWith (RECOILCMDONLY))
            {
                QStringList recoilSL = line.split (' ', Qt::SkipEmptyParts);
                if(recoilSL[1] == "1")
                {
                    gameRecoilsSupported[1] = true;
                    recoilSL[0].prepend('#');
                    recoilCMDs << recoilSL[0];

                    if(recoilSL.length() > 2)
                    {
                        recoilSL[2].prepend('#');
                        recoilCMDs << recoilSL[2];
                        gameReloadsSupported[1] = true;
                    }
                    else
                        recoilCMDs << "";
                }
                else
                    recoilCMDs << "" << "";
            }
            else
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "The line didn't match 'Recoil' Recoil Options\nLine"+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }

            recoilCMDsMap.insert (1, recoilCMDs);

            //Get Recoil_R2S: Recoil and Reload Info from Game File Header
            line = in.readLine();
            lineNumber++;
            line.replace("  ", " ");

            if(line.startsWith (RECOIL_R2SONLY))
            {
                QStringList recoilR2SSL = line.split (' ', Qt::SkipEmptyParts);
                if(recoilR2SSL[1] == "1")
                {
                    gameRecoilsSupported[2] = true;
                    recoilR2SSL[0].prepend('#');
                    recoilR2SCMDs << recoilR2SSL[0];

                    if(recoilR2SSL.length() > 2)
                    {
                        recoilR2SSL[2].prepend('#');
                        recoilR2SCMDs << recoilR2SSL[2];
                        gameReloadsSupported[2] = true;
                    }
                    else
                        recoilR2SCMDs << "";
                }
                else
                    recoilR2SCMDs << "" << "";
            }
            else
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "The line didn't match 'Recoil_R2S' Recoil Options\nLine"+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }

            recoilCMDsMap.insert (2, recoilR2SCMDs);

            //Get Recoil_Value: Recoil and Reload Info from Game File Header
            line = in.readLine();
            lineNumber++;
            line.replace("  ", " ");

            if(line.startsWith (RECOILVALUEONLY))
            {
                QStringList recoilValueSL = line.split (' ', Qt::SkipEmptyParts);
                if(recoilValueSL[1] == "1")
                {
                    gameRecoilsSupported[3] = true;
                    recoilValueSL[0].prepend('#');
                    recoilValueCMDs << recoilValueSL[0];

                    if(recoilValueSL.length() > 2)
                    {
                        recoilValueSL[2].prepend('#');
                        recoilValueCMDs << recoilValueSL[2];
                        gameReloadsSupported[3] = true;
                    }
                    else
                        recoilValueCMDs << "";
                }
                else
                    recoilValueCMDs << "" << "";
            }
            else
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "The line didn't match 'Recoil_Value' Recoil Options\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }

            recoilCMDsMap.insert (3, recoilValueCMDs);

            //Light Gun Recoil & Priority Matching with the Game File Supported
            for(quint8 i = 0; i < numberLGPlayers; i++)
            {
                if(loadedLGNumbers[i] != UNASSIGN)
                {
                    while(!foundRecoilForPlayer[i] && index < NUMBEROFRECOILS)
                    {
                        if(p_loadedLGRecoilPriority[i][index] == 0 && gameRecoilsSupported[0])
                        {
                            loadRecoilForPLayer[i] = 0;
                            foundRecoilForPlayer[i] = true;
                        }
                        else if(p_loadedLGRecoilPriority[i][index] == 1 && gameRecoilsSupported[1])
                        {
                            loadRecoilForPLayer[i] = 1;
                            foundRecoilForPlayer[i] = true;
                        }
                        else if(p_loadedLGRecoilPriority[i][index] == 2 && gameRecoilsSupported[2])
                        {
                            loadRecoilForPLayer[i] = 2;
                            foundRecoilForPlayer[i] = true;
                        }
                        else if(p_loadedLGRecoilPriority[i][index] == 3 && loadedLGSupportRecoilValue[i] && gameRecoilsSupported[3])
                        {
                            loadRecoilForPLayer[i] = 3;
                            foundRecoilForPlayer[i] = true;
                        }

                        index++;
                    }

                    //qDebug() << "Player" << i+1 << "foundRecoilForPlayer" << foundRecoilForPlayer[i] << "loadRecoilForPLayer" << loadRecoilForPLayer[i];
                }
                index = 0;
            }

            //Check to Make Sure Recoil Option Found for All Loaded Light Gun Players
            for(quint8 i = 0; i < numberLGPlayers; i++)
            {
                if(!foundRecoilForPlayer[i] && loadedLGNumbers[i] != UNASSIGN)
                {
                    lgFileLoadFail = true;
                    lgFile.close();
                    QString tempCrit = "Cannot find a supported recoil option.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return;
                }
            }

            hasRecoilOption = true;

        }  //else if(line == RECOILRELOAD)
        else if(line[0] == '[')
        {
            //Nothing In Here Yet, But Might Be Used Later
            //if(line.startsWith("[Sig"))
            //    isOutput = true;

            if(line.startsWith(STATESSECTION))
                isStates = true;
            else if(line.startsWith(OPTIONSSECTION) && isStates)
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "Options need to come before the States.\nLine"+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }
            else if(line.startsWith(OPTIONSSECTION))
            {
                //Only Options Supported is Block for Shake & Recoil_R2S command, using the player's life value.
                //Go through the Lines, until "End Options" happens

                //Get a line of data from file
                line = in.readLine();
                lineNumber++;

                while(line.startsWith(ENDOPTIONS) == false)
                {
                    if(line.startsWith(BLOCKSHAKEOPTION))
                    {
                        for(quint8 k = 0; k < numberLGPlayers; k++)
                            blockShake[k] = true;

                        QStringList blockShakeSL = line.split (' ', Qt::SkipEmptyParts);
                        quint8 blockShakeLength = blockShakeSL.length();

                        if(blockShakeLength < BLOCKSHAKELENGTH || blockShakeLength > BLOCKSHAKESTARTLENGTH)
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "Block Shake has too many or too few elements.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        if(blockShakeSL[2] == "NE")
                            isBlockShakeEqual = false;
                        else if(blockShakeSL[2] == "E")
                            isBlockShakeEqual = true;
                        else
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "Block Shake needs E or NE next, and then the value.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        blockShakeValue = blockShakeSL[3].toUShort(&isNumber);

                        if(!isNumber)
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "Block Shake needs a number at the end.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        //If the Block from Start Option is Set
                        if(blockShakeLength == BLOCKSHAKESTARTLENGTH)
                        {
                            if(blockShakeSL[4] != BLOCKSTART)
                            {
                                lgFileLoadFail = true;
                                lgFile.close();
                                QString tempCrit = "Block Shake Start option is not correct.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                                if(displayMB)
                                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                                return;
                            }
                            else
                            {
                                for(quint8 k = 0; k < numberLGPlayers; k++)
                                    blockShakeActive[k] = true;
                            }
                        }
                    }
                    else if(line.startsWith(BLOCKRECOIL_R2SOPTION))
                    {
                        for(quint8 k = 0; k < numberLGPlayers; k++)
                            blockRecoil_R2S[k] = true;

                        QStringList blockRecoil_R2SSL = line.split (' ', Qt::SkipEmptyParts);
                        quint8 blockRecoil_R2SLength = blockRecoil_R2SSL.length();

                        if(blockRecoil_R2SLength < BLOCKRECOIL_R2SLENGTH || blockRecoil_R2SLength > BLOCKRECOIL_R2STRLENGTH)
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "Block Recoil_R2S has too many or too few elements.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        if(blockRecoil_R2SSL[2] == "NE")
                            isBlockRecoil_R2SEqual = false;
                        else if(blockRecoil_R2SSL[2] == "E")
                            isBlockRecoil_R2SEqual = true;
                        else
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "Block Recoil_R2S needs E or NE next, and then the value.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        blockRecoil_R2SValue = blockRecoil_R2SSL[3].toUShort(&isNumber);

                        if(!isNumber)
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "Block Recoil_R2S needs a number at the end.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        //If the Block from Start Option is Set
                        if(blockRecoil_R2SLength == BLOCKSHAKESTARTLENGTH)
                        {
                            if(blockRecoil_R2SSL[4] != BLOCKSTART)
                            {
                                lgFileLoadFail = true;
                                lgFile.close();
                                QString tempCrit = "Block Recoil_R2S Start option is not correct.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                                if(displayMB)
                                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                                return;
                            }
                            else
                            {
                                for(quint8 k = 0; k < numberLGPlayers; k++)
                                    blockRecoil_R2SActive[k] = true;
                            }
                        }
                    }
                    else if(line.startsWith(OVERRIDERECOILVOLT))
                    {
                        QStringList overrideRecoilVoltSL = line.split (' ', Qt::SkipEmptyParts);

                        if(overrideRecoilVoltSL.length() != OVERRIDERECOILLENGTH)
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "The Override_Recoil_Voltage should have 2 parts. The Override_Recoil_Voltage and a number from 0-10.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        quint8 orRecoilVolt = overrideRecoilVoltSL[OVERRIDERECOILVOLTNUM].toUShort(&isNumber);

                        if(!isNumber || orRecoilVolt > RECOILVOLTAGEMAX)
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "The Override Recoil Voltage number is not a number or greater than 10.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        for(quint8 k = 0; k < numberLGPlayers; k++)
                        {
                            if(lgOutputConnection[k] == TCP)
                            {
                                if(p_comDeviceList->p_lightGunList[loadedLGNumbers[k]]->GetDefaultLightGunNumber() == SINDEN)
                                    p_comDeviceList->p_lightGunList[loadedLGNumbers[k]]->SetRecoilVoltageOverride (orRecoilVolt);
                            }
                        }
                    }
                    else if(line.startsWith(SINDENTRIGGERRECOIL))
                    {
                        QStringList sindenTriggerRecoilSL = line.split (' ', Qt::SkipEmptyParts);

                        if(sindenTriggerRecoilSL.length() != SINDENTRIGGERRECOILLEN)
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "The Sinden_Trigger_Recoil should have 2 parts. The Sinden_Trigger_Recoil and a number from 0-3.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        quint8 sinTrigRec = sindenTriggerRecoilSL[SINDENTRIGGERRECOILNUM].toUShort(&isNumber);

                        if(!isNumber || sinTrigRec > SINDENTRIGGERRECOILMAX)
                        {
                            lgFileLoadFail = true;
                            lgFile.close();
                            QString tempCrit = "The Sinden Trigger Recoil number is not a number or greater than 3.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                            if(displayMB)
                                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                            return;
                        }

                        for(quint8 k = 0; k < numberLGPlayers; k++)
                        {
                            if(lgOutputConnection[k] == TCP)
                            {
                                if(p_comDeviceList->p_lightGunList[loadedLGNumbers[k]]->GetDefaultLightGunNumber() == SINDEN)
                                    p_comDeviceList->p_lightGunList[loadedLGNumbers[k]]->SetSindenRecoilOverride (sinTrigRec);
                            }
                        }
                    }
                    else if(line.startsWith(AMMOCHECKOPTION))
                    {
                        for(quint8 k = 0; k < numberLGPlayers; k++)
                        {
                            if(loadedLGNumbers[k] != UNASSIGN)
                                p_comDeviceList->p_lightGunList[loadedLGNumbers[k]]->SetAmmoCheck ();
                        }
                    }
                    else
                    {
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "This Option doesn't exists in the DefaultLG protocol.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }

                    //Get Next Line and Increment the Line Count
                    line = in.readLine();
                    lineNumber++;
                }
            }
        }
        else if(line[0] == SIGNALSTARTCHAR)
        {
            //Got a Signal, order is Signal, Player Commands, where there could be multiple Players, But a Command is needed after Player
            //But first process older data

            //If Got a Signal & Player(s) and Command(s), then Load into QMap
            if(gotSignal && gotPlayer && gotCommands)
            {
                isGoodLG = false;

                if(playerForCMD == -1)
                    isGoodLG = true;
                else if(playerForCMD < MAXGAMEPLAYERS && loadedLGNumbers[playerForCMD] != UNASSIGN)
                    isGoodLG = true;

                //If Players is not All Players or Light Gun for Player is not UNASSIGN, then put Command into QMap. If Not, then ignore
                //and clear things out
                if(isGoodLG)
                {
                    //qDebug() << "Signal: " << signal << " Commands: " << commands;

                    quint8 i;
                    bool goodCMD;
                    QStringList goodCommands;
                    //Get the Player 9IE All or P1 or P3)
                    goodCommands << commands[0];

                    for(i = 1; i < commands.count(); i++)
                    {
                        goodCMD = CheckCommandsWithLightGun(playerForCMD, commands[i]);

                        if(goodCMD)
                            goodCommands << commands[i];
                    }


                    if(goodCommands.count() > 1)
                    {
                        signalsAndCommands.insert(signal, goodCommands);

                        outputSignalsStates << signal;

                        if(!signal.startsWith (MAMESTAFTER) || (signal.startsWith (MAMESTAFTER) && goodCommands.count() > 2))
                            signalsAndCommandsCountTest++;
                    }
                }

                gotPlayer = false;
                gotCommands = false;

                //Clear out Command String List, Since loaded into QMap
                commands.clear ();
            }
            else if(gotSignal && !gotPlayer && !gotCommands)
            {
                //Signal that has no players and no commands
                signalsNoCommands << signal;
            }
            else if(gotSignal && gotPlayer && !gotCommands)
            {
                //Not Using that Recoil/Reload Option
                gotPlayer = false;
                commands.clear ();
            }

            //Check if Signal Line has an Equal Sign. User Probably Thinks this is like a MAMEHooker INI File and hasn't Read the ReadMe
            if(line.contains ('='))
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "A signal cannot have a = for the Default Light Gun game format. You might be confussed with MAMEHooker INI format, which uses the = sign.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }

            //Remove the ':' from the Front
            line.remove (0,1);

            signal = line;

            gotSignal = true;

        }
        else if(line[0] == PLAYERSTARTCHAR)
        {
            //Got a Player

            if(gotPlayer && !gotCommands)
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "There are two or more Player lines back to back.\nLine: "+line+"\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }


            //If Got a Signal, Add The Player. If Not then Sometrhing Went Wrong
            if(gotSignal)
            {

                //Check If it is a Player Number, like *P1 or *P3
                if(line[1] == PLAYER2CHHAR)
                {

                    tempLine = line;

                    //Remove the '*P' from the Front of Players
                    tempLine.remove (0,2);

                    tempPlayer = tempLine.toUInt (&isNumber);

                    //Check if Number
                    if(!isNumber)
                    {
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "Player Number is not a number in defaultLG game file.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number:"+tempLine+"\nFile: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }

                    playerForCMD = tempPlayer - 1;

                    //Check if Player Number Matches Loaded Players Above
                    bool playerMatch = false;

                    for(quint8 i = 0; i < numberLGPlayers; i++)
                    {
                        if(playerForCMD == i)
                            playerMatch = true;
                    }

                    if(!playerMatch)
                    {
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "Player Number does not match loaded Players in defaultLG game file.\nLine Number: "+QString::number(lineNumber)+"\nPlayer Number:"+tempLine+"\nFile: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }

                } //If not *P[1-4], then must be *All, If not something went wrong
                else if(line[1] == ALL2CHAR)
                {
                    //All Players Been Selected
                    playerForCMD = -1;

                }
                else if(line != ALLPLAYERS)
                {
                    lgFileLoadFail = true;
                    lgFile.close();
                    QString tempCrit = "Something is wrong with Player Number in defaultLG game file. Can be *All or *P[1-4].\nLine Number: "+QString::number(lineNumber)+"\nLine: "+line+"\nFile: "+gameLGFilePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return;
                }

                //First thing of the Command is the Player(s)
                //if(!gotPlayer)
                //    commands << line;

                //Only allow to change players in mame_start or mame_stop
                if(!gotPlayer || signal.startsWith (MAMESTAFTER))
                    commands << line;

                gotPlayer = true;
            }
            else if(in.atEnd ())
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "File cannot end on a player(*). Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }
            else
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "Player(*) came before a signal(:). Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }
        }
        else if(line[0] == COMMANDSTARTCHAR)
        {
            //Got a Command - Check if Good Command

            if(line.contains ('|'))
            {
                if(line.startsWith (RECOIL_R2SCMD))
                {
                    lgFileLoadFail = true;
                    lgFile.close();
                    QString tempCrit = "Recoil_R2S command doesn't support the '|'.\nLine Number: "+QString::number(lineNumber)+"\nCMD: Recoil_R2S"+"\nFile: "+gameLGFilePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return;
                }

                line.replace(" |","|");
                line.replace("| ","|");

                subCommands = line.split('|', Qt::SkipEmptyParts);
                subCmdsCnt = subCommands.size ();

                for(quint8 i = 0; i < subCmdsCnt; i++)
                {
                    isGoodCmd = CheckLGCommand(subCommands[i]);

                    if(!isGoodCmd)
                    {
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "Command(>) is not found in the command list. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+subCommands[i]+"\nFile: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }
                }

            }
            else
            {
                if(line.startsWith (RECOIL_R2SCMD) && line.size() > RECOIL_R2SCMDCNT)
                {
                    subCommands = line.split(' ', Qt::SkipEmptyParts);

                    if(subCommands.size() > 2)
                    {
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "Too many variables after Recoil_R2S command. Can only have 0-1 variable. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+subCommands[2]+"\nFile: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }

                    recoilR2SSkewPrec[playerForCMD] = subCommands[1].toULong (&isNumber);

                    if(!isNumber)
                    {
                        recoilR2SSkewPrec[playerForCMD] = 100;
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "Recoil_R2S Skew is not a number.\nLine Number: "+QString::number(lineNumber)+"\nSkew Number:"+subCommands[1]+"\nFile: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }

                    line = subCommands[0];
                }

                //Check Command to to see if it is good
                isGoodCmd = CheckLGCommand(line);

                if(!isGoodCmd)
                {
                    lgFileLoadFail = true;
                    lgFile.close();
                    QString tempCrit = "Command(>) is not found in the command list. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+gameLGFilePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return;
                }
            }

            //If Got a Signal and Player, then add Command
            if(gotSignal && gotPlayer)
            {   
                //Check to See iff at Last Line
                if(!in.atEnd ())
                {
                    //Last Data Must Be Commands
                    commands << line;

                    gotCommands = true;
                }
                else
                {
                    isGoodLG = false;

                    if(playerForCMD == -1)
                        isGoodLG = true;
                    else if(playerForCMD < MAXGAMEPLAYERS && loadedLGNumbers[playerForCMD] != UNASSIGN)
                        isGoodLG = true;

                    if(isGoodLG)
                    {
                        quint8 i;
                        bool goodCMD;
                        QStringList goodCommands;

                        commands << line;

                        //Get the Player 9IE All or P1 or P3)
                        goodCommands << commands[0];

                        for(i = 1; i < commands.count(); i++)
                        {
                            goodCMD = CheckCommandsWithLightGun(playerForCMD, commands[i]);

                            if(goodCMD)
                                goodCommands << commands[i];
                        }

                        if(goodCommands.count() > 1)
                        {
                            signalsAndCommands.insert(signal, goodCommands);

                            outputSignalsStates << signal;

                            if(!signal.startsWith (MAMESTAFTER) || (signal.startsWith (MAMESTAFTER) && goodCommands.count() > 2))
                                signalsAndCommandsCountTest++;
                        }

                        //At Last Line of the File
                        //qDebug() << "Last1 Signal: " << signal << " Commands: " << commands;
                    }

                    gotPlayer = false;
                    gotCommands = false;
                    gotSignal = false;

                    commands.clear ();
                }
            }
            else if(gotSignal && !gotPlayer && gotCommands)
            {
                //If Got a Signal and Command, with No Player
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "No player(*) between a signal(:) and command(>). Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nSignal: "+signal+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }
        }  //else if(line[0] == COMMANDSTARTCHAR)
        else if(line[0] == RECOMMANDSTARTCHAR)
        {
            //Got a Optional Recoil/Reload Command - Check if Choosen Option and if Good Command

            //Do 4 Checks before Start Processing
            //If No Recoil Option Data Loaded - hasRecoilOption
            if(!hasRecoilOption)
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "Recoil\\Reload Option command data is missing in game file.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }

            //Check for |
            if(line.contains ('|'))
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "Recoil\\Reload Option Commands command doesn't support the '|'.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }

            //Check for All Players
            if(playerForCMD < 0)
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "Recoil\\Reload Option Commands command doesn't support the *All players.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }

            //Check for Got Output Signal, but No Player
            if(gotSignal && !gotPlayer)
            {
                lgFileLoadFail = true;
                lgFile.close();
                QString tempCrit = "Recoil\\Reload Option Commands command has Output Signal, but no Player.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                return;
            }

            QString tmpCMD;
            bool skipReloadCMD = false;
            bool isReloadCMD = false;
            QString chkLine = line;

            //Get Recoil/Reload Option for Loaded Light Gun Player
            quint8 reOption = loadRecoilForPLayer[playerForCMD];

            //If Loaded Light Gun Doesn't Support Reload & Is a Reload Option Command, then Skip
            if(line.startsWith(OPTIONRELOADCMD))
                isReloadCMD = true;

            if(isReloadCMD && !loadedLGSupportReload[playerForCMD])
                skipReloadCMD = true;

            //If Recoil_R2S with Skew Percentage, then Need to Remove Skew Percentage for Check
            if(line.startsWith (OPTIONRECOIL_R2SCMD) && line.size() > RECOIL_R2SCMDCNT)
            {
                subCommands = line.split(' ', Qt::SkipEmptyParts);
                chkLine = subCommands[0];
            }

            //Check if Command is in Loaded Light Gun Recoil/Reload String List and Not to Skip Reload Command
            if(recoilCMDsMap[reOption].contains(chkLine) && !skipReloadCMD)
            {
                //If Command Matches Loaded Light Gun, then Replace # with >
                tmpCMD = line;
                tmpCMD[0] = COMMANDSTARTCHAR;

                //If Recoil_R2S, then Set-up and Check
                if(tmpCMD.startsWith (RECOIL_R2SCMD) && tmpCMD.size() > RECOIL_R2SCMDCNT)
                {
                    if(subCommands.size() > 2)
                    {
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "Too many variables after Recoil_R2S command. Can only have 0-1 variable. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+subCommands[2]+"\nFile: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }

                    recoilR2SSkewPrec[playerForCMD] = subCommands[1].toULong (&isNumber);

                    if(!isNumber)
                    {
                        recoilR2SSkewPrec[playerForCMD] = 100;
                        lgFileLoadFail = true;
                        lgFile.close();
                        QString tempCrit = "Recoil_R2S Skew is not a number.\nLine Number: "+QString::number(lineNumber)+"\nSkew Number:"+subCommands[1]+"\nFile: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }

                    tmpCMD = subCommands[0];
                    tmpCMD[0] = COMMANDSTARTCHAR;
                }


                //Check Command to to see if it is good
                isGoodCmd = CheckLGCommand(tmpCMD);

                if(!isGoodCmd)
                {
                    lgFileLoadFail = true;
                    lgFile.close();
                    QString tempCrit = "Command(>) is not found in the command list. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nCMD: "+line+"\nFile: "+gameLGFilePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return;
                }


                //If Got a Signal and Player, then add Command
                if(gotSignal && gotPlayer)
                {

                    //Check to See if Not at Last Line
                    if(!in.atEnd ())
                    {
                        //Last Data Must Be Commands
                        commands << tmpCMD;

                        gotCommands = true;
                    }
                    else
                    {
                        isGoodLG = false;

                        if(playerForCMD == -1)
                            isGoodLG = true;
                        else if(playerForCMD < MAXGAMEPLAYERS && loadedLGNumbers[playerForCMD] != UNASSIGN)
                            isGoodLG = true;

                        if(isGoodLG)
                        {
                            quint8 i;
                            bool goodCMD;
                            QStringList goodCommands;

                            commands << tmpCMD;

                            //Get the Player 9IE All or P1 or P3)
                            goodCommands << commands[0];

                            for(i = 1; i < commands.count(); i++)
                            {
                                goodCMD = CheckCommandsWithLightGun(playerForCMD, commands[i]);

                                if(goodCMD)
                                    goodCommands << commands[i];
                            }

                            if(goodCommands.count() > 1)
                            {
                                signalsAndCommands.insert(signal, goodCommands);

                                outputSignalsStates << signal;

                                if(!signal.startsWith (MAMESTAFTER) || (signal.startsWith (MAMESTAFTER) && goodCommands.count() > 2))
                                    signalsAndCommandsCountTest++;
                            }
                        }

                        gotPlayer = false;
                        gotCommands = false;
                        gotSignal = false;

                        commands.clear ();
                    }
                }
                else if(gotSignal && !gotPlayer && gotCommands)
                {
                    //If Got a Signal and Command, with No Player
                    lgFileLoadFail = true;
                    lgFile.close();
                    QString tempCrit = "No player(*) between a signal(:) and command(>). Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nSignal: "+signal+"\nFile: "+gameLGFilePath;
                    if(displayMB)
                        QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
                    return;
                }
            }  //if(recoilCMDsMap[reOption].contains(line))
        }  //else if(line[0] == RECOMMANDSTARTCHAR)
        else
        {
            //Something went wrong. Extra Line
            lgFileLoadFail = true;
            lgFile.close();
            QString tempCrit = "This Line has no place in DefaultLG game file. Please close program and solve file problem.\nLine Number: "+QString::number(lineNumber)+"\nLine: "+line+"\nFile: "+gameLGFilePath;
            if(displayMB)
                QMessageBox::critical (p_guiConnect, "Default Light Gun Game File Error", tempCrit, QMessageBox::Ok);
            return;
        }

    }

    //If there is a blank line at the end, then need to enter the last data
    if(gotSignal && gotPlayer && gotCommands)
    {
        isGoodLG = false;

        if(playerForCMD == -1)
            isGoodLG = true;
        else if(playerForCMD < MAXGAMEPLAYERS && loadedLGNumbers[playerForCMD] != UNASSIGN)
            isGoodLG = true;

        if(isGoodLG)
        {
            //qDebug() << "Last2 Signal: " << signal << " Commands: " << commands;

            quint8 i;
            bool goodCMD;
            QStringList goodCommands;

            //Get the Player 9IE All or P1 or P3)
            goodCommands << commands[0];

            for(i = 1; i < commands.count(); i++)
            {
                goodCMD = CheckCommandsWithLightGun(playerForCMD, commands[i]);

                if(goodCMD)
                    goodCommands << commands[i];
            }

            if(goodCommands.count() > 1)
            {
                signalsAndCommands.insert(signal, goodCommands);

                outputSignalsStates << signal;

                if(!signal.startsWith (MAMESTAFTER) || (signal.startsWith (MAMESTAFTER) && goodCommands.count() > 2))
                    signalsAndCommandsCountTest++;
            }
        }
    }
    else if(gotSignal && !gotPlayer && !gotCommands)
    {
        //qDebug() << "Signal with No Commands, Signal: " << signal;
        //Signal that has no players and no commands
        signalsNoCommands << signal;
    }


    if(signalsAndCommandsCountTest == 0 && ignoreUselessDLGGF == false)
    {
        //No Signal, Player, & Command was Loaded
        lgFileLoadFail = true;
        lgFile.close();
        QString tempCrit = "Hook Of The Reaper is only connecting & disconnecting to the light gun(s) for this game. There are no signals to watch out for, so there is nothing to do for this game. This warning can be turned off in Settings.\nFile: "+gameLGFilePath;
        if(displayMB)
            QMessageBox::warning (p_guiConnect, "Default Light Gun Game File Warning", tempCrit, QMessageBox::Ok);
        return;
    }


    //qDebug() << "Loaded Signals & Commands QMap: " << signalsAndCommands;

    lgFile.close();
    lgFileLoaded = true;

    //Add mame_stop and States to outputSignalsStates
    //outputSignalsStates << PAUSE << ORIENTATION;

    //qDebug() << "Output Signals & States String List:" << outputSignalsStates;

    //Tell TCP Socket a Game has Started
    emit TCPGameStart(outputSignalsStates);

    //Don't Bypass the COM Port Connection Fail Warning Pop-up, for the DefaultLG Side
    emit SetBypassComPortConnectFailWarning(true);

    //Process the mame_start Command
    ProcessLGCommands(MAMESTARTAFTER, gameName);
}


bool HookerEngine::CheckCommandsWithLightGun(qint8 player, QString command)
{
    quint8 i = 0;
    quint8 count = 0;
    bool hasDisplay, hasReload, hasDamage, hasDeath, hasShake;
    bool allPlayers = false;

    if(player == -1)
        allPlayers = true;
    else
        i = player;


    if(command.startsWith(DISPLAYCMDFRONT))
    {
        if(allPlayers)
        {
            for(i = 0; i < numberLGPlayers; i++)
            {
                //Check if Any Light Gun are Not Unassigned
                if(loadedLGNumbers[i] != UNASSIGN)
                {
                    hasDisplay = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->HasDisplay ();

                    if(hasDisplay)
                        count++;
                }
            }

            if(count > 0)
                return true;
            else
                return false;
        }
        else
        {   //Don't need to Check for Unassign as it was Check Before Calling Function
            hasDisplay = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->HasDisplay ();
            return hasDisplay;
        }
    }
    else if(command.startsWith(RELOADCMDFRONT))
    {
        if(allPlayers)
        {
            for(i = 0; i < numberLGPlayers; i++)
            {
                if(loadedLGNumbers[i] != UNASSIGN)
                {
                    hasReload = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->HasReload ();

                    if(hasReload)
                        count++;
                }
            }

            if(count > 0)
                return true;
            else
                return false;
        }
        else
        {
            hasReload = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->HasReload ();
            return hasReload;
        }
    }
    else if(command.startsWith(DAMAGECMDFRONT))
    {
        if(allPlayers)
        {
            for(i = 0; i < numberLGPlayers; i++)
            {
                if(loadedLGNumbers[i] != UNASSIGN)
                {
                    hasDamage = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->HasDamage ();

                    if(hasDamage)
                        count++;
                }
            }

            if(count > 0)
                return true;
            else
                return false;
        }
        else
        {
                hasDamage = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->HasDamage ();
                return hasDamage;
        }
    }
    else if(command.startsWith(DEATHCMDFRONT) || command.startsWith(LIFECMDFRONT))
    {
        if(allPlayers)
        {
            for(i = 0; i < numberLGPlayers; i++)
            {
                if(loadedLGNumbers[i] != UNASSIGN)
                {
                    hasDeath = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->HasDeath ();

                    if(hasDeath)
                        count++;
                }
            }

            if(count > 0)
                return true;
            else
                return false;
        }
        else
        {
                hasDeath = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->HasDeath ();
                return hasDeath;
        }
    }
    else if(command.startsWith(SHAKECMDFRONT))
    {
        if(allPlayers)
        {
            for(i = 0; i < numberLGPlayers; i++)
            {
                if(loadedLGNumbers[i] != UNASSIGN)
                {
                    hasShake = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->HasShake ();

                    if(hasShake)
                        count++;
                }
            }

            if(count > 0)
                return true;
            else
                return false;
        }
        else
        {
            hasShake = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->HasShake ();
            return hasShake;
        }
    }


    return true;
}



bool HookerEngine::CheckLGCommand(QString commndNotChk)
{
    if(commandLGList.contains (commndNotChk))
        return true;

    return false;
}

void HookerEngine::ProcessLGCommands(const QString &signalName, const QString &value)
{
    QStringList commands, dlgCommands, multiValue;
    quint8 cmdCount;
    bool allPlayers;
    quint8 playerNum = UNASSIGN;
    quint8 i, j, k;
    quint8 howManyPlayers;
    quint8 player, lightGun;
    bool dlgCMDFound;
    quint8 charToNumber;


    //qDebug() << "Signal: " << signalName << " Value: " << value;

    //Get the Player(s) & Command(s) From sinalsAndCommands QMap using signalName
    commands = signalsAndCommands[signalName];
    cmdCount = commands.length ();

    //qDebug() << "Commnads: " << commands;

    //First Command Is Always a Player
    if(commands[0] == ALLPLAYERS)
        allPlayers = true;
    else
    {
        //Remove the *P, to get number
        commands[0].remove(0,2);
        playerNum = commands[0].toUInt ()-1;
        allPlayers = false;
    }

    for(i = 1; i < cmdCount; i++)
    {

        //qDebug() << "CMDSIGNAL: " << commands[i] << "Value:" << value;

        //First Check if there are multiple commands with '|', is so split.
        if(commands[i].contains ("|"))
        {
            //Done this when checking commands, when loaded the file
            //commands[i].replace(" |","|");
            //commands[i].replace("| ","|");

            multiValue = commands[i].split('|', Qt::SkipEmptyParts);

            //Pick the command to use, based on the value, as 0 Starts on the right and increases to the left. If value is blank, then use 0
            if(value != "")
            {
                qint16 dataNum = value.toUInt ();
                //It is -1 Since it Starts at 0
                quint16 mvSize = multiValue.size () - 1;

                //If Value is Larger than the Number of Commands, then pick the highest command
                if(dataNum > mvSize)
                    dataNum = mvSize;

                //Add Command sign in front, if not there
                if(multiValue[dataNum][0] != CMDSIGNAL)
                    multiValue[dataNum].prepend (CMDSIGNAL);

                commands[i] = multiValue[dataNum];
            }
            else
            {
                commands[i] = multiValue[0];
            }

        }


        //Check Chars for Faster Processing
        if(commands[i][1] == OPENCOMPORT2CHAR && commands[i][2] == OPENCOMPORT2CHAR2)
        {
            bool noInitOpen;
            if(commands[i].length () > OPENCOMPORTLENGTH)
                noInitOpen = true;
            else
                noInitOpen = false;

            if(allPlayers)
            {
                for(quint8 p = 0; p < numberLGPlayers; p++)
                {
                    if(loadedLGNumbers[p] != UNASSIGN)
                        lgGamePlayers[p].Connect (noInitOpen);
                }
            }
            else
            {
                if(loadedLGNumbers[playerNum] != UNASSIGN)
                    lgGamePlayers[playerNum].Connect (noInitOpen);
            }
        }
        else if(commands[i][1] == CLOSECOMPORT2CHAR)
        {
            bool noInitClose = false;
            bool initOnlyClose = false;

            if(commands[i].length () > CLOSECOMPORTLENGTH)
            {
                if(commands[i][CLOSECOMPORTINITCHK] == CLOSECOMPORTNOINIT11)
                    noInitClose = true;
                else
                    initOnlyClose = true;
            }

            if(allPlayers)
            {
                for(quint8 p = 0; p < numberLGPlayers; p++)
                {
                    if(loadedLGNumbers[p] != UNASSIGN)
                        lgGamePlayers[p].Disconnect (noInitClose, initOnlyClose);
                }
            }
            else
            {
                if(loadedLGNumbers[playerNum] != UNASSIGN)
                    lgGamePlayers[playerNum].Disconnect (noInitClose, initOnlyClose);
            }
        }
        else if(commands[i][0] == CMDSIGNAL)
        {

            //qDebug() << "Processing CMDSIGNAL:" << commands[i] << "Value:" << value << "for Player:" << playerNum << "All Players" <<allPlayers;


            if(allPlayers)
                howManyPlayers = numberLGPlayers;
            else
                howManyPlayers = 1;

            for(j = 0; j < howManyPlayers; j++)
            {   
                if(allPlayers)
                    player = j;
                else
                    player = playerNum;


                lightGun = loadedLGNumbers[player];

                //qDebug() << "isLGConnected[player]" << isLGConnected[player] << "player" << player << "lightGun" << lightGun;

                //Check if light gun has an assign value, if not then don't run
                //if(isLGConnected[player])
                if(lightGun != UNASSIGN)
                {
                    //qDebug() << "Processing Command";

                    //Get COM Port Number
                    //tempCPNum = loadedLGComPortNumber[player];

                    //Set True. If No Command or Null, then it is set to false
                    dlgCMDFound = false;

                    charToNumber = static_cast<quint8>(commands[i][1].toLatin1 ());

                    //If First Char in Command is A-D, as E = 69
                    if(charToNumber < 69)
                    {
                        //Have to Search Commands Multiple Times as Multiple Default Light Guns Could be Used
                        //Faster to Search for 1 char than the whole string
                        //There are 4 Commands that start with ">A"
                        if(commands[i][1] == 'A')
                        {
                            if(commands[i][2] == 'm')
                            {
                                //Has to Be Ammo_Value - Reload is checked in AmmoValueCommands
                                //For Recoil Delay
                                if(isRecoilDelaySet[player])
                                {
                                    if(blockRecoil[player])
                                    {
                                        doRecoilDelayEnds[player] = true;
                                        dlgCMDFound = false;
                                    }
                                    else
                                    {
                                        dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AmmoValueCommands(&dlgCMDFound, value.toUShort());

                                        openSolenoidOrRecoilDelay[player].start();
                                        blockRecoil[player] = true;
                                        doRecoilDelayEnds[player] = false;
                                    }
                                } //For Slow Mode
                                else if(loadedLGSlowMode[player])
                                {
                                    //Do recoil only when ammo is even number, so it gets 0 and only even ammo. Cuts recoil in half
                                    quint16 ammoValue = value.toUShort();
                                    //qDebug() << "Ammo Value" << ammoValue << "Modulus value" << ammoValue % 2;
                                    if(ammoValue % 2 != 0)
                                    {
                                        dlgCMDFound = false;
                                    }
                                    else
                                    {
                                        dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AmmoValueCommands(&dlgCMDFound, ammoValue);
                                    }
                                } //Normal Ammo_Value
                                else
                                {
                                    //qDebug() << "CMD: " << commands[i] << " value: " << value;
                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AmmoValueCommands(&dlgCMDFound, value.toUShort());
                                }
                            }
                            else if(commands[i][2] == 's')
                            {
                                if(commands[i][13] == ARATIO169CMD13CHAR)
                                {
                                    //Must Be AspectRatio_16:9 Command
                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AspectRatio16b9Commands(&dlgCMDFound);
                                }
                                else //Must Be AspectRatio_4:3 Command
                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AspectRatio4b3Commands(&dlgCMDFound);
                            }
                            else if(commands[i][2] == AUTOLEDCMD3CHAR)
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AutoLEDCommands(&dlgCMDFound);
                        }
                        else if(commands[i][1] == 'D')
                        {
                            if(commands[i][2] == 'a' && value != "0")
                            {
                                //Must Be Damage Command, Only Do Damage if Value != 0
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->DamageCommands(&dlgCMDFound);
                            }
                            else if(commands[i][2] == 'i')
                            {
                                if(commands[i][DISPLAYCHECK] == DISPLAYAMMOCMD9CHAR)
                                {
                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->DisplayAmmoCommands(&dlgCMDFound, value.toUShort());

                                    if(dlgCMDFound)
                                    {
                                        //If LG Display is on Delay, store CMD in Queue and set dlgCMDFound to false, so nothing happens
                                        if(isLGDisplayOnDelay[player])
                                        {
                                            lgDisplayDelayAmmoCMDs[player] = dlgCommands[0];
                                            dlgCMDFound = false;
                                        }
                                        else
                                        {
                                            //If not on delay, let CMD go through, But on Delay & Start Timer
                                            //qDebug() << "Display Ammo - No Delay: " << dlgCommands;
                                            isLGDisplayOnDelay[player] = true;
                                            didDisplayWrite[player] = true;
                                            lgDisplayDelayTimer[player].start();
                                        }
                                    }
                                }
                                else if(commands[i][DISPLAYCHECK] == DISPLAYLIFECMD9CHAR)
                                {
                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->DisplayLifeCommands(&dlgCMDFound, value.toUShort());

                                    if(dlgCMDFound)
                                    {
                                        //If LG Display is on Delay, store CMD in Queue and set dlgCMDFound to false, so nothing happens
                                        if(isLGDisplayOnDelay[player])
                                        {
                                            lgDisplayDelayLifeCMDs[player] = dlgCommands[0];
                                            dlgCMDFound = false;
                                        }
                                        else
                                        {
                                            //If not on delay, let CMD go through, But on Delay & Start Timer
                                            //qDebug() << "Display Life - No Delay: " << dlgCommands;
                                            isLGDisplayOnDelay[player] = true;
                                            didDisplayWrite[player] = true;
                                            lgDisplayDelayTimer[player].start();
                                        }
                                    }
                                }
                                else if(commands[i][DISPLAYCHECK] == DISPLAYOTHERCMD9CHAR)
                                {
                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->DisplayOtherCommands(&dlgCMDFound, value.toUShort());

                                    if(dlgCMDFound)
                                    {
                                        //If LG Display is on Delay, store CMD in Queue and set dlgCMDFound to false, so nothing happens
                                        if(isLGDisplayOnDelay[player])
                                        {
                                            lgDisplayDelayOtherCMDs[player] = dlgCommands[0];
                                            dlgCMDFound = false;
                                        }
                                        else
                                        {
                                            //If not on delay, let CMD go through, But on Delay & Start Timer
                                            isLGDisplayOnDelay[player] = true;
                                            didDisplayWrite[player] = true;
                                            lgDisplayDelayTimer[player].start();
                                        }
                                    }
                                }
                            }
                            else if(commands[i][2] == 'e')
                            {
                                //Must Be Death_Value
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->DeathValueCommands(&dlgCMDFound, value.toUShort());
                            }
                        }
                        else if(commands[i][1] == 'B')
                        {
                            quint16 blockSignalValue = value.toUShort();

                            if(blockShake[player])
                            {
                                if(isBlockShakeEqual)
                                {
                                    if(blockShakeValue == blockSignalValue)
                                        blockShakeActive[player] = true;
                                    else
                                        blockShakeActive[player] = false;
                                }
                                else
                                {
                                    if(blockShakeValue != blockSignalValue)
                                        blockShakeActive[player] = true;
                                    else
                                        blockShakeActive[player] = false;
                                }
                            }

                            if(blockRecoil_R2S[player])
                            {
                                if(isBlockRecoil_R2SEqual)
                                {
                                    if(blockRecoil_R2SValue == blockSignalValue)
                                        blockRecoil_R2SActive[player] = true;
                                    else
                                        blockRecoil_R2SActive[player] = false;
                                }
                                else
                                {
                                    if(blockRecoil_R2SValue != blockSignalValue)
                                        blockRecoil_R2SActive[player] = true;
                                    else
                                        blockRecoil_R2SActive[player] = false;
                                }
                            }

                        }

                    }
                    else  //E-Z
                    {
                        if(commands[i][1] == 'R')
                        {
                            //Two Commands Start with ">R", If ">Rel" then Reload, If Not then Recoil, and Then only when value != 0
                            if(commands[i][3] == 'l')
                            {
                                //Reload_Value And Reload
                                if(commands[i].size() > RELOADVALUECMDSIZE)
                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->ReloadValueCommands(&dlgCMDFound, value.toUShort());
                                else if(value != "0")
                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->ReloadCommands(&dlgCMDFound);
                            }
                            else if(commands[i][3] == 'c')
                            {
                                if(commands[i].size() > RECOILCMDCNT)
                                {
                                    if(commands[i][RECOILCMDCNT] == RECOIL_R2SCMD8CHAR)
                                    {
                                        //Recoil_R2S Command

                                        //qDebug() << "In RecoilR2S, isPRecoilR2SFirstTime[player]: " << isPRecoilR2SFirstTime[player];

                                        if(isPRecoilR2SFirstTime[player])
                                        {
                                            QStringList tmpCMDs = p_comDeviceList->p_lightGunList[lightGun]->RecoilR2SCommands(&dlgCMDFound);
                                            if(dlgCMDFound && tmpCMDs.count() > 0)
                                            {
                                                quint32 delay = tmpCMDs[0].toULong();

                                                //qDebug() << "recoilR2SSkewPrec[player]: " << recoilR2SSkewPrec[player];

                                                if(recoilR2SSkewPrec[player] != 100)
                                                {
                                                    if(recoilR2SSkewPrec[player] < RECOIL_R2SMINPERCT)
                                                        recoilR2SSkewPrec[player] = RECOIL_R2SMINPERCT;

                                                    delay = delay * recoilR2SSkewPrec[player];
                                                    delay = delay/100;
                                                }
                                                //qDebug() << "Delay for P" << QString::number(player+1) << " is " << delay;

                                                pRecoilR2STimer[player].setTimerType (Qt::CoarseTimer);

                                                pRecoilR2STimer[player].setInterval(delay);
                                                isPRecoilR2SFirstTime[player] = false;
                                            }

                                            if(value != "0")
                                            {
                                                if(blockRecoil_R2SActive[player])
                                                    dlgCMDFound = false;
                                                else
                                                {

                                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->RecoilCommands(&dlgCMDFound);
                                                    pRecoilR2STimer[player].start ();

                                                    if(isRecoilDelaySet[player])
                                                    {
                                                        openSolenoidOrRecoilDelay[player].start();
                                                        blockRecoil[player] = true;
                                                        doRecoilDelayEnds[player] = false;
                                                    }
                                                }
                                            }
                                        }
                                        else
                                        {
                                            //This is When Command Goes High to Start Recoil
                                            if(value != "0")
                                            {
                                                if(blockRecoil_R2SActive[player])
                                                    dlgCMDFound = false;
                                                else
                                                {
                                                    pRecoilR2STimer[player].start ();

                                                    if(isRecoilDelaySet[player])
                                                    {
                                                        if(blockRecoil[player])
                                                        {
                                                            doRecoilDelayEnds[player] = true;
                                                            dlgCMDFound = false;
                                                        }
                                                        else
                                                        {
                                                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->RecoilCommands(&dlgCMDFound);
                                                            openSolenoidOrRecoilDelay[player].start();
                                                            blockRecoil[player] = true;
                                                            doRecoilDelayEnds[player] = false;
                                                        }
                                                    }
                                                    else
                                                        dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->RecoilCommands(&dlgCMDFound);
                                                }
                                            }
                                            else
                                            {
                                                //Value is 0, so stop timer and do nothing
                                                pRecoilR2STimer[player].stop ();
                                                dlgCMDFound = false;

                                                doRecoilDelayEnds[player] = false;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        //Recoil_Value Command - No Support for Delay Recoil
                                        dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->RecoilValueCommands(&dlgCMDFound, value.toUShort());
                                        //qDebug() << "Recoil_Value: " << value << " CMDs: " << dlgCommands[0];

                                        if(dlgCMDFound)
                                        {
                                            if(value.toUShort() == 0)
                                            {
                                                if(closeSolenoidCMDs[player].isEmpty ())
                                                    closeSolenoidCMDs[player] = dlgCommands;

                                                if(isLGSolenoidOpen[player])
                                                {
                                                    openSolenoidOrRecoilDelay[player].stop ();
                                                    isLGSolenoidOpen[player] = false;
                                                }
                                            }
                                            else
                                            {
                                                //If Solenoid is Close, then mark it is Open and Start Timer
                                                if(!isLGSolenoidOpen[player])
                                                {
                                                    isLGSolenoidOpen[player] = true;
                                                    openSolenoidOrRecoilDelay[player].start ();
                                                }
                                                else
                                                {
                                                    //If Solenoid is Already Open, then Do Nothing
                                                    dlgCMDFound = false;

                                                    //Make Sure if Timer is Running, if not Start it Then
                                                    if(!openSolenoidOrRecoilDelay[player].isActive())
                                                        openSolenoidOrRecoilDelay[player].start ();
                                                }
                                            }
                                        }
                                    }
                                }  //Recoil Command
                                else if(value != "0")
                                {
                                    //Must be Recoil Command, Only Do when Value != 0
                                    //For Recoil Delay
                                    if(isRecoilDelaySet[player])
                                    {
                                        if(blockRecoil[player])
                                        {
                                            //If blockRecoil[player], then do nothing and set doRecoilDelayEnds[player]
                                            doRecoilDelayEnds[player] = true;
                                            dlgCMDFound = false;
                                        }
                                        else
                                        {
                                            //Do a recoil and start the timer, and set blockRecoil[player]
                                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->RecoilCommands(&dlgCMDFound);
                                            openSolenoidOrRecoilDelay[player].start();
                                            blockRecoil[player] = true;
                                            doRecoilDelayEnds[player] = false;
                                        }
                                    }  //For Slow Mode
                                    else if(loadedLGSlowMode[player])
                                    {
                                        if(skipRecoilSlowMode[player])
                                        {
                                            dlgCMDFound = false;
                                            skipRecoilSlowMode[player] = false;
                                        }
                                        else
                                        {
                                            skipRecoilSlowMode[player] = true;
                                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->RecoilCommands(&dlgCMDFound);
                                        }
                                    } //For Normal Recoil
                                    else
                                        dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->RecoilCommands(&dlgCMDFound);
                                }
                            }
                        }  //if(commands[i][1] == 'R')
                        else if(commands[i][1] == 'L')
                        {
                            //qDebug() << "Command is Life_Value:" << commands[i];
                            //Must Be Life_Value
                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->LifeValueCommands(&dlgCMDFound, value.toUShort());
                        }
                        else if(commands[i][1] == 'S' && value != "0")
                        {
                            if(blockShakeActive[player])
                                dlgCMDFound = false;
                            else
                            {
                                //Must be Shake Command, Only Do Shake if Value != 0
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->ShakeCommands(&dlgCMDFound);
                            }
                        }
                        else if(commands[i][1] == 'J')
                        {
                            //Must Be Joystick_Mode Command
                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->JoystickModeCommands(&dlgCMDFound);
                        }
                        else if(commands[i][1] == 'K')
                        {
                            //Must be Keyboard_Mouse_Command
                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->MouseAndKeyboardModeCommands(&dlgCMDFound);
                        }
                        else if(commands[i][1] == 'N')
                        {
                            //Must Be Null Command - Do Nothing
                            dlgCMDFound = false;
                        }
                        else if(commands[i][1] == 'O')
                        {
                            //Offscreen Commands
                            if(commands[i][OFFSCREENCHARAT] == OFFSCREENCHARATBUTTON)
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->OffscreenButtonCommands(&dlgCMDFound);
                            else if(commands[i][OFFSCREENCHARAT] == OFFSCREENCHARATNORMAL)
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->OffscreenNormalShotCommands(&dlgCMDFound);
                            else if(commands[i][OFFSCREENCHARAT] == OFFSCREENCHARATLEFT)
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->OffscreenLeftCornerCommands(&dlgCMDFound);
                            else
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->OffscreenDisableCommands(&dlgCMDFound);
                        }
                    }


                    //isEmpty() is true when Empty
                    if(dlgCMDFound)
                    {
                        //Write Command(s) to the Default Light Gun's COM Port
                        for(k = 0; k < dlgCommands.count(); k++)
                        {
                            //qDebug() << "Writting Commands: " << dlgCommands[k];

                            if(dlgCommands[k][0] == DELAYCMD0 && dlgCommands[k][1] == DELAYCMD1)
                            {
                                //Remove the "D:" from the front
                                dlgCommands[k].remove (0,2);
                                quint32 delay = dlgCommands[k].toUInt ();
                                //qDebug() << "Delay Commands for" << delay << "isNum" << isNum;
                                QThread::msleep (delay);
                            }
                            else
                                lgGamePlayers[player].Write (dlgCommands[k]);
                        }
                    }
                } //if(lightGun != UNASSIGN)

            } //for(j = 0; j < howManyPlayers; j++)

        } //If Multiple Players are In Commands for Some Reason
        else if(commands[i].startsWith ('*'))
        {
            if(commands[i] == ALLPLAYERS)
                allPlayers = true;
            else
            {
                //Take out the *P and get the Number *P2 -> 2
                commands[i].remove(0,2);
                playerNum = commands[i].toUInt ()-1;
                allPlayers = false;
                //qDebug() << "Changing to Single Player:" << playerNum << "allPlayers:" << allPlayers;
            }

        }//Command Searching If and Else If

    }//for(i = 1; i < cmdCount; i++) Main Command Loop
}


void HookerEngine::AddSignalForDisplay(QString sig, QString dat)
{
    addSignalDataDisplay.insert (sig, dat);
}

void HookerEngine::UpdateSignalForDisplay(QString sig, QString dat)
{
    //First Check if signal is in the Update QMap, then Add QMap
    //If Not, then add to Update QMap
    if(updateSignalDataDisplay.contains (sig))
        updateSignalDataDisplay[sig] = dat;
    else if(addSignalDataDisplay.contains (sig))
        addSignalDataDisplay[sig] = dat;
    else
        updateSignalDataDisplay.insert (sig, dat);
}



void HookerEngine::DisconnectGamePlayers()
{
    quint8 i;

    for(i = 0; i < MAXGAMEPLAYERS; i++)
    {
        if(lgOutputConnection[i] == SERIALPORT)
        {
            disconnect(&lgGamePlayers[i], &GamePlayer::ConnectToLG, this, &HookerEngine::OpenSerialPortSlot);
            disconnect(&lgGamePlayers[i], &GamePlayer::DisconnectFromLG, this, &HookerEngine::CloseSerialPortSlot);
            disconnect(&lgGamePlayers[i], &GamePlayer::WriteToLG, this, &HookerEngine::WriteSerialPortSlot);
        }
        else if(lgOutputConnection[i] == USBHID)
        {
            disconnect(&lgGamePlayers[i], &GamePlayer::ConnectToLG, this, &HookerEngine::OpenUSBHIDSlot);
            disconnect(&lgGamePlayers[i], &GamePlayer::DisconnectFromLG, this, &HookerEngine::CloseUSBHIDSlot);
            disconnect(&lgGamePlayers[i], &GamePlayer::WriteToLG, this, &HookerEngine::WriteUSBHIDSlot);
        }
        else if(lgOutputConnection[i] == BTLE)
        {

        }
        else if(lgOutputConnection[i] == TCP)
        {
            //qDebug() << "Disconnecting Player" << i << "from TCP";
            disconnect(&lgGamePlayers[i], &GamePlayer::ConnectToLG, this, &HookerEngine::OpenTCPServerSlot);
            disconnect(&lgGamePlayers[i], &GamePlayer::DisconnectFromLG, this, &HookerEngine::CloseTCPServerSlot);
            disconnect(&lgGamePlayers[i], &GamePlayer::WriteToLG, this, &HookerEngine::WriteTCPServerSlot);
        }
    }
}
