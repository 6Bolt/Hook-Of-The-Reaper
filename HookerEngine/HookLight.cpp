#include "HookLight.h"

HookLight::HookLight(ComDeviceList *cdList, QString currentPath, QObject *parent)
    : QObject{parent}
{

    //Connect to the List
    p_comDeviceList = cdList;

    //Paths for Files
    pathLightFiles = currentPath+"/"+LIGHTGAMEFILESDIR+"/";
    defaultFilePath = pathLightFiles + DEFAULTLIGHTFILE;

    //Has Game File Been Loaded
    isFileLoaded = false;

    //Check the number of Light Controllers. If 0, then do nothing
    numberLightCntlrs = p_comDeviceList->GetNumberLightControllers ();

    quint8 i;
    for(i = 0; i < MAXGAMEPLAYERS; i++)
    {
        ammoValue[i] = 0;
        lifeValue[i] = 0;
        playerAlive[i] = false;
        maxLifeValue[i] = 0;
        maxDamage[i] = 0;
    }
}


HookLight::~HookLight()
{

}


//Load Light File that is the Game of Default
bool HookLight::LoadLightFile()
{
    bool openFile;
    QString line;
    QFile loadLightData(lightFilePath);
    QStringList fileData;
    quint16 lineNumber = 0;
    quint16 lineCount;
    quint16 cntlrLine;
    bool isNumber;
    QString outputSignal;
    QMap<quint8,QList<quint8>> cntlrGroups;
    QList<quint8> groups;
    bool gotOutputSignal = false;
    bool gotCntlrGroups = false;
    bool gotCommands = false;
    quint8 cntlrNum;
    QString command;
    bool invertData = false;
    bool isBackgroundAll = false;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    openFile = loadLightData.open (QIODeviceBase::ReadOnly | QIODevice::Text);
#else
    openFile = loadLightData.open (QIODevice::ReadOnly | QIODevice::Text);
#endif

    if(!openFile)
    {
        QString message, title;

        if(isGameFile)
            message = "Can not open light game file to read. Please close program and solve file problem. Might be permissions problem.";
        else
            message = "Can not open light default file to read. Please close program and solve file problem. Might be permissions problem.";

        title = "Light Game/Default File Error";

        emit ShowErrorMessage(title, message);
        return false;
    }



    //Clear out Map and List
    signalLightCommand.clear();
    outputSignals.clear();

    //Title for QMeesageBox Fail
    QString title = "Light Game/Default File Error";

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&loadLightData);

    while(!in.atEnd ())
    {
        //Read Line
        line = in.readLine();

        //Chop New Line off
        if(line.endsWith('\n') || line.endsWith('\r'))
            line.chop(1);

        //Store line into the QStringList, if not Empty
        if(!line.isEmpty())
            fileData << line;
    }

    //Now that we have the Data, close File
    loadLightData.close ();

    lineCount = fileData.length ();

    while(lineNumber < lineCount)
    {
        fileData[lineNumber] = fileData[lineNumber].trimmed ();

        //Check for Output Signal
        if(fileData[lineNumber][0] == ':')
        {
            if((gotOutputSignal && gotCntlrGroups && gotCommands) || (!gotCntlrGroups && !gotCommands))
            {
                gotCntlrGroups = false;
                gotCommands = false;

                outputSignal = fileData[lineNumber];
                //Remove the :
                outputSignal.remove(0,1);
                outputSignal = outputSignal.trimmed ();

                if(outputSignal.endsWith (INVERTDATASYMBOL))
                {
                    invertData = true;
                    outputSignal.chop(1);
                }
                else
                    invertData = false;

                gotOutputSignal = true;
            }
            else
            {
                QString failMeg = "Failed when getting output signal. It needs the controller & groups and command after it.\nFailing Line: "+fileData[lineNumber];
                emit ShowErrorMessage(title, failMeg);
                return false;
            }
        }
        else if(fileData[lineNumber][0] == '!')
        {
            if(gotOutputSignal && !gotCommands)
            {
                //Remove the ! from Controller Number
                fileData[lineNumber].remove(0,1);

                //Split Up Line Data with a Space
                QStringList splitData = fileData[lineNumber].split(' ', Qt::SkipEmptyParts);

                if(splitData.length() < 2)
                {
                    QString failMeg = "Light controller number and groups, don't have enough data.\nFailing Line: "+fileData[lineNumber];
                    emit ShowErrorMessage(title, failMeg);
                    return false;
                }

                cntlrNum = splitData[0].toUInt(&isNumber);

                if(!isNumber)
                {
                    QString failMeg = "Light controller number is not a number.\nFailing Number: "+splitData[0];
                    emit ShowErrorMessage(title, failMeg);
                    return false;
                }

                if(p_comDeviceList->p_lightCntlrList[cntlrNum] == nullptr)
                {
                    QString failMeg = "Light controller with that number, doesn't exist.\nFailing Number: "+splitData[0];
                    emit ShowErrorMessage(title, failMeg);
                    return false;
                }

                if(!cntlrGroups.isEmpty ())
                {
                    if(cntlrGroups.contains (cntlrNum))
                    {
                        QString failMeg = "Light controller number aready used for this command.\nController Number: "+splitData[0];
                        emit ShowErrorMessage(title, failMeg);
                        return false;
                    }
                }

                quint8 i;
                groups.clear();

                for(i = 1; i < splitData.length(); i++)
                {
                    quint8 tempGrp = splitData[i].toUInt(&isNumber);

                    if(!isNumber)
                    {
                        QString failMeg = "Light controller group number is not a number.\nFailing Number: "+splitData[i]+"\nFailing Controller Number: "+splitData[0];
                        emit ShowErrorMessage(title, failMeg);
                        return false;
                    }

                    groups << tempGrp;

                    //Check Groups moved below, in the Command Area
                }

                //Add Controller Number and Group Numbers to cntlrGroups Map
                cntlrGroups.insert(cntlrNum, groups);

                cntlrLine = lineNumber;

                gotCntlrGroups = true;
            }
            else
            {
                QString failMeg = "Failed when getting controller and groups. It needs the output signal before, and command after.\nFailing Line: "+fileData[lineNumber];
                emit ShowErrorMessage(title, failMeg);
                return false;
            }
        }
        else if(fileData[lineNumber][0] == '>')
        {
            if(gotOutputSignal && gotCntlrGroups && !gotCommands)
            {
                //Remove the > from Controller Number
                fileData[lineNumber].remove(0,1);

                //Split Up Line Data with a Space
                QStringList splitData = fileData[lineNumber].split(' ', Qt::SkipEmptyParts);

                command = splitData[0];

                quint8 numArgs = CheckCommandArgs(command);

                if(numArgs == 254)
                {
                    if(splitData.count() < 3)
                    {
                        QString failMeg = "There is too little arguments for the background command.\nFailing Line: "+fileData[lineNumber];
                        emit ShowErrorMessage(title, failMeg);
                        return false;
                    }
                }
                else if(numArgs != 255)
                {
                    if(splitData.count() != (numArgs+1))
                    {
                        QString failMeg = "There is too many or too little arguments for the command.\nFailing Line: "+fileData[lineNumber];
                        emit ShowErrorMessage(title, failMeg);
                        return false;
                    }
                }
                else
                {
                    QString failMeg = "Command for light controller is not found.\nFailing Command: "+command;
                    emit ShowErrorMessage(title, failMeg);
                    return false;
                }

                //Load Up the Command Args
                QStringList tempArg;
                quint8 i, j;
                for(i = 1; i < splitData.count(); i++)
                    tempArg << splitData[i];

                LightCommand tempCmd(outputSignal,command,tempArg,cntlrGroups);

                bool validCmd = tempCmd.IsCommandValid();

                if(!validCmd)
                {
                    QString failMeg = "Light controller command is not valid. Something is wrong with the command arguments.\nFailing Line: "+fileData[lineNumber];
                    emit ShowErrorMessage(title, failMeg);
                    return false;
                }

                if(invertData)
                    tempCmd.SetInvertData();

                //Check Controllers Group Numbers for Command
                bool isRGB = tempCmd.IsRGB();
                bool isGeneralCommand = tempCmd.IsGeneralCommand ();
                bool isBackground = tempCmd.IsBackgroundCommand ();
                quint8 numCntlrs = tempCmd.GetNumberControllers();

                if(isBackground)
                    isBackgroundAll = true;

                if(!isGeneralCommand)
                {
                    for(i = 0; i < numCntlrs; i++)
                    {
                        quint8 cntrlNumber = tempCmd.GetControllerNumber(i);
                        QList<quint8> groups = tempCmd.GetControllerGroups(cntrlNumber);

                        //qDebug() << "Number of Controllers:" << numCntlrs << "Controller Number:" << cntrlNumber;

                        for(j = 0; j < groups.count(); j++)
                        {
                            bool chkGrp;

                            if(isRGB)
                                chkGrp = p_comDeviceList->p_lightCntlrList[cntrlNumber]->CheckRGBGroupNumber (groups[j]);
                            else
                                chkGrp = p_comDeviceList->p_lightCntlrList[cntrlNumber]->CheckRegularGroupNumber (groups[j]);

                            //qDebug() << "chkGrp" << chkGrp << "Controller Number:" << cntrlNumber << "Group Number:" << groups[j];

                            if(!chkGrp)
                            {
                                QString failMeg = "Group number doesn't exist in the light controller.\nFailing Group Number: "+QString::number(groups[j])+"\nFailing Controller Number: "+QString::number(cntrlNumber);
                                emit ShowErrorMessage(title, failMeg);
                                return false;
                            }
                        }
                    }
                }


                //Check Color, Side Color, and Color Map, if RGB
                if(isRGB && !isGeneralCommand)
                {
                    //Checking Color
                    if(tempCmd.IsColor())
                    {
                        QString colorCHK = tempCmd.GetColor ();

                        //Check Light Controllers for Color
                        for(i = 0; i < numCntlrs; i++)
                        {
                            quint8 cntrlNumber = tempCmd.GetControllerNumber(i);

                            bool chkColor = p_comDeviceList->p_lightCntlrList[cntrlNumber]->CheckColor (colorCHK);

                            if(!chkColor)
                            {
                                QString failMeg = "Light controller cannot find color "+colorCHK+" in it's color map. Please make sure color is defined in the group file.\nController Number: "+QString::number(cntrlNumber);
                                emit ShowErrorMessage(title, failMeg);
                                return false;
                            }

                            //Is there a Side Color, if so then Check Color and Groups
                            if(tempCmd.IsSideColor())
                            {
                                QString sideColorCHK = tempCmd.GetSideColor ();

                                bool chkSideColor = p_comDeviceList->p_lightCntlrList[cntrlNumber]->CheckColor (sideColorCHK);

                                if(!chkSideColor)
                                {
                                    QString failMeg = "Light controller cannot find color "+sideColorCHK+" in it's color map. Please make sure color is defined in the group file.\nController Number: "+QString::number(cntrlNumber);
                                    emit ShowErrorMessage(title, failMeg);
                                    return false;
                                }

                                QList<quint8> groups = tempCmd.GetControllerGroups(cntrlNumber);
                                bool groupsChk = p_comDeviceList->p_lightCntlrList[cntrlNumber]->CheckGroupsfor2Colors (groups);

                                if(!groupsChk)
                                {
                                    QString failMeg = "Groups using the random flash with 2 colors need 3 lights or more in each group. Please check random flash with 2 colors for the light controller below.\nController Number: "+QString::number(cntrlNumber);
                                    emit ShowErrorMessage(title, failMeg);
                                    return false;
                                }

                            }
                        }
                    }

                    //Checking Color Map
                    if(tempCmd.IsColorMap())
                    {
                        QString colorMapCHK = tempCmd.GetColorMap ();

                        //Check Light Controllers for Color Map
                        for(i = 0; i < numCntlrs; i++)
                        {
                            quint8 cntrlNumber = tempCmd.GetControllerNumber(i);

                            bool chkColor = p_comDeviceList->p_lightCntlrList[cntrlNumber]->CheckColorMap (colorMapCHK);

                            if(!chkColor)
                            {
                                QString failMeg = "Light controller cannot find color map, "+colorMapCHK+", in it's color map map. Please make sure the colors and color map is defined in the group file.\nController Number: "+QString::number(cntrlNumber);
                                emit ShowErrorMessage(title, failMeg);
                                return false;
                            }
                        }
                    }
                }

                //Check Background Commands to make sure they have 1 Group per Controller
                if(isBackground)
                {
                    for(i = 0; i < numCntlrs; i++)
                    {
                        quint8 cntrlNumber = tempCmd.GetControllerNumber(i);
                        QList<quint8> groups = tempCmd.GetControllerGroups(cntrlNumber);

                        if(groups.count() != 1)
                        {
                            QString failMeg = "Background command can only have one group with it.\nController Number: "+QString::number(cntrlNumber)+"\nCommand: "+tempCmd.GetCommand ();
                            emit ShowErrorMessage(title, failMeg);
                            return false;
                        }
                    }
                }


                signalLightCommand.insert(outputSignal,tempCmd);

                outputSignals << outputSignal;

                //Clear Out the Controller Groups Map
                cntlrGroups.clear();

                gotCommands = true;
            }
            else
            {
                QString failMeg = "Failed when getting command. It needs the output signal and controller & groups before it.\nFailing Line: "+fileData[lineNumber];
                emit ShowErrorMessage(title, failMeg);
                return false;
            }
        }

        lineNumber++;
    }

    if((gotOutputSignal && gotCntlrGroups && gotCommands) || (!gotCntlrGroups && !gotCommands))
    {
        //File Loaded Good and Ended Well

        //Check for Background Commands
        if(isBackgroundAll)
        {
            quint8 i;
            QMapIterator<QString, LightCommand> x(signalLightCommand);
            while (x.hasNext())
            {
                x.next();
                LightCommand lightCmd = signalLightCommand[x.key()];
                if(lightCmd.IsBackgroundCommand ())
                {
                    quint8 numberCntlrs = lightCmd.GetNumberControllers();
                    QString colorMap = lightCmd.GetColorMap ();
                    //QString command = lightCmd.GetCommand ();
                    quint8 playerNumber = lightCmd.GetPlayerNumber ();
                    QList<quint8> otherGroups = lightCmd.GetOtherBGGroups();
                    quint16 delay = lightCmd.GetTimeDelay ();
                    quint8 highCount = lightCmd.GetHighCount ();
                    quint16 reloadDelay = lightCmd.GetBGTimeDelayReload ();

                    //qDebug() << "highCount" << highCount << "delay" << delay << "reloadDelay" << reloadDelay;

                    for(i = 0; i < numberCntlrs; i++)
                    {
                        quint8 cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                        if(groups.count() != 1)
                        {
                            QString failMeg = "Background command can only have 1 group. It has too many or 0. Please fix in the game/default file.\nPlayer: "+QString::number(playerNumber+1);
                            emit ShowErrorMessage(title, failMeg);
                            return false;
                        }

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->SetUpBackgroundRGB(groups, colorMap, playerNumber, delay, reloadDelay, highCount, otherGroups);
                    }
                }
            }
        }


        //Send Output Signals to TCP Socket
        emit GameStartSignals(outputSignals);

        //if(isGameFile)
        //    qDebug() << "Game File Loaded:" << lightFilePath;
        //else
        //    qDebug() << "Default File Loaded:" << lightFilePath;

        //qDebug() << "Output Signals List:" << outputSignals;

        return true;
    }
    else
    {
        QString failMeg = "File failed to load, as it ended with with a controller and groups.\nFailing Line: "+fileData[cntlrLine];
        emit ShowErrorMessage(title, failMeg);
        return false;
    }

}


quint8 HookLight::CheckCommandArgs(QString command)
{
    if(command == FLASHRGB)
        return FLASHRGBARGS;
    else if(command == RELOADFLASHRGB)
        return RELOADFLASHRGBARGS;
    else if(command == DEATHFLASHRGB)
        return DEATHFLASHRGBARGS;
    else if(command == RANDOMFLASHRGB)
        return RANDOMFLASHRGBARGS;
    else if(command == RANDOMFLASHRGB2C)
        return RANDOMFLASHRGB2CARGS;
    else if(command == SEQUENCERGB)
        return SEQUENCERGBARGS;
    else if(command == RELOADSEQUENCERGB)
        return RELOADSEQUENCERGBARGS;
    else if(command == FOLLOWERRGB)
        return FOLLOWERRGBARGS;
    else if(command == FOLLOWERRANDOMRGB)
        return FOLLOWERRANDOMRGBARGS;
    else if(command == FLASHREG)
        return FLASHREGARGS;
    else if(command == RELOADFLASHREG)
        return RELOADFLASHREGARGS;
    else if(command == DEATHFLASHREG)
        return DEATHFLASHREGARGS;
    else if(command == RANDOMFLASHREG)
        return RANDOMFLASHREGARGS;
    else if(command == SEQUENCEREG)
        return SEQUENCEREGARGS;
    else if(command == RELOADSEQUENCEREG)
        return RELOADSEQUENCEREGARGS;
    else if(command == FOLLOWERREG)
        return FOLLOWERREGARGS;
    else if(command == TURNOFFLIGHTS)
        return TURNOFFLIGHTSARGS;
    else if(command == RANDOMFLASHRGBCM)
        return RANDOMFLASHRGBCMARGS;
    else if(command == SEQUENCERGBCM)
        return SEQUENCERGBCMARGS;
    else if(command == RELOADSEQUENCERGBCM)
        return RELOADSEQUENCERGBCMARGS;
    else if(command == BACKGROUNDRGB)
        return 254;
    else
        return 255;
}


//Public Slots

void HookLight::GameStarted(const QString &gameN)
{
    //Set Game Name
    gameName = gameN;

    //Check the number of Light Controllers. If 0, then do nothing
    numberLightCntlrs = p_comDeviceList->GetNumberLightControllers ();

    if(numberLightCntlrs == 0)
        return;


    QString gameFilePath = pathLightFiles + gameName + ENDOFLIGHTFILE;

    //Check if Game File Exists, if Yes, then Load it
    bool fileFound = QFile::exists (gameFilePath);

    if(fileFound)
    {
        //Load Light Game File
        lightFilePath = gameFilePath;
        isGameFile = true;
        isFileLoaded = LoadLightFile();

        //Send mame_start signal if file loaded
        if(isFileLoaded)
            ProcessSignal(MAMESTARTAFTER, "1");
    }
    else
    {
        //If No Game File, Check if Default File Exists
        bool defaultFileFound = QFile::exists (defaultFilePath);

        if(defaultFileFound)
        {
            //Load Default Light File, when Light Game File Doesn't Exist
            lightFilePath = defaultFilePath;
            isGameFile = false;
            isFileLoaded = LoadLightFile();

            //qDebug() << "Default File Loaded, isFileLoaded:" << isFileLoaded;

            //Send mame_start signal if file loaded
            if(isFileLoaded)
                ProcessSignal(MAMESTARTAFTER, "1");
        }
    }
}


void HookLight::GameStopped()
{
    //Send mame_stop Signal to Signal Processor
    if(isFileLoaded)
        ProcessSignal(MAMESTOPFRONT, "1");

    //Game Has Stopped
    isFileLoaded = false;

    quint8 i;

    for(i = 0; i < numberLightCntlrs; i++)
        p_comDeviceList->p_lightCntlrList[i]->ResetLightController();

}

void HookLight::TCPSocketDisconnected()
{
    quint8 i;


    //for(i = 0; i < numberLightCntlrs; i++)
    //    p_comDeviceList->p_lightCntlrList[i]->DoReset();


    //TCP Socket has Been Disconnected
    isFileLoaded = false;


    for(i = 0; i < MAXGAMEPLAYERS; i++)
    {
        ammoValue[i] = 0;
        lifeValue[i] = 0;
        playerAlive[i] = false;
        maxLifeValue[i] = 0;
        maxDamage[i] = 0;
    }
}

void HookLight::ProcessSignal(const QString &signal, const QString &data)
{
    //Process Filtered Output Signals From TCP Socket

    //qDebug() << "Process Signals for Light - signal:" << signal << "data:" << data;

    bool isNumber;
    quint16 dataNumber = data.toUInt(&isNumber);

    if(!isNumber)
        dataNumber = 0;


    //Now Get Light Command
    LightCommand lightCmd = signalLightCommand[signal];
    quint8 kindOfCommand = lightCmd.GetKindOfCommand ();
    quint8 commandNumber = lightCmd.GetCommandNumber();
    quint8 numberCntlrs = lightCmd.GetNumberControllers();
    bool invertData = lightCmd.IsInvertData ();

    if(invertData)
    {
        if(dataNumber > 0)
            dataNumber = 0;
        else
            dataNumber = 1;
    }

    quint8 i, cntlrNumber;

    //qDebug() << "kindOfCommand" << kindOfCommand << "commandNumber" << commandNumber;

    if(kindOfCommand < REGCOMMANDS)
    {

        if(kindOfCommand == FLASHCOMMAND)
        {
            //Flash Command - Get Normal Flash Command Args Ready
            QString color = lightCmd.GetColor();
            quint16 timeOn = lightCmd.GetTimeOn();
            quint16 timeOff = lightCmd.GetTimeOff();
            quint8 numberFlashs = lightCmd.GetNumberFlashes();

            if(commandNumber < 3) //For Flash Command 0-2
            {

                if(commandNumber == FLASHRGBCMD && dataNumber != 0)
                {
                    //Flash RGB Command
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->FlashRGBLights(groups, timeOn, timeOff, numberFlashs, color);
                    }
                }
                else if(commandNumber == RELOADFLASHRGBCMD)
                {
                    //Reload Flash RGB Command
                    //Get New Ammo and Player Number
                    quint8 playerNum = lightCmd.GetPlayerNumber();

                    if(dataNumber > ammoValue[playerNum])
                    {
                        //Then a Reload Happened
                        for(i = 0; i < numberCntlrs; i++)
                        {
                            cntlrNumber = lightCmd.GetControllerNumber(i);
                            QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                            p_comDeviceList->p_lightCntlrList[cntlrNumber]->FlashRGBLights(groups, timeOn, timeOff, numberFlashs, color);
                        }
                    }

                    //Make New Ammo the Old Ammo Value
                    ammoValue[playerNum] = dataNumber;
                }
                else if(commandNumber == DEATHFLASHRGBCMD)
                {
                    //Reload Flash RGB Command
                    //Get New Ammo and Player Number
                    quint8 playerNum = lightCmd.GetPlayerNumber();

                    if(!playerAlive[playerNum])
                    {
                        //If player starts game, make playerAlive = true
                        //And Do Nothing
                        if(dataNumber > lifeValue[playerNum])
                        {
                            playerAlive[playerNum] = true;
                            maxLifeValue[playerNum] = dataNumber;

                            if(maxDamage[playerNum] == 0)
                                maxDamage[playerNum] = maxLifeValue[playerNum] * MAXDAMAGEPERCENTAGE;
                        }
                    }
                    else  //Player is in the game now and Alive
                    {
                        if(dataNumber < lifeValue[playerNum] && dataNumber == 0)
                        {
                            //Try to catch skip cut scene stuff
                            quint8 damage = lifeValue[playerNum] - dataNumber;

                            if(damage <= maxDamage[playerNum])
                            {
                                playerAlive[playerNum] = false;

                                //Death Happened, Show Lights
                                for(i = 0; i < numberCntlrs; i++)
                                {
                                    cntlrNumber = lightCmd.GetControllerNumber(i);
                                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->FlashRGBLights(groups, timeOn, timeOff, numberFlashs, color);
                                }
                            }
                        }
                    }

                    //Make New Life the Old Life Value
                    lifeValue[playerNum] = dataNumber;
                }

            }
            else if(commandNumber > 2) //For Flash Commands 3-4
            {

                if(commandNumber == RANDOMFLASHRGBCMD && dataNumber != 0)
                {
                    //Random Flash RGB Command
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->FlashRandomRGBLights(groups, timeOn, timeOff, numberFlashs, color);
                    }
                }
                else if(commandNumber == RANDOMFLASHRGB2CCMD && dataNumber != 0)
                {
                    //Random Flash RGB Command
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        QString sideColor = lightCmd.GetSideColor();

                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->FlashRandomRGB2CLights(groups, timeOn, timeOff, numberFlashs, color, sideColor);
                    }
                }

            }
        }
        else if(kindOfCommand == FLASHCMCOMMAND)
        {
            //Flash Command - Get Normal Flash Command Args Ready
            QString colorMap = lightCmd.GetColorMap();
            quint16 timeOn = lightCmd.GetTimeOn();
            quint16 timeOff = lightCmd.GetTimeOff();
            quint8 numberFlashs = lightCmd.GetNumberFlashes();

            if(commandNumber == RANDOMFLASHRGBCMCMD && dataNumber != 0)
            {
                //Random Flash RGB Color Map Command
                for(i = 0; i < numberCntlrs; i++)
                {
                    cntlrNumber = lightCmd.GetControllerNumber(i);
                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->FlashRandomRGBLightsCM(groups, timeOn, timeOff, numberFlashs, colorMap);
                }
            }

        }
        else if(kindOfCommand == SEQUENCECOMMAND)
        {
            //Sequence Command - Get Normal Flash Command Args Ready
            QString color = lightCmd.GetColor();
            quint16 timeDelay = lightCmd.GetTimeDelay();


            if(commandNumber == SEQUENCERGBCMD && dataNumber != 0)
            {
                for(i = 0; i < numberCntlrs; i++)
                {
                    cntlrNumber = lightCmd.GetControllerNumber(i);
                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->SequenceRGBLights(groups, timeDelay, color);
                }
            }
            else if(commandNumber == RELOADSEQUENCERGBCMD)
            {
                //Sequence Flash RGB Command
                //Get New Ammo and Player Number
                quint8 playerNum = lightCmd.GetPlayerNumber();

                if(dataNumber > ammoValue[playerNum])
                {
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->SequenceRGBLights(groups, timeDelay, color);
                    }
                }

                //Make New Ammo the Old Ammo Value
                ammoValue[playerNum] = dataNumber;
            }
        }
        else if(kindOfCommand == SEQUENCECMCOMMAND)
        {
            //Sequence Color Map Command - Get Normal Sequence Command Args Ready
            QString colorMap = lightCmd.GetColorMap();
            quint16 timeDelay = lightCmd.GetTimeDelay();

            if(commandNumber == SEQUENCERGBCMCMD && dataNumber != 0)
            {
                for(i = 0; i < numberCntlrs; i++)
                {
                    cntlrNumber = lightCmd.GetControllerNumber(i);
                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->SequenceRGBLightsCM(groups, timeDelay, colorMap);
                }
            }
            else if(commandNumber == RELOADSEQUENCERGBCMCMD)
            {
                //Sequence Flash RGB Command
                //Get New Ammo and Player Number
                quint8 playerNum = lightCmd.GetPlayerNumber();

                if(dataNumber > ammoValue[playerNum])
                {
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->SequenceRGBLightsCM(groups, timeDelay, colorMap);
                    }
                }

                //Make New Ammo the Old Ammo Value
                ammoValue[playerNum] = dataNumber;
            }
        }
        else if(kindOfCommand == FOLLOWERCOMMAND)
        {
            QString color = lightCmd.GetColor();

            if(commandNumber == FOLLOWERRGBCMD)
            {
                for(i = 0; i < numberCntlrs; i++)
                {
                    cntlrNumber = lightCmd.GetControllerNumber(i);
                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->FollowerRGBLights(groups, color, dataNumber);
                }
            }
            else if(commandNumber == FOLLOWERRANDOMRGBCMD)
            {
                for(i = 0; i < numberCntlrs; i++)
                {
                    cntlrNumber = lightCmd.GetControllerNumber(i);
                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->FollowerRandomRGBLights(groups, dataNumber);
                }
            }
        }
    } //End of RGB Commands
    else if(kindOfCommand > REGCOMMANDS && kindOfCommand < BACKGROUNDCOMMAND)
    {
        if(kindOfCommand == REGFLASHCOMMAND)
        {
            //Flash Command - Get Normal Flash Command Args Ready
            quint16 timeOn = lightCmd.GetTimeOn();
            quint16 timeOff = lightCmd.GetTimeOff();
            quint8 numberFlashs = lightCmd.GetNumberFlashes();

            if(commandNumber < 3) //For Flash Command 0-2
            {

                if(commandNumber == FLASHREGCMD && dataNumber != 0)
                {
                    //Flash Regular Command
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->FlashRegularLights(groups, timeOn, timeOff, numberFlashs);
                    }
                }
                else if(commandNumber == RELOADFLASHREGCMD)
                {
                    //Reload Flash RGB Command
                    //Get New Ammo and Player Number
                    quint8 playerNum = lightCmd.GetPlayerNumber();

                    if(dataNumber > ammoValue[playerNum])
                    {
                        //Then a Reload Happened
                        for(i = 0; i < numberCntlrs; i++)
                        {
                            cntlrNumber = lightCmd.GetControllerNumber(i);
                            QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                            p_comDeviceList->p_lightCntlrList[cntlrNumber]->FlashRegularLights(groups, timeOn, timeOff, numberFlashs);
                        }
                    }

                    //Make New Ammo the Old Ammo Value
                    ammoValue[playerNum] = dataNumber;
                }
                else if(commandNumber == DEATHFLASHREGCMD)
                {
                    //Reload Flash RGB Command
                    //Get New Ammo and Player Number
                    quint8 playerNum = lightCmd.GetPlayerNumber();

                    if(!playerAlive[playerNum])
                    {
                        //If player starts game, make playerAlive = true
                        //And Do Nothing
                        if(dataNumber > lifeValue[playerNum])
                        {
                            playerAlive[playerNum] = true;
                            maxLifeValue[playerNum] = dataNumber;

                            if(maxDamage[playerNum] == 0)
                                maxDamage[playerNum] = maxLifeValue[playerNum] * MAXDAMAGEPERCENTAGE;
                        }
                    }
                    else  //Player is in the game now and Alive
                    {
                        if(dataNumber < lifeValue[playerNum] && dataNumber == 0)
                        {
                            //Try to catch skip cut scene stuff
                            quint8 damage = lifeValue[playerNum] - dataNumber;

                            if(damage <= maxDamage[playerNum])
                            {
                                playerAlive[playerNum] = false;

                                //Death Happened, Show Lights
                                for(i = 0; i < numberCntlrs; i++)
                                {
                                    cntlrNumber = lightCmd.GetControllerNumber(i);
                                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->FlashRegularLights(groups, timeOn, timeOff, numberFlashs);
                                }
                            }
                        }
                    }

                    //Make New Life the Old Life Value
                    lifeValue[playerNum] = dataNumber;
                }

            }
            else if(commandNumber > 2) //For Flash Commands 3-4
            {

                if(commandNumber == RANDOMFLASHREGCMD && dataNumber != 0)
                {
                    //Random Flash RGB Command
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->FlashRandomRegularLights(groups, timeOn, timeOff, numberFlashs);
                    }
                }

            }
        } //End of Regular Flash Commands
        else if(kindOfCommand == REGSEQUENCECOMMAND)
        {
            //Sequence Command - Get Normal Flash Command Args Ready
            quint16 timeDelay = lightCmd.GetTimeDelay();


            if(commandNumber == SEQUENCEREGCMD && dataNumber != 0)
            {
                for(i = 0; i < numberCntlrs; i++)
                {
                    cntlrNumber = lightCmd.GetControllerNumber(i);
                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->SequenceRegularLights(groups, timeDelay);
                }
            }
            else if(commandNumber == RELOADSEQUENCEREGCMD)
            {
                //Sequence Flash RGB Command
                //Get New Ammo and Player Number
                quint8 playerNum = lightCmd.GetPlayerNumber();

                if(dataNumber > ammoValue[playerNum])
                {
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->SequenceRegularLights(groups, timeDelay);
                    }
                }

                //Make New Ammo the Old Ammo Value
                ammoValue[playerNum] = dataNumber;
            }
        }//End of Regular Sequence Commands
        else if(kindOfCommand == REGFOLLOWERCOMMAND)
        {
            if(commandNumber == FOLLOWERREGCMD)
            {
                for(i = 0; i < numberCntlrs; i++)
                {
                    cntlrNumber = lightCmd.GetControllerNumber(i);
                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->FollowerRegularLights(groups, dataNumber);
                }
            }

        } //End of Regular Follower Commands

    } //End of Regular Commands
    else if(kindOfCommand == BACKGROUNDCOMMAND)
    {
        if(commandNumber == BACKGROUNDRGBCMD)
        {
            quint8 playerNum = lightCmd.GetPlayerNumber();

            for(i = 0; i < numberCntlrs; i++)
            {
                cntlrNumber = lightCmd.GetControllerNumber(i);
                QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                p_comDeviceList->p_lightCntlrList[cntlrNumber]->BackgroundRGB(playerNum, dataNumber);
            }
        }

    }
    else if(kindOfCommand == GENERALCOMMAND)
    {
        if(commandNumber == TURNOFFLIGHTSCMD)
        {
            for(i = 0; i < numberCntlrs; i++)
            {
                cntlrNumber = lightCmd.GetControllerNumber(i);

                p_comDeviceList->p_lightCntlrList[cntlrNumber]->TurnOffLights();
            }
        }
    }


}


void HookLight::ErrorMessage(const QString &title, const QString &message)
{
    //Goes Up the Chain to Main Thread
    emit ShowErrorMessage(title, message);
}












