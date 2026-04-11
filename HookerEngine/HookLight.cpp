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

    // If in a Game
    inGame = false;

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

    //Set Counts to Zero
    for(quint8 i = 0; i < MAXLIGHTCONTROLLERS; i++)
    {
        flashCount[i] = 0;
        rndFlashCount[i] = 0;
        sequentialCount[i] = 0;
    }
}


HookLight::~HookLight()
{

}

// Update Setting ,like when a new light controller is added
void HookLight::UpdateSettings()
{
    //Check the number of Light Controllers. If 0, then do nothing
    numberLightCntlrs = p_comDeviceList->GetNumberLightControllers ();

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
    bool isALEDStripAll = false;
    bool endOutputSignal = false;
    bool endGroups = false;
    bool endCommands = false;
    //bool firstOutputSignal = true;

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

        if(!line.isEmpty())
        {
            //Only Get Lines that Start with :, !, and >
            if(line[0] == OUTPUTSIGNALSTART || line[0] == CNTLRANDGROUPSSTART || line[0] == COMMANDSTART)
            {
                //Chop New Line off at Front and End
                line = line.trimmed();

                //Store line into the QStringList, if not Empty
                fileData << line;
            }
        }
    }

    //Now that we have the Data, close File
    loadLightData.close ();

    lineCount = fileData.length ();

    // Check Light Game/Default File Sequence
    bool chkSeq = CheckLightFile(fileData, lineCount);

    if(!chkSeq)
        return false;

    while(lineNumber < lineCount)
    {
        //Check for Output Signal
        if(fileData[lineNumber][0] == OUTPUTSIGNALSTART)
        {
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

        }
        else if(fileData[lineNumber][0] == '!')
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
        }
        else if(fileData[lineNumber][0] == '>')
        {
            //Remove the > from Controller Number
            fileData[lineNumber].remove(0,1);

            //Split Up Line Data with a Space
            QStringList splitData = fileData[lineNumber].split(' ', Qt::SkipEmptyParts);

            command = splitData[0];

            // Check the Args in the Command
            quint8 numArgs = CheckCommandArgs(command);

            if(numArgs == 254)
            {
                if(splitData.count() < 6)
                {
                    QString failMeg = "There is too little arguments for the RGB background command.\nFailing Line: "+fileData[lineNumber];
                    emit ShowErrorMessage(title, failMeg);
                    return false;
                }
            }
            else if(numArgs == 253)
            {
                if(splitData.count() < 5)
                {
                    QString failMeg = "There is too little arguments for the regular background command.\nFailing Line: "+fileData[lineNumber];
                    emit ShowErrorMessage(title, failMeg);
                    return false;
                }
            }
            else if(numArgs == 252)
            {
                if(splitData.count() != 7 && splitData.count() != 9)
                {
                    QString failMeg = "The arguments count is off for the ALED Random Flash command.\nFailing Line: "+fileData[lineNumber];
                    emit ShowErrorMessage(title, failMeg);
                    return false;
                }
            }
            else if(numArgs == 251)
            {
                if(splitData.count() != 6 && splitData.count() != 8)
                {
                    QString failMeg = "The arguments count is off for the ALED Display Range command.\nFailing Line: "+fileData[lineNumber];
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
            bool isALEDStrip = tempCmd.IsALEDStrip ();
            quint8 numCntlrs = tempCmd.GetNumberControllers();

            if(isBackground)
                isBackgroundAll = true;

            if(isALEDStrip)
                isALEDStripAll = true;

            // Check RGB or Regular Groups, if Not General or ALED Strip
            if(!isGeneralCommand && !isALEDStrip)
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
            if((isRGB || isALEDStrip) && !isGeneralCommand)
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

                            if(isRGB)
                            {
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



            if(signalLightCommand.contains (outputSignal))
            {
                bool chkCmds = CheckExsitingCommands(outputSignal, tempCmd);

                //qDebug() << "Check for " << outputSignal << "was" << chkCmds;

                if(chkCmds)
                    signalLightCommand[outputSignal].append (tempCmd);
                else
                    return false;
            }
            else
            {
                QList<LightCommand> tempCmdList;
                tempCmdList << tempCmd;

                signalLightCommand.insert(outputSignal,tempCmdList);

                outputSignals << outputSignal;
            }

            //Clear Out Controllers and Groups
            cntlrGroups.clear();
        }

        //Increament the Line Number
        lineNumber++;
    }

    // Light Game/Default File has Been Loaded

    //Set-Up Background Commands, as it Needs to be Done Beforehand
    if(isBackgroundAll)
    {
        quint8 i;
        QMapIterator<QString, QList<LightCommand>> x(signalLightCommand);
        while (x.hasNext())
        {
            x.next();
            QList<LightCommand> lightCmdList = signalLightCommand[x.key()];

            for(quint8 k = 0; k < lightCmdList.count(); k++)
            {
                LightCommand lightCmd =  lightCmdList[k];

                if(lightCmd.IsBackgroundCommand ())
                {
                    quint8 numberCntlrs = lightCmd.GetNumberControllers();
                    bool isRGB = lightCmd.IsRGB();
                    QString colorMap;
                    quint8 playerNumber = lightCmd.GetPlayerNumber ();
                    QList<quint8> otherGroups = lightCmd.GetOtherBGGroups();
                    quint16 delay = lightCmd.GetTimeDelay ();
                    quint8 highCount = lightCmd.GetHighCount ();
                    quint16 reloadDelay = lightCmd.GetBGTimeDelayReload ();

                    if(isRGB)
                        colorMap = lightCmd.GetColorMap ();

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

                        if(isRGB)
                            p_comDeviceList->p_lightCntlrList[cntlrNumber]->SetUpBackgroundRGB(groups, colorMap, playerNumber, delay, reloadDelay, highCount, otherGroups);
                        else
                            p_comDeviceList->p_lightCntlrList[cntlrNumber]->SetUpBackgroundRegular(groups, playerNumber, delay, reloadDelay, highCount, otherGroups);
                    }
                }
            }
        }
    }

    // Set-Up ALED Strip Commands, as they need to be Done Beforehand
    if(isALEDStripAll)
    {
        quint8 i;
        QMapIterator<QString, QList<LightCommand>> x(signalLightCommand);
        while (x.hasNext())
        {
            x.next();
            QList<LightCommand> lightCmdList = signalLightCommand[x.key()];

            for(quint8 k = 0; k < lightCmdList.count(); k++)
            {
                LightCommand lightCmd = lightCmdList[k];

                if(lightCmd.IsDisplayRange())
                {
                    //qDebug() << "Setting Up Display Range";

                    quint8 numberCntlrs = lightCmd.GetNumberControllers();
                    QString colorMap = lightCmd.GetColorMap ();
                    quint16 maxRange = lightCmd.GetMaxRange ();
                    quint8 numberSteps = lightCmd.GetNumberSteps ();
                    quint16 timeOff = lightCmd.GetTimeOff ();
                    bool enSeqR = lightCmd.GetSeqReloadDR();
                    quint16 tDelay = 0;
                    quint8 numLEDs = 0;

                    if(enSeqR)
                    {
                        tDelay = lightCmd.GetTimeDelay();
                        numLEDs = lightCmd.GetStripSeqNumberLEDs();
                    }

                    for(i = 0; i < numberCntlrs; i++)
                    {
                        quint8 cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> strips = lightCmd.GetControllerGroups(cntlrNumber);

                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->SetUpDisplayRange(strips, maxRange, numberSteps, timeOff, colorMap, enSeqR, tDelay, numLEDs);
                    }
                }
                else if(lightCmd.IsALEDStripFlash() || lightCmd.IsALEDStripRndFlash())
                {
                    quint8 numberCntlrs = lightCmd.GetNumberControllers();
                    quint16 timeOn = lightCmd.GetTimeOn ();
                    quint16 timeOff = lightCmd.GetTimeOff ();
                    quint8 numFlash = lightCmd.GetNumberFlashes();
                    QString color = lightCmd.GetColor();
                    quint8 command = lightCmd.GetCommandNumber();

                    if(command != RANDOMFLASHALEDSCMD)
                    {
                        for(i = 0; i < numberCntlrs; i++)
                        {
                            quint8 cntlrNumber = lightCmd.GetControllerNumber(i);
                            QList<quint8> strips = lightCmd.GetControllerGroups(cntlrNumber);
                            QList<quint8> structNums;

                            for(quint8 j = 0; j < strips.count(); j++)
                            {
                                p_comDeviceList->p_lightCntlrList[cntlrNumber]->SetUpStripFlash(flashCount[cntlrNumber], strips[j], timeOn, timeOff, numFlash, color);
                                structNums << flashCount[cntlrNumber];
                                flashCount[cntlrNumber]++;
                            }

                            lightCmd.SetStructNumber(cntlrNumber, structNums);
                        }
                    }
                    else
                    {
                        quint8 numLEDs = lightCmd.GetNumberLEDs();

                        for(i = 0; i < numberCntlrs; i++)
                        {
                            quint8 cntlrNumber = lightCmd.GetControllerNumber(i);
                            QList<quint8> strips = lightCmd.GetControllerGroups(cntlrNumber);
                            QList<quint8> structNums;
                            bool enable2nd = lightCmd.GetStripEnable2ndColor();
                            quint8 prob = 0;
                            QString color2;

                            if(enable2nd)
                            {
                                prob = lightCmd.GetProbability2ndColor();
                                color2 = lightCmd.GetSideColor();
                            }

                            for(quint8 j = 0; j < strips.count(); j++)
                            {
                                p_comDeviceList->p_lightCntlrList[cntlrNumber]->SetUpStripRndFlash(rndFlashCount[cntlrNumber], strips[j], numLEDs, timeOn, timeOff, numFlash, color, enable2nd, prob, color2);
                                structNums << rndFlashCount[cntlrNumber];
                                rndFlashCount[cntlrNumber]++;
                            }

                            lightCmd.SetStructNumber(cntlrNumber, structNums);
                        }
                    }
                }
                else if(lightCmd.IsALEDStripSequential())
                {
                    quint8 numberCntlrs = lightCmd.GetNumberControllers();
                    quint16 timeDelay = lightCmd.GetTimeDelay ();
                    QString color = lightCmd.GetColor();
                    quint8 numLEDs = lightCmd.GetStripSeqNumberLEDs();

                    for(i = 0; i < numberCntlrs; i++)
                    {
                        quint8 cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> strips = lightCmd.GetControllerGroups(cntlrNumber);
                        QList<quint8> structNums;

                        for(quint8 j = 0; j < strips.count(); j++)
                        {
                            p_comDeviceList->p_lightCntlrList[cntlrNumber]->SetUpStripSequential(sequentialCount[cntlrNumber], strips[j], timeDelay, color, numLEDs);
                            structNums << sequentialCount[cntlrNumber];
                            sequentialCount[cntlrNumber]++;
                        }

                        lightCmd.SetStructNumber(cntlrNumber, structNums);
                    }
                }

                lightCmdList[k] = lightCmd;
            }

            signalLightCommand[x.key()] = lightCmdList;
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


bool HookLight::CheckLightFile(QStringList file, quint16 lineCount)
{
    // This Checks the Sequence of the Light File, with only the 3 commands, as Everything else is Taken Out
    // File Starts with an Output Signal
    //      Cannot be Controller and Groups or Command
    // From Output Signal, can go to another Output Signal (lone), or Controller and Group
    //      Cannot Go to a Command
    // From Controller and Group, can go to another Controller and Group, or Command
    //      Cannot Go to an Output Signal
    // From Command, can go to an Output Signal, or Controller and Group
    //      Cannot Go to a Command
    // File End with a Command or lone Output Signal
    //      Cannot be a Controller and Groups

    //Last Line
    // OUTPUTSIG (0) - Output Signal
    // CNTLRGRPS (1) - Controller and Groups
    // CMD (2) - Command
    // UNASSIGN - Unknown

    quint8 lastLine = UNASSIGN;
    QString title = "Light Game/Default File Failed Sequence Check";

    for(quint16 i = 0; i < lineCount; i++)
    {
        if(i == 0)
        {
            //First Line needs to be an Output Signal
            if(file[i][0] != OUTPUTSIGNALSTART)
            {
                QString failMes = "The first sequence of the Light Game/Default file needs to be an Output Signal (:).\nFirst Sequence: " + file[i];
                emit ShowErrorMessage(title, failMes);
                return false;
            }
            else
                lastLine = OUTPUTSIG;
        }
        else if(i == (lineCount - 1))
        {
            //Last Line
            // Check Seq
            bool chkSeq = CheckLightFileSequence(lastLine, file[i-1], file[i]);

            if(!chkSeq)
                return false;
            else
            {
                //Check Last Line, tom Make Sure it is not a Controller and Group
                if(file[i][0] == CNTLRANDGROUPSSTART)
                {
                    QString failMes = "The last sequence of the Light Game/Default file cannot be a Controller and Groups (!).\nLast Sequence: " + file[i];
                    emit ShowErrorMessage(title, failMes);
                    return false;
                }
                // No Need for lastLine, as this is the Last Line
            }
        }
        else
        {
            //Everything in the Middle, Just Check Sequence
            bool chkSeq = CheckLightFileSequence(lastLine, file[i-1], file[i]);

            if(!chkSeq)
                return false;
            else
            {
                if(file[i][0] == OUTPUTSIGNALSTART)
                    lastLine = OUTPUTSIG;
                else if(file[i][0] == CNTLRANDGROUPSSTART)
                    lastLine = CNTLRGRPS;
                else if(file[i][0] == COMMANDSTART)
                    lastLine = CMD;
                else
                    lastLine = UNASSIGN;
            }
        }
    }

    //Ran the Gauntlet
    return true;
}

bool HookLight::CheckLightFileSequence(quint8 lastLineType, QString lastLine, QString line)
{
    quint8 lineType = UNASSIGN;
    QString title = "Light Game/Default File Failed Sequence Check";

    if(line[0] == OUTPUTSIGNALSTART)
        lineType = OUTPUTSIG;
    else if(line[0] == CNTLRANDGROUPSSTART)
        lineType = CNTLRGRPS;
    else if(line[0] == COMMANDSTART)
        lineType = CMD;

    // Check for UNASSIGN
    if(lineType == UNASSIGN || lastLineType == UNASSIGN)
        return false;

    if(lastLineType == OUTPUTSIG)
    {
        if(lineType == CMD)
        {
            QString failMes = "The sequence check failed for the Light Game/Default file. An Output Signal (:) and then a Command (>) happened, which is illegal.\nLast Sequence: " + lastLine + "\nSequence: " + line;
            emit ShowErrorMessage(title, failMes);
            return false;
        }
        else
            return true;
    }
    else if(lastLineType == CNTLRGRPS)
    {
        if(lineType == OUTPUTSIG)
        {
            QString failMes = "The sequence check failed for the Light Game/Default file. A Controller & Groups (!) and then an Output Signal (:) happened, which is illegal.\nLast Sequence: " + lastLine + "\nSequence: " + line;
            emit ShowErrorMessage(title, failMes);
            return false;
        }
        else
            return true;
    }
    else if(lastLineType == CMD)
    {
        if(lineType == CMD)
        {
            QString failMes = "The sequence check failed for the Light Game/Default file. A Command (>) and then an Command (>) happened, which is illegal.\nLast Sequence: " + lastLine + "\nSequence: " + line;
            emit ShowErrorMessage(title, failMes);
            return false;
        }
        else
            return true;
    }

    //Something Went Wrong if You got Here
    return false;

}

bool HookLight::CheckExsitingCommands(QString outputSignal, LightCommand newCmd)
{
    quint8 k, i, j, l;
    quint8 newNumberCntlrs = newCmd.GetNumberControllers();
    QString title = "Light Game/Default File Failed Exsiting Command Check";
    LightCommand lightCmd;
    quint8 numberCntlrs, cntlrNumber, newCntlrNumber;

    //Get Exsiting Commands
    QList<LightCommand> lightCmdList = signalLightCommand[outputSignal];

    for(k = 0; k < lightCmdList.count(); k++)
    {
        lightCmd = lightCmdList[k];

        numberCntlrs = lightCmd.GetNumberControllers();

        for(i = 0; i < numberCntlrs; i++)
        {
            cntlrNumber = lightCmd.GetControllerNumber(i);

            for(j = 0; j < newNumberCntlrs; j++)
            {
                newCntlrNumber = newCmd.GetControllerNumber(j);

                if(cntlrNumber == newCntlrNumber)
                {
                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);
                    QList<quint8> newGroups = newCmd.GetControllerGroups(newCntlrNumber);

                    for(l = 0; l < newGroups.length(); l++)
                    {
                        if(groups.contains(newGroups[l]))
                        {
                            QString failMes = "The Output Signal, \'" + outputSignal + "\', has 2 Commands using Light Controller " + QString::number(cntlrNumber) + " with Group " + QString::number(newGroups[l]);
                            emit ShowErrorMessage(title, failMes);
                            return false;
                        }
                    }
                }
            }
        }
    }

    //Ran the Gauntlet
    return true;
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
    else if(command == BACKGROUNDREG)
        return 253;
    else if(command == DISPLAYRANGEALEDS)
        return 251;
    else if(command == FLASHALEDS)
        return FLASHALEDSARGS;
    else if(command == RELOADFLASHALEDS)
        return RELOADFLASHALEDSARGS;
    else if(command == DEATHFLASHALEDS)
        return DEATHFLASHALEDSARGS;
    else if(command == RANDOMFLASHALEDS)
        return 252;
    else if(command == SEQUENCEALEDS)
        return SEQUENCEALEDSARGS;
    else if(command == RELOADSEQUENCEALEDS)
        return RELOADSEQUENCEALEDSARGS;
    else
        return 255;
}

void HookLight::GameEnded()
{
    if(inGame)
    {
        quint8 i;

        if(isFileLoaded)
        {
            ProcessSignal(MAMESTOPFRONT, "1");
            isFileLoaded = false;
        }

        //Set Counts back to Zero
        for(i = 0; i < numberLightCntlrs; i++)
        {
            p_comDeviceList->p_lightCntlrList[i]->ResetLightController();
            p_comDeviceList->p_lightCntlrList[i]->GameEnded();
            flashCount[i] = 0;
            rndFlashCount[i] = 0;
            sequentialCount[i] = 0;
        }

        //Set Player Values back to 0
        for(i = 0; i < MAXGAMEPLAYERS; i++)
        {
            ammoValue[i] = 0;
            lifeValue[i] = 0;
            playerAlive[i] = false;
            maxLifeValue[i] = 0;
            maxDamage[i] = 0;
        }

        //Game Ended
        inGame = false;
    }
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
        {
            //Now in Game
            inGame = true;

            //Tell Light Controllers Game Started
            for(quint8 i = 0; i < numberLightCntlrs; i++)
                p_comDeviceList->p_lightCntlrList[i]->GameStarted();

            ProcessSignal(MAMESTARTAFTER, "1");
        }
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
            {
                //Now in Game
                inGame = true;

                //Tell Light Controllers Game Started
                for(quint8 i = 0; i < numberLightCntlrs; i++)
                    p_comDeviceList->p_lightCntlrList[i]->GameStarted();

                ProcessSignal(MAMESTARTAFTER, "1");
            }
        }
    }
}


void HookLight::GameStopped()
{
    GameEnded();
}

void HookLight::TCPSocketDisconnected()
{
    GameEnded();
}

void HookLight::ProcessSignal(const QString &signal, const QString &data)
{
    //Process Filtered Output Signals From TCP Socket

    //qDebug() << "Process Signals for Light - signal:" << signal << "data:" << data;

    bool isNumber;
    quint16 dataNumber = data.toUInt(&isNumber);

    //For Ammo and Life values
    bool updateAmmo = false;
    QList<quint8> playerAmmo;
    bool updateLife = false;
    QList<quint8> playerLife;
    bool updatePlayerAlive = false;
    QList<quint8> playerSpawn;
    bool updatePlayerDead = false;
    QList<quint8> playerDead;


    if(!isNumber)
        dataNumber = 0;

    //Get List of Commands
    QList<LightCommand> lightCmdList = signalLightCommand[signal];

    for(quint8 k = 0; k < lightCmdList.count(); k++)
    {
        //Now Get Light Command
        LightCommand lightCmd = lightCmdList[k];
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
                        //ammoValue[playerNum] = dataNumber;
                        updateAmmo = true;
                        playerAmmo << playerNum;
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
                                //playerAlive[playerNum] = true;
                                updatePlayerAlive = true;
                                playerSpawn << playerNum;
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
                                    //playerAlive[playerNum] = false;
                                    updatePlayerDead = true;
                                    playerDead << playerNum;

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
                        //lifeValue[playerNum] = dataNumber;
                        updateLife = true;
                        playerLife << playerNum;
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
                    //ammoValue[playerNum] = dataNumber;
                    updateAmmo = true;
                    playerAmmo << playerNum;
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
                    //ammoValue[playerNum] = dataNumber;
                    updateAmmo = true;
                    playerAmmo << playerNum;
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
                        //ammoValue[playerNum] = dataNumber;
                        updateAmmo = true;
                        playerAmmo << playerNum;
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
                                //playerAlive[playerNum] = true;
                                updatePlayerAlive = true;
                                playerSpawn << playerNum;
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
                                    //playerAlive[playerNum] = false;
                                    updatePlayerDead = true;
                                    playerDead << playerNum;

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
                        //lifeValue[playerNum] = dataNumber;
                        updateLife = true;
                        playerLife << playerNum;
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
                    //ammoValue[playerNum] = dataNumber;
                    updateAmmo = true;
                    playerAmmo << playerNum;
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
        else if(kindOfCommand == BACKGROUNDCOMMAND)  //Background Commands
        {
            //For Both RGB and Regular
            if(commandNumber == BACKGROUNDRGBCMD || commandNumber == BACKGROUNDREGCMD)
            {
                quint8 playerNum = lightCmd.GetPlayerNumber();

                for(i = 0; i < numberCntlrs; i++)
                {
                    cntlrNumber = lightCmd.GetControllerNumber(i);
                    QList<quint8> groups = lightCmd.GetControllerGroups(cntlrNumber);

                    if(commandNumber == BACKGROUNDRGBCMD)
                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->BackgroundRGB(playerNum, dataNumber);
                    else
                        p_comDeviceList->p_lightCntlrList[cntlrNumber]->BackgroundRegular(playerNum, dataNumber);
                }
            }

        }
        else if(kindOfCommand > ALEDSTRIP)
        {
            if(kindOfCommand == ALEDSDISPLAYRANGE)
            {
                for(i = 0; i < numberCntlrs; i++)
                {
                    cntlrNumber = lightCmd.GetControllerNumber(i);
                    QList<quint8> strips = lightCmd.GetControllerGroups(cntlrNumber);

                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->UpdateDisplayRange(strips, dataNumber);
                }
            }
            else if(kindOfCommand == ALEDSFLASHCMD)
            {
                bool stripFlashWait = lightCmd.GetStripFlashWait ();

                if(commandNumber == FLASHALEDSCMD && dataNumber != 0)
                {
                    //Flash ALED Strip Command
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> structNumbers = lightCmd.GetStructNumber(cntlrNumber);

                        for(quint8 j = 0; j < structNumbers.count(); j++)
                        {
                            if(stripFlashWait)
                                p_comDeviceList->p_lightCntlrList[cntlrNumber]->DoStripFlashWait(structNumbers[j]);
                            else
                                p_comDeviceList->p_lightCntlrList[cntlrNumber]->DoStripFlash(structNumbers[j]);
                        }
                    }
                }
                else if(commandNumber == RELOADFLASHALEDSCMD)
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
                            QList<quint8> structNumbers = lightCmd.GetStructNumber(cntlrNumber);

                            for(quint8 j = 0; j < structNumbers.count(); j++)
                            {
                                if(stripFlashWait)
                                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->DoStripFlashWait(structNumbers[j]);
                                else
                                    p_comDeviceList->p_lightCntlrList[cntlrNumber]->DoStripFlash(structNumbers[j]);
                            }
                        }
                    }

                    //Make New Ammo the Old Ammo Value
                    //ammoValue[playerNum] = dataNumber;
                    updateAmmo = true;
                    playerAmmo << playerNum;
                }
                else if(commandNumber == DEATHFLASHALEDSCMD)
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
                            //playerAlive[playerNum] = true;
                            updatePlayerAlive = true;
                            playerSpawn << playerNum;
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
                                //qDebug() << "Player Died";

                                //playerAlive[playerNum] = false;
                                updatePlayerDead = true;
                                playerDead << playerNum;

                                //Death Happened, Show Lights
                                for(i = 0; i < numberCntlrs; i++)
                                {
                                    cntlrNumber = lightCmd.GetControllerNumber(i);
                                    QList<quint8> structNumbers = lightCmd.GetStructNumber(cntlrNumber);

                                    //qDebug() << "structNumbers" << structNumbers;

                                    for(quint8 j = 0; j < structNumbers.count(); j++)
                                    {
                                        if(stripFlashWait)
                                            p_comDeviceList->p_lightCntlrList[cntlrNumber]->DoStripFlashWait(structNumbers[j]);
                                        else
                                            p_comDeviceList->p_lightCntlrList[cntlrNumber]->DoStripFlash(structNumbers[j]);
                                    }
                                }
                            }
                        }
                    }

                    //Make New Life the Old Life Value
                    //lifeValue[playerNum] = dataNumber;
                    updateLife = true;
                    playerLife << playerNum;
                }
            }
            else if(kindOfCommand == ALEDSRNDFLASH)
            {
                if(dataNumber != 0)
                {
                    //Random Flash RGB Command
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> structNumbers = lightCmd.GetStructNumber(cntlrNumber);

                        for(quint8 j = 0; j < structNumbers.count(); j++)
                            p_comDeviceList->p_lightCntlrList[cntlrNumber]->DoStripRndFlash(structNumbers[j]);
                    }
                }
            }
            else if(kindOfCommand == ALEDSSEQUENCECMD)
            {
                if(commandNumber == SEQUENCEALEDSCMD && dataNumber != 0)
                {
                    //Sequential ALED Strip Command
                    for(i = 0; i < numberCntlrs; i++)
                    {
                        cntlrNumber = lightCmd.GetControllerNumber(i);
                        QList<quint8> structNumbers = lightCmd.GetStructNumber(cntlrNumber);

                        for(quint8 j = 0; j < structNumbers.count(); j++)
                            p_comDeviceList->p_lightCntlrList[cntlrNumber]->DoStripSequential(structNumbers[j]);
                    }
                }
                else if(commandNumber == RELOADSEQUENCEALEDSCMD)
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
                            QList<quint8> structNumbers = lightCmd.GetStructNumber(cntlrNumber);

                            for(quint8 j = 0; j < structNumbers.count(); j++)
                                p_comDeviceList->p_lightCntlrList[cntlrNumber]->DoStripSequential(structNumbers[j]);
                        }
                    }

                    //Make New Ammo the Old Ammo Value
                    //ammoValue[playerNum] = dataNumber;
                    updateAmmo = true;
                    playerAmmo << playerNum;
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

    if(updateAmmo)
    {
        for(quint8 i = 0; i < playerAmmo.count(); i++)
            ammoValue[playerAmmo[i]] = dataNumber;
    }

    if(updateLife)
    {
        for(quint8 i = 0; i < playerLife.count(); i++)
            lifeValue[playerLife[i]] = dataNumber;
    }

    if(updatePlayerAlive)
    {
        for(quint8 i = 0; i < playerSpawn.count(); i++)
            playerAlive[playerSpawn[i]] = true;
    }

    if(updatePlayerDead)
    {
        for(quint8 i = 0; i < playerDead.count(); i++)
            playerAlive[playerDead[i]] = false;
    }

}


void HookLight::ErrorMessage(const QString &title, const QString &message)
{
    //Goes Up the Chain to Main Thread
    emit ShowErrorMessage(title, message);
}












