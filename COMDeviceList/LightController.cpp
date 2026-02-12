#include "LightController.h"


LightController::LightController(quint8 cntlrNum, LightController const &other, QObject *parent)
    : QObject{parent}
{
    lightCntlrNum = cntlrNum;

    dataUltimarc = other.dataUltimarc;

    id = other.id;

    lightCntlrMaker = ULTIMARC;

    numberLEDs = ULTIMARCTYPELEDCOUNT[dataUltimarc.type];

    numberRGBLEDs = ULTIMARCTYPERGBLEDCOUNT[dataUltimarc.type];

    maxBrightness = ULTIMARCTYPEBRIGHTNESS[dataUltimarc.type];

    groupFilePath = other.groupFilePath;

    regularLEDMap = other.regularLEDMap;

    rgbLEDMap = other.rgbLEDMap;

    rgbFastMode = other.rgbFastMode;

    noRegularGroups = other.noRegularGroups;

    noRGBGroups = other.noRGBGroups;

    rgbColorMapMap = other.rgbColorMapMap;

    didGroupFileLoad = false;

    defaultBrightness = 255;

    isBackground = false;

    title = "Light Controller Group File Error";

    rgbOff.r = 0;
    rgbOff.g = 0;
    rgbOff.b = 0;

    doReset = false;

    numberPins = ULTIMARCTYPELEDCOUNT[dataUltimarc.type];

    if(dataUltimarc.type == IPACULTIMATEIO)
        numberGroups = ULTIMATEGRPS;
    else
        numberGroups = OTHERGRPS;

    p_usedPins = new quint8[numberGroups];

    quint8 i;

    for(i = 0; i < numberGroups; i++)
        p_usedPins[i] = 0;

    for(i = 0; i < MAXGAMEPLAYERS; i++)
        backgroundActive[i] = false;

    //Load the Group File
    LoadGroupFile();

    if(didGroupFileLoad)
    {
        //Build Group Data for Regular and RGB
        BuildRegularGroupData();
        BuildRGBGroupData();

        //PrintGroupData();
    }

}




LightController::LightController(quint8 cntlrNum, UltimarcData dataU, QObject *parent)
    : QObject{parent}
{
    initDone = false;

    lightCntlrNum = cntlrNum;

    dataUltimarc = dataU;

    id = dataUltimarc.id;

    //qDebug() << "Light Controller ID:" << id;

    lightCntlrMaker = ULTIMARC;

    numberLEDs = ULTIMARCTYPELEDCOUNT[dataUltimarc.type];

    numberRGBLEDs = ULTIMARCTYPERGBLEDCOUNT[dataUltimarc.type];

    maxBrightness = ULTIMARCTYPEBRIGHTNESS[dataUltimarc.type];

    groupFilePath = dataUltimarc.groupFile;

    rgbFastMode = false;

    noRegularGroups = false;

    noRGBGroups = false;

    didGroupFileLoad = false;

    defaultBrightness = 255;

    isBackground = false;

    title = "Light Controller Group File Error";
    titleGF = "Light Controller Game File Error";
    titleC = "Light Controller Color Error";

    rgbOff.r = 0;
    rgbOff.g = 0;
    rgbOff.b = 0;

    doReset = false;

    numberPins = ULTIMARCTYPELEDCOUNT[dataUltimarc.type];

    if(dataUltimarc.type == IPACULTIMATEIO)
        numberGroups = ULTIMATEGRPS;
    else
        numberGroups = OTHERGRPS;


    p_usedPins = new quint8[numberGroups];

    quint16 i;

    for(i = 0; i < numberGroups; i++)
        p_usedPins[i] = 0;

    for(i = 0; i < NUMBEREXECUTIONS; i++)
        p_execution[i] = nullptr;

    for(i = 0; i < MAXGAMEPLAYERS; i++)
        backgroundActive[i] = false;

    executionCount = 0;

    //Load the Group File
    LoadGroupFile();

    if(didGroupFileLoad)
    {
        //qDebug() << "Creating Group Pin Data";

        //Build Group Data for Regular and RGB
        BuildRegularGroupData();
        BuildRGBGroupData();

        //PrintGroupData();
    }

    initDone = true;
}


//Deconstructor

LightController::~LightController()
{
    quint16 i;

    //Light Execution Classes
    for(i = 0; i < NUMBEREXECUTIONS; i++)
    {
        if(p_execution[i] != nullptr)
        {
            delete p_execution[i];
            p_execution[i] = nullptr;
        }
    }

    //Used Pins Array
    delete[] p_usedPins;

    TurnOffLights();
    ResetLightController();
}


//Copy Light Controller
void LightController::CopyLightController(LightController const &lcMember)
{
    dataUltimarc = lcMember.dataUltimarc;

    id = lcMember.id;

    lightCntlrMaker = ULTIMARC;

    numberLEDs = ULTIMARCTYPELEDCOUNT[dataUltimarc.type];

    numberRGBLEDs = ULTIMARCTYPERGBLEDCOUNT[dataUltimarc.type];

    maxBrightness = ULTIMARCTYPEBRIGHTNESS[dataUltimarc.type];

    groupFilePath = dataUltimarc.groupFile;

    regularLEDMap = lcMember.regularLEDMap;

    rgbLEDMap = lcMember.rgbLEDMap;

    rgbColorMapMap = lcMember.rgbColorMapMap;

    rgbFastMode = false;

    noRegularGroups = false;

    noRGBGroups = false;

    didGroupFileLoad = false;

    defaultBrightness = lcMember.defaultBrightness;

    title = "Light Controller Group File Error";

    rgbOff.r = 0;
    rgbOff.g = 0;
    rgbOff.b = 0;

    doReset = false;

    numberPins = lcMember.numberPins;

    if(numberGroups != lcMember.numberGroups)
    {
        delete[] p_usedPins;
        p_usedPins = nullptr;

        numberGroups = lcMember.numberGroups;

        p_usedPins = new quint8[numberGroups];
    }

    quint8 i;

    for(i = 0; i < numberGroups; i++)
        p_usedPins[i] = 0;


    //Load the Group File
    LoadGroupFile();

    if(didGroupFileLoad)
    {
        //Build Group Data for Regular and RGB
        BuildRegularGroupData();
        BuildRGBGroupData();

        //PrintGroupData();
    }
}

bool LightController::SetGroupFile(QString filePath)
{
    groupFilePath = filePath;
    dataUltimarc.groupFile = filePath;

    rgbFastMode = false;

    noRegularGroups = false;

    noRGBGroups = false;

    didGroupFileLoad = false;

    //Load the Group File
    LoadGroupFile();

    if(didGroupFileLoad)
    {
        //Build Group Data for Regular and RGB
        BuildRegularGroupData();
        BuildRGBGroupData();
    }
    else
        return didGroupFileLoad;

    if(!noRegularGroups && didGroupFileLoad)
    {
        quint8 i;

        p_pinsState = new quint8[numberGroups];

        for(i = 0; i < numberGroups; i++)
            p_pinsState[i] = 0xFF;

        if(dataUltimarc.type == NANOLED)
            p_pinsState[numberGroups-1] = 0x0F;

        //Conver Regular Lights from Intensity to State
        ConvertRegularToState();
    }

    return didGroupFileLoad;
}

bool LightController::ReloadGroupFile()
{
    rgbFastMode = false;

    noRegularGroups = false;

    noRGBGroups = false;

    didGroupFileLoad = false;

    //Load the Group File
    LoadGroupFile();

    if(didGroupFileLoad)
    {
        //Build Group Data for Regular and RGB
        BuildRegularGroupData();
        BuildRGBGroupData();
    }
    else
        return didGroupFileLoad;


    if(!noRegularGroups && didGroupFileLoad)
    {
        quint8 i;

        p_pinsState = new quint8[numberGroups];

        for(i = 0; i < numberGroups; i++)
            p_pinsState[i] = 0xFF;

        if(dataUltimarc.type == NANOLED)
            p_pinsState[numberGroups-1] = 0x0F;

        //Conver Regular Lights from Intensity to State
        ConvertRegularToState();
    }

    return didGroupFileLoad;
}



bool LightController::IsRegularGroups()
{
    if(noRegularGroups)
        return false;
    else
        return true;
}

bool LightController::IsRGBGroups()
{
    if(noRGBGroups)
        return false;
    else
        return true;
}



void LightController::LoadGroupFile()
{
    bool openFile;
    QString line;
    QFile loadGroupData(groupFilePath);
    QStringList fileData;
    quint16 lineNumber = 0;
    quint16 lineCount;
    bool didGroupLoad = true;
    bool didColorLoad = true;
    bool groupLoading = true;
    bool loadedRGBNormal = false;
    bool gotData = false;
    bool didColorMapLoad = true;
    bool didDefaultBrightLoad = true;
    quint8 defaultBrightCount = 0;

    //Clear Out Maps and Lists
    regularLEDMap.clear();
    rgbLEDMap.clear();
    //regularPinsCount.clear();
    //rgbPinsCount.clear();
    //regularBrightnessMap.clear();
    //rgbBrightnessMap.clear();
    regularLEDGroupList.clear();
    rgbLEDGroupList.clear();
    rgbColorMap.clear();
    usedPinsList.clear();
    colorNameList.clear();
    numberColors = 0;
    rgbColorMapMap.clear();

    if(loadGroupData.exists() == false)
    {
        QString message = "The light controller group file doesn't exisits. Please close program and solve file problem.";
        emit ShowErrorMessage(title, message);
        didGroupFileLoad = false;
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    openFile = loadGroupData.open (QIODeviceBase::ReadOnly | QIODevice::Text);
#else
    openFile = loadGroupData.open (QIODevice::ReadOnly | QIODevice::Text);
#endif

    if(!openFile)
    {
        QString message = "Can not open light controller group data file to read. Please close program and solve file problem. Might be permissions problem.";
        emit ShowErrorMessage(title, message);
        didGroupFileLoad = false;
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&loadGroupData);

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
    loadGroupData.close ();

    lineCount = fileData.length ();

    while(lineNumber < lineCount && didGroupLoad && groupLoading && didColorLoad && didColorMapLoad && didDefaultBrightLoad)
    {
        if(fileData[lineNumber].startsWith(RGBFASTGROUP))
        {
            if(loadedRGBNormal)
            {
                QString tempCrit = "Cannot mix GRP_RGB and GRP_RGB_FAST.\nFile: "+groupFilePath;
                emit ShowErrorMessage(title, tempCrit);
                groupLoading = false;
            }
            else
            {
                //Set RGB Fast Mode
                rgbFastMode = true;

                gotData = true;

                didGroupLoad = LoadGroupRGBFast(lineNumber, lineCount, fileData);
            }
        }
        else if(fileData[lineNumber].startsWith(RGBGROUP))
        {
            if(rgbFastMode)
            {
                QString tempCrit = "Cannot mix GRP_RGB and GRP_RGB_FAST.\nFile: "+groupFilePath;
                emit ShowErrorMessage(title, tempCrit);
                groupLoading = false;
            }
            else
            {
                //Set RGB Normal Mode
                loadedRGBNormal = true;

                gotData = true;

                didGroupLoad = LoadGroupRGB(lineNumber, lineCount, fileData);
            }
        }
        else if(fileData[lineNumber].startsWith(REGULARGROUP))
        {
            gotData = true;
            didGroupLoad = LoadGroupRegular(lineNumber, lineCount, fileData);
        }
        else if(fileData[lineNumber].startsWith(COLORMAP))
        {
            didColorMapLoad = LoadRGBColorMap(fileData[lineNumber]);
        }
        else if(fileData[lineNumber].startsWith(RGBCOLOR))
        {
            didColorLoad = LoadRGBColor(fileData[lineNumber]);
        }
        else if(fileData[lineNumber].startsWith(DEFAULTBRIGHTNESS))
        {
            if(defaultBrightCount == 0)
            {
                didDefaultBrightLoad = LoadDefaultBrightness(fileData[lineNumber]);
                defaultBrightCount++;
            }
            else
            {
                didDefaultBrightLoad = false;
                QString tempCrit = "Default brightness already loaded. Please remove extra default brightness statements.\nLine: "+fileData[lineNumber]+"\nFile: "+groupFilePath;
                emit ShowErrorMessage(title, tempCrit);
            }
        }

        lineNumber++;
    }

    if(!didGroupLoad || !groupLoading)
    {
        QString tempCrit = "Pin group failed to load. Please correct the group file for this light controller.\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        didGroupFileLoad = false;
    }
    else if(!didColorLoad)
    {
        QString tempCrit = "A RGB color failed to load. Please correct the group file for this light controller.\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        didGroupFileLoad = false;
    }
    else if(!gotData)
    {
        QString tempCrit = "No group data was loaded. Please correct the group file for this light controller.\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        didGroupFileLoad = false;
    }
    else if(usedPinsList.count() > numberLEDs)
    {
        QString tempCrit = "Too many pins were loaded into the Light Controller. Please correct the group file for this light controller.\nUsed Pins: "+QString::number(usedPinsList.count())+"File: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        didGroupFileLoad = false;
    }
    else if(!didColorMapLoad)
    {
        didGroupFileLoad = false;
    }
    else if(!didDefaultBrightLoad)
    {
        didGroupFileLoad = false;
    }
    else
        didGroupFileLoad = true;

    if(regularLEDGroupList.isEmpty ())
        noRegularGroups = true;

    if(rgbLEDGroupList.isEmpty ())
        noRGBGroups = true;

    //No Group Data Loaded
    if(noRegularGroups && noRGBGroups && gotData)
    {
        QString tempCrit = "No group data was loaded. Please correct the group file for this light controller.\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        didGroupFileLoad = false;
    }
}



bool LightController::LoadGroupRGBFast(quint16 lineNum, quint16 lineMax, QStringList fileData)
{
    quint16 lineNumber = lineNum;
    quint16 grpStart = lineNum + 1;
    quint16 grpEnd = lineNum + 2;

    //qDebug() << "GRP_RGB_FAST found";

    LightControllerTop groupNumberBright = GetGroupNumber(2, lineNum, fileData[lineNumber]);

    if(!groupNumberBright.isDataSet)
        return false;

    LCPinData pinDataSruct = GetGroupPinData(2, grpStart, grpEnd, lineMax, fileData);

    if(!pinDataSruct.isDataSet)
        return false;

    //qDebug() << "Number of Pins" << pinDataSruct.numberPins << "Pin List" << pinDataSruct.pinData;

    //Check if Group Already Exists
    if(rgbLEDGroupList.contains(groupNumberBright.groupNumber))
    {
        QString tempCrit = "The group number already exists in the RGB group(s).\nGroup Number: "+QString::number(groupNumberBright.groupNumber)+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    //Add Group Number to RGB Group List
    rgbLEDGroupList << groupNumberBright.groupNumber;

    //Add Default Brightness to Group Map
    //rgbBrightnessMap.insert(groupNumberBright.groupNumber, groupNumberBright.defaultBrightness);

    //Add Group with its pins to the Map
    rgbLEDMap.insert(groupNumberBright.groupNumber, pinDataSruct.pinData);

    //Map Group Number with RGB Pin Count
    //rgbPinsCount.insert(groupNumberBright.groupNumber, pinDataSruct.numberPins);

    return true;
}



bool LightController::LoadGroupRGB(quint16 lineNum, quint16 lineMax, QStringList fileData)
{
    quint16 lineNumber = lineNum;
    quint16 grpStart = lineNum + 1;
    quint16 grpEnd = lineNum + 2;

    //qDebug() << "GRP_RGB found";

    LightControllerTop groupNumberBright = GetGroupNumber(1, lineNum, fileData[lineNumber]);

    if(!groupNumberBright.isDataSet)
        return false;

    //qDebug() << "Group Number" << groupNumberBright.groupNumber; // << "Default Brightness" << groupNumberBright.defaultBrightness;

    LCPinData pinDataSruct = GetGroupPinData(1, grpStart, grpEnd, lineMax, fileData);

    if(!pinDataSruct.isDataSet)
        return false;

    //qDebug() << "Number of Pins" << pinDataSruct.numberPins << "Pin List" << pinDataSruct.pinData;

    //Check if Group Already Exists
    if(rgbLEDGroupList.contains(groupNumberBright.groupNumber))
    {
        QString tempCrit = "The group number already exists in the RGB group(s).\nGroup Number: "+QString::number(groupNumberBright.groupNumber)+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    //Add Group Number to RGB Group List
    rgbLEDGroupList << groupNumberBright.groupNumber;

    //Add Default Brightness to Group Map
    //rgbBrightnessMap.insert(groupNumberBright.groupNumber, groupNumberBright.defaultBrightness);

    //Add Group with its pins to the Map
    rgbLEDMap.insert(groupNumberBright.groupNumber, pinDataSruct.pinData);

    //Map Group Number with RGB Pin Count
    //rgbPinsCount.insert(groupNumberBright.groupNumber, pinDataSruct.numberPins);

    return true;
}


bool LightController::LoadGroupRegular(quint16 lineNum, quint16 lineMax, QStringList fileData)
{
    quint16 lineNumber = lineNum;
    quint16 grpStart = lineNum + 1;
    quint16 grpEnd = lineNum + 2;

    //qDebug() << "GRP found";

    LightControllerTop groupNumberBright = GetGroupNumber(0, lineNum, fileData[lineNumber]);

    if(!groupNumberBright.isDataSet)
        return false;

    //qDebug() << "Group Number" << groupNumberBright.groupNumber << "Default Brightness" << groupNumberBright.defaultBrightness;

    LCPinData pinDataSruct = GetGroupPinData(0, grpStart, grpEnd, lineMax, fileData);

    if(!pinDataSruct.isDataSet)
        return false;

    //qDebug() << "Number of Pins" << pinDataSruct.numberPins << "Pin List" << pinDataSruct.pinData;

    //Check if Group Already Exists in Regular LEDs
    if(regularLEDGroupList.contains(groupNumberBright.groupNumber))
    {
        QString tempCrit = "The group number already exists in the regular group(s).\nGroup Number: "+QString::number(groupNumberBright.groupNumber)+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    //Add Group Number to Regular Group List
    regularLEDGroupList << groupNumberBright.groupNumber;

    //Add Default Brightness to Group Map
    //regularBrightnessMap.insert(groupNumberBright.groupNumber, groupNumberBright.defaultBrightness);

    //Add Group with its pins to the Map
    regularLEDMap.insert(groupNumberBright.groupNumber, pinDataSruct.pinData);

    //Map Group Number with Regular Pin Count
    //regularPinsCount.insert(groupNumberBright.groupNumber, pinDataSruct.numberPins);

    return true;
}


LightControllerTop LightController::GetGroupNumber(quint8 groupMode, quint16 lineNum, QString lineData)
{
    bool isNumber;
    qint16 groupNumber;
    //qint16 defaultBrightness;
    quint16 lineNumber = lineNum;
    QString groupKind;
    LightControllerTop groupNumberBright;

    if(groupMode == 0)
        groupKind = REGULARGROUP;
    else if(groupMode == 1)
        groupKind = RGBGROUP;
    else if(groupMode == 2)
        groupKind = RGBFASTGROUP;

    //Split Up Line Data with a Space
    QStringList splitData = lineData.split(' ', Qt::SkipEmptyParts);

    if(splitData[0] != groupKind)
    {
        QString tempCrit = "The group kind is not matching the 3 known groups.\nLine Number: "+QString::number(lineNumber)+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        groupNumberBright.isDataSet = false;
        return groupNumberBright;
    }

    groupNumber = splitData[1].toInt(&isNumber);

    if(!isNumber)
    {
        QString tempCrit = "The group number, is not a number.\nLine Number: "+QString::number(lineNumber)+"\nGroup Number "+splitData[1]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        groupNumberBright.isDataSet = false;
        return groupNumberBright;
    }

    if(groupNumber < 0 || groupNumber > 255)
    {
        QString tempCrit = "The group number, needs to be set from 0 to 255.\nLine Number: "+QString::number(lineNumber)+"\nGroup Number "+splitData[1]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        groupNumberBright.isDataSet = false;
        return groupNumberBright;
    }


    //defaultBrightness = DEFAULTBRIGHTNESS;

    /*
    defaultBrightness = splitData[2].toInt(&isNumber);

    if(!isNumber)
    {
        QString tempCrit = "The group number default brightness, is not a number.\nLine Number: "+QString::number(lineNumber)+"\nDefault Brightness "+splitData[2]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        groupNumberBright.isDataSet = false;
        return groupNumberBright;
    }

    if(defaultBrightness < 0 || defaultBrightness > 255)
    {
        QString tempCrit = "The group number default brightness, needs to be set from 0 to 255.\nLine Number: "+QString::number(lineNumber)+"\nDefault Brightness "+splitData[2]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        groupNumberBright.isDataSet = false;
        return groupNumberBright;
    }
    */

    groupNumberBright.groupNumber = static_cast<quint8>(groupNumber);
    //groupNumberBright.defaultBrightness = static_cast<quint8>(defaultBrightness);
    groupNumberBright.isDataSet = true;
    return groupNumberBright;
}


LCPinData LightController::GetGroupPinData(quint8 lcKind, quint16 grpStart, quint16 grpEnd, quint16 lineMax, QStringList fileData)
{
    bool isNumber;
    QList<quint8> grpData;
    quint16 i, j;
    quint8 addedPins = 0;
    LCPinData pinDataStruct;
    quint16 numberLEDsCheck;
    quint16 groupEnd = grpEnd;


    if(lcKind == 2)
        numberLEDsCheck = numberLEDs-2;
    else
        numberLEDsCheck = numberLEDs;


    //Find Where the Group Ends, If new line Starts with GRP, Color, or END_GRP. Then Found End
    while(groupEnd < lineMax && (!fileData[groupEnd].startsWith(REGULARGROUP) && !fileData[groupEnd].startsWith(RGBCOLOR) && !fileData[groupEnd].startsWith(ENDGROUP)))
    {
        groupEnd++;
    }


    //qDebug() << "grpStart:" << grpStart << "groupEnd:" << groupEnd;

    //Now Collect the Pin Numbers in the Group Line(s)
    for(i = grpStart; i < groupEnd; i++)
    {

        //qDebug() << fileData[i];

        QStringList splitGrpData = fileData[i].split(' ', Qt::SkipEmptyParts);

        for(j = 0; j < splitGrpData.length(); j++)
        {
            //Trim things up
            splitGrpData[j] = splitGrpData[j].trimmed();

            qint16 pinData = splitGrpData[j].toInt(&isNumber);

            if(!isNumber)
            {
                QString tempCrit = "The pin number, is not a number.\nPin Number "+splitGrpData[j]+"\nFile: "+groupFilePath;
                emit ShowErrorMessage(title, tempCrit);
                pinDataStruct.isDataSet = false;
                return pinDataStruct;
            }

            if(pinData < 1 || pinData > numberLEDsCheck)
            {
                //qDebug() << "Pin Data:" << QString::number(pinData) << "numberLEDsCheck" << QString::number(numberLEDsCheck);
                QString tempCrit = "The pin number is out of range.\nPin Number "+splitGrpData[j]+"\nFile: "+groupFilePath;
                emit ShowErrorMessage(title, tempCrit);
                pinDataStruct.isDataSet = false;
                return pinDataStruct;
            }

            quint8 pin = static_cast<quint8>(pinData);

            if(lcKind == 2)
            {
                if(grpData.contains(pin) || grpData.contains(pin+1) || grpData.contains(pin+2))
                {
                    QString tempCrit = "The pin number (+0,+1,+2) is already in the group.\nPin Number "+splitGrpData[j]+"\nFile: "+groupFilePath;
                    emit ShowErrorMessage(title, tempCrit);
                    pinDataStruct.isDataSet = false;
                    return pinDataStruct;
                }
            }
            else
            {
                if(grpData.contains(pin))
                {
                    QString tempCrit = "The pin number is already in the group.\nPin Number "+splitGrpData[j]+"\nFile: "+groupFilePath;
                    emit ShowErrorMessage(title, tempCrit);
                    pinDataStruct.isDataSet = false;
                    return pinDataStruct;
                }
            }

            //Check if Pin is in other Light Group. If Regular, then Check RGB
            if(lcKind == 2 || lcKind == 1)
            {
                //RGB Fast, Check Regular Group
                if(regularLEDGroupList.count() > 0)
                {
                    quint8 i;

                    for(i = 0; i < regularLEDGroupList.count(); i++)
                    {
                        QList<quint8> pins = regularLEDMap[regularLEDGroupList[i]];

                        if(lcKind == 2)
                        {
                            if(pins.contains(pin) || pins.contains(pin+1) || pins.contains(pin+2))
                            {
                                QString tempCrit = "The pin number (+0,+1,+2) is already used in a regular group.\nPin Number: "+splitGrpData[j]+"\nRegular Group: "+QString::number(regularLEDGroupList[i])+"\nFile: "+groupFilePath;
                                emit ShowErrorMessage(title, tempCrit);
                                pinDataStruct.isDataSet = false;
                                return pinDataStruct;
                            }
                        }
                        else
                        {
                            if(pins.contains(pin))
                            {
                                QString tempCrit = "The pin number is already used in a regular group.\nPin Number: "+splitGrpData[j]+"\nRegular Group: "+QString::number(regularLEDGroupList[i])+"\nFile: "+groupFilePath;
                                emit ShowErrorMessage(title, tempCrit);
                                pinDataStruct.isDataSet = false;
                                return pinDataStruct;
                            }
                        }
                    }
                }
            }
            else if(lcKind == 0)
            {
                //Regular, Check RGB Group
                if(rgbLEDGroupList.count() > 0)
                {
                    quint8 i;

                    for(i = 0; i < rgbLEDGroupList.count(); i++)
                    {
                        QList<quint8> pins = rgbLEDMap[rgbLEDGroupList[i]];

                        if(pins.contains(pin))
                        {
                            QString tempCrit = "The pin number is already used in a RGB group.\nPin Number: "+splitGrpData[j]+"\nRGB Group: "+QString::number(rgbLEDGroupList[i])+"\nFile: "+groupFilePath;
                            emit ShowErrorMessage(title, tempCrit);
                            pinDataStruct.isDataSet = false;
                            return pinDataStruct;
                        }
                    }
                }
            }

            grpData << pin;

            if(!usedPinsList.contains(pin))
            {
                usedPinsList << pin;

                //Check if it is RGB Fast
                if(lcKind == 2)
                {
                    usedPinsList << pin+1;
                    usedPinsList << pin+2;
                }
            }

            //Add RGB
            addedPins++;
        }
    }

    if(lcKind == 0)
    {
        if(addedPins > numberLEDs)
        {
            QString tempCrit = "Too many regular pins were added to the group than the light controller supports.\nNumber of Pins "+QString::number(addedPins)+"\nFile: "+groupFilePath;
            emit ShowErrorMessage(title, tempCrit);
            pinDataStruct.isDataSet = false;
            return pinDataStruct;
        }
    }
    else if(lcKind == 1)
    {
        if(addedPins > numberLEDs)
        {
            QString tempCrit = "Too many RGB pins were added to the group than the light controller supports.\nNumber of Pins "+QString::number(addedPins)+"\nFile: "+groupFilePath;
            emit ShowErrorMessage(title, tempCrit);
            pinDataStruct.isDataSet = false;
            return pinDataStruct;
        }

        if(addedPins%3 != 0)
        {
            QString tempCrit = "The pins in the RGB group need to be divided by 3.\nNumber of Pins "+QString::number(addedPins)+"\nFile: "+groupFilePath;
            emit ShowErrorMessage(title, tempCrit);
            pinDataStruct.isDataSet = false;
            return pinDataStruct;
        }
    }
    else if(lcKind == 2)
    {
        if(addedPins > numberRGBLEDs)
        {
            QString tempCrit = "Too many RGB Fast pins were added to the group than the light controller supports.\nNumber of Pins "+QString::number(addedPins)+"\nFile: "+groupFilePath;
            emit ShowErrorMessage(title, tempCrit);
            pinDataStruct.isDataSet = false;
            return pinDataStruct;
        }
    }

    //qDebug() << "Pin List:" << grpData;

    pinDataStruct.pinData = grpData;
    pinDataStruct.numberPins = addedPins;
    pinDataStruct.isDataSet = true;
    return pinDataStruct;
}


bool LightController::LoadRGBColor(QString line)
{
    bool isNumber;
    QString colorName;
    RGBColor tempColor;
    qint16 tempColorData;

    QStringList splitData = line.split(' ', Qt::SkipEmptyParts);

    if(splitData.length() != COLORSIZE)
    {
        QString tempCrit = "The RGB color has too much or too little data.\nColor Data: "+line+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    colorName = splitData[1];

    //qDebug() << "Loading Color:" << colorName;

    //Red 8bit Value
    tempColorData = splitData[2].toInt (&isNumber);

    if(!isNumber)
    {
        QString tempCrit = "The RGB color red value, is not a number.\nRed Color Data: "+splitData[2]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    if(tempColorData < 0 || tempColorData > 255)
    {
        QString tempCrit = "The RGB color red value needs to be set from 0 to 255.\nRed Color Data: "+splitData[2]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    tempColor.r = static_cast<quint8>(tempColorData);


    //Green 8bit Value
    tempColorData = splitData[3].toInt (&isNumber);

    if(!isNumber)
    {
        QString tempCrit = "The RGB color green value, is not a number.\nGreen Color Data: "+splitData[3]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    if(tempColorData < 0 || tempColorData > 255)
    {
        QString tempCrit = "The RGB color green value needs to be set from 0 to 255.\nGreen Color Data: "+splitData[3]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    tempColor.g = static_cast<quint8>(tempColorData);


    //Blue 8bit Value
    tempColorData = splitData[4].toInt (&isNumber);

    if(!isNumber)
    {
        QString tempCrit = "The RGB color blue value, is not a number.\nBlue Color Data: "+splitData[4]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    if(tempColorData < 0 || tempColorData > 255)
    {
        QString tempCrit = "The RGB color blue value needs to be set from 0 to 255.\nBlue Color Data: "+splitData[4]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    tempColor.b = static_cast<quint8>(tempColorData);

    rgbColorMap.insert(colorName, tempColor);

    colorNameList.insert(numberColors, colorName);

    numberColors++;

    //qDebug() << "Color:" << colorName << "R:" << tempColor.r << "G:" << tempColor.g << "B:" << tempColor.b;

    return true;
}

bool LightController::LoadRGBColorMap(QString line)
{
    quint8 i;
    QString colorName, colorMapName;
    QStringList colorNames;

    QStringList splitData = line.split(' ', Qt::SkipEmptyParts);

    if(splitData.length() < COLORMAPSIZE)
    {
        QString tempCrit = "The RGB color map needs more data or color names.\nColor Map Data: "+line+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    colorMapName = splitData[1];

    colorMapName = colorMapName.trimmed ();

    for(i = 2; i < splitData.length(); i++)
    {
        colorName = splitData[i];
        colorName = colorName.trimmed ();

        if(!colorNameList.contains(colorName))
        {
            QString tempCrit = "The RGB color doesn't exists in the color names list. Please add all colors, beforing doing color maps.\nColor Data: "+colorName+"\nFile: "+groupFilePath;
            emit ShowErrorMessage(title, tempCrit);
            return false;
        }
        else
            colorNames << colorName;
    }

    //Add Color Map to the RGB Color Map Map
    rgbColorMapMap.insert(colorMapName,colorNames);

    //qDebug() << "Loaded Color Map:" << colorMapName << "With these Colors:" << colorNames;

    //Ran the Gaunlet
    return true;
}

bool LightController::LoadDefaultBrightness(QString line)
{
    quint8 i;
    bool isNumber;
    quint16 tempDB;
    QString tempDBS;

    QStringList splitData = line.split(' ', Qt::SkipEmptyParts);

    if(splitData.length() != DEFAULTBRIGHTNESSSIZE)
    {
        QString tempCrit = "The default brightness has no number, or 2 or more numbers. Only 1 number after the definition.\nDefault Brightness Line: "+line+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    tempDBS = splitData[1];

    tempDBS = tempDBS.trimmed ();

    tempDB = tempDBS.toUInt(&isNumber);

    if(!isNumber)
    {
        QString tempCrit = "The default brightness is not a number. Please fix the group file.\nDefault Brightness: "+tempDBS+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    if(tempDB == 0 || tempDB > 255)
    {
        QString tempCrit = "The default brightness is out of range. It is 0, or greater than 255. Please fix the group file.\nDefault Brightness: "+tempDBS+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        return false;
    }

    //qDebug() << "Setting Default Brightness to" << tempDB;

    //Ran the Gaunlet
    defaultBrightness = tempDB;
    return true;
}



//Build Byte Arrays for Checking


//For Regular Groups

void LightController::BuildRegularGroupData()
{
    if(!noRegularGroups)
    {
        //i is for group, and j is for pin
        quint8 i, j;

        //Clear Out QMaps
        regularArrayPosition.clear();
        regularArrayData.clear();

        for(i = 0; i < regularLEDGroupList.count(); i++)
        {
            quint8 groupNum = regularLEDGroupList[i];
            QList<quint8> pins = regularLEDMap[groupNum];
            QList<quint8> arrayPosAll;
            QList<quint8> arrayData;

            //Zero Out the Data Array
            for(j = 0; j < numberGroups; j++)
                arrayData.insert(j,0);

            for(j = 0; j < pins.count(); j++)
            {
                quint8 pin = pins[j] - 1;

                quint8 arrayPos = (pin >> 3);
                quint8 pinPos = pin & 0x7;
                quint8 pinData = (1 << pinPos);

                if(!arrayPosAll.contains(arrayPos))
                    arrayPosAll << arrayPos;

                arrayData[arrayPos] = arrayData[arrayPos] | pinData;

            }

            //Now Sort the arrayPosAll List
            std::sort(arrayPosAll.begin(), arrayPosAll.end());

            //Insert the All Array Positions and Array Data
            regularArrayPosition.insert(groupNum,arrayPosAll);
            regularArrayData.insert(groupNum,arrayData);
        }
    }
}


//For RGB Groups

void LightController::BuildRGBGroupData()
{
    if(!noRGBGroups)
    {
        //i is for group, and j is for pin
        quint8 i, j;

        //Clear Out QMaps
        rgbArrayPosition.clear();
        rgbArrayData.clear();

        for(i = 0; i < rgbLEDGroupList.count(); i++)
        {
            quint8 groupNum = rgbLEDGroupList[i];
            QList<quint8> pins = rgbLEDMap[groupNum];
            QList<quint8> arrayPosAll;
            QList<quint8> arrayData;

            //Zero Out the Data Array with Number of Groups
            for(j = 0; j < numberGroups; j++)
                arrayData.insert(j,0);

            for(j = 0; j < pins.count(); j++)
            {
                quint8 pin = pins[j] - 1;

                quint8 arrayPos = (pin >> 3);
                quint8 pinPos = pin & 0x7;
                quint8 pinData = (1 << pinPos);
                bool nextArray1 = false;
                bool nextArray2 = false;

               //qDebug() << "arrayPos" << arrayPos << "pinPos" << pinPos << "pinData" << pinData << "arrayData" << arrayData[arrayPos] << arrayData;

                //If Using RGB Fast, then Add 2 Pins After
                if(rgbFastMode)
                {
                    //If Position is 0-5
                    if(pinPos < 6)
                    {
                        quint8 otherData = (0x3 << (pinPos+1));
                        pinData = pinData | otherData;
                    }
                    else if(pinPos == 6) //If 6
                    {
                        nextArray1 = true;
                        pinData = pinData | 128;
                    }
                    else if(pinPos == 7) //If 7
                        nextArray2 = true;
                }

                if(!arrayPosAll.contains(arrayPos))
                    arrayPosAll << arrayPos;

                arrayData[arrayPos] = arrayData[arrayPos] | pinData;


                //qDebug() << "arrayPos" << arrayPos << "pinPos" << pinPos << "pinData" << pinData << "arrayData" << arrayData[arrayPos] << nextArray1 << nextArray2;


                if(nextArray1 || nextArray2)
                {
                    quint8 otherData2;

                    if(nextArray1)
                        otherData2 = 1;
                    else
                        otherData2 = 3;

                    if(!arrayPosAll.contains(arrayPos+1))
                        arrayPosAll << arrayPos+1;

                    arrayData[arrayPos+1] = arrayData[arrayPos+1] | otherData2;
                }

            } //End of for j, pins data finished

            //qDebug() << "For Group:" << groupNum << "Position Data:" << arrayPosAll;

            //Now Sort the arrayPosAll List
            std::sort(arrayPosAll.begin(), arrayPosAll.end());

            //Insert the All Array Positions and Array Data
            rgbArrayPosition.insert(groupNum,arrayPosAll);
            rgbArrayData.insert(groupNum,arrayData);

            //qDebug() << "For Group:" << groupNum << "Position Data:" << arrayPosAll << "Data:" << arrayData;

        } //End of for i, RGB Groups finished
    } //End of No RGB Groups Check
}


void LightController::PrintGroupData()
{
    if(!noRGBGroups)
    {
        //i is for group, and j is for pin
        quint8 i, j;

        for(i = 0; i < rgbLEDGroupList.count(); i++)
        {
            quint8 groupNum = rgbLEDGroupList[i];

            QList<quint8> arrayPos = rgbArrayPosition[groupNum];
            QList<quint8> arrayData = rgbArrayData[groupNum];

            qDebug() << "For Group:" << groupNum << "Position Data:" << arrayPos;

            for(j = 0; j < arrayPos.count(); j++)
            {

                quint8 position = arrayPos[j];
                quint8 data = arrayData[position];
                QString binaryData = QString("%1").arg(data, 8, 2, QChar('0'));

                qDebug() << "Group:" << groupNum << "Position:" << position << "Data:" << binaryData;
            }
        }
    }

    if(!noRegularGroups)
    {
        //i is for group, and j is for pin
        quint8 i, j;

        for(i = 0; i < regularLEDGroupList.count(); i++)
        {
            quint8 groupNum = regularLEDGroupList[i];

            QList<quint8> arrayPos = regularArrayPosition[groupNum];
            QList<quint8> arrayData = regularArrayData[groupNum];

            qDebug() << "For Group:" << groupNum << "Position Data:" << arrayPos;

            for(j = 0; j < arrayPos.count(); j++)
            {

                quint8 position = arrayPos[j];
                quint8 data = arrayData[position];
                QString binaryData = QString("%1").arg(data, 8, 2, QChar('0'));

                qDebug() << "Group:" << groupNum << "Position:" << position << "Data:" << binaryData;
            }
        }
    }

}


void LightController::ConvertRegularToState()
{
    //i is for group, and j is for pin
    quint8 i, j, pin;
    QList<quint8> wrotePins;

    //First Set the State to false, then Set Intensity
    for(i = 0; i < regularLEDGroupList.count(); i++)
    {
        quint8 groupNum = regularLEDGroupList[i];
        QList<quint8> pins = regularLEDMap[groupNum];

        for(j = 0; j < pins.count(); j++)
        {
            pin = pins[j] - 1;

            if(!wrotePins.contains(pin))
            {
                //Set Intensity to Default Brightness
                emit SetLightIntensity(id, pin, defaultBrightness);

                //Turn State Off
                emit SetPinState(id, pin, false);

                wrotePins << pin;
            }
        }
    }

    //Then Set the State for the Regular Lights to Off (0)
    for(i = 0; i < regularLEDGroupList.count(); i++)
    {
        quint8 groupNum = regularLEDGroupList[i];

        QList<quint8> arrayPos = regularArrayPosition[groupNum];
        QList<quint8> arrayData = regularArrayData[groupNum];

        for(j = 0; j < arrayPos.count(); j++)
        {
            quint8 position = arrayPos[j];
            quint8 data = arrayData[position];

            p_pinsState[position] = p_pinsState[position] & ~(data);
        }
    }

    //for(i = 0; i < numberGroups; i++)
    //    qDebug() << "Pins State - Array Position:" << i << "State Data:" << p_pinsState[i];
}






bool LightController::CheckRegularGroups(QList<quint8> groupNumber)
{
    //i is for group, and j is for pin
    quint8 i, j;

    for(i = 0; i < groupNumber.count(); i++)
    {
        QList<quint8> arrayPos = regularArrayPosition[groupNumber[i]];
        QList<quint8> arrayData = regularArrayData[groupNumber[i]];

        for(j = 0; j < arrayPos.count(); j++)
        {
            quint8 checkData = arrayData[arrayPos[j]] & p_usedPins[arrayPos[j]];

            if(checkData != 0)
                return false;
        }
    }

    //Ran the Gaunlet
    return true;
}


void LightController::SetRegularGroups(QList<quint8> groupNumber)
{
    //i is for group, and j is for pin
    quint8 i, j;

    for(i = 0; i < groupNumber.count(); i++)
    {
        QList<quint8> arrayPos = regularArrayPosition[groupNumber[i]];
        QList<quint8> arrayData = regularArrayData[groupNumber[i]];

        for(j = 0; j < arrayPos.count(); j++)
            p_usedPins[arrayPos[j]] = p_usedPins[arrayPos[j]] | arrayData[arrayPos[j]];

    }
}


void LightController::UnsetRegularGroups(QList<quint8> groupNumber)
{
    //i is for group, and j is for pin
    quint8 i, j;

    for(i = 0; i < groupNumber.count(); i++)
    {
        QList<quint8> arrayPos = regularArrayPosition[groupNumber[i]];
        QList<quint8> arrayData = regularArrayData[groupNumber[i]];

        for(j = 0; j < arrayPos.count(); j++)
            p_usedPins[arrayPos[j]] = p_usedPins[arrayPos[j]] & ~(arrayData[arrayPos[j]]);

    }
}





bool LightController::CheckRGBGroups(QList<quint8> groupNumber, quint8 *failedGroup)
{
    //i is for group, and j is for pin
    quint8 i, j;

    for(i = 0; i < groupNumber.count(); i++)
    {
        QList<quint8> arrayPos = rgbArrayPosition[groupNumber[i]];
        QList<quint8> arrayData = rgbArrayData[groupNumber[i]];

        for(j = 0; j < arrayPos.count(); j++)
        {
            quint8 checkData = arrayData[arrayPos[j]] & p_usedPins[arrayPos[j]];

            if(checkData != 0)
            {
                *failedGroup = groupNumber[i];
                return false;
            }
        }
    }

    //Ran the Gaunlet
    return true;
}


void LightController::SetRGBGroups(QList<quint8> groupNumber)
{
    //i is for group, and j is for pin
    quint8 i, j;

    for(i = 0; i < groupNumber.count(); i++)
    {
        QList<quint8> arrayPos = rgbArrayPosition[groupNumber[i]];
        QList<quint8> arrayData = rgbArrayData[groupNumber[i]];

        for(j = 0; j < arrayPos.count(); j++)
            p_usedPins[arrayPos[j]] = p_usedPins[arrayPos[j]] | arrayData[arrayPos[j]];
    }
}


void LightController::UnsetRGBGroups(QList<quint8> groupNumber)
{
    //i is for group, and j is for pin
    quint8 i, j;

    for(i = 0; i < groupNumber.count(); i++)
    {
        QList<quint8> arrayPos = rgbArrayPosition[groupNumber[i]];
        QList<quint8> arrayData = rgbArrayData[groupNumber[i]];

        for(j = 0; j < arrayPos.count(); j++)
            p_usedPins[arrayPos[j]] = p_usedPins[arrayPos[j]] & ~(arrayData[arrayPos[j]]);

    }
}






//Light Commands


//Flash Commands

void LightController::FlashRegularLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes)
{
    bool runCMD = CheckRegularGroups(grpNumList);

    if(runCMD)
    {
        //Set Used Pins
        SetRegularGroups(grpNumList);

        //Make New Light Execution Class
        p_execution[executionCount] = new LightExecution(executionCount, grpNumList, regularLEDMap, false, rgbFastMode);

        //Connect Signals and Slots
        ConnectRegular(executionCount);

        //Execute the Command
        p_execution[executionCount]->FlashRegularLights(timeOnMs, timeOffMs, numFlashes);

        //Increament Count
        executionCount++;
    }
}

void LightController::FlashRGBLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color)
{
    quint8 failedGroup;
    bool runCMD = CheckRGBGroups(grpNumList, &failedGroup);

    if(runCMD)
    {
        //Color is Checked when Loading Game File
        RGBColor rgbFlashColor;
        rgbFlashColor = rgbColorMap[color];

        //Set Used Pins
        SetRGBGroups(grpNumList);

        //Make New Light Execution Class
        p_execution[executionCount] = new LightExecution(executionCount, grpNumList, rgbLEDMap, true, rgbFastMode);

        //Connect Signals and Slots
        ConnectRGB(executionCount);

        //Execute the Command
        p_execution[executionCount]->FlashRGBLights(timeOnMs, timeOffMs, numFlashes, rgbFlashColor);

        //Increament Count
        executionCount++;
    }
    else
    {
        if(isBackground)
        {
            quint8 i;

            for(i = 0; i < MAXGAMEPLAYERS; i++)
            {
                if(backgroundActive[i])
                {
                    if(backgroundGroup[i] == failedGroup)
                    {
                        p_background[i]->FlashRGBLights(grpNumList, timeOnMs, timeOffMs, numFlashes, rgbColorMap[color]);
                        return;
                    }
                    else
                    {
                        quint8 j;
                        //Check Groups
                        for(j = 0; j < grpNumList.count(); j++)
                        {
                            //otherBGGroups[i]
                            if(otherBGGroups[i].contains(grpNumList[j]))
                            {
                                p_background[i]->FlashRGBLights(grpNumList, timeOnMs, timeOffMs, numFlashes, rgbColorMap[color]);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}

void LightController::FlashRandomRegularLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes)
{
    bool runCMD = CheckRegularGroups(grpNumList);

    if(runCMD)
    {
        //Set Used Pins
        SetRegularGroups(grpNumList);

        //Make New Light Execution Class
        p_execution[executionCount] = new LightExecution(executionCount, grpNumList, regularLEDMap, false, rgbFastMode);

        //Connect Signals and Slots
        ConnectRegular(executionCount);

        //Execute the Command
        p_execution[executionCount]->FlashRandomRegularLights(timeOnMs, timeOffMs, numFlashes);

        //Increament Count
        executionCount++;
    }
}

void LightController::FlashRandomRGBLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color)
{
    quint8 failedGroup;
    bool runCMD = CheckRGBGroups(grpNumList, &failedGroup);

    if(runCMD)
    {
        //Color is Checked when Loading Game File
        RGBColor rgbFlashColor;
        rgbFlashColor = rgbColorMap[color];

        //Set Used Pins
        SetRGBGroups(grpNumList);

        //Make New Light Execution Class
        p_execution[executionCount] = new LightExecution(executionCount, grpNumList, rgbLEDMap, true, rgbFastMode);

        //Connect Signals and Slots
        ConnectRGB(executionCount);

        //Execute the Command
        p_execution[executionCount]->FlashRandomRGBLights(timeOnMs, timeOffMs, numFlashes, rgbFlashColor);

        //Increament Count
        executionCount++;
    }
    else
    {
        if(isBackground)
        {
            quint8 i;

            for(i = 0; i < MAXGAMEPLAYERS; i++)
            {
                if(backgroundActive[i])
                {
                    if(backgroundGroup[i] == failedGroup)
                    {
                        p_background[i]->FlashRandomRGBLights(grpNumList, timeOnMs, timeOffMs, numFlashes, rgbColorMap[color]);
                        return;
                    }
                    else
                    {
                        quint8 j;
                        //Check Groups
                        for(j = 0; j < grpNumList.count(); j++)
                        {
                            //otherBGGroups[i]
                            if(otherBGGroups[i].contains(grpNumList[j]))
                            {
                                p_background[i]->FlashRandomRGBLights(grpNumList, timeOnMs, timeOffMs, numFlashes, rgbColorMap[color]);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}



void LightController::FlashRandomRGB2CLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color, QString sColor)
{
    quint8 failedGroup;
    bool runCMD = CheckRGBGroups(grpNumList, &failedGroup);

    if(runCMD)
    {
        //Color is Checked when Loading Game File
        RGBColor rgbFlashColor, sideColor;
        rgbFlashColor = rgbColorMap[color];
        sideColor = rgbColorMap[sColor];

        //Set Used Pins
        SetRGBGroups(grpNumList);

        //Make New Light Execution Class
        p_execution[executionCount] = new LightExecution(executionCount, grpNumList, rgbLEDMap, true, rgbFastMode);

        //Connect Signals and Slots
        ConnectRGB(executionCount);

        //Execute the Command
        p_execution[executionCount]->FlashRandomRGB2CLights(timeOnMs, timeOffMs, numFlashes, rgbFlashColor, sideColor);

        //Increament Count
        executionCount++;
    }
    else
    {
        if(isBackground)
        {
            quint8 i;

            for(i = 0; i < MAXGAMEPLAYERS; i++)
            {
                if(backgroundActive[i])
                {
                    if(backgroundGroup[i] == failedGroup)
                    {
                        p_background[i]->FlashRandomRGB2CLights(grpNumList, timeOnMs, timeOffMs, numFlashes, rgbColorMap[color], rgbColorMap[sColor]);
                        return;
                    }
                    else
                    {
                        quint8 j;
                        //Check Groups
                        for(j = 0; j < grpNumList.count(); j++)
                        {
                            if(otherBGGroups[i].contains(grpNumList[j]))
                            {
                                p_background[i]->FlashRandomRGB2CLights(grpNumList, timeOnMs, timeOffMs, numFlashes, rgbColorMap[color], rgbColorMap[sColor]);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}


void LightController::FlashRandomRGBLightsCM(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString colorMap)
{
    quint8 failedGroup;
    bool runCMD = CheckRGBGroups(grpNumList, &failedGroup);

    if(runCMD)
    {
        QStringList colors = rgbColorMapMap[colorMap];

        quint8 randomNum = QRandomGenerator::global()->bounded(0, colors.count());

        QString color = colors[randomNum];

        RGBColor rgbFlashColor = rgbColorMap[color];

        //Set Used Pins
        SetRGBGroups(grpNumList);

        //Make New Light Execution Class
        p_execution[executionCount] = new LightExecution(executionCount, grpNumList, rgbLEDMap, true, rgbFastMode);

        //Connect Signals and Slots
        ConnectRGB(executionCount);

        //Execute the Command
        p_execution[executionCount]->FlashRandomRGBLights(timeOnMs, timeOffMs, numFlashes, rgbFlashColor);

        //Increament Count
        executionCount++;
    }
    else
    {
        if(isBackground)
        {
            quint8 i;

            for(i = 0; i < MAXGAMEPLAYERS; i++)
            {
                if(backgroundActive[i])
                {
                    if(backgroundGroup[i] == failedGroup)
                    {
                        QStringList colors = rgbColorMapMap[colorMap];

                        quint8 randomNum = QRandomGenerator::global()->bounded(0, colors.count());

                        QString color = colors[randomNum];

                        RGBColor rgbFlashColor = rgbColorMap[color];

                        p_background[i]->FlashRandomRGBLights(grpNumList, timeOnMs, timeOffMs, numFlashes, rgbFlashColor);
                        return;
                    }
                    else
                    {
                        quint8 j;
                        //Check Groups
                        for(j = 0; j < grpNumList.count(); j++)
                        {
                            //otherBGGroups[i]
                            if(otherBGGroups[i].contains(grpNumList[j]))
                            {
                                QStringList colors = rgbColorMapMap[colorMap];

                                quint8 randomNum = QRandomGenerator::global()->bounded(0, colors.count());

                                QString color = colors[randomNum];

                                RGBColor rgbFlashColor = rgbColorMap[color];

                                p_background[i]->FlashRandomRGBLights(grpNumList, timeOnMs, timeOffMs, numFlashes, rgbFlashColor);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}







void LightController::SequenceRegularLights(QList<quint8> grpNumList, quint16 delay)
{
    bool runCMD = CheckRegularGroups(grpNumList);

    if(runCMD)
    {
        //Set Used Pins
        SetRegularGroups(grpNumList);

        //Make New Light Execution Class
        p_execution[executionCount] = new LightExecution(executionCount, grpNumList, regularLEDMap, false, rgbFastMode);

        //Connect Signals and Slots
        ConnectRegular(executionCount);

        //Execute the Command
        p_execution[executionCount]->SequenceRegularLights(delay);

        //Increament Count
        executionCount++;
    }
}



void LightController::SequenceRGBLights(QList<quint8> grpNumList, quint16 delay, QString color)
{
    quint8 failedGroup;
    bool runCMD = CheckRGBGroups(grpNumList, &failedGroup);

    if(runCMD)
    {
        //Color is Checked when Loading Game File
        RGBColor rgbSequenceColor;
        rgbSequenceColor = rgbColorMap[color];

        //Set Used Pins
        SetRGBGroups(grpNumList);

        //Make New Light Execution Class
        p_execution[executionCount] = new LightExecution(executionCount, grpNumList, rgbLEDMap, true, rgbFastMode);

        //Connect Signals and Slots
        ConnectRGB(executionCount);

        //Execute the Command
        p_execution[executionCount]->SequenceRGBLights(delay, rgbSequenceColor);

        //Increament Count
        executionCount++;
    }
    else
    {
        if(isBackground)
        {
            quint8 i;

            for(i = 0; i < MAXGAMEPLAYERS; i++)
            {
                if(backgroundActive[i])
                {
                    if(backgroundGroup[i] == failedGroup)
                    {
                        p_background[i]->SequenceRGBLights(grpNumList, delay, rgbColorMap[color]);
                        return;
                    }
                    else
                    {
                        quint8 j;
                        //Check Groups
                        for(j = 0; j < grpNumList.count(); j++)
                        {
                            //otherBGGroups[i]
                            if(otherBGGroups[i].contains(grpNumList[j]))
                            {
                                p_background[i]->SequenceRGBLights(grpNumList, delay, rgbColorMap[color]);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}

void LightController::SequenceRGBLightsCM(QList<quint8> grpNumList, quint16 delay, QString colorMap)
{
    quint8 failedGroup;
    bool runCMD = CheckRGBGroups(grpNumList, &failedGroup);

    if(runCMD)
    {
        //Color is Checked when Loading Game File
        //Make RGBColor List
        QStringList colors = rgbColorMapMap[colorMap];
        quint8 i;
        QList<RGBColor> colorList;

        for(i = 0; i < colors.count(); i++)
            colorList.insert(i,rgbColorMap[colors[i]]);

        //Set Used Pins
        SetRGBGroups(grpNumList);

        //Make New Light Execution Class
        p_execution[executionCount] = new LightExecution(executionCount, grpNumList, rgbLEDMap, true, rgbFastMode);

        //Connect Signals and Slots
        ConnectRGB(executionCount);

        //Execute the Command
        p_execution[executionCount]->SequenceRGBLightsCM(delay, colorList);

        //Increament Count
        executionCount++;
    }
    else
    {
        if(isBackground)
        {
            quint8 i;

            for(i = 0; i < MAXGAMEPLAYERS; i++)
            {
                if(backgroundActive[i])
                {
                    if(backgroundGroup[i] == failedGroup)
                    {
                        QStringList colors = rgbColorMapMap[colorMap];
                        QList<RGBColor> colorList;
                        quint8 k;

                        for(k = 0; k < colors.count(); k++)
                            colorList.insert(k,rgbColorMap[colors[k]]);

                        p_background[i]->SequenceRGBLightsCM(grpNumList, delay, colorList);
                        return;
                    }
                    else
                    {
                        quint8 j;
                        //Check Groups
                        for(j = 0; j < grpNumList.count(); j++)
                        {
                            //otherBGGroups[i]
                            if(otherBGGroups[i].contains(grpNumList[j]))
                            {
                                QStringList colors = rgbColorMapMap[colorMap];
                                QList<RGBColor> colorList;
                                quint8 k;

                                for(k = 0; k < colors.count(); k++)
                                    colorList.insert(k,rgbColorMap[colors[k]]);

                                p_background[i]->SequenceRGBLightsCM(grpNumList, delay, colorList);
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}


void LightController::FollowerRegularLights(QList<quint8> grpNumList, quint16 data)
{
    if(data > 0)
    {
        //ShowRegularIntensity(grpNumList, intensity);
        ShowRegularState(grpNumList, true);
    }
    else
    {
        //ShowRegularIntensity(grpNumList, 0);
        ShowRegularState(grpNumList, false);
    }
}


void LightController::FollowerRGBLights(QList<quint8> grpNumList, QString color, quint16 data)
{  
    if(data > 0)
    {
        //Color is Checked when Loading Game File
        rgbFollowerColor = rgbColorMap[color];

        ShowRGBColor(grpNumList, rgbFollowerColor);
    }
    else
    {
        //TurnRGBState(grpNumList, false);
        ShowRGBColor(grpNumList, rgbOff);
    }
}

void LightController::FollowerRandomRGBLights(QList<quint8> grpNumList, quint16 data)
{
    if(data > 0)
    {
        quint16 randomColor = QRandomGenerator::global()->bounded(0, numberColors);
        rgbFollowerColor = rgbColorMap[colorNameList[randomColor]];
    }
    else
        rgbFollowerColor = rgbOff;

    ShowRGBColor(grpNumList, rgbFollowerColor);
}


void LightController::TurnOffLights()
{
    quint16 i;

    //Go through all the Light Executions and Turn Off Lights and End
    for(i = 0; i < NUMBEREXECUTIONS; i++)
    {
        if(p_execution[i] != nullptr)
            p_execution[i]->TurnOffLightsEnd ();
    }

    //Turn Off All Regular Lights, if Any
    if(!noRegularGroups)
        ShowRegularState(regularLEDGroupList, false);

    //Turn Off All RGB Lights, if Any
    if(!noRGBGroups)
        ShowRGBColor(rgbLEDGroupList, rgbOff);
}


void LightController::SetUpBackgroundRGB(QList<quint8> grpNumList, QString colorMap, quint8 playerNumber, quint16 delay, quint16 delayBGR, quint8 highCount, QList<quint8> otherGrpList)
{
    if(!backgroundActive[playerNumber])
    {
        quint8 i;
        isBackground = true;
        backgroundActive[playerNumber] = true;
        backgroundGroup[playerNumber] = grpNumList[0];
        otherBGGroups[playerNumber] = otherGrpList;
        //Put the Background Group Number into
        otherBGGroups[playerNumber].prepend (grpNumList[0]);

        quint8 highCountNew;
        quint8 rgbPins = rgbLEDMap[grpNumList[0]].count();

        if(!rgbFastMode)
            rgbPins = rgbPins / 3;

        //Check High Count
        if(highCount == 0)
            highCountNew = rgbPins;
        else if(highCount > rgbPins)
            highCountNew = rgbPins;
        else
            highCountNew = highCount;


        QStringList bgColorMap = rgbColorMapMap[colorMap];
        QList<RGBColor> bgRGBColorMap;

        //Make QList of RGB Colors from Map
        for(i = 0; i < bgColorMap.count(); i++)
        {
            RGBColor tempRGB = rgbColorMap[bgColorMap[i]];
            bgRGBColorMap << tempRGB;
        }

        //Set Used Pins
        SetRGBGroups(grpNumList);

        p_background[playerNumber] = new LightBackground(playerNumber, 1, grpNumList[0], rgbLEDMap, true, rgbFastMode, bgRGBColorMap, highCountNew, delay, delayBGR);

        ConnectRGBBG(playerNumber);

        p_background[playerNumber]->TurnOnBackGround ();
    }
}

void LightController::BackgroundRGB(quint8 playerNumber, quint16 ammoValue)
{
    if(backgroundActive[playerNumber])
    {
        p_background[playerNumber]->UpdateCount(ammoValue);
    }
}



void LightController::ConnectRegular(quint8 index)
{
    connect(p_execution[index],&LightExecution::ShowRegularState, this, &LightController::ShowRegularState);

    connect(p_execution[index],&LightExecution::ShowRegularStateOne, this, &LightController::ShowRegularStateOne);

    connect(p_execution[index],&LightExecution::ShowRegularStateOneSequence, this, &LightController::ShowRegularStateOneSequence);

    connect(p_execution[index],&LightExecution::CommandExecuted, this, &LightController::RegularExecutionFinished);
}

void LightController::ConnectRGB(quint8 index)
{
    connect(p_execution[index],&LightExecution::ShowRGBColor, this, &LightController::ShowRGBColor);

    connect(p_execution[index],&LightExecution::ShowRGBColorOne, this, &LightController::ShowRGBColorOne);

    connect(p_execution[index],&LightExecution::ShowRGBColorOneSequence, this, &LightController::ShowRGBColorOneSequence);

    connect(p_execution[index],&LightExecution::CommandExecuted, this, &LightController::RGBExecutionFinished);
}

void LightController::DisconnectRegular(quint8 index)
{
    disconnect(p_execution[index],&LightExecution::ShowRegularState, this, &LightController::ShowRegularState);

    disconnect(p_execution[index],&LightExecution::ShowRegularStateOne, this, &LightController::ShowRegularStateOne);

    disconnect(p_execution[index],&LightExecution::ShowRegularStateOneSequence, this, &LightController::ShowRegularStateOneSequence);

    disconnect(p_execution[index],&LightExecution::CommandExecuted, this, &LightController::RegularExecutionFinished);
}

void LightController::DisconnectRGB(quint8 index)
{
    disconnect(p_execution[index],&LightExecution::ShowRGBColor, this, &LightController::ShowRGBColor);

    disconnect(p_execution[index],&LightExecution::ShowRGBColorOne, this, &LightController::ShowRGBColorOne);

    disconnect(p_execution[index],&LightExecution::ShowRGBColorOneSequence, this, &LightController::ShowRGBColorOneSequence);

    disconnect(p_execution[index],&LightExecution::CommandExecuted, this, &LightController::RGBExecutionFinished);
}


void LightController::ConnectRGBBG(quint8 player)
{
    connect(p_background[player],&LightBackground::ShowRGBColor, this, &LightController::ShowRGBColor);

    connect(p_background[player],&LightBackground::ShowRGBColorOne, this, &LightController::ShowRGBColorOne);

    connect(p_background[player],&LightBackground::ShowRGBColorOneSequence, this, &LightController::ShowRGBColorOneSequence);
}


void LightController::DisconnectRGBBG(quint8 player)
{
    disconnect(p_background[player],&LightBackground::ShowRGBColor, this, &LightController::ShowRGBColor);

    disconnect(p_background[player],&LightBackground::ShowRGBColorOne, this, &LightController::ShowRGBColorOne);

    disconnect(p_background[player],&LightBackground::ShowRGBColorOneSequence, this, &LightController::ShowRGBColorOneSequence);
}






void LightController::ShowRegularState(QList<quint8> grpNumList, bool state)
{
    quint8 i, j;

    for(i = 0; i < grpNumList.count(); i++)
    {
        QList<quint8> arrayPos = regularArrayPosition[grpNumList[i]];
        QList<quint8> arrayData = regularArrayData[grpNumList[i]];

        //qDebug() << "For Group:" << grpNumList[i] << "Position Data:" << arrayPos;

        for(j = 0; j < arrayPos.count(); j++)
        {
            quint8 position = arrayPos[j];
            quint8 data = arrayData[position];

            //qDebug() << "Position:" << position << "Data:" << data << "State Data:" << p_pinsState[position] << "State:" << state;

            if(state)
                p_pinsState[position] = p_pinsState[position] | data;
            else
                p_pinsState[position] = p_pinsState[position] & ~(data);

            emit SetPinStates(id, position, p_pinsState[position], false);

            //qDebug() << "State Data:" << p_pinsState[position] << "State:" << state;
        }
    }
}


void LightController::ShowRegularStateOne(QList<quint8> grpNumList, bool state, QList<quint8> indexList, qint8 offset)
{
    quint8 i;

    for(i = 0; i < grpNumList.count(); i++)
    {
        //Get Pin Data and Position
        QList<quint8> pins = regularLEDMap[grpNumList[i]];
        quint8 pin = pins[indexList[i]+offset]-1;
        quint8 position = (pin >> 3);
        quint8 pinPos = pin & 0x07;
        quint8 pinData = (1 << pinPos);

        //Get State Data of Pin
        quint8 dataChk = p_pinsState[position] & pinData;
        bool stateChk;

        if(dataChk == 0)
            stateChk = false;
        else
            stateChk = true;

        //If State doesn't Match, then do a Write
        if(stateChk != state)
        {
            if(state)
                p_pinsState[position] = p_pinsState[position] | pinData;
            else
                p_pinsState[position] = p_pinsState[position] & ~(pinData);

            emit SetPinState(id, pin, state);
        }
    }
}

void LightController::ShowRegularStateOneSequence(QList<quint8> grpNumList, bool state, quint8 index)
{
    quint8 i;

    for(i = 0; i < grpNumList.count(); i++)
    {
        //Get Pin Data and Position
        QList<quint8> pins = regularLEDMap[grpNumList[i]];
        quint8 pin = pins[index]-1;
        quint8 position = (pin >> 3);
        quint8 pinPos = pin & 0x07;
        quint8 pinData = (1 << pinPos);

        //Get State Data of Pin
        quint8 dataChk = p_pinsState[position] & pinData;
        bool stateChk;

        if(dataChk == 0)
            stateChk = false;
        else
            stateChk = true;

        //If State doesn't Match, then do a Write
        if(stateChk != state)
        {
            if(state)
                p_pinsState[position] = p_pinsState[position] | pinData;
            else
                p_pinsState[position] = p_pinsState[position] & ~(pinData);

            emit SetPinState(id, pin, state);
        }
    }
}



void LightController::ShowRGBColor(QList<quint8> grpNumList, RGBColor color)
{
    quint8 i, j;

    //qDebug() << "rgbFastMode:" << rgbFastMode;

    //For Groups
    for(i = 0; i < grpNumList.count(); i++)
    {
        //For Pins in Groups
        QList pins = rgbLEDMap[grpNumList[i]];
        RGBPins rgbPins;

        for(j = 0; j < pins.count(); j++)
        {
            //Get the RGB Pin Number
            if(rgbFastMode)
            {
                rgbPins.g = pins[j];
                rgbPins.r = rgbPins.g - 1;
                rgbPins.b = rgbPins.g + 1;
            }
            else
            {
                rgbPins.r = pins[j]-1;
                j++;
                rgbPins.g = pins[j]-1;
                j++;
                rgbPins.b = pins[j]-1;
            }

            //qDebug() << "Red Pin" << rgbPins.r << "Green Pin" << rgbPins.g << "Blue Pin" << rgbPins.b;
            //qDebug() << "Red" << color.r << "Green" << color.g << "Blue" << color.b;

            //Set RGB Lights to the Color
            emit SetRGBLightIntensity(id, rgbPins, color);
        }
    }
}


void LightController::ShowRGBColorOne(QList<quint8> grpNumList, RGBColor color, QList<quint8> indexList, qint8 offset)
{
    quint8 i, j;

    //For Groups
    for(i = 0; i < grpNumList.count(); i++)
    {
        //For Pins in Groups
        QList pins = rgbLEDMap[grpNumList[i]];
        RGBPins rgbPins;
        quint8 count = pins.count();
        bool isOutOfRange = false;

        //Get the RGB Pin Number
        if(rgbFastMode && indexList[i] < count)
        {
            //Pins are -1, So Green is +1, to make it 0
            rgbPins.g = pins[indexList[i]+offset];
            rgbPins.r = rgbPins.g - 1;
            rgbPins.b = rgbPins.g + 1;
        }
        else if(!rgbFastMode && indexList[i]+2 < count)
        {
            //quint8 newOffset = offset*3;
            //Offset shifted by 1 to left (x2) + Offset = 3 x Offset
            quint8 newOffset = (offset << 1) + offset;
            rgbPins.r = pins[indexList[i]+newOffset]-1;
            rgbPins.g = pins[indexList[i]+newOffset+1]-1;
            rgbPins.b = pins[indexList[i]+newOffset+2]-1;
        }
        else
            isOutOfRange = true;

        //Set RGB Lights to the Color
        if(!isOutOfRange)
            emit SetRGBLightIntensity(id, rgbPins, color);
    }
}


void LightController::ShowRGBColorOneSequence(QList<quint8> grpNumList, RGBColor color, quint8 index)
{
    quint8 i;

    //For Groups
    for(i = 0; i < grpNumList.count(); i++)
    {
        //For Pins in Groups
        QList pins = rgbLEDMap[grpNumList[i]];
        RGBPins rgbPins;
        quint8 count = pins.count();
        bool isOutOfRange = false;

        //Check if Index is Out of Range of Array
        if(rgbFastMode && index < count)
        {
            rgbPins.g = pins[index];
            rgbPins.r = rgbPins.g - 1;
            rgbPins.b = rgbPins.g + 1;
        }
        else if(!rgbFastMode && index+2 < count)
        {
            rgbPins.r = pins[index]-1;
            rgbPins.g = pins[index+1]-1;
            rgbPins.b = pins[index+2]-1;
        }
        else
            isOutOfRange = true;

        //Set RGB Lights to the Color
        if(!isOutOfRange)
            emit SetRGBLightIntensity(id, rgbPins, color);
    }
}



void LightController::TurnRegularState(QList<quint8> grpNumList, bool state)
{
    quint8 i, j;

    for(i = 0; i < grpNumList.count(); i++)
    {
        //For Pins in Groups
        QList pins = regularLEDMap[grpNumList[i]];
        quint8 pin;

        for(j = 0; j < pins.count(); j++)
        {
            pin = pins[j]-1;

            //Set RGB Lights to the Color
            emit SetPinState(id, pin, state);
        }
    }
}



void LightController::TurnRGBState(QList<quint8> grpNumList, bool state)
{
    quint8 i, j;

    for(i = 0; i < grpNumList.count(); i++)
    {
        //For Pins in Groups
        QList pins = rgbLEDMap[grpNumList[i]];
        RGBPins rgbPins;

        for(j = 0; j < pins.count(); j++)
        {
            //Get the RGB Pin Number
            if(rgbFastMode)
            {
                rgbPins.g = pins[j];
                rgbPins.r = rgbPins.g - 1;
                rgbPins.b = rgbPins.g + 1;
            }
            else
            {
                rgbPins.r = pins[j]-1;
                j++;
                rgbPins.g = pins[j]-1;
                j++;
                rgbPins.b = pins[j]-1;
            }

            //qDebug() << "Red Pin" << rgbPins.r << "Green Pin" << rgbPins.g << "Blue Pin" << rgbPins.b;
            //qDebug() << "Red" << color.r << "Green" << color.g << "Blue" << color.b;

            //Set RGB Lights to the Color
            emit SetPinState(id, rgbPins.r, state);
            emit SetPinState(id, rgbPins.g, state);
            emit SetPinState(id, rgbPins.b, state);
        }
    }
}


void LightController::RegularExecutionFinished(quint8 exeNum, QList<quint8> grpNumList)
{
    //Unset Pins
    UnsetRegularGroups(grpNumList);

    //Disconnect Signals and Slots
    DisconnectRegular(exeNum);

    //Delete the Light Execution
    delete p_execution[exeNum];
    p_execution[exeNum] = nullptr;
}

void LightController::RGBExecutionFinished(quint8 exeNum, QList<quint8> grpNumList)
{
    //Unset Pins
    UnsetRGBGroups(grpNumList);

    //Disconnect Signals and Slots
    DisconnectRGB(exeNum);

    //Delete the Light Execution
    delete p_execution[exeNum];
    p_execution[exeNum] = nullptr;
}






bool LightController::CheckRegularGroupNumber(quint8 grpNum)
{
    if(regularLEDMap.contains (grpNum))
        return true;
    else
        return false;
}

bool LightController::CheckRGBGroupNumber(quint8 grpNum)
{
    if(rgbLEDMap.contains (grpNum))
        return true;
    else
        return false;
}

bool LightController::CheckColor(QString color)
{
    if(rgbColorMap.contains (color))
        return true;
    else
        return false;
}

bool LightController::CheckSideColor(QString sideColor)
{
    if(rgbColorMap.contains (sideColor))
        return true;
    else
        return false;
}

bool LightController::CheckColorMap(QString colorMap)
{
    if(rgbColorMapMap.contains (colorMap))
        return true;
    else
        return false;
}

bool LightController::CheckGroupsfor2Colors(QList<quint8> grpNumList)
{
    quint8 i;

    for(i = 0; i < grpNumList.count(); i++)
    {
        quint8 pinsCount = rgbLEDMap[grpNumList[i]].count();

        if(rgbFastMode)
        {
            if(pinsCount < 3)
                return false;
        }
        else
        {
            if(pinsCount < 9)
                return false;
        }
    }

    //Ran the Gauntlet
    return true;
}


void LightController::ResetLightController()
{
    quint8 i;

    doReset = false;

    if(isBackground)
    {
        isBackground = false;

        for(i = 0; i < MAXGAMEPLAYERS; i++)
        {
            if(backgroundActive[i])
            {
                //Turn Off Lights
                p_background[i]->TurnOffLightsEnd();
                //Disconnect Background Signals & Slots
                DisconnectRGBBG(i);
                //Delete Background
                delete p_background[i];
                p_background[i] = nullptr;
                //Unset Used Pins
                QList<quint8> tempList;
                tempList << backgroundGroup[i];
                UnsetRGBGroups(tempList);
                backgroundActive[i] = false;
            }
        }
    }
}


void LightController::SetUpLights()
{
    if(!noRegularGroups && didGroupFileLoad)
    {
        quint8 i;

        p_pinsState = new quint8[numberGroups];

        for(i = 0; i < numberGroups; i++)
            p_pinsState[i] = 0xFF;

        if(dataUltimarc.type == NANOLED)
            p_pinsState[numberGroups-1] = 0x0F;

        //Conver Regular Lights from Intensity to State
        ConvertRegularToState();
    }
}




//Private Slots





















