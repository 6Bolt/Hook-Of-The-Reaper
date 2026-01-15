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

    quint8 i;
    for(i = 0; i < MAXGAMEPLAYERS; i++)
    {
        ammoValue[i] = 0;
        lifeValue[i] = 0;
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

    /*
    //Disconnect Light Commands from Error Message Before Clearing them
    if(!signalLightCommand.isEmpty())
    {
        QMapIterator<QString, LightCommand> x(signalLightCommand);
        while (x.hasNext())
        {
            x.next();
            disconnect(&signalLightCommand[x.key()],&LightCommand::ShowErrorMessage, this, &HookLight::ErrorMessage);
        }
    }
    */

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
                        QString failMeg = "Light controller group number is not a number.\nFailing Number: "+splitData[i];
                        emit ShowErrorMessage(title, failMeg);
                        return false;
                    }

                    bool chkGrp = p_comDeviceList->p_lightCntlrList[cntlrNum]->CheckRGBGroupNumber (tempGrp);

                    if(chkGrp)
                    {
                        groups << tempGrp;
                    }
                    else
                    {
                        QString failMeg = "Light controller group number doesn't exist in controller.\nFailing Group Number: "+splitData[i];
                        emit ShowErrorMessage(title, failMeg);
                        return false;
                    }
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

                if(numArgs != 255)
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
                quint8 i;
                for(i = 1; i < splitData.count(); i++)
                    tempArg << splitData[i];

                LightCommand tempCmd(outputSignal,command,tempArg,cntlrGroups);

                //Connect Error Message to Light Commands
                //connect(&tempCmd,&LightCommand::ShowErrorMessage, this, &HookLight::ErrorMessage);

                //tempCmd.ProcessLightCommand ();

                bool validCmd = tempCmd.IsCommandValid();

                if(!validCmd)
                {
                    QString failMeg = "Light controller command is not valid. Something is wrong with the command arguments.\nFailing Line: "+fileData[lineNumber];
                    emit ShowErrorMessage(title, failMeg);
                    return false;
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
        }
    }
}


void HookLight::GameStopped()
{
    //Game Has Stopped
    isFileLoaded = false;


}

void HookLight::TCPSocketDisconnected()
{
    //TCP Socket has Been Disconnected
    isFileLoaded = false;

    quint8 i;
    for(i = 0; i < MAXGAMEPLAYERS; i++)
    {
        ammoValue[i] = 0;
        lifeValue[i] = 0;
    }
}

void HookLight::ProcessSignal(const QString &signal, const QString &data)
{
    //Process Filtered Output Signals From TCP Socket

    //qDebug() << "Process Signals for Light - signal:" << signal << "data:" << data;

    quint16 dataNumber = data.toUInt();

    //Check if in Game and File was Loaded
    if(isFileLoaded)
    {
        //Now Get Light Command
        LightCommand lightCmd = signalLightCommand[signal];
        quint8 kindOfCommand = lightCmd.GetKindOfCommand ();
        quint8 commandNumber = lightCmd.GetCommandNumber();
        quint8 numberCntlrs = lightCmd.GetNumberControllers();

        QString color = lightCmd.GetColor();
        quint8 i, cntlrNumber;

        if(kindOfCommand == FLASHCOMMAND)
        {
            //Flash Command - Get Normal Flash Command Args Ready
            quint16 timeOn = lightCmd.GetTimeOn();
            quint16 timeOff = lightCmd.GetTimeOff();
            quint8 numberFlashs = lightCmd.GetNumberFlashes();


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
                quint16 newAmmo = data.toUInt ();
                quint8 playerNum = lightCmd.GetPlayerNumber();

                if(newAmmo > ammoValue[playerNum])
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
                ammoValue[playerNum] = newAmmo;
            }
            else if(commandNumber == DEATHFLASHRGBCMD)
            {
                //Reload Flash RGB Command
                //Get New Ammo and Player Number
                quint16 newLife = data.toUInt ();
                quint8 playerNum = lightCmd.GetPlayerNumber();

                if(newLife == 0 && lifeValue[playerNum] == 1)
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
                lifeValue[playerNum] = newLife;
            }
            else if(commandNumber == RANDOMFLASHRGBCMD && dataNumber != 0)
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
        else if(kindOfCommand == SEQUENCECOMMAND)
        {
            //Sequence Command - Get Normal Flash Command Args Ready
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
                quint16 newAmmo = data.toUInt ();
                quint8 playerNum = lightCmd.GetPlayerNumber();

                if(newAmmo > ammoValue[playerNum])
                {
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->SequenceRGBLights(groups, timeDelay, color);
                    }
                }

                //Make New Ammo the Old Ammo Value
                ammoValue[playerNum] = newAmmo;
            }
        }


    }
}


void HookLight::ErrorMessage(const QString &title, const QString &message)
{
    //Goes Up the Chain to Main Thread
    emit ShowErrorMessage(title, message);
}












