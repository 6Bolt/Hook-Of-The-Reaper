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

    //INI or Default LG Game File Loaded
    iniFileLoaded = false;
    lgFileLoaded = false;

    //New INI or Default LG Game File Made
    newINIFileMade = false;
    newLGFileMade = false;
    //New File Pointer, set to Null
    p_newFile = nullptr;

    //bool to check if mkdir
    canMKDIR = true;

    //Used For INI COM Port Read
    bufferNum = 0;


    //qDebug() << "Hooker Engine Started";

    //Pass in QWidget, So Engine can Still Use QMessageBox
    p_guiConnect = guiConnect;

    displayMB = displayGUI;

    //Move Over the ComDevice List
    p_comDeviceList = cdList;


    //Start of Directory and Path Checking. If Directory Doesn't Exists, It will Make it
    currentPath = QDir::currentPath();

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

    //Settings Values
    useDefaultLGFirst = p_comDeviceList->GetUseDefaultLGFirst ();
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

    //Set Up the TCP Socket Connection Timer
    p_waitingForConnection = new QTimer(this);

    //Set Interval a little more the the TCP Socket Wait for Connection
    p_waitingForConnection->setInterval (TCPTIMERTIME);

    //Turn on signal shot, so timer not running when connected
    p_waitingForConnection->setSingleShot (true);

    //Set Up the Signal and Slots for the timer
    connect(p_waitingForConnection, SIGNAL(timeout()), this, SLOT(TCPConnectionTimeOut()));


    if(useMultiThreading)
    {
        //Move TCP Socket onto Different Thread
        p_hookSocket->moveToThread (&threadForTCPSocket);
        connect(&threadForTCPSocket, &QThread::finished, p_hookSocket, &QObject::deleteLater);
    }


    //Connect the Signals & Slots for Multi-Thread Communication for TCP Socket
    connect(this, &HookerEngine::StartTCPSocket, p_hookSocket, &HookTCPSocket::Connect);
    connect(this, &HookerEngine::StopTCPSocket, p_hookSocket, &HookTCPSocket::Disconnect);
    connect(p_hookSocket,&HookTCPSocket::DataRead, this, &HookerEngine::TCPReadyRead);
    connect(p_hookSocket,&HookTCPSocket::SocketConnectedSignal, this, &HookerEngine::TCPConnected);
    connect(p_hookSocket,&HookTCPSocket::SocketDisconnectedSignal, this, &HookerEngine::TCPDisconnected);

    if(useMultiThreading)
    {
        threadForTCPSocket.start(QThread::HighPriority);
    }



    //Set Up the Serial COM Port(s)
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
    connect(this, &HookerEngine::StopAllComPorts, p_hookComPort, &HookCOMPort::DisconnectAll);
    connect(p_hookComPort, &HookCOMPort::ErrorMessage, this, &HookerEngine::ErrorMessageCom);

    if(useMultiThreading)
    {
        threadForCOMPort.start(QThread::HighPriority);
    }

}

//Deconstructor
HookerEngine::~HookerEngine()
{
    //Clean Up Pointers
    if(p_newFile != nullptr)
        delete p_newFile;

    //Close all COM Port Connections
    emit StopAllComPorts();

    //Clean Up threads
    if(useMultiThreading)
    {
        threadForTCPSocket.quit();
        threadForCOMPort.quit();
        threadForTCPSocket.wait();
        threadForCOMPort.wait();
    }

    delete p_hookSocket;
    delete p_hookComPort;
}

//Public Member Functions

void HookerEngine::HookerEngine::Start()
{
    isEngineStarted = true;

    //Load Player's Light Gun Assignments, In Case Something Changed
    for(quint8 i = 0; i < MAXPLAYERLIGHTGUNS; i++)
    {
        playersLGAssignment[i] = p_comDeviceList->GetPlayerLightGunAssignment(i);
    }

    //Load Settings Values, In Case Something Changed. Multi-Thread Needs Restart
    useDefaultLGFirst = p_comDeviceList->GetUseDefaultLGFirst ();
    quint32 newRefreshTimeDisplay = p_comDeviceList->GetRefreshTimeDisplay ();

    if(newRefreshTimeDisplay != refreshTimeDisplay)
    {
        //Set Interval to the Setting of refresh Time Display
        p_refreshDisplayTimer->setInterval (newRefreshTimeDisplay);
        refreshTimeDisplay = newRefreshTimeDisplay;

    }

    p_waitingForConnection->start ();

    //Start the TCP Connection
    emit StartTCPSocket();
}

void HookerEngine::HookerEngine::Stop()
{
    isEngineStarted = false;

    //Stop the 2 timers
    if(p_waitingForConnection->isActive ())
        p_waitingForConnection->stop ();

    if(p_refreshDisplayTimer->isActive ())
        p_refreshDisplayTimer->stop ();

    //Disconnect the TCP Socket, Which Will Shut Things Down.
    emit StopTCPSocket();
}


//Public Slots

void HookerEngine::TCPReadyRead(const QByteArray &readBA)
{
    //qDebug() << "Got Data from TCP Socket";


    QByteArray messageBA = readBA;
    QString message = QString::fromStdString (messageBA.toStdString ());

    //Remove the \r at the end
    message.chop(1);

    //qDebug() << message;

    //If Multiple Data Lines, they will be seperated into lines, using \r
    //If if had 2 data lines together, then \r would be at end, and middle
    QStringList tcpSocketReadData = message.split(MAMEENDLINE);


    ProcessTCPData(tcpSocketReadData);
}

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

//Private Slots

void HookerEngine::TCPConnectionTimeOut()
{
    //No TCP Socket Connection in time Interval
    //Restart TCP Socket & timer, if TCP Socket is Not Connected

    if(!isTCPSocketConnected)
    {
        //Timer Start  - Interval Already Set
        p_waitingForConnection->start ();

        //Start the TCP Connection
        emit StartTCPSocket();
    }
}

void HookerEngine::TCPConnected()
{
    //qDebug() << "TCP Socket is Connected - Hooker Engine TCP Connected";

    isTCPSocketConnected = true;

    //Stop Timer
    p_waitingForConnection->stop();
}

void HookerEngine::TCPDisconnected()
{
    if(isEngineStarted)
    {
        //qDebug() << "TCP Socket is Disconnected - Hooker Engine TCP Disconnected. Trying to Reconnect";

        isTCPSocketConnected = false;

        p_waitingForConnection->stop();

        emit StopTCPSocket();

        QObject().thread()->usleep(100);

        //Timer Start  - Interval Already Set
        p_waitingForConnection->start ();

        //Start the TCP Connection
        emit StartTCPSocket();
    }
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


//Private Member Functions

//Process the Read Data from The TCP Socket. This is MAME or Demulshooter
void HookerEngine::ProcessTCPData(QStringList tcpReadData)
{
    quint8 i, j;
    quint16 count;


    QStringList tcpSocketData = tcpReadData;

    //How Many Lines Need Processing
    count = tcpSocketData.count ();

    //qDebug() << "Processing TCP Data, size of QStingList: " << count;

    for(i = 0; i < count; i++)
    {

        //qDebug() << tcpSocketData[i];

        //Check if No Game is Found
        if(!isGameFound)
        {
            //Look for "mame_start = " and not "mame_start = ___empty" to find game
            if(tcpSocketData[i].startsWith(MAMESTART, Qt::CaseInsensitive) && tcpSocketData[i] != MAMESTARTNOGAME)
            {
                //Game Found
                gameName = tcpSocketData[i].remove(MAMESTART);
                isGameFound = true;

                //qDebug() << "Game Found: " << gameName;

                //Update Display with Game Name
                emit MameConnectedGame(gameName);

                //Start Refresh Time for Display Timer
                p_refreshDisplayTimer->start ();

                if(!firstTimeGame)
                {
                    //Check old stuff to make sure it is close & cleared

                    signalsAndCommands.clear ();
                    stateAndCommands.clear ();
                    signalsNoCommands.clear ();
                    statesNoCommands.clear ();

                    firstTimeGame = false;
                }

                //Start Looking For Game Files to Load
                GameFound();
            }
            //If equal to "mame_start = ___empty", MAME has No Game Loaded
            else if(tcpSocketData[i] == MAMESTARTNOGAME)
            {
                //Display No Game Loaded
                emit MameConnectedNoGame();
            }
        }
        else
        {
            //Now in When Game Has Been Found
            //qDebug() << tcpSocketData[i];

            //Looks for "mame_stop" to Know Game has Stopped
            if(tcpSocketData[i].startsWith(MAMESTOPFRONT, Qt::CaseInsensitive))
            {
                //qDebug() << "Game Has Stopped!!!!!!!!!!!";

                //Stop Refresh Time for Display Timer
                p_refreshDisplayTimer->stop ();

                //Clear Refresh Display QMaps
                addSignalDataDisplay.clear();
                updateSignalDataDisplay.clear();

                //Run the commands attached to 'mame_stop'
                ProcessCommands(MAMESTOPFRONT, "", true);

                //Game Has Stopped
                isGameFound = false;


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
                        iniFileLoaded = false;
                    }


                }

                //Clear out Old Games Signal & Data and States & Data
                signalsAndData.clear ();
                statesAndData.clear ();

                //The other 3 File Loaded bools where checked before, so Clear the Last
                lgFileLoaded = false;

            }
            else
            {

                //This is in a Game, and Getting States/Signals and Data

                //Split the Signal and Data
                QStringList tempData = tcpSocketData[i].split(" = ", Qt::SkipEmptyParts);
                QString signal = tempData[0];
                QString data = tempData[1];


                //qDebug() << "0: " << signal << " 1: " << data;

                //Check if it is a State, else it is a Signal
                //if(signal == STATECHANGE || signal == PAUSE || signal == ROTATE || signal == REFRESHTIME || signal == ORIENTATION)
                if(signal == PAUSE || signal.startsWith (ORIENTATION))
                {
                    //qDebug() << "State: " << signal << " Data: " << data;

                    //If not In the QMap, then Add it
                    if(statesAndData.contains(signal) == false)
                    {
                        statesAndData.insert(signal,data);
                    }
                    else
                    {
                        statesAndData[signal] = data;
                    }

                    //Process the Command(s) attached to the State
                    if(stateAndCommands.contains(signal))
                        ProcessCommands(signal, data, true);

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

                    //Process the Command(s) attached to the Signal
                    if(signalsAndCommands.contains(signal))
                        ProcessCommands(signal, data, false);

                }

            } //In Game, Getting States/Signals and Data
        } //isGameFound
    }//for(i
}


void HookerEngine::GameFound()
{
    bool lgFileFound;
    bool iniFileFound;


    //If Using Default LG Files First
    if(useDefaultLGFirst)
    {
        //Check if File Exists
        lgFileFound = IsDefaultLGFile();

        if(lgFileFound)
            LoadLGFile();
        else
        {
            //Check if there is an INI file when no Default LG Game File
            iniFileFound = IsINIFile();

            if(iniFileFound)
            {
                //Load and Process the INI File
                LoadINIFile();
            }
            else
            {
                //If no Default LG or INI Game File, then create New Default LG file
                NewLGFile();
            }
        }
    }
    else
    {
        //Check if there is an INI file
        iniFileFound = IsINIFile();

        if(iniFileFound)
        {
            //Load and Process the INI File
            LoadINIFile();
        }
        else
        {
            //Check is a Default LG File
            lgFileFound = IsDefaultLGFile();

            if(lgFileFound)
                LoadLGFile();
            else
            {
                //Save a New INI File with Signals Connected
                NewINIFile();
            }
        }
    }

}

void HookerEngine::ProcessCommands(QString signalName, QString value, bool isState)
{
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


void HookerEngine::LoadINIFile()
{
    QString line;
    QString signal;
    QString commands;
    QStringList tempSplit;
    quint16 indexEqual;
    bool isOutput = false;

    //Open File. If Failed to Open, so Critical Message Box
    QFile iniFile(gameINIFilePath);

    bool openFile = iniFile.open (QIODeviceBase::ReadOnly | QIODevice::Text);

    if(!openFile)
    {
        QString tempCrit = "Can not open INI data file to read. Please close program and solve file problem. Might be permissions problem. File: "+gameINIFilePath;
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

        //All lines have an '=', except for the one with '[' and ']'
        if(!line.startsWith("["))
        {
            //Get the Index of the Equal
            indexEqual = line.indexOf('=',0);

            //If there is Nothing After '=', but in NoCommnds List
            //If something after '=' then Split it up into Signals and Command(s)
            if(line.length() != indexEqual+1)
            {

                if(line[indexEqual-1] == ' ')
                    signal = line.first(indexEqual-1);
                else
                    signal = line.first(indexEqual);

                if(line[indexEqual+1] == ' ')
                    commands = line.sliced(indexEqual+2);
                else
                    commands = line.sliced(indexEqual+1);

                commands.replace(", ", ",");
                commands.replace(" ,", ",");
                tempSplit = commands.split(',');

                if(!isOutput)
                {
                    if(signal.startsWith ("On"))
                        signal.remove(0,2);

                    if(signal.startsWith(JUSTMAME, Qt::CaseInsensitive))
                        signal.insert(4,'_');

                    signal = signal.toLower();
                }

                //qDebug() << "Loaded INI Signal or State: " << signal;

                if(isOutput)
                    signalsAndCommands.insert(signal, tempSplit);
                else
                    stateAndCommands.insert(signal, tempSplit);

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

    //Process the "mame_start" Signal
    ProcessINICommands(MAMESTARTAFTER, "", true);
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
        QString tempCrit = "Can not open INI data file to read. Please close program and solve file problem. Might be permissions problem. File: "+gameINIFilePath;
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
    bool foundCommand;
    bool foundNoCommand;

    if(isState)
        foundCommand = stateAndCommands.contains(signalName);
    else
        foundCommand = signalsAndCommands.contains(signalName);

    //If No Command Found, then something went wrong
    if(!foundCommand)
    {
        QString critMessage;

        if(isState)
            foundNoCommand = statesNoCommands.contains(signalName);
        else
            foundNoCommand = signalsNoCommands.contains(signalName);

        if(foundNoCommand)
        {
            critMessage = "Signal name cannot be processed, as it is not found in any of the QMaps. Something really messed up happened. Signal name: ";
            critMessage.append (signalName);
        }
        else
        {
            critMessage = "Signal name cannot be processed, as it has no commands for it. Found in QStringList for signals with no commands. Signal name: ";
            critMessage.append (signalName);
        }
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "Processes Error",critMessage, QMessageBox::Ok);
        return;

    }

    if(isState)
        commands = stateAndCommands[signalName];
    else
        commands = signalsAndCommands[signalName];


    //qDebug() << "Processing Commands from: " << signalName << "   Is Found in QMap: " << foundCommand;

    for(i = 0; i < commands.size (); i++)
    {
        //qDebug() << commands[i];

        //First Check if there are more values with '|', is so split.
        if(commands[i].contains ("|"))
        {
            commands[i].replace(" |","|");
            commands[i].replace("| ","|");

            temp1 = commands[i].split("|");

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
        if(commands[i].contains(SIGNALDATAVARIBLE))
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
                temp1 = commands[i].split(' ');
                comPortNumber = temp1[1].toUInt();

                //qDebug() << "Open COM Port Command: " << temp1[0] << " " << temp1[1] << " " << temp1[2];

                //Split the Settings into 4 Strings  1: Baud  2: Parity  3: Data  4: Stop
                temp2 = temp1[2].split('_');
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
                temp1 = commands[i].split(' ');

                //qDebug() << "Close COM Port on Number: " << temp1[1];

                CloseINIComPort(temp1[1].toUInt());
            }
            else if(commands[i].startsWith(COMPORTREAD, Qt::CaseInsensitive))
            {
                //This will give 4 Strings 1: cmr  2: Com Port #  3: Buffer # 4: length
                temp1 = commands[i].split(' ');

                //ReadINIComPort(temp1[1].toUInt(), temp1[2].toUInt(), temp1[3].toUInt());
            }
            else if(commands[i].startsWith(COMPORTWRITE, Qt::CaseInsensitive))
            {
                //This will give 3 Strings 1: cmw  2: Com Port #  3: Data
                temp1 = commands[i].split(' ');

                //qDebug() << "Write to COM Port on Number: " << temp1[1] << " with Data: " << temp1[2];

                WriteINIComPort(temp1[1].toUInt(), temp1[2]);

            }
            else if(commands[i].startsWith(ININULLCMD, Qt::CaseInsensitive))
            {
                //Null Command - Do Nothing
            }
        }//COM Port Commands, Starts with "cm" or "cs"
    }//for(i
}


void HookerEngine::OpenINIComPort(quint8 cpNum)
{
    QString cpName = BEGINCOMPORTNAME+QString::number(cpNum);

    //qDebug() << "OpenINIComPort with name: " << cpName << " Emitting StartComPort";

    emit StartComPort(cpNum, cpName, iniPortMap[cpNum].baud, iniPortMap[cpNum].data, iniPortMap[cpNum].parity, iniPortMap[cpNum].stop, 0);
}

void HookerEngine::CloseINIComPort(quint8 cpNum)
{
    emit StopComPort(cpNum);
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




///////////////////////////////////////////////////////////////////////////
//Default LG Side, New Way of Doing Light Guns
///////////////////////////////////////////////////////////////////////////


bool HookerEngine::HookerEngine::IsDefaultLGFile()
{
    bool fileFound;

    gameLGFilePath = defaultLGPath+"/"+gameName+ENDOFLGFILE;

    fileFound = QFile::exists (gameLGFilePath);

    //qDebug() << gameLGFilePath << " is found: " << fileFound;

    return fileFound;
}

void HookerEngine::NewLGFile()
{
    //Copy default INI file over with game's name as the file's name
    bool copyFile = QFile::copy(lgDefaultPath, gameLGFilePath);

    if(!copyFile)
    {
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "File Error", "Can not copy default Light Gun file to defaultLG. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }


    //After copy, open file
    p_newFile = new QFile(gameLGFilePath);


    bool openFile = p_newFile->open (QIODevice::Append | QIODevice::Text);

    if(!openFile)
    {
        QString tempCrit = "Can not open default light gun file to read. Please close program and solve file problem. Might be permissions problem. File: "+gameLGFilePath;
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
    bool isDefaultLG;
    quint8 tempDLGNum;
    // bool isOutput = false;


    QFile lgFile(gameLGFilePath);

    bool openFile = lgFile.open (QIODeviceBase::ReadOnly | QIODevice::Text);

    if(!openFile)
    {
        QString tempCrit = "Can not open default light gun game file to read. Please close program and solve file problem. Might be permissions problem. File: "+gameLGFilePath;
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "File Error", tempCrit, QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&lgFile);

    while(!in.atEnd ())
    {

        //Get a line of data from file
        line = in.readLine();

        //For Faster Processing, search first char
        if(line[0] == 'P'  && begin)
        {
            //File should Start with "Players"
            if(line.startsWith(PLAYERSSTART))
            {
                //Next Line is the number of Players
                line = in.readLine();
                numberLGPlayers = line.toUInt ();

                //qDebug() << "Number of Players: " << numberLGPlayers;

                //Read in Players and there Order IE P1, P2 or P3, P1, P4
                for(quint8 i = 0; i < numberLGPlayers; i++)
                {
                    line = in.readLine();
                    //Remove 'P'
                    line.remove (0,1);
                    playerNumber = line.toUInt (&isNumber);
                    if(isNumber)
                        lgPlayerOrder[i] = playerNumber-1;
                    else
                    {
                        QString tempCrit = "Default light gun game file corrupted. Please close program and solve file problem. Player number was not a number. File: "+gameLGFilePath;
                        if(displayMB)
                            QMessageBox::critical (p_guiConnect, "File Error", tempCrit, QMessageBox::Ok);
                        return;
                    }
                    //qDebug() << "Player " << i << " connected to: " << line;

                    //Check if Light Gun is a Default Light Gun
                    lgNumber = playersLGAssignment[lgPlayerOrder[i]];

                    //If Player Assignment is Unassign, then fail load and go to INI file
                    if(lgNumber != UNASSIGN)
                        isDefaultLG = p_comDeviceList->p_lightGunList[lgNumber]->GetDefaultLightGun();
                    else
                        return;

                    //If Light Gun is not a Default Light Gun, the fail and go to INI file
                    if(!isDefaultLG)
                        return;

                    //Load Light Gun Order, based on Player Order and Player's Assignment
                    loadedLGNumbers[i] = lgNumber;

                }

                //Clear Out List and Number
                defaultLGNumbers.clear ();
                uniqueDefaultLG = 0;

                //Check the Default Light Guns Number to See How Many Unique Default Light Guns There Are
                if(numberLGPlayers > 1)
                {
                    for(quint8 i = 0; i < numberLGPlayers; i++)
                    {
                        tempDLGNum = p_comDeviceList->p_lightGunList[loadedLGNumbers[i]]->GetDefaultLightGunNumber();

                        if(i == 0)
                        {
                            defaultLGNumbers << tempDLGNum;
                            uniqueDefaultLG++;
                        }
                        else
                        {
                            //Check if Default Light Gun Number is Not In The List
                            if(defaultLGNumbers.count(tempDLGNum) == 0)
                            {
                                defaultLGNumbers << tempDLGNum;
                                uniqueDefaultLG++;
                            }
                        }
                    }
                }
                else
                {
                    //Only 1 Player for Game
                    tempDLGNum = p_comDeviceList->p_lightGunList[loadedLGNumbers[0]]->GetDefaultLightGunNumber();
                    defaultLGNumbers << tempDLGNum;
                    uniqueDefaultLG++;
                }
                //Don't Run the Players Again
                begin = false;
            }
        }
        //else if(line[0] == '[')
        //{
        //    //Nothing In Here Yet, But Might Be Used Later
        //    if(line.startsWith("[Sig"))
        //        isOutput = true;
        //}
        else if(line[0] == SIGNALSTARTCHAR)
        {
            //Got a Signal, order is Signal, Player Commands, where there could be multiple Players, But a Command is needed after Player
            //But first process older data

            //If Got a Signal & Player(s) and Command(s), then Load into QMap
            if(gotSignal && gotPlayer && gotCommands)
            {
                //qDebug() << "Signal: " << signal << " Commands: " << commands;

                signalsAndCommands.insert(signal, commands);
                gotPlayer = false;
                gotCommands = false;
                commands.clear ();
            }
            else if(gotSignal && !gotPlayer && !gotCommands)
            {
                //Signal that has no players and no commands
                signalsNoCommands << signal;
            }

            //Process Current Line

            //Remove the ':' from the Front
            line.remove (0,1);

            signal = line;

            gotSignal = true;

        }
        else if(line[0] == PLAYERSTARTCHAR)
        {
            //Got a Player

            //If Got a Signal, Add The Player. If Not then Sometrhing Went Wrong
            if(gotSignal)
            {
                //Remove the '*' from the Front of Players
                //line.remove (0,1);

                //First thing of the Command is the Player(s)
                commands << line;

                gotPlayer = true;
            }
            else if(in.atEnd ())
            {
                QString tempCrit = "File cannot end on a player(*). Please close program and solve file problem. Need to open defualt light gun file for game and fixed player problem. File: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Process File Error", tempCrit, QMessageBox::Ok);
                return;
            }
            else
            {
                QString tempCrit = "Player(*) came before a signal(:). Please close program and solve file problem. Need to open defualt light gun file for game and fixed player problem. File: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Process File Error", tempCrit, QMessageBox::Ok);
                return;
            }
        }
        else if(line[0] == COMMANDSTARTCHAR)
        {
            //Got a Command

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
                    commands << line;

                    //At Last Line of the File
                    //qDebug() << "Last1 Signal: " << signal << " Commands: " << commands;

                    signalsAndCommands.insert(signal, commands);
                    gotPlayer = false;
                    gotCommands = false;
                    gotSignal = false;
                    commands.clear ();

                }
            }
            else if(gotSignal && !gotPlayer && gotCommands)
            {
                //If Got a Signal and Command, with No Player
                QString tempCrit = "No player(*) between a signal(:) and command(>). Please close program and solve file problem. Need to open defualt light gun file for game, and add in player at signal, "+signal+". File: "+gameLGFilePath;
                if(displayMB)
                    QMessageBox::critical (p_guiConnect, "Process File Error", tempCrit, QMessageBox::Ok);
                return;
            }

        }

    }

    //If there is a blank line at the end, then need to enter the last data
    if(gotSignal && gotPlayer && gotCommands)
    {
        //qDebug() << "Last2 Signal: " << signal << " Commands: " << commands;

        signalsAndCommands.insert(signal, commands);
    }
    else if(gotSignal && !gotPlayer && !gotCommands)
    {
        //qDebug() << "Signal with No Commands, Signal: " << signal;
        //Signal that has no players and no commands
        signalsNoCommands << signal;
    }

    lgFile.close();
    lgFileLoaded = true;

    //Process the mame_start Command
    ProcessLGCommands(MAMESTARTAFTER, gameName);
}


void HookerEngine::ProcessLGCommands(QString signalName, QString value)
{
    QStringList commands, dlgCommands, multiValue;
    quint8 cmdCount;
    bool allPlayers = false;
    quint8 playerNum = 69;
    quint8 i, j, k, tempCPNum;
    quint8 howManyPlayers;
    quint8 player, lightGun;
    bool dlgCMDFound;
    bool findDLGCMDs;



    //Search Signals & Commands QMap for the Signal
    if(signalsAndCommands.contains (signalName))
        commands = signalsAndCommands[signalName];
    else
    {
        QString critMessage = "Signal name cannot be processed, as it is not found in any of the QMaps. Something really messed up happened. Signal: "+signalName;
        if(displayMB)
            QMessageBox::critical (p_guiConnect, "Processes Error",critMessage, QMessageBox::Ok);
        return;
    }

    //Get the Player(s) & Command(s) for Signal
    cmdCount = signalsAndCommands[signalName].count ();

    //First Command Is Always a Player
    if(commands[0] == ALLPLAYERS)
        allPlayers = true;
    else
    {
        commands[0].remove(0,2);
        playerNum = commands[0].toUInt ()-1;
    }

    for(i = 1; i < cmdCount; i++)
    {

        //First Check if there are multiple commands with '|', is so split.
        if(commands[i].contains ("|"))
        {
            commands[i].replace(" |","|");
            commands[i].replace("| ","|");

            multiValue = commands[i].split("|");

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
        if(commands[i][1] == OPENCOMPORT2CHAR)
        {
            if(commands[i] == OPENCOMPORT)
            {
                //Open COM Port from Players Info
                OpenLGComPort(allPlayers, playerNum);
            }
        }
        else if(commands[i][1] == CLOSECOMPORT2CHAR)
        {
            if(commands[i] == CLOSECOMPORT)
            {
                //Close COM Port from Players Info
                CloseLGComPort(allPlayers, playerNum);
            }
        }
        else if(commands[i][0] == CMDSIGNAL)
        {

            if(allPlayers)
                howManyPlayers = numberLGPlayers;
            else
                howManyPlayers = 1;

            for(j = 0; j < howManyPlayers; j++)
            {   
                if(allPlayers)
                    player = lgPlayerOrder[j];
                else
                    player = playerNum;


                lightGun = loadedLGNumbers[player];
                tempCPNum = p_comDeviceList->p_lightGunList[lightGun]->GetComPortNumber();

                //Search Commands For First time. Then if All Light Guns the Same, Then Don't Search Commands Beyond the 1st Time
                //and Re-Use the Found Commands, Since Same Light Gun, but Different Port
                //If 2 or More Unique Default Light Guns, then see if Default Light Gun Matches the Last Search Default Light Gun
                if(j == 0)
                    findDLGCMDs = true;
                else
                {
                    //All Light Guns are the Same Default Light Gun
                    //Else If Last Default Light Gun Number Matches Current Default Light Gun Number
                    if(uniqueDefaultLG == 1)
                        findDLGCMDs = false;
                    else if(defaultLGNumbers[j] == defaultLGNumbers[j-1])
                        findDLGCMDs = false;
                    else
                        findDLGCMDs = true;
                }

                if(findDLGCMDs)
                {
                    //Set True. If No Command or Null, then it is set to false
                    dlgCMDFound = true;

                    //Have to Search Commands Multiple Times as Multiple Default Light Guns Could be Used
                    //Faster to Search for 1 char than the whole string
                    //There are 4 Commands that start with ">A"
                    if(commands[i][1] == 'A')
                    {
                        if(commands[i][2] == 'm')
                        {
                            if(commands[i].size() > AMMOCMDCOUNT)
                            {
                                if(commands[i] == AMMOVALUECMD)
                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AmmoValueCommands(value.toUInt ());
                            }
                            else if(commands[i] == AMMOCMD)
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AmmoCommands();
                        }
                        else if(commands[i][2] == 's')
                        {
                            if(commands[i][13] == ARATIO169CMD13CHAR)
                            {
                                if(commands[i] == ARATIO169CMD)
                                    dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AspectRatio16b9Commands();
                            }
                            else if(commands[i] == ARATIO43CMD)
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AspectRatio4b3Commands();
                        }
                        else if(commands[i] == AUTOLEDCMD)
                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AutoLEDCommands();
                    }
                    else if(commands[i][1] == 'R')
                    {
                        //Two Commands Start with ">R", If ">Rel" then Reload, If Not then Recoil, and Then only when value != 0
                        if(commands[i][3] == 'l')
                        {
                            if(commands[i] == RELOADCMD)
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->ReloadCommands();
                        }
                        else if(commands[i][3] == 'c' && value != "0")
                        {
                            if(commands[i] == RECOILCMD)
                                dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->RecoilCommands();
                        }
                    }
                    else if(commands[i][0] == 'D' && value != "0")
                    {
                        //Only Do Damage if Value != 0
                        if(commands[i] == DAMAGECMD)
                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->DamageCommands();
                    }
                    else if(commands[i][0] == 'S' && value != "0")
                    {
                        //Only Do Shake if Value != 0
                        if(commands[i] == SHAKECMD)
                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->ShakeCommands();
                    }
                    else if(commands[i][0] == 'J')
                    {
                        if(commands[i] == JOYMODECMD)
                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->JoystickModeCommands();
                    }
                    else if(commands[i][0] == 'K')
                    {
                        if(commands[i] == KANDMMODECMD)
                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->MouseAndKeyboardModeCommands();
                    }
                    else if(commands[i][0] == 'N')
                    {
                        //If Null Command - Do Nothing
                        if(commands[i] == DLGNULLCMD)
                            dlgCMDFound = false;
                    }
                    else
                        dlgCMDFound = false;

                }//if(findDLGCMDs)
                else
                {
                    if(commands[i][1] == 'A' && commands[i][2] == 'm')
                    {
                        if(commands[i] == AMMOVALUECMD)
                            dlgCommands = p_comDeviceList->p_lightGunList[lightGun]->AmmoValueCommands(value.toUInt ());
                    }
                }

                //isEmpty() is true when Empty
                if(dlgCMDFound)
                {
                    //Write Command(s) to the Default Light Gun's COM Port
                    for(k = 0; k < dlgCommands.count(); k++)
                    {
                        //qDebug() << "Writting to Port: " << tempCPNum << " with Commands: " << dlgCommands[k];
                        WriteLGComPort(tempCPNum, dlgCommands[k]);
                    }
                }

            }//for(j = 0; j < howManyPlayers; j++)

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
            }

        }//Command Searching If and Else If

    }//for(i = 1; i < cmdCount; i++) Main Command Loop
}

void HookerEngine::OpenLGComPort(bool allPlayers, quint8 playerNum)
{
    quint8 howManyPlayers, i, player, lightGun, j;

    quint8 tempCPNum;
    QString tempCPName;
    qint32 tempBaud;
    quint8 tempData;
    quint8 tempParity;
    quint8 tempStop;
    quint8 tempFlow;
    QStringList commands;
    quint8 cmdCount;

    if(allPlayers)
        howManyPlayers = numberLGPlayers;
    else
        howManyPlayers = 1;

    for(i = 0; i < howManyPlayers; i++)
    {
        if(allPlayers)
            player = lgPlayerOrder[i];
        else
            player = playerNum;


        //Get Light Gun Number
        lightGun = loadedLGNumbers[player];

        //qDebug() << "Player Number: " << player << " Light Gun Number: " << lightGun;

        //Gets COM Port Settings
        tempCPNum = p_comDeviceList->p_lightGunList[lightGun]->GetComPortNumber();
        tempCPName = p_comDeviceList->p_lightGunList[lightGun]->GetComPortString();
        tempBaud = p_comDeviceList->p_lightGunList[lightGun]->GetComPortBaud();
        tempData = p_comDeviceList->p_lightGunList[lightGun]->GetComPortDataBits();
        tempParity = p_comDeviceList->p_lightGunList[lightGun]->GetComPortParity();
        tempStop = p_comDeviceList->p_lightGunList[lightGun]->GetComPortStopBits();
        tempFlow = p_comDeviceList->p_lightGunList[lightGun]->GetComPortFlow();

        //Opens the COM Port
        emit StartComPort(tempCPNum, tempCPName, tempBaud, tempData, tempParity, tempStop, tempFlow);

        //Get the Commnds for Open COM Port
        commands = p_comDeviceList->p_lightGunList[lightGun]->OpenComPortCommands();
        cmdCount = commands.count();

        //qDebug() << "Command Count: " << cmdCount << " Commands: " << commands;

        //Write Commands to the COM Port
        if(cmdCount > 0)
        {
            for(j = 0; j < cmdCount; j++)
            {
                WriteLGComPort(tempCPNum, commands[j]);
            }
        }
    }
}

void HookerEngine::CloseLGComPort(bool allPlayers, quint8 playerNum)
{
    quint8 howManyPlayers, i, j, player, lightGun;
    quint8 tempCPNum;
    QStringList commands;
    quint8 cmdCount;

    if(allPlayers)
        howManyPlayers = numberLGPlayers;
    else
        howManyPlayers = 1;

    for(i = 0; i < howManyPlayers; i++)
    {
        if(allPlayers)
            player = lgPlayerOrder[i];
        else
            player = playerNum;

        //Get Light Gun Number
        lightGun = loadedLGNumbers[player];
        //Get COM Port For Light Gun
        tempCPNum = p_comDeviceList->p_lightGunList[lightGun]->GetComPortNumber();

        //Get Close COM Port Commands for Light Gun
        commands = p_comDeviceList->p_lightGunList[lightGun]->CloseComPortCommands();
        cmdCount = commands.count();

        //Write Commnds to COM Port
        if(cmdCount > 0)
        {
            for(j = 0; j < cmdCount; j++)
            {
                WriteLGComPort(tempCPNum, commands[j]);
            }
        }

        //Closes The COM Port
        emit StopComPort(tempCPNum);
    }

}

void HookerEngine::WriteLGComPort(quint8 cpNum, QString cpData)
{
    QByteArray cpBA = cpData.toUtf8 ();

    //Send Data to COM Port
    emit WriteComPortSig(cpNum, cpBA);
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



