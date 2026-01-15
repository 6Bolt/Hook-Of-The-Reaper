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

    groupFilePath = dataUltimarc.groupFile;

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

    //Load the Group File
    LoadGroupFile();

    //Regular Timer Set-up
    p_regularTimer = new QTimer(this);
    p_regularTimer->setSingleShot (true);

    //RGB Timer Set-up
    p_rgbTimer = new QTimer(this);
    p_rgbTimer->setSingleShot (true);

    if(!rgbColorMap.contains("black"))
    {
        RGBColor black;
        black.r = 0;
        black.g = 0;
        black.b = 0;
        rgbColorMap.insert("black", black);
    }
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

    //Load the Group File
    LoadGroupFile();

    //Regular Timer Set-up
    p_regularTimer = new QTimer(this);
    p_regularTimer->setSingleShot (true);

    //RGB Timer Set-up
    p_rgbTimer = new QTimer(this);
    p_rgbTimer->setSingleShot (true);

    RGBColor black;
    black.r = 0;
    black.g = 0;
    black.b = 0;
    rgbColorMap.insert("black", black);


    //connect(p_regularTimer, SIGNAL(timeout()), this, SLOT(RegularTimeOut()));
    //connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBTimeOut()));
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

    title = "Light Controller Group File Error";

    rgbOff.r = 0;
    rgbOff.g = 0;
    rgbOff.b = 0;

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
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", "Can not open light controller group data file to read. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
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
                //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
                //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        didGroupFileLoad = false;
    }
    else if(!didColorLoad)
    {
        QString tempCrit = "A RGB color failed to load. Please correct the group file for this light controller.\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        didGroupFileLoad = false;
    }
    else if(!gotData)
    {
        QString tempCrit = "No group data was loaded. Please correct the group file for this light controller.\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        didGroupFileLoad = false;
    }
    else if(usedPinsList.count() > numberLEDs)
    {
        QString tempCrit = "Too many pins were loaded into the Light Controller. Please correct the group file for this light controller.\nUsed Pins: "+QString::number(usedPinsList.count())+"File: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        groupNumberBright.isDataSet = false;
        return groupNumberBright;
    }

    groupNumber = splitData[1].toInt(&isNumber);

    if(!isNumber)
    {
        QString tempCrit = "The group number, is not a number.\nLine Number: "+QString::number(lineNumber)+"\nGroup Number "+splitData[1]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        groupNumberBright.isDataSet = false;
        return groupNumberBright;
    }

    if(groupNumber < 0 || groupNumber > 255)
    {
        QString tempCrit = "The group number, needs to be set from 0 to 255.\nLine Number: "+QString::number(lineNumber)+"\nGroup Number "+splitData[1]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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


    while(groupEnd < lineMax && (fileData[groupEnd][0] != 'G' && fileData[groupEnd][0] != 'C'))
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
                //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
                pinDataStruct.isDataSet = false;
                return pinDataStruct;
            }

            if(pinData < 1 || pinData > numberLEDsCheck)
            {
                QString tempCrit = "The pin number is out of range.\nPin Number "+splitGrpData[j]+"\nFile: "+groupFilePath;
                emit ShowErrorMessage(title, tempCrit);
                //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
                    //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
                    pinDataStruct.isDataSet = false;
                    return pinDataStruct;
                }

                /*
                if(j > 1)
                {
                    for(k = 0; k < j; k++)
                    {
                        if(grpData[k]+2 >= pin)
                        {
                            QString tempCrit = "The RGB Fast pin number (+0,+1,+2) is already in the group.\n1st Pin: "+QString::number(grpData[k])+"\n2nd Pin: "+splitGrpData[j]+"\nFile: "+groupFilePath;
                            emit ShowErrorMessage(title, tempCrit);
                            //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
                            pinDataStruct.isDataSet = false;
                            return pinDataStruct;
                        }
                    }
                }
                */

            }
            else
            {
                if(grpData.contains(pin))
                {
                    QString tempCrit = "The pin number is already in the group.\nPin Number "+splitGrpData[j]+"\nFile: "+groupFilePath;
                    emit ShowErrorMessage(title, tempCrit);
                    //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
            //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
            //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
            pinDataStruct.isDataSet = false;
            return pinDataStruct;
        }

        if(addedPins%3 != 0)
        {
            QString tempCrit = "The pins in the RGB group need to be divided by 3.\nNumber of Pins "+QString::number(addedPins)+"\nFile: "+groupFilePath;
            emit ShowErrorMessage(title, tempCrit);
            //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
            //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
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
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        return false;
    }

    if(tempColorData < 0 || tempColorData > 255)
    {
        QString tempCrit = "The RGB color red value needs to be set from 0 to 255.\nRed Color Data: "+splitData[2]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        return false;
    }

    tempColor.r = static_cast<quint8>(tempColorData);


    //Green 8bit Value
    tempColorData = splitData[3].toInt (&isNumber);

    if(!isNumber)
    {
        QString tempCrit = "The RGB color green value, is not a number.\nGreen Color Data: "+splitData[3]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        return false;
    }

    if(tempColorData < 0 || tempColorData > 255)
    {
        QString tempCrit = "The RGB color green value needs to be set from 0 to 255.\nGreen Color Data: "+splitData[3]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        return false;
    }

    tempColor.g = static_cast<quint8>(tempColorData);


    //Blue 8bit Value
    tempColorData = splitData[4].toInt (&isNumber);

    if(!isNumber)
    {
        QString tempCrit = "The RGB color blue value, is not a number.\nBlue Color Data: "+splitData[4]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        return false;
    }

    if(tempColorData < 0 || tempColorData > 255)
    {
        QString tempCrit = "The RGB color blue value needs to be set from 0 to 255.\nBlue Color Data: "+splitData[4]+"\nFile: "+groupFilePath;
        emit ShowErrorMessage(title, tempCrit);
        //QMessageBox::critical (nullptr, "Light Controller Group File Error", tempCrit, QMessageBox::Ok);
        return false;
    }

    tempColor.b = static_cast<quint8>(tempColorData);


    rgbColorMap.insert(colorName, tempColor);

    //qDebug() << "Color:" << colorName << "R:" << tempColor.r << "G:" << tempColor.g << "B:" << tempColor.b;

    return true;
}



void LightController::FlashRegularLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes)
{

}

void LightController::FlashRGBLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color)
{
    if(!isCommandRunning)
    {
        rgbFlashGroupList = grpNumList;
        rgbNumberFlash = numFlashes;
        rgbTimesFlashed = 0;
        rgbFlashTimeOn = timeOnMs;
        rgbFlashTimeOff = timeOffMs;

        if(rgbColorMap.contains (color))
            rgbFlashColor = rgbColorMap[color];
        else
        {
            QString tempCrit = "The RGB color was not found in the RGB color map. Failing Color: " + color;
            emit ShowErrorMessage(title, tempCrit);
            //QMessageBox::critical (nullptr, "Light Controller RGB Color Fail", tempCrit, QMessageBox::Ok);
            return;
        }

        isCommandRunning = true;

        ShowRGBColor(rgbFlashGroupList, rgbFlashColor);

        //Connect Timer to Turn Off LEDs
        connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
        p_rgbTimer->setInterval (rgbFlashTimeOn);
        p_rgbTimer->start();

        //Now Wait until timer runs out
    }
}


void LightController::FlashRandomRGBLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color)
{
    if(!isCommandRunning)
    {
        rgbFlashGroupList = grpNumList;
        rgbNumberFlash = numFlashes;
        rgbTimesFlashed = 0;
        rgbFlashTimeOn = timeOnMs;
        rgbFlashTimeOff = timeOffMs;
        randomPins.clear();

        if(rgbColorMap.contains (color))
            rgbFlashColor = rgbColorMap[color];
        else
        {
            QString tempCrit = "The RGB color was not found in the RGB color map. Failing Color: " + color;
            emit ShowErrorMessage(title, tempCrit);
            //QMessageBox::critical (nullptr, "Light Controller RGB Color Fail", tempCrit, QMessageBox::Ok);
            return;
        }

        isCommandRunning = true;
        useRandomPins = true;

        //Find Random Pin for Groups
        quint8 i;
        for(i = 0; i < rgbFlashGroupList.count(); i++)
        {
            quint8 count = rgbLEDMap[rgbFlashGroupList[i]].count();

            if(rgbFastMode)
                count--;
            else
            {
                count = count / 3;
                count = count - 1;
            }

            quint8 randomPin = QRandomGenerator::global()->bounded(0, count);

            if(!rgbFastMode)
                randomPin = randomPin * 3;

            randomPins << randomPin;

            ShowRGBColorOne(rgbFlashGroupList, rgbFlashColor, randomPin);
        }

        //Connect Timer to Turn Off LEDs
        connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
        p_rgbTimer->setInterval (rgbFlashTimeOn);
        p_rgbTimer->start();

        //Now Wait until timer runs out

    }
}

void LightController::FlashRandomRGB2CLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color, QString sColor)
{
    if(!isCommandRunning)
    {
        rgbFlashGroupList = grpNumList;
        rgbNumberFlash = numFlashes;
        rgbTimesFlashed = 0;
        rgbFlashTimeOn = timeOnMs;
        rgbFlashTimeOff = timeOffMs;
        randomPins.clear();

        if(rgbColorMap.contains (color))
            rgbFlashColor = rgbColorMap[color];
        else
        {
            QString tempCrit = "The RGB color was not found in the RGB color map. Failing Color: " + color;
            emit ShowErrorMessage(title, tempCrit);
            //QMessageBox::critical (nullptr, "Light Controller RGB Color Fail", tempCrit, QMessageBox::Ok);
            return;
        }

        sideColor = rgbColorMap[sColor];

        isCommandRunning = true;
        useRandomPins = true;
        useSideColor = true;

        //Find Random Pin for Groups
        quint8 i;
        quint8 offset = 1;
        for(i = 0; i < rgbFlashGroupList.count(); i++)
        {
            quint8 count = rgbLEDMap[rgbFlashGroupList[i]].count();
            quint8 randomPin;

            if(rgbFastMode)
                count--;
            else
            {
                count = count / 3;
                count = count - 1;
                offset = 3;
            }

            randomPin = QRandomGenerator::global()->bounded(1, count);

            if(!rgbFastMode)
                randomPin = randomPin * 3;

            randomPins << randomPin;

            //qDebug() << "Group:" << i << "Random Pin:" << randomPin << "Count" << count;

            ShowRGBColorOne(rgbFlashGroupList, rgbFlashColor, randomPin);
            ShowRGBColorOne(rgbFlashGroupList, sideColor, randomPin-offset);
            ShowRGBColorOne(rgbFlashGroupList, sideColor, randomPin+offset);
        }

        //Connect Timer to Turn Off LEDs
        connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
        p_rgbTimer->setInterval (rgbFlashTimeOn);
        p_rgbTimer->start();

        //Now Wait until timer runs out

    }
}



void LightController::SequenceRGBLights(QList<quint8> grpNumList, quint16 delay, QString color)
{
    if(!isCommandRunning)
    {
        rgbFlashGroupList = grpNumList;
        sequenceDelay = delay;
        sequenceCount = 0;
        sequenceMaxCount = 1;
        sequenceMaxCountSet = false;


        if(rgbColorMap.contains (color))
            rgbFlashColor = rgbColorMap[color];
        else
        {
            QString tempCrit = "The RGB color was not found in the RGB color map. Failing Color: " + color;
            QString title = "Light Controller RGB Color Fail";
            emit ShowErrorMessage(title, tempCrit);
            //QMessageBox::critical (nullptr, "Light Controller RGB Color Fail", tempCrit, QMessageBox::Ok);
            return;
        }

        isCommandRunning = true;

        ShowRGBColorOne(rgbFlashGroupList, rgbFlashColor, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(SequenceDelayDone()));
        p_rgbTimer->setInterval (delay);
        p_rgbTimer->start();
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
        quint8 red, green, blue;

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


void LightController::ShowRGBColorOne(QList<quint8> grpNumList, RGBColor color, quint8 index)
{
    quint8 i, j;

    //For Groups
    for(i = 0; i < grpNumList.count(); i++)
    {
        //For Pins in Groups
        QList pins = rgbLEDMap[grpNumList[i]];
        RGBPins rgbPins;
        quint8 red, green, blue;
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
            sequenceCount++;
        }
        else if(!rgbFastMode && index+2 < count)
        {
            rgbPins.r = pins[index]-1;
            rgbPins.g = pins[index+1]-1;
            rgbPins.b = pins[index+2]-1;
            sequenceCount = sequenceCount + 3;
        }
        else
            isOutOfRange = true;


        //qDebug() << "Writing to Light Controller ID:" << id;

        //Set RGB Lights to the Color
        if(!isOutOfRange)
            emit SetRGBLightIntensity(id, rgbPins, color);
    }

    sequenceMaxCountSet = true;
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


//Private Slots

void LightController::RGBFlashOff()
{
    //Unconnect the Timer
    disconnect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));

    if(useRandomPins)
    {
        quint8 i;
        for(i = 0; i < rgbFlashGroupList.count(); i++)
        {
            ShowRGBColorOne(rgbFlashGroupList, rgbOff, randomPins[i]);

            if(useSideColor)
            {
                quint8 offset = 1;
                if(!rgbFastMode)
                    offset = 3;

                ShowRGBColorOne(rgbFlashGroupList, rgbOff, randomPins[i]-offset);
                ShowRGBColorOne(rgbFlashGroupList, rgbOff, randomPins[i]+offset);
            }
        }
    }
    else
        ShowRGBColor(rgbFlashGroupList, rgbOff);

    //Connect Timer to Turn Off LEDs
    connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOn()));
    p_rgbTimer->setInterval (rgbFlashTimeOff);
    p_rgbTimer->start();
}


void LightController::RGBFlashOn()
{
    //Increament Flash Time when Off Happens
    rgbTimesFlashed++;

    //Unconnect the Timer
    disconnect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOn()));

    if(rgbTimesFlashed < rgbNumberFlash)
    {
        if(useRandomPins)
        {
            quint8 i;
            for(i = 0; i < rgbFlashGroupList.count(); i++)
            {
                ShowRGBColorOne(rgbFlashGroupList, rgbFlashColor, randomPins[i]);

                if(useSideColor)
                {
                    quint8 offset = 1;
                    if(!rgbFastMode)
                        offset = 3;

                    ShowRGBColorOne(rgbFlashGroupList, sideColor, randomPins[i]-offset);
                    ShowRGBColorOne(rgbFlashGroupList, sideColor, randomPins[i]+offset);
                }
            }
        }
        else
            ShowRGBColor(rgbFlashGroupList, rgbFlashColor);

        //Connect Timer to Turn Off LEDs
        connect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
        p_rgbTimer->setInterval (rgbFlashTimeOn);
        p_rgbTimer->start();

        //Now Wait until timer runs out
    }
    else
    {
        isCommandRunning = false;
        useRandomPins = false;
        useSideColor = false;
    }
}


void LightController::SequenceDelayDone()
{
    if(sequenceCount < sequenceMaxCount)
    {
        //Light Up the Next Sequence
        ShowRGBColorOne(rgbFlashGroupList, rgbFlashColor, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        p_rgbTimer->start();
    }
    else
    {
        //Turn Off Lights
        ShowRGBColor(rgbFlashGroupList, rgbColorMap["black"]);

        //Disconnect the Timer
        disconnect(p_rgbTimer, SIGNAL(timeout()), this, SLOT(SequenceDelayDone()));
        isCommandRunning = false;
    }
}







