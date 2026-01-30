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

    didGroupFileLoad = false;

    title = "Light Controller Group File Error";

    isCommandRunning = other.isCommandRunning;

    rgbOff.r = 0;
    rgbOff.g = 0;
    rgbOff.b = 0;

    useRandomPins = false;
    useSideColor = false;
    rgbFollowerFirstTime = false;
    doReset = false;

    //Load the Group File
    LoadGroupFile();

    //Regular Timer Set-up
    p_regularTimer = new QTimer(this);
    p_regularTimer->setSingleShot (true);

    //RGB Timer Set-up
    p_rgbTimer = new QTimer(this);
    p_rgbTimer->setSingleShot (true);

}




LightController::LightController(quint8 cntlrNum, UltimarcData dataU, QObject *parent)
    : QObject{parent}
{
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

    isCommandRunning = false;

    didGroupFileLoad = false;

    title = "Light Controller Group File Error";

    rgbOff.r = 0;
    rgbOff.g = 0;
    rgbOff.b = 0;

    useRandomPins = false;
    useSideColor = false;
    rgbFollowerFirstTime = false;
    doReset = false;

    //Load the Group File
    LoadGroupFile();

    //Regular Timer Set-up
    p_regularTimer = new QTimer(this);
    p_regularTimer->setSingleShot (true);

    //RGB Timer Set-up
    p_rgbTimer = new QTimer(this);
    p_rgbTimer->setSingleShot (true);

}


//Deconstructor

LightController::~LightController()
{
    //if(p_regularTimer != nullptr)
    //    delete p_regularTimer;

    //if(p_rgbTimer != nullptr)
    //    delete p_rgbTimer;
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

    rgbFastMode = false;

    noRegularGroups = false;

    noRGBGroups = false;

    didGroupFileLoad = false;

    isCommandRunning = lcMember.isCommandRunning;

    title = "Light Controller Group File Error";

    rgbOff.r = 0;
    rgbOff.g = 0;
    rgbOff.b = 0;

    useRandomPins = false;
    useSideColor = false;
    rgbFollowerFirstTime = false;
    doReset = false;

    //Load the Group File
    LoadGroupFile();
}

void LightController::SetGroupFile(QString filePath)
{
    groupFilePath = filePath;
    dataUltimarc.groupFile = filePath;

    rgbFastMode = false;

    noRegularGroups = false;

    noRGBGroups = false;

    didGroupFileLoad = false;

    //Load the Group File
    LoadGroupFile();
}

void LightController::LoadGroupFile()
{
    bool openFile;
    QString line;
    QFile loadGroupData(groupFilePath);
    QStringList fileData;
    quint16 lineNumber = 0;
    quint16 lineCount;
    bool isNumber;
    qint16 groupNumber;
    qint16 defaultBrightness;
    bool didGroupLoad = true;
    bool didColorLoad = true;
    bool groupLoading = true;
    bool loadedRGBNormal = false;
    bool gotData = false;

    //Clear Out Maps and Lists
    regularLEDMap.clear();
    rgbLEDMap.clear();
    regularPinsCount.clear();
    rgbPinsCount.clear();
    regularBrightnessMap.clear();
    rgbBrightnessMap.clear();
    regularLEDGroupList.clear();
    rgbLEDGroupList.clear();
    rgbColorMap.clear();
    usedPinsList.clear();
    colorNameList.clear();
    numberColors = 0;

    if(loadGroupData.exists() == false)
        return;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    openFile = loadGroupData.open (QIODeviceBase::ReadOnly | QIODevice::Text);
#else
    openFile = loadGroupData.open (QIODevice::ReadOnly | QIODevice::Text);
#endif

    if(!openFile)
    {
        QString message = "Can not open light controller group data file to read. Please close program and solve file problem. Might be permissions problem.";
        emit ShowErrorMessage(title, message);
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

    while(lineNumber < lineCount && didGroupLoad && groupLoading && didColorLoad)
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
        else if(fileData[lineNumber].startsWith(RGBCOLOR))
        {
            didColorLoad = LoadRGBColor(fileData[lineNumber]);
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
    rgbBrightnessMap.insert(groupNumberBright.groupNumber, groupNumberBright.defaultBrightness);

    //Add Group with its pins to the Map
    rgbLEDMap.insert(groupNumberBright.groupNumber, pinDataSruct.pinData);

    //Map Group Number with RGB Pin Count
    rgbPinsCount.insert(groupNumberBright.groupNumber, pinDataSruct.numberPins);

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

    //qDebug() << "Group Number" << groupNumberBright.groupNumber << "Default Brightness" << groupNumberBright.defaultBrightness;

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
    rgbBrightnessMap.insert(groupNumberBright.groupNumber, groupNumberBright.defaultBrightness);

    //Add Group with its pins to the Map
    rgbLEDMap.insert(groupNumberBright.groupNumber, pinDataSruct.pinData);

    //Map Group Number with RGB Pin Count
    rgbPinsCount.insert(groupNumberBright.groupNumber, pinDataSruct.numberPins);

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
    regularBrightnessMap.insert(groupNumberBright.groupNumber, groupNumberBright.defaultBrightness);

    //Add Group with its pins to the Map
    regularLEDMap.insert(groupNumberBright.groupNumber, pinDataSruct.pinData);

    //Map Group Number with Regular Pin Count
    regularPinsCount.insert(groupNumberBright.groupNumber, pinDataSruct.numberPins);

    return true;
}


LightControllerTop LightController::GetGroupNumber(quint8 groupMode, quint16 lineNum, QString lineData)
{
    bool isNumber;
    qint16 groupNumber;
    qint16 defaultBrightness;
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


    defaultBrightness = DEFAULTBRIGHTNESS;

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
    groupNumberBright.defaultBrightness = static_cast<quint8>(defaultBrightness);
    groupNumberBright.isDataSet = true;
    return groupNumberBright;
}


LCPinData LightController::GetGroupPinData(quint8 lcKind, quint16 grpStart, quint16 grpEnd, quint16 lineMax, QStringList fileData)
{
    bool isNumber;
    QList<quint8> grpData;
    quint16 i, j, k;
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



void LightController::FlashRegularLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity)
{
    if(!isCommandRunning)
    {
        flashGroupList = grpNumList;
        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;
        flashIntensity = intensity;

        isCommandRunning = true;

        ShowRegularIntensity(flashGroupList, flashIntensity);

        //Connect Timer to Turn Off LEDs
        connect(p_regularTimer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
        p_regularTimer->setInterval (flashTimeOn);
        p_regularTimer->start();

        //Now Wait until timer runs out
    }
}

void LightController::FlashRGBLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color)
{
    if(!isCommandRunning)
    {
        flashGroupList = grpNumList;
        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;

        if(rgbColorMap.contains (color))
            rgbFlashColor = rgbColorMap[color];
        else
        {
            QString tempCrit = "The RGB color was not found in the RGB color map. Failing Color: " + color;
            emit ShowErrorMessage(title, tempCrit);
            return;
        }

        isCommandRunning = true;

        ShowRGBColor(flashGroupList, rgbFlashColor);

        //Connect Timer to Turn Off LEDs
        connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
        p_rgbTimer->setInterval (flashTimeOn);
        p_rgbTimer->start();

        //Now Wait until timer runs out
    }
}

void LightController::FlashRandomRegularLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity)
{
    if(!isCommandRunning)
    {
        flashGroupList = grpNumList;
        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;
        randomPins.clear();
        flashIntensity = intensity;

        isCommandRunning = true;
        useRandomPins = true;

        //Find Random Pin for Groups
        quint8 i;
        for(i = 0; i < flashGroupList.count(); i++)
        {
            quint8 count = regularLEDMap[flashGroupList[i]].count();

            quint8 randomPin = QRandomGenerator::global()->bounded(0, count);

            //But Random Number into List
            randomPins << randomPin;
        }

        ShowRegularIntensityOne(flashGroupList, flashIntensity, randomPins, 0);

        //Connect Timer to Turn Off LEDs
        connect(p_regularTimer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
        p_regularTimer->setInterval (flashTimeOn);
        p_regularTimer->start();

        //Now Wait until timer runs out
    }
}

void LightController::FlashRandomRGBLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color)
{
    if(!isCommandRunning)
    {
        flashGroupList = grpNumList;
        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;
        randomPins.clear();

        if(rgbColorMap.contains (color))
            rgbFlashColor = rgbColorMap[color];
        else
        {
            QString tempCrit = "The RGB color was not found in the RGB color map. Failing Color: " + color;
            emit ShowErrorMessage(title, tempCrit);
            return;
        }

        isCommandRunning = true;
        useRandomPins = true;

        //Find Random Pin for Groups
        quint8 i;
        for(i = 0; i < flashGroupList.count(); i++)
        {
            quint8 count = rgbLEDMap[flashGroupList[i]].count();

            if(!rgbFastMode)
                count = count / 3;

            quint8 randomPin = QRandomGenerator::global()->bounded(0, count);

            if(!rgbFastMode)
                randomPin = randomPin * 3;

            randomPins << randomPin;
        }

        ShowRGBColorOne(flashGroupList, rgbFlashColor, randomPins, 0);

        //Connect Timer to Turn Off LEDs
        connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
        p_rgbTimer->setInterval (flashTimeOn);
        p_rgbTimer->start();

        //Now Wait until timer runs out
    }
}

void LightController::FlashRandomRegular2ILights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity, quint8 sIntensity)
{
    if(!isCommandRunning)
    {
        flashGroupList = grpNumList;
        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;
        flashIntensity = intensity;
        flashSideIntensity = sIntensity;
        randomPins.clear();

        isCommandRunning = true;
        useRandomPins = true;
        useSideColor = true;

        //Find Random Pin for Groups
        quint8 i;

        for(i = 0; i < flashGroupList.count(); i++)
        {
            quint8 count = regularLEDMap[flashGroupList[i]].count();
            quint8 randomPin;

            randomPin = QRandomGenerator::global()->bounded(1, count-1);

            randomPins << randomPin;

            //qDebug() << "Group:" << i << "Random Pin:" << randomPin << "Count" << count;
        }

        ShowRegularIntensityOne(flashGroupList, flashIntensity, randomPins, 0);
        ShowRegularIntensityOne(flashGroupList, flashSideIntensity, randomPins, 1);
        ShowRegularIntensityOne(flashGroupList, flashSideIntensity, randomPins, -1);

        //Connect Timer to Turn Off LEDs
        connect(p_regularTimer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
        p_regularTimer->setInterval (flashTimeOn);
        p_regularTimer->start();

        //Now Wait until timer runs out

    }
}


void LightController::FlashRandomRGB2CLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color, QString sColor)
{
    if(!isCommandRunning)
    {
        flashGroupList = grpNumList;
        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;
        randomPins.clear();

        if(rgbColorMap.contains (color))
            rgbFlashColor = rgbColorMap[color];
        else
        {
            QString tempCrit = "The RGB color was not found in the RGB color map. Failing Color: " + color;
            emit ShowErrorMessage(title, tempCrit);
            return;
        }

        if(rgbColorMap.contains (sColor))
            sideColor = rgbColorMap[sColor];
        else
        {
            QString tempCrit = "The RGB color was not found in the RGB color map. Failing Color: " + color;
            emit ShowErrorMessage(title, tempCrit);
            return;
        }

        isCommandRunning = true;
        useRandomPins = true;
        useSideColor = true;

        //Find Random Pin for Groups
        quint8 i;

        for(i = 0; i < flashGroupList.count(); i++)
        {
            quint8 count = rgbLEDMap[flashGroupList[i]].count();
            quint8 randomPin;

            if(!rgbFastMode)
                count = count / 3;

            randomPin = QRandomGenerator::global()->bounded(1, count-1);

            if(!rgbFastMode)
                randomPin = randomPin * 3;

            randomPins << randomPin;

            //qDebug() << "Group:" << i << "Random Pin:" << randomPin << "Count" << count;
        }

        ShowRGBColorOne(flashGroupList, rgbFlashColor, randomPins, 0);
        ShowRGBColorOne(flashGroupList, sideColor, randomPins, 1);
        ShowRGBColorOne(flashGroupList, sideColor, randomPins, -1);

        //Connect Timer to Turn Off LEDs
        connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
        p_rgbTimer->setInterval (flashTimeOn);
        p_rgbTimer->start();

        //Now Wait until timer runs out

    }
}


void LightController::SequenceRegularLights(QList<quint8> grpNumList, quint16 delay, quint8 intensity)
{
    if(!isCommandRunning)
    {
        sequenceGroupList = grpNumList;
        sequenceDelay = delay;
        sequenceCount = 0;
        sequenceMaxCount = 1;
        sequenceMaxCountSet = false;
        sequenceIntensity = intensity;

        isCommandRunning = true;

        ShowRegularIntensityOneSequence(sequenceGroupList, sequenceIntensity, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        connect(p_regularTimer, SIGNAL(timeout()), this, SLOT(RegularSequenceDelayDone()));
        p_regularTimer->setInterval (delay);
        p_regularTimer->start();
    }
}



void LightController::SequenceRGBLights(QList<quint8> grpNumList, quint16 delay, QString color)
{
    if(!isCommandRunning)
    {
        sequenceGroupList = grpNumList;
        sequenceDelay = delay;
        sequenceCount = 0;
        sequenceMaxCount = 1;
        sequenceMaxCountSet = false;


        if(rgbColorMap.contains (color))
            rgbSequenceColor = rgbColorMap[color];
        else
        {
            QString tempCrit = "The RGB color was not found in the RGB color map. Failing Color: " + color;
            QString title = "Light Controller RGB Color Fail";
            emit ShowErrorMessage(title, tempCrit);
            return;
        }

        isCommandRunning = true;

        ShowRGBColorOneSequence(sequenceGroupList, rgbSequenceColor, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBSequenceDelayDone()));
        p_rgbTimer->setInterval (delay);
        p_rgbTimer->start();
    }
}


void LightController::FollowerRegularLights(QList<quint8> grpNumList, quint8 intensity, quint16 data)
{
    if(data > 0)
    {
        ShowRegularIntensity(grpNumList, intensity);
    }
    else
    {
        ShowRegularIntensity(grpNumList, 0);
    }
}


void LightController::FollowerRGBLights(QList<quint8> grpNumList, QString color, quint16 data)
{  
    if(data > 0)
    {
        if(rgbColorMap.contains (color))
            rgbFollowerColor = rgbColorMap[color];
        else
        {
            QString tempCrit = "The RGB color was not found in the RGB color map. Failing Color: " + color;
            emit ShowErrorMessage(title, tempCrit);
            return;
        }

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





void LightController::ShowRegularIntensity(QList<quint8> grpNumList, quint8 intensity)
{
    quint8 i;

    //qDebug() << "rgbFastMode:" << rgbFastMode;

    //For Groups
    for(i = 0; i < grpNumList.count(); i++)
    {
        //For Pins in Groups
        QList<quint8> pins = regularLEDMap[grpNumList[i]];

        //Set Regular Lights Group to Intensity
        emit SetLightIntensityGroup(id, pins, intensity);
    }
}


void LightController::ShowRegularIntensityOne(QList<quint8> grpNumList, quint8 intensity, QList<quint8> indexList, qint8 offset)
{
    quint8 i;

    //qDebug() << "rgbFastMode:" << rgbFastMode;

    //For Groups
    for(i = 0; i < grpNumList.count(); i++)
    {
        //For Pins in Groups
        QList<quint8> pins = regularLEDMap[grpNumList[i]];
        quint8 pin = pins[indexList[i]+offset]-1;

        //Set Regular Lights Group to Intensity
        emit SetLightIntensity(id, pin, intensity);
    }
}

void LightController::ShowRegularIntensityOneSequence(QList<quint8> grpNumList, quint8 intensity, quint8 index)
{
    quint8 i;

    //For Groups
    for(i = 0; i < grpNumList.count(); i++)
    {
        //For Pins in Groups
        QList pins = regularLEDMap[grpNumList[i]];
        quint8 pin;
        quint8 count = pins.count();
        bool isOutOfRange = false;

        if(!sequenceMaxCountSet)
        {
            if(count > sequenceMaxCount)
                sequenceMaxCount = count;
        }

        //Get the RGB Pin Number
        if(index < count)
            pin = pins[index]-1;
        else
            isOutOfRange = true;

        //qDebug() << "Writing to Light Controller ID:" << id << "sequenceCount" << sequenceCount << "sequenceMaxCount" << sequenceMaxCount;

        //Set RGB Lights to the Color
        if(!isOutOfRange)
            emit SetLightIntensity(id, pin, intensity);
    }

    //Incease the Count
    sequenceCount++;

    sequenceMaxCountSet = true;
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
                rgbPins.r = pins[j]-1;
                rgbPins.g = rgbPins.r + 1;
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

            rgbPins.r = pins[indexList[i]+offset]-1;
            rgbPins.g = rgbPins.r + 1;
            rgbPins.b = rgbPins.g + 1;
        }
        else if(!rgbFastMode && indexList[i]+2 < count)
        {
            qint8 newOffset = offset*3;
            rgbPins.r = pins[indexList[i]+newOffset]-1;
            rgbPins.g = pins[indexList[i]+newOffset+1]-1;
            rgbPins.b = pins[indexList[i]+newOffset+2]-1;
        }
        else
            isOutOfRange = true;

        //qDebug() << "Writing to Light Controller ID:" << id << "sequenceCount" << sequenceCount << "sequenceMaxCount" << sequenceMaxCount;

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

        if(!sequenceMaxCountSet)
        {
            if(count > sequenceMaxCount)
                sequenceMaxCount = count;
        }

        //Get the RGB Pin Number
        if(rgbFastMode && index < count)
        {
            rgbPins.r = pins[index]-1;
            rgbPins.g = rgbPins.r + 1;
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


        //qDebug() << "Writing to Light Controller ID:" << id << "sequenceCount" << sequenceCount << "sequenceMaxCount" << sequenceMaxCount;

        //Set RGB Lights to the Color
        if(!isOutOfRange)
            emit SetRGBLightIntensity(id, rgbPins, color);
    }

    if(rgbFastMode)
        sequenceCount++;
    else
        sequenceCount = sequenceCount + 3;

    sequenceMaxCountSet = true;
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
                rgbPins.r = pins[j]-1;
                rgbPins.g = rgbPins.r + 1;
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

void LightController::ResetLightController()
{
    numberFlash = 0;
    timesFlashed = 0;
    useRandomPins = false;
    useSideColor = false;

    sequenceCount = 0;
    sequenceMaxCount = 0;
    sequenceMaxCountSet = false;

    rgbFollowerFirstTime = false;

    doReset = false;

}

//Private Slots

void LightController::RGBFlashOff()
{
    //Unconnect the Timer
    disconnect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));

    if(useRandomPins)
    {
        quint8 i;
        for(i = 0; i < flashGroupList.count(); i++)
        {
            ShowRGBColorOne(flashGroupList, rgbOff, randomPins, 0);

            if(useSideColor)
            {
                ShowRGBColorOne(flashGroupList, rgbOff, randomPins, 1);
                ShowRGBColorOne(flashGroupList, rgbOff, randomPins, -1);
            }
        }
    }
    else
        ShowRGBColor(flashGroupList, rgbOff);

    //Connect Timer to Turn Off LEDs
    connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOn()));
    p_rgbTimer->setInterval (flashTimeOff);
    p_rgbTimer->start();
}


void LightController::RGBFlashOn()
{
    //Increament Flash Time when Off Happens
    timesFlashed++;

    //Unconnect the Timer
    disconnect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOn()));

    if(timesFlashed < numberFlash)
    {
        if(useRandomPins)
        {
            quint8 i;
            for(i = 0; i < flashGroupList.count(); i++)
            {
                ShowRGBColorOne(flashGroupList, rgbFlashColor, randomPins, 0);

                if(useSideColor)
                {
                    ShowRGBColorOne(flashGroupList, sideColor, randomPins, 1);
                    ShowRGBColorOne(flashGroupList, sideColor, randomPins, -1);
                }
            }
        }
        else
            ShowRGBColor(flashGroupList, rgbFlashColor);

        //Connect Timer to Turn Off LEDs
        connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
        p_rgbTimer->setInterval (flashTimeOn);
        p_rgbTimer->start();

        //Now Wait until timer runs out
    }
    else
    {
        isCommandRunning = false;
        useRandomPins = false;
        useSideColor = false;

        if(doReset)
            ResetLightController();
    }
}


void LightController::RGBSequenceDelayDone()
{
    if(sequenceCount < sequenceMaxCount)
    {
        //qDebug() << "Sequence Timer Ended: sequence" << sequenceCount << "sequenceMaxCount" << sequenceMaxCount;
        //Light Up the Next Sequence
        ShowRGBColorOneSequence(sequenceGroupList, rgbSequenceColor, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        p_rgbTimer->start();
    }
    else
    {
        //Turn Off Lights
        ShowRGBColor(sequenceGroupList, rgbOff);

        //Disconnect the Timer
        disconnect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBSequenceDelayDone()));
        isCommandRunning = false;

        if(doReset)
            ResetLightController();
    }
}




void LightController::RegularFlashOff()
{
    //Unconnect the Timer
    disconnect(p_regularTimer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));

    if(useRandomPins)
    {
        quint8 i;
        for(i = 0; i < flashGroupList.count(); i++)
        {
            ShowRegularIntensityOne(flashGroupList, 0, randomPins, 0);

            if(useSideColor)
            {
                ShowRegularIntensityOne(flashGroupList, 0, randomPins, 1);
                ShowRegularIntensityOne(flashGroupList, 0, randomPins, -1);
            }
        }
    }
    else
        ShowRegularIntensity(flashGroupList, 0);

    //Connect Timer to Turn Off LEDs
    connect(p_regularTimer, SIGNAL(timeout()), this, SLOT(RegularFlashOn()));
    p_regularTimer->setInterval (flashTimeOff);
    p_regularTimer->start();
}

void LightController::RegularFlashOn()
{
    //Increament Flash Time when Off Happens
    timesFlashed++;

    //Unconnect the Timer
    disconnect(p_regularTimer, SIGNAL(timeout()), this, SLOT(RegularFlashOn()));

    if(timesFlashed < numberFlash)
    {
        if(useRandomPins)
        {
            quint8 i;
            for(i = 0; i < flashGroupList.count(); i++)
            {
                ShowRegularIntensityOne(flashGroupList, flashIntensity, randomPins, 0);

                if(useSideColor)
                {
                    ShowRegularIntensityOne(flashGroupList, flashSideIntensity, randomPins, 1);
                    ShowRegularIntensityOne(flashGroupList, flashSideIntensity, randomPins, -1);
                }
            }
        }
        else
            ShowRegularIntensity(flashGroupList, flashIntensity);

        //Connect Timer to Turn Off LEDs
        connect(p_regularTimer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
        p_regularTimer->setInterval (flashTimeOn);
        p_regularTimer->start();

        //Now Wait until timer runs out
    }
    else
    {
        isCommandRunning = false;
        useRandomPins = false;
        useSideColor = false;

        if(doReset)
            ResetLightController();
    }
}


void LightController::RegularSequenceDelayDone()
{
    if(sequenceCount < sequenceMaxCount)
    {
        //qDebug() << "Sequence Timer Ended: sequence" << sequenceCount << "sequenceMaxCount" << sequenceMaxCount;
        //Light Up the Next Sequence
        ShowRegularIntensityOneSequence(sequenceGroupList, sequenceIntensity, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        p_regularTimer->start();
    }
    else
    {
        //Turn Off Lights
        ShowRegularIntensity(sequenceGroupList, 0);

        //Disconnect the Timer
        disconnect(p_regularTimer, SIGNAL(timeout()), this, SLOT(RegularSequenceDelayDone()));
        isCommandRunning = false;

        if(doReset)
            ResetLightController();
    }
}









