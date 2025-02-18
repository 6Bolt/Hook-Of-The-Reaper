#include "LightGun.h"
#include "../Global.h"

//Constructors

//For RS3 Reaper Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, quint16 maNumber, quint16 rvNumber)
{
    defaultLightGun = lgDefault;
    if(defaultLightGun)
        defaultLightGunNum = dlgNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgName;
    lightGunNum = lgNumber;

    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

    maxAmmo = maNumber;
    reloadValue = rvNumber;

    maxAmmoSet = true;
    reloadValueSet = true;

    isDipSwitchPlayerNumberSet = false;

    analogStrength = DEFAULTANALOGSTRENGTH;
    isAnalogStrengthSet = false;

    hubComPortNumber = UNASSIGN;

    LoadDefaultLGCommands();

}

//For Normal Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow)
{
    defaultLightGun = lgDefault;
    if(defaultLightGun)
        defaultLightGunNum = dlgNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgName;
    lightGunNum = lgNumber;

    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

    if(defaultLightGun)
    {
        if(defaultLightGunNum == RS3_REAPER)
        {
            maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
            reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
            maxAmmoSet = true;
            reloadValueSet = true;
        }
        else
        {
            maxAmmoSet = false;
            reloadValueSet = false;
        }
    }
    else
    {
        maxAmmoSet = false;
        reloadValueSet = false;
    }

    isDipSwitchPlayerNumberSet = false;
    hubComPortNumber = UNASSIGN;

    analogStrength = DEFAULTANALOGSTRENGTH;
    isAnalogStrengthSet = false;


    LoadDefaultLGCommands();
}

//For Copy Light Gun
LightGun::LightGun(LightGun const &lgMember)
{
    defaultLightGun = lgMember.defaultLightGun;
    if(defaultLightGun)
        defaultLightGunNum = lgMember.defaultLightGunNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgMember.lightGunName;
    lightGunNum = lgMember.lightGunNum;

    comPortNum = lgMember.comPortNum;
    comPortString = lgMember.comPortString;
    comPortInfo = lgMember.comPortInfo;
    comPortBaud = lgMember.comPortBaud;
    comPortDataBits = lgMember.comPortDataBits;
    comPortParity = lgMember.comPortParity;
    comPortStopBits = lgMember.comPortStopBits;
    comPortFlow = lgMember.comPortFlow;

    if(lgMember.maxAmmoSet)
    {
        maxAmmo = lgMember.maxAmmo;
        maxAmmoSet = true;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
        maxAmmoSet = true;
    }
    else
    {
        maxAmmoSet = false;
    }

    if(lgMember.reloadValueSet)
    {
        reloadValue = lgMember.reloadValue;
        reloadValueSet = true;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
        reloadValueSet = true;
    }
    else
    {
        reloadValueSet = false;
    }

    if(lgMember.isDipSwitchPlayerNumberSet)
    {
        dipSwitchPlayerNumber = lgMember.dipSwitchPlayerNumber;
        isDipSwitchPlayerNumberSet = true;
    }
    else
        isDipSwitchPlayerNumberSet = false;

    hubComPortNumber = lgMember.hubComPortNumber;

    if(lgMember.isAnalogStrengthSet)
    {
        analogStrength = lgMember.analogStrength;
        isAnalogStrengthSet = true;
    }
    else
    {
        analogStrength = DEFAULTANALOGSTRENGTH;
        isAnalogStrengthSet = false;
    }


    LoadDefaultLGCommands();

}

//For MX24 Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, bool dipSwitchSet, quint8 dipSwitchNumber, quint8 hubcpNumber)
{
    defaultLightGun = lgDefault;
    if(defaultLightGun)
        defaultLightGunNum = dlgNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgName;
    lightGunNum = lgNumber;

    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

    isDipSwitchPlayerNumberSet = dipSwitchSet;
    dipSwitchPlayerNumber = dipSwitchNumber;

    hubComPortNumber = hubcpNumber;

    if(defaultLightGun)
    {
        if(defaultLightGunNum == RS3_REAPER)
        {
            maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
            reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
            maxAmmoSet = true;
            reloadValueSet = true;
        }
        else
        {
            maxAmmoSet = false;
            reloadValueSet = false;
        }
    }
    else
    {
        maxAmmoSet = false;
        reloadValueSet = false;
    }

    analogStrength = DEFAULTANALOGSTRENGTH;
    isAnalogStrengthSet = false;



    LoadDefaultLGCommands();
}

//For JB Gun4IR
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, quint8 analStrength)
{
    defaultLightGun = lgDefault;
    if(defaultLightGun)
        defaultLightGunNum = dlgNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgName;
    lightGunNum = lgNumber;

    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

    //For MX24
    isDipSwitchPlayerNumberSet = false;
    hubComPortNumber = UNASSIGN;


    //For RS3 Reaper
    if(defaultLightGun)
    {
        if(defaultLightGunNum == RS3_REAPER)
        {
            maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
            reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
            maxAmmoSet = true;
            reloadValueSet = true;
        }
        else
        {
            maxAmmoSet = false;
            reloadValueSet = false;
        }
    }
    else
    {
        maxAmmoSet = false;
        reloadValueSet = false;
    }

    analogStrength = analStrength;
    isAnalogStrengthSet = true;

    LoadDefaultLGCommands();
}

//public member functions


//Set Member Functions

void LightGun::SetDefaultLightGun(bool lgDefault)
{
    defaultLightGun = lgDefault;
}

void LightGun::SetDefaultLightGunNumer(quint8 dlgNum)
{
    defaultLightGunNum = dlgNum;
}

void LightGun::SetLightGunName(QString lgnString)
{
    lightGunName = lgnString;
}

void LightGun::SetLightGunNumber(quint8 lgNumber)
{
    lightGunNum = lgNumber;
}

void LightGun::SetComPortNumber(quint8 cpNumber)
{
    comPortNum = cpNumber;
}

void LightGun::SetComPortString(QString cpString)
{
    comPortString = cpString;
}

void LightGun::SetComPortInfo(QSerialPortInfo cpInfo)
{
    comPortInfo = cpInfo;
}

void LightGun::SetComPortBaud(qint32 cpBaud)
{
    comPortBaud = cpBaud;
}

void LightGun::SetComPortDataBits(quint8 cpDataBits)
{
    comPortDataBits = cpDataBits;
}

void LightGun::SetComPortParity(quint8 cpParity)
{
    comPortParity = cpParity;
}

void LightGun::SetComPortStopBits(quint8 cpStopBits)
{
    comPortStopBits = cpStopBits;
}

void LightGun::SetComPortFlow(quint8 cpFlow)
{
    comPortFlow = cpFlow;
}

void LightGun::SetMaxAmmo(quint16 maNumber)
{
    maxAmmo = maNumber;
    maxAmmoSet = true;
}

void LightGun::SetReloadValue(quint16 rvNumber)
{
    reloadValue = rvNumber;
    reloadValueSet = true;
}

void LightGun::SetDipSwitchPlayerNumber(quint8 dsNumber)
{
    dipSwitchPlayerNumber = dsNumber;
    isDipSwitchPlayerNumberSet = true;
}

void LightGun::SetAnalogStrength(quint8 analStrength)
{
    analogStrength = analStrength;
    isAnalogStrengthSet = true;
}

void LightGun::SetHubComPortNumber(quint8 hcpNumber)
{
    hubComPortNumber = hcpNumber;
}

//Get Member Functions

bool LightGun::GetDefaultLightGun()
{
    return defaultLightGun;
}

quint8 LightGun::GetDefaultLightGunNumber()
{
    return defaultLightGunNum;
}

QString LightGun::GetLightGunName()
{
    return lightGunName;
}

quint8 LightGun::GetLightGunNumber()
{
    return lightGunNum;
}

quint8 LightGun::GetComPortNumber()
{
    if(defaultLightGun && defaultLightGunNum == MX24)
        return hubComPortNumber;

    return comPortNum;
}

quint8 LightGun::GetComPortNumberBypass()
{
    return comPortNum;
}

QString LightGun::GetComPortString()
{
    return comPortString;
}

QSerialPortInfo LightGun::GetComPortInfo()
{
    return comPortInfo;
}

qint32 LightGun::GetComPortBaud()
{
    return comPortBaud;
}

quint8 LightGun::GetComPortDataBits()
{
    return comPortDataBits;
}

quint8 LightGun::GetComPortParity()
{
    return comPortParity;
}

quint8 LightGun::GetComPortStopBits()
{
    return comPortStopBits;
}

quint8 LightGun::GetComPortFlow()
{
    return comPortFlow;
}

quint16 LightGun::GetMaxAmmo()
{
    return maxAmmo;
}

quint16 LightGun::GetReloadValue()
{
    return reloadValue;
}

bool LightGun::IsMaxAmmoSet()
{
    return maxAmmoSet;
}

bool LightGun::IsReloadValueSet()
{
    return reloadValueSet;
}

quint8 LightGun::GetDipSwitchPlayerNumber(bool *isSet)
{
    if(isDipSwitchPlayerNumberSet)
    {
        *isSet = true;
        return dipSwitchPlayerNumber;
    }
    else
    {
        *isSet = false;
        return UNASSIGN;
    }
}

quint8 LightGun::GetDipSwitchPlayerNumber()
{
    if(isDipSwitchPlayerNumberSet)
        return dipSwitchPlayerNumber;
    else
        return UNASSIGN;
}

bool LightGun::GetIsDipSwitchPlayerNumberSet()
{
    return isDipSwitchPlayerNumberSet;
}

quint8 LightGun::GetAnalogStrength(bool *isSet)
{
    if(isAnalogStrengthSet)
    {
        *isSet = true;
        return analogStrength;
    }
    else
    {
        *isSet = false;
        return UNASSIGN;
    }
}

quint8 LightGun::GetAnalogStrength()
{
    if(isAnalogStrengthSet)
        return analogStrength;
    else
        return UNASSIGN;
}

bool LightGun::GetIsAnalogStrengthSet()
{
    return isAnalogStrengthSet;
}

quint8 LightGun::GetHubComPortNumber()
{
    return hubComPortNumber;
}

void LightGun::CopyLightGun(LightGun const &lgMember)
{
    defaultLightGun = lgMember.defaultLightGun;
    defaultLightGunNum = lgMember.defaultLightGunNum;

    lightGunName = lgMember.lightGunName;
    lightGunNum = lgMember.lightGunNum;

    comPortNum = lgMember.comPortNum;
    comPortString = lgMember.comPortString;
    comPortInfo = lgMember.comPortInfo;
    comPortBaud = lgMember.comPortBaud;
    comPortDataBits = lgMember.comPortDataBits;
    comPortParity = lgMember.comPortParity;
    comPortStopBits = lgMember.comPortStopBits;
    comPortFlow = lgMember.comPortFlow;

    //For RS3 Reaper
    maxAmmoSet = lgMember.maxAmmoSet;
    reloadValueSet = lgMember.reloadValueSet;

    if(maxAmmoSet)
    {
        maxAmmo = lgMember.maxAmmo;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
        maxAmmoSet = true;
    }

    if(reloadValueSet)
    {
        reloadValue = lgMember.reloadValue;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
        reloadValueSet = true;
    }

    //For MX24
    isDipSwitchPlayerNumberSet = lgMember.isDipSwitchPlayerNumberSet;

    if(isDipSwitchPlayerNumberSet)
        dipSwitchPlayerNumber = lgMember.dipSwitchPlayerNumber;
    else
        dipSwitchPlayerNumber = 0;

    //For JB Gun4IR
    isAnalogStrengthSet = lgMember.isAnalogStrengthSet;

    if(isAnalogStrengthSet)
        analogStrength = lgMember.analogStrength;
    else
        analogStrength = UNASSIGN;


    currentPath = lgMember.currentPath;
    dataPath = lgMember.dataPath;
    defaultLGFilePath = lgMember.defaultLGFilePath;

    openComPortCmdsSet = lgMember.openComPortCmdsSet;
    if(openComPortCmdsSet)
        openComPortCmds = lgMember.openComPortCmds;

    closeComPortCmdsSet = lgMember.closeComPortCmdsSet;
    if(closeComPortCmdsSet)
        closeComPortCmds = lgMember.closeComPortCmds;

    damageCmdsSet = lgMember.damageCmdsSet;
    if(damageCmdsSet)
        damageCmds = lgMember.damageCmds;

    recoilCmdsSet = lgMember.recoilCmdsSet;
    if(recoilCmdsSet)
        recoilCmds = lgMember.recoilCmds;

    reloadCmdsSet = lgMember.reloadCmdsSet;
    if(reloadCmdsSet)
        reloadCmds = lgMember.reloadCmds;

    ammoCmdsSet = lgMember.ammoCmdsSet;
    if(ammoCmdsSet)
        ammoCmds = lgMember.ammoCmds;

    ammoValueCmdsSet = lgMember.ammoValueCmdsSet;
    if(ammoValueCmdsSet)
        ammoValueCmds = lgMember.ammoValueCmds;

    shakeCmdsSet = lgMember.shakeCmdsSet;
    if(shakeCmdsSet)
        shakeCmds = lgMember.shakeCmds;

    autoLedCmdsSet = lgMember.autoLedCmdsSet;
    if(autoLedCmdsSet)
        autoLedCmds = lgMember.autoLedCmds;

    aspect16x9CmdsSet = lgMember.aspect16x9CmdsSet;
    if(aspect16x9CmdsSet)
        aspect16x9Cmds = lgMember.aspect16x9Cmds;

    aspect4x3CmdsSet = lgMember.aspect4x3CmdsSet;
    if(aspect4x3CmdsSet)
        aspect4x3Cmds = lgMember.aspect4x3Cmds;

    joystickCmdsSet = lgMember.joystickCmdsSet;
    if(joystickCmdsSet)
        joystickCmds = lgMember.joystickCmds;

    keyMouseCmdsSet = lgMember.keyMouseCmdsSet;
    if(keyMouseCmdsSet)
        keyMouseCmds = lgMember.keyMouseCmds;

}


void LightGun::LoadDefaultLGCommands()
{
    QString line;
    QStringList splitLines, commands;
    quint8 numberCommands, i, equalIndex;
    quint16 lineLength;
    bool noCommands;

    openComPortCmdsSet = false;
    closeComPortCmdsSet = false;
    damageCmdsSet = false;
    recoilCmdsSet = false;
    reloadCmdsSet = false;
    ammoCmdsSet = false;
    ammoValueCmdsSet = false;
    shakeCmdsSet = false;
    autoLedCmdsSet = false;
    aspect16x9CmdsSet = false;
    aspect4x3CmdsSet = false;
    joystickCmdsSet = false;
    keyMouseCmdsSet = false;

    openComPortCmds.clear();
    closeComPortCmds.clear();
    damageCmds.clear();
    recoilCmds.clear();
    reloadCmds.clear();
    ammoCmds.clear();
    ammoValueCmds.clear();
    shakeCmds.clear();
    autoLedCmds.clear();
    aspect16x9Cmds.clear();
    aspect4x3Cmds.clear();
    joystickCmds.clear();
    keyMouseCmds.clear();

    //If Not a Default Light Gun, then make sure defaultLightGunNum is 0
    //To load in nonDefaultLG.hor file
    if(!defaultLightGun)
        defaultLightGunNum = 0;

    //Get Current Path
    //currentPath = QDir::currentPath();
    currentPath = QApplication::applicationDirPath();

    dataPath = currentPath + "/" + DATAFILEDIR;

    defaultLGFilePath = dataPath + "/" + DEFAULTLGFILENAMES_ARRAY[defaultLightGunNum];

    QFile defaultLGCmdFile(defaultLGFilePath);

    bool openFile = defaultLGCmdFile.open (QIODeviceBase::ReadOnly | QIODevice::Text);

    if(!openFile)
    {
        QString tempCrit = "Can not open Default Light Gun command file to read. Please close program and solve file problem. Might be permissions problem.\nFile: "+defaultLGFilePath;
        QMessageBox::critical (nullptr, "File Error", tempCrit, QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&defaultLGCmdFile);

    while(!in.atEnd ())
    {
        line = in.readLine();

        if(line.contains ('='))
        {
            equalIndex = line.indexOf ('=',0);
            lineLength = line.length ();

            if(lineLength == equalIndex+1)
                noCommands = true;
            else
                noCommands = false;
        }
        else
           noCommands = true;


        if(line != ENDOFFILE && !noCommands)
        {


            splitLines = line.split ('=', Qt::SkipEmptyParts);

            if(splitLines[1].contains ('|'))
                commands = SplitLoadedCommands(splitLines[1]);
            else
                commands = splitLines[1].split (' ', Qt::SkipEmptyParts);


            numberCommands = commands.length();

            //See if Commands
            if(numberCommands > 0)
            {
                //Get rid of before and after open spaces
                splitLines[0] = splitLines[0].trimmed ();

                //Now Find the
                if(splitLines[0] == OPENCOMPORTONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        openComPortCmds << commands[i];
                    }
                    openComPortCmdsSet = true;
                }
                else if(splitLines[0] == CLOSECOMPORTONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        closeComPortCmds << commands[i];
                    }
                    closeComPortCmdsSet = true;
                }
                else if(splitLines[0] == DAMAGECMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        damageCmds << commands[i];
                    }
                    damageCmdsSet = true;
                }
                else if(splitLines[0] == RECOILCMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        recoilCmds << commands[i];
                    }
                    recoilCmdsSet = true;
                }
                else if(splitLines[0] == RELOADCMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        reloadCmds << commands[i];
                    }
                    reloadCmdsSet = true;
                }
                else if(splitLines[0] == AMMOCMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        ammoCmds << commands[i];
                    }
                    ammoCmdsSet = true;
                }
                else if(splitLines[0] == AMMOVALUECMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        ammoValueCmds << commands[i];
                    }
                    ammoValueCmdsSet = true;
                }
                else if(splitLines[0] == SHAKECMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        shakeCmds << commands[i];
                    }
                    shakeCmdsSet = true;
                }
                else if(splitLines[0] == AUTOLEDCMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        autoLedCmds << commands[i];
                    }
                    autoLedCmdsSet = true;
                }
                else if(splitLines[0] == ARATIO169CMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        aspect16x9Cmds << commands[i];
                    }
                    aspect16x9CmdsSet = true;
                }
                else if(splitLines[0] == ARATIO43CMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        aspect4x3Cmds << commands[i];
                    }
                    aspect4x3CmdsSet = true;
                }
                else if(splitLines[0] == JOYMODECMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        joystickCmds << commands[i];
                    }
                    joystickCmdsSet = true;
                }
                else if(splitLines[0] == KANDMMODECMDONLY)
                {
                    for(i = 0; i < numberCommands; i++)
                    {
                        commands[i] = commands[i].trimmed ();
                        keyMouseCmds << commands[i];
                    }
                    keyMouseCmdsSet = true;
                }
            }//if(numberCommands > 1)

        }//if(line != ENDOFFILE)

    }//while(!in.atEnd ())

    //Close the File
    defaultLGCmdFile.close ();

/*
    qDebug() << openComPortCmdsSet;
    qDebug() << closeComPortCmdsSet;
    qDebug() << damageCmdsSet;
    qDebug() << recoilCmdsSet;
    qDebug() << reloadCmdsSet;
    qDebug() << ammoCmdsSet;
    qDebug() << ammoValueCmdsSet;
    qDebug() << shakeCmdsSet;
    qDebug() << autoLedCmdsSet;
    qDebug() << aspect16x9CmdsSet;
    qDebug() << aspect4x3CmdsSet;
    qDebug() << joystickCmdsSet;
    qDebug() << keyMouseCmdsSet;

    qDebug() << openComPortCmds;
    qDebug() << closeComPortCmds;
    qDebug() << damageCmds;
    qDebug() << recoilCmds;
    qDebug() << reloadCmds;
    qDebug() << ammoCmds;
    qDebug() << ammoValueCmds;
    qDebug() << shakeCmds;
    qDebug() << autoLedCmds;
    qDebug() << aspect16x9Cmds;
    qDebug() << aspect4x3Cmds;
    qDebug() << joystickCmds;
    qDebug() << keyMouseCmds;

*/
}





QStringList LightGun::SplitLoadedCommands(QString commandList)
{
    QStringList cmdSplit, returnList;
    quint8 cmdCount;
    QString goodCMD;

    cmdSplit = commandList.split ('|', Qt::SkipEmptyParts);

    if(defaultLightGunNum == MX24 && isDipSwitchPlayerNumberSet)
    {
        if(dipSwitchPlayerNumber == 0)
        {
            if(cmdSplit[0][1] == '1')
                goodCMD = cmdSplit[0].remove (0,3);
            else if (cmdSplit[1][1] == '1')
                goodCMD = cmdSplit[1].remove (0,3);
        }
        else if(dipSwitchPlayerNumber == 1)
        {
            if(cmdSplit[0][1] == '2')
                goodCMD = cmdSplit[0].remove (0,3);
            else if (cmdSplit[1][1] == '2')
                goodCMD = cmdSplit[1].remove (0,3);
        }

        returnList = goodCMD.split (' ', Qt::SkipEmptyParts);
    }

    return returnList;
}














//Default Light Gun Commands for Certain Signals
QStringList LightGun::OpenComPortCommands(bool *isSet)
{
    *isSet = openComPortCmdsSet;

    if(openComPortCmdsSet)
        return openComPortCmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}

QStringList LightGun::CloseComPortCommands(bool *isSet)
{
    *isSet = closeComPortCmdsSet;

    if(closeComPortCmdsSet)
        return closeComPortCmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}

QStringList LightGun::DamageCommands(bool *isSet)
{
    *isSet = damageCmdsSet;

    if(damageCmdsSet)
        return damageCmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}

QStringList LightGun::RecoilCommands(bool *isSet)
{
    *isSet = recoilCmdsSet;

    if(recoilCmdsSet)
        return recoilCmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}

QStringList LightGun::ReloadCommands(bool *isSet)
{
    *isSet = reloadCmdsSet;

    if(reloadCmdsSet)
        return reloadCmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}

QStringList LightGun::AmmoCommands(bool *isSet)
{
    *isSet = ammoCmdsSet;

    if(ammoCmdsSet)
        return ammoCmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}

QStringList LightGun::AmmoValueCommands(bool *isSet, quint16 ammoValue)
{

    QString tempAVS;
    QString cmdString;
    QStringList tempSL;
    quint8 cmdCount = ammoValueCmds.length ();


    *isSet = ammoValueCmdsSet;

    if(ammoValueCmdsSet)
    {

        if(maxAmmoSet && reloadValueSet)
        {
            quint16 tempAV;

            if(lastAmmoValue == 0 && ammoValue > 0)
                tempAV = reloadValue;
            else if(ammoValue > maxAmmo)
                tempAV = maxAmmo;
            else
                tempAV = ammoValue;

            tempAVS = QString::number (tempAV);
            lastAmmoValue = ammoValue;
        }
        else
        {
            tempAVS = QString::number (ammoValue);
        }


        for(quint8 i = 0; i < cmdCount; i++)
        {
            cmdString = ammoValueCmds[i];
            if(cmdString.contains (SIGNALDATAVARIBLE))
                cmdString.replace (SIGNALDATAVARIBLE,tempAVS);
            tempSL << cmdString;
        }



    }

    return tempSL;
}

QStringList LightGun::ShakeCommands(bool *isSet)
{
    *isSet = shakeCmdsSet;

    if(shakeCmdsSet)
        return shakeCmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}

QStringList LightGun::AutoLEDCommands(bool *isSet)
{
    *isSet = autoLedCmdsSet;

    if(autoLedCmdsSet)
        return autoLedCmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}

QStringList LightGun::AspectRatio16b9Commands(bool *isSet)
{
    *isSet = aspect16x9CmdsSet;

    if(aspect16x9CmdsSet)
        return aspect16x9Cmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}

QStringList LightGun::AspectRatio4b3Commands(bool *isSet)
{
    *isSet = aspect4x3CmdsSet;

    if(aspect4x3CmdsSet)
        return aspect4x3Cmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}

QStringList LightGun::JoystickModeCommands(bool *isSet)
{
    *isSet = joystickCmdsSet;

    if(joystickCmdsSet)
        return joystickCmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}


QStringList LightGun::MouseAndKeyboardModeCommands(bool *isSet)
{
    *isSet = keyMouseCmdsSet;

    if(keyMouseCmdsSet)
        return keyMouseCmds;
    else
    {
        QStringList tempSL;
        return tempSL;
    }
}













