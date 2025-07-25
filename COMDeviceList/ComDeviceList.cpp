#include "ComDeviceList.h"

//Constructor
ComDeviceList::ComDeviceList()
{
    quint8 i;
    bool canMKDIR = true;

    //Set Defaults
    numberLightGuns = 0;
    numberComPortDevices = 0;
    useDefaultLGFirst = true;

    //Set Settings to Defaults
    useDefaultLGFirst = true;
    useMultiThreading = true;
    refreshTimeDisplay = DEFAULTREFRESHDISPLAY;
    closeComPortGameExit = true;
    ignoreUselessDLGGF = false;
    bypassSerialWriteChecks = false;
    disbleReaperLEDs = false;
    displayAmmoPriority = true;
    displayLifePriority = false;
    displayOtherPriority = false;
    enableNewGameFileCreation = false;
    enableReaperAmmo0Delay = true;
    repearAmmo0Delay = DEFAULTAMMO0DELAY;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;

    //More Set Defaults
    for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
    {
        availableComPorts[comPortIndx] = true;
        p_lightGunList[comPortIndx] = nullptr;
        p_comPortDeviceList[comPortIndx] = nullptr;
    }

    //More Set Defaults
    for(i = 0; i < MAXPLAYERLIGHTGUNS; i++)
    {
        playersLightGun[i] = UNASSIGN;
    }

    //Get Current Path
    //currentPath = QDir::currentPath();
    currentPath = QApplication::applicationDirPath();

    //qDebug() << currentPath;
    currentPathDir.setPath (currentPath);

    //Check if data Directory Exsits
    dataDirExists = currentPathDir.exists (DATAFILEDIR);

    //If Not, then Make Directory
    if(!dataDirExists)
    {
        //qDebug() << "Dir DOES NOT exists";
        currentPathDir.mkdir (DATAFILEDIR);
    }

    dataPath = currentPath + "/" + DATAFILEDIR;

    //Define Save Files and Their Paths
    lightGunsSaveFile = dataPath + "/" + LIGHTGUNSAVEFILE;
    comDevicesSaveFile = dataPath + "/" + COMDEVICESAVEFILE;
    settingsSaveFile = dataPath + "/" + SETTINGSSAVEFILE;
    playersAssSaveFile = dataPath + "/" + PASAVEFILE;

    //Checks For INI Directory
    iniDirExists = currentPathDir.exists (INIFILEDIR);


    //If Not, then Make Directory
    if(!iniDirExists)
        canMKDIR = currentPathDir.mkdir (INIFILEDIR);
    else
        canMKDIR = true;

    //qDebug() << "Current Path: " << currentPath;
    //qDebug() << "iniDirExists: " << iniDirExists << " canMKDIR: " << canMKDIR;

    if(!canMKDIR)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not make the directory ini. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    currentPathDir.setPath (currentPath+"/"+INIFILEDIR);

    //Checks For MAME Directory
    iniMAMEDirExists = currentPathDir.exists (INIMAMEFILEDIR);

    //If Not, then Make Directory
    if(!iniMAMEDirExists)
        currentPathDir.mkdir (INIMAMEFILEDIR);

    //Set Dir back to Current Path
    currentPathDir.setPath (currentPath);

    //Load Light Gun Data
    LoadLightGunList();

    //Load COM Device Data
    LoadComDeviceList();

    //Load Seetings
    LoadSettings();

}

//Deconstructor
ComDeviceList::~ComDeviceList()
{
    //Clean Up Pointers
    for(quint8 deleteIndx=0;deleteIndx<MAXCOMPORTS;deleteIndx++)
    {
        if(p_lightGunList[deleteIndx] != nullptr)
            delete p_lightGunList[deleteIndx];
        if(p_comPortDeviceList[deleteIndx] != nullptr)
            delete p_comPortDeviceList[deleteIndx];
    }
}

//Add Light Gun to the List
//Copy Light Gun
void ComDeviceList::AddLightGun(LightGun const &lgMember)
{
    quint8 usedComPortNum;

    p_lightGunList[numberLightGuns] = new LightGun(lgMember);

    usedComPortNum = p_lightGunList[numberLightGuns]->GetComPortNumberBypass ();
    availableComPorts[usedComPortNum] = false;

    numberLightGuns++;
}

//For RS3 Reaper Light Gun
void ComDeviceList::AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, quint16 maNumber, quint16 rvNumber, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    quint8 usedComPortNum;

    p_lightGunList[numberLightGuns] = new LightGun(lgDefault, dlgNum, lgName, lgNumber, cpNumber, cpString, cpInfo, cpBaud, cpDataBits, cpParity, cpStopBits, cpFlow, maNumber, rvNumber, lgRecoils, reloadNR, reloadDis);

    usedComPortNum = p_lightGunList[numberLightGuns]->GetComPortNumberBypass ();
    availableComPorts[usedComPortNum] = false;

    numberLightGuns++;
}

//For Normal Light Gun
void ComDeviceList::AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    quint8 usedComPortNum;

    p_lightGunList[numberLightGuns] = new LightGun(lgDefault, dlgNum, lgName, lgNumber, cpNumber, cpString, cpInfo, cpBaud, cpDataBits, cpParity, cpStopBits, cpFlow, lgRecoils, reloadNR, reloadDis);

    usedComPortNum = p_lightGunList[numberLightGuns]->GetComPortNumberBypass ();
    availableComPorts[usedComPortNum] = false;

    numberLightGuns++;
}

//For MX24 Light Gun
void ComDeviceList::AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, bool dipSwitchSet, quint8 dipSwitchNumber, quint8 hcpNum, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    quint8 usedComPortNum;

    p_lightGunList[numberLightGuns] = new LightGun(lgDefault, dlgNum, lgName, lgNumber, cpNumber, cpString, cpInfo, cpBaud, cpDataBits, cpParity, cpStopBits, cpFlow, dipSwitchSet, dipSwitchNumber, hcpNum, lgRecoils, reloadNR, reloadDis);

    //Remove Hub COM port From List
    availableComPorts[hcpNum] = false;


    //Set Up DIP Switch Player for Hub Com Port
    if(dipSwitchSet)
    {
        if(usedHubComDipPlayers.contains (hcpNum))
            usedHubComDipPlayers[hcpNum][dipSwitchNumber] = true;
        else
        {
            QList<bool> tempDP;
            for(quint8 i = 0; i < DIPSWITCH_NUMBER; i++)
            {
                if(i == dipSwitchNumber)
                    tempDP << true;
                else
                    tempDP << false;
            }
            usedHubComDipPlayers.insert(hcpNum, tempDP);
        }
    }
    numberLightGuns++;
}

//For JB Gun4IR Light Gun
void ComDeviceList::AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow, quint8 analStrength, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    quint8 usedComPortNum;

    p_lightGunList[numberLightGuns] = new LightGun(lgDefault, dlgNum, lgName, lgNumber, cpNumber, cpString, cpInfo, cpBaud, cpDataBits, cpParity, cpStopBits, cpFlow, analStrength, lgRecoils, reloadNR, reloadDis);

    usedComPortNum = p_lightGunList[numberLightGuns]->GetComPortNumberBypass ();
    availableComPorts[usedComPortNum] = false;

    numberLightGuns++;
}


//For Alien USB Light Gun
void ComDeviceList::AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    p_lightGunList[numberLightGuns] = new LightGun(lgDefault, dlgNum, lgName, lgNumber, hidInfoStruct, lgRecoils, reloadNR, reloadDis);

    numberLightGuns++;
}

//For USB Light Gun
void ComDeviceList::AddLightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, quint16 rcDelay, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    p_lightGunList[numberLightGuns] = new LightGun(lgDefault, dlgNum, lgName, lgNumber, hidInfoStruct, rcDelay, lgRecoils, reloadNR, reloadDis);

    numberLightGuns++;
}



//Add COM Devices to the List
void ComDeviceList::AddComPortDevice(ComPortDevice const &cpdMember)
{
    quint8 usedComPortNum;

    p_comPortDeviceList[numberComPortDevices] = new ComPortDevice(cpdMember);

    usedComPortNum = p_comPortDeviceList[numberComPortDevices]->GetComPortNumber ();
    availableComPorts[usedComPortNum] = false;

    numberComPortDevices++;
}

void ComDeviceList::AddComPortDevice(QString cpdString, quint8 cpdNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, quint32 cpBaud, quint16 cpDataBits, quint16 cpParity, quint16 cpStopBits, quint16 cpFlow)
{
    quint8 usedComPortNum;

    p_comPortDeviceList[numberComPortDevices] = new ComPortDevice(cpdString, cpdNumber, cpNumber, cpString, cpInfo, cpBaud, cpDataBits, cpParity, cpStopBits, cpFlow);

    usedComPortNum = p_comPortDeviceList[numberComPortDevices]->GetComPortNumber ();
    availableComPorts[usedComPortNum] = false;

    numberComPortDevices++;
}

void ComDeviceList::CopyAvailableComPortsArray(bool *targetArray, quint8 size)
{
    for (quint8 i = 0; i < size; i++)
    {
        *(targetArray + i) = *(availableComPorts + i);
    }
}

//Get Numbers in Certain Lists
quint8 ComDeviceList::GetNumberLightGuns()
{
    return numberLightGuns;
}

quint8 ComDeviceList::GetNumberComPortDevices()
{
    return numberComPortDevices;
}

//Switch Available COM Port
void ComDeviceList::SwitchComPortsInList(quint8 oldPort, quint8 newPort)
{
    if(oldPort != UNASSIGN)
        availableComPorts[oldPort] = true;

    if(newPort != UNASSIGN)
        availableComPorts[newPort] = false;
}

void ComDeviceList::ModifyComPortArray(quint8 index, bool valueBool)
{
    availableComPorts[index] = valueBool;
}


//Delete a Certain Light Gun in the List
void ComDeviceList::DeleteLightGun(quint8 lgNumber)
{
    quint8 index;
    quint8 openComPort;

    openComPort = p_lightGunList[lgNumber]->GetComPortNumberBypass ();
    if(openComPort != UNASSIGN)
        availableComPorts[openComPort] = true;

    //Check if it is MX24, for Used Dip Switch Player
    bool isDefaultLG = p_lightGunList[lgNumber]->GetDefaultLightGun ();

    if(isDefaultLG)
    {
        quint8 defaultLGNum = p_lightGunList[lgNumber]->GetDefaultLightGunNumber ();

        if(defaultLGNum == MX24)
        {
            bool isSet;
            quint8 dipSwNumber = p_lightGunList[lgNumber]->GetDipSwitchPlayerNumber (&isSet);
            quint8 hubCPNum = p_lightGunList[lgNumber]->GetHubComPortNumber ();
            if(isSet)
                ChangeUsedDipPlayersArray(hubCPNum, dipSwNumber, false);
        }
    }

    //Only One Light Gun In the List
    if(numberLightGuns == 1 && lgNumber == 0)
    {
        delete p_lightGunList[lgNumber];
        numberLightGuns--;
        p_lightGunList[numberLightGuns] = nullptr;

        for(index = 0; index < MAXPLAYERLIGHTGUNS; index++)
        {
            playersLightGun[index] = UNASSIGN;
        }

    }
    else if((lgNumber+1) == numberLightGuns)
    {
        //Targeted Light Gun is the Last in the List. So Don't have to Move Light guns Around
        delete p_lightGunList[lgNumber];
        numberLightGuns--;
        p_lightGunList[numberLightGuns] = nullptr;

        for(index = 0; index < MAXPLAYERLIGHTGUNS; index++)
        {
            if(playersLightGun[index] == lgNumber)
                playersLightGun[index] = UNASSIGN;
        }
    }
    else if(lgNumber < numberLightGuns)
    {
        //Move Higher Light Guns Down, After the Deleted Targeted Light Gun
        for(index = lgNumber; index < (numberLightGuns-1); index++)
        {
            p_lightGunList[index]->CopyLightGun (*p_lightGunList[(index+1)]);
        }

        numberLightGuns--;
        delete p_lightGunList[numberLightGuns];
        p_lightGunList[numberLightGuns] = nullptr;


        for(index = 0; index < MAXPLAYERLIGHTGUNS; index++)
        {
            if(playersLightGun[index] == lgNumber)
                playersLightGun[index] = UNASSIGN;
            else if(playersLightGun[index] > lgNumber && playersLightGun[index] != UNASSIGN)
                playersLightGun[index]--;
        }
    }

}

//Delete a Certain COM Device in the List
void ComDeviceList::DeleteComDevice(quint8 cdNumber)
{
    quint8 index;
    quint8 openComPort;

    openComPort = p_comPortDeviceList[cdNumber]->GetComPortNumber ();
    availableComPorts[openComPort] = true;

    //Only One COM Device In the List
    if(numberComPortDevices == 1 && cdNumber == 0)
    {
        delete p_comPortDeviceList[cdNumber];
        p_comPortDeviceList[numberComPortDevices] = nullptr;
        numberComPortDevices--;
    }
    else if((cdNumber+1) == numberComPortDevices)
    {
        //Targeted COM Device is the Last in the List. So Don't have to Move COM Devices Around
        delete p_comPortDeviceList[cdNumber];
        p_comPortDeviceList[numberComPortDevices] = nullptr;
        numberComPortDevices--;
    }
    else if(cdNumber < numberComPortDevices)
    {
        //Move Higher COM Devices Down 1, After the Deleted Target COM Device
        for(index = cdNumber; index < (numberComPortDevices-1); index++)
        {
            p_comPortDeviceList[index]->CopyComDevice (*p_comPortDeviceList[(index+1)]);
        }

        delete p_comPortDeviceList[numberComPortDevices];
        p_comPortDeviceList[numberComPortDevices] = nullptr;
        numberComPortDevices--;
    }
}

//Players Light Gun Assignment Functions
bool ComDeviceList::AssignPlayerLightGun(quint8 playerNum, quint8 lgNum)
{
    quint8 i;
    quint8 count = 0;

    //Check if Player Number is Higher Than the Max Player
    if(playerNum >= MAXPLAYERLIGHTGUNS || lgNum == UNASSIGN)
        return false;

    //Check other Players to see if that Light Gun has Been Assign Already
    for(i = 0; i < MAXPLAYERLIGHTGUNS; i++)
    {
        if(i != playerNum)
        {
            if(playersLightGun[i] == lgNum)
            {
                if(count > 2)
                    return false;

                //Different Player Already Assign to that Light Gun. Make Old Player UNASSIGN.
                playersLightGun[i] = UNASSIGN;
                count++;
            }
        }
    }

    //Ran the Ganulet
    playersLightGun[playerNum] = lgNum;


    return true;

}

void ComDeviceList::DeassignPlayerLightGun(quint8 playerNum)
{
    playersLightGun[playerNum] = UNASSIGN;
}

quint8 ComDeviceList::GetPlayerLightGunAssignment(quint8 playerNum)
{
    if(playerNum < MAXPLAYERLIGHTGUNS)
    {
        return playersLightGun[playerNum];
    }

    return UNASSIGN;
}

//Save & Load Light guns from/to File
void ComDeviceList::SaveLightGunList()
{
    bool removedFile = true;
    bool openFile;
    quint8 i;
    QFile saveLGData(lightGunsSaveFile);

    //If Light Gun Number is Zero, then Erase Old Save File & Done
    if(numberLightGuns == 0)
    {
        SavePlayersAss();

        if(saveLGData.exists ())
            removedFile = saveLGData.remove ();

        if(!removedFile)
        {
            QMessageBox::critical (nullptr, "File Error", "Can not remove old light gun save data file. Failed to remove. Please close program and solve file problem. Might be open or permissions changed.", QMessageBox::Ok);
            return;
        }

        return;
    }

    //Erase Old Save File, if it exisits
    if(saveLGData.exists ())
        removedFile = saveLGData.remove ();

    if(!removedFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not remove old light gun save data file. Failed to remove. Please close program and solve file problem. Might be open or permissions changed.", QMessageBox::Ok);
        return;
    }

    openFile = saveLGData.open(QIODeviceBase::WriteOnly | QIODevice::Text);

    if(!openFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not create light gun save data file. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream out(&saveLGData);

    out << STARTLIGHTGUNSAVEFILE << "\n";
    out << numberLightGuns << "\n";


    for(i = 0; i < numberLightGuns; i++)
    {

        out << LIGHTGUNNUMBERFILE << i << "\n";

        //Light Gun Number
        out << p_lightGunList[i]->GetLightGunNumber()<< "\n";

        //Light Gun Name
        out << p_lightGunList[i]->GetLightGunName ()<< "\n";

        //Default Light Gun
        if(p_lightGunList[i]->GetDefaultLightGun())
        {
            out << "1\n";
            out << p_lightGunList[i]->GetDefaultLightGunNumber() << "\n";
        }
        else
        {
            out << "0\n";
            out << "0\n";
        }

        //Recoil Priorites & Reload Options
        quint8 *p_recoilPriority = p_lightGunList[i]->GetRecoilPriority();

        bool reloadNR = p_lightGunList[i]->GetReloadNoRumble();
        bool reloadDis = p_lightGunList[i]->GetReloadDisabled();

        for(quint8 j = 0; j < NUMBEROFRECOILS; j++)
            out << p_recoilPriority[j] << "\n";

        if(reloadNR)
            out << "1\n";
        else
             out << "0\n";

        if(reloadDis)
            out << "1\n";
        else
            out << "0\n";

        //Check if Ligth Gun is a Serial COM Port
        if(!p_lightGunList[i]->IsLightGunUSB ())
        {

            //COM Port Data
            out << p_lightGunList[i]->GetComPortNumberBypass() << "\n";
            out << p_lightGunList[i]->GetComPortString() << "\n";
            out << p_lightGunList[i]->GetComPortBaud() << "\n";
            out << p_lightGunList[i]->GetComPortDataBits() << "\n";
            out << p_lightGunList[i]->GetComPortParity() << "\n";
            out << p_lightGunList[i]->GetComPortStopBits() << "\n";
            out << p_lightGunList[i]->GetComPortFlow() << "\n";


            if(p_lightGunList[i]->GetDefaultLightGun() && p_lightGunList[i]->GetDefaultLightGunNumber () == RS3_REAPER)
            {
                bool mxAmmoSet = p_lightGunList[i]->IsMaxAmmoSet();
                bool rlValueSet = p_lightGunList[i]->IsReloadValueSet();

                if(mxAmmoSet)
                {
                    out << "1\n";
                    out << p_lightGunList[i]->GetMaxAmmo() << "\n";
                }
                else
                {
                    out << "0\n";
                    out << "69\n";
                }

                if(rlValueSet)
                {
                    out << "1\n";
                    out << p_lightGunList[i]->GetReloadValue() << "\n";
                }
                else
                {
                    out << "0\n";
                    out << "69\n";
                }
            }
            else if(p_lightGunList[i]->GetDefaultLightGun() && p_lightGunList[i]->GetDefaultLightGunNumber () == MX24)
            {
                bool isDipSet;
                quint8 dipNumber = p_lightGunList[i]->GetDipSwitchPlayerNumber (&isDipSet);

                if(isDipSet)
                {
                    out << "1\n";
                    out << dipNumber << "\n";
                }
                else
                {
                    out << "0\n";
                    out << "69\n";
                }

                out << p_lightGunList[i]->GetHubComPortNumber () << "\n";
            }
            else if(p_lightGunList[i]->GetDefaultLightGun() && (p_lightGunList[i]->GetDefaultLightGunNumber () == JBGUN4IR || p_lightGunList[i]->GetDefaultLightGunNumber () == OPENFIRE))
            {
                bool isAnalSet;
                quint8 analNumber = p_lightGunList[i]->GetAnalogStrength (&isAnalSet);

                if(isAnalSet)
                {
                    out << "1\n";
                    out << analNumber << "\n";
                }
                else
                {
                    out << "0\n";
                    out << "69\n";
                }
            }

        } //Light Gun is a USB Light Gun
        else
        {
            HIDInfo tempHIDInfo = p_lightGunList[i]->GetUSBHIDInfo ();

            out << tempHIDInfo.path << "\n";
            out << tempHIDInfo.vendorID << "\n";
            out << tempHIDInfo.vendorIDString << "\n";
            out << tempHIDInfo.productID << "\n";
            out << tempHIDInfo.productIDString << "\n";
            out << tempHIDInfo.serialNumber << "\n";
            out << tempHIDInfo.releaseNumber << "\n";
            out << tempHIDInfo.releaseString << "\n";
            out << tempHIDInfo.manufacturer << "\n";
            out << tempHIDInfo.productDiscription << "\n";
            out << tempHIDInfo.usagePage << "\n";
            out << tempHIDInfo.usage << "\n";
            out << tempHIDInfo.usageString << "\n";
            out << tempHIDInfo.interfaceNumber << "\n";
        }
    }

    out << ENDOFFILE;

    //Close File
    saveLGData.close ();

    SavePlayersAss();

}

void ComDeviceList::LoadLightGunList()
{
    bool fileExists;
    bool openFile;
    quint8 i;

    quint8 tempNumLightGuns;
    quint8 tenpLightGunNum;
    QString tempLightGunName;
    bool tempIsDefaultGun;
    quint8 tempDefaultGunNum;
    quint8 tempComPortNum;
    QString tempComPortName;

    quint32 tempComPortBaud;
    quint16 tempComPortDataBits;
    quint16 tempComPortParity;
    quint16 tempComPortStopBits;
    quint16 tempComPortFlow;
    quint16 tempMaxAmmo;
    quint16 tempReloadValue;
    bool tempMaxAmmoSet;
    bool tempReloadValueSet;
    QString line, cmpLine;
    bool dipSet, analSet;
    quint8 dipNumber, analNumber, hcpNumber;
    SupportedRecoils recoilPriority;
    bool reloadNR;
    bool reloadDis;

    QFile loadLGData(lightGunsSaveFile);

    //Check if the File Exists, as it might not be created yet. If no File, then exit out of member function
    if(loadLGData.exists() == false)
    {
        //QMessageBox::critical (nullptr, "File Error", "Can not open light gun save data file. It does not exists. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    openFile = loadLGData.open (QIODeviceBase::ReadOnly | QIODevice::Text);

    if(!openFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not open light gun save data file to read. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&loadLGData);

    //Check First Line for title
    line = in.readLine();


    if(line != STARTLIGHTGUNSAVEFILE)
    {
        //qDebug() << line;
        QMessageBox::critical (nullptr, "File Error", "Light gun save data file is corrupted. Please try to reload file, or re-enter the light guns again.", QMessageBox::Ok);
    }

    //Next Line is Number of Light Guns
    line = in.readLine();
    tempNumLightGuns = line.toUInt ();

    for(i = 0; i < tempNumLightGuns; i++)
    {
        QSerialPortInfo *p_tempComPortInfo;

        line = in.readLine();

        //Line Should be "Light Gun #i"
        cmpLine = LIGHTGUNNUMBERFILE + QString::number (i);

        if(line != cmpLine)
        {
            //qDebug() << line;
            QMessageBox::critical (nullptr, "File Error", "Light gun save data file is corrupted. Please try to reload file, or re-enter the light guns again.", QMessageBox::Ok);
        }

        //Next Line is the Light Gun Number
        line = in.readLine();
        tenpLightGunNum = line.toUInt ();

        //Light Gun Name
        tempLightGunName = in.readLine();

        //Default Light Gun
        line = in.readLine();
        if(line == "0")
            tempIsDefaultGun = false;
        else
            tempIsDefaultGun = true;

        //Default Light Gun Number
        line = in.readLine();
        tempDefaultGunNum = line.toUInt ();

        //Recoil Priority

        //Ammo_Value Priority
        line = in.readLine();
        recoilPriority.ammoValue = line.toUInt ();

        //Recoil Priority
        line = in.readLine();
        recoilPriority.recoil = line.toUInt ();

        //Recoil_R2S Priority
        line = in.readLine();
        recoilPriority.recoilR2S = line.toUInt ();

        //Recoil_Value Priority
        line = in.readLine();
        recoilPriority.recoilValue = line.toUInt ();

        //Reload Options
        line = in.readLine();
        if(line == "0")
            reloadNR = false;
        else
            reloadNR = true;

        line = in.readLine();
        if(line == "0")
            reloadDis = false;
        else
            reloadDis = true;


        //For Serial Port Light Guns
        if(!tempIsDefaultGun || (tempIsDefaultGun && (tempDefaultGunNum != ALIENUSB && tempDefaultGunNum != AIMTRAK)))
        {

            //COM Port Number
            line = in.readLine();
            tempComPortNum = line.toUInt ();

            //COM Port Name
            tempComPortName = in.readLine();

            //COM Port Info

            p_tempComPortInfo = new QSerialPortInfo(tempComPortName);

            //COM Port BAUD
            line = in.readLine();
            tempComPortBaud = line.toUInt ();

            //COM Port Data Bits
            line = in.readLine();
            tempComPortDataBits = line.toUInt ();

            //COM Port Parity
            line = in.readLine();
            tempComPortParity = line.toUInt ();

            //COM Port Stop Bits
            line = in.readLine();
            tempComPortStopBits = line.toUInt ();

            //COM Port Flow
            line = in.readLine();
            tempComPortFlow = line.toUInt ();

            if(tempIsDefaultGun && tempDefaultGunNum==RS3_REAPER)
            {
                line = in.readLine();

                if(line == "1")
                    tempMaxAmmoSet = true;
                else
                    tempMaxAmmoSet = false;

                line = in.readLine();
                tempMaxAmmo = line.toUInt ();

                line = in.readLine();

                if(line == "1")
                    tempReloadValueSet = true;
                else
                    tempReloadValueSet = false;

                line = in.readLine();
                tempReloadValue = line.toUInt ();


                if(tempMaxAmmoSet && tempReloadValueSet)
                    AddLightGun(tempIsDefaultGun, tempDefaultGunNum, tempLightGunName, tenpLightGunNum, tempComPortNum, tempComPortName, *p_tempComPortInfo, tempComPortBaud, tempComPortDataBits, tempComPortParity, tempComPortStopBits, tempComPortFlow, tempMaxAmmo, tempReloadValue, recoilPriority, reloadNR, reloadDis);
                else
                    AddLightGun(tempIsDefaultGun, tempDefaultGunNum, tempLightGunName, tenpLightGunNum, tempComPortNum, tempComPortName, *p_tempComPortInfo, tempComPortBaud, tempComPortDataBits, tempComPortParity, tempComPortStopBits, tempComPortFlow, recoilPriority, reloadNR, reloadDis);
            }
            else if(tempIsDefaultGun && tempDefaultGunNum==MX24)
            {
                line = in.readLine();
                if(line == "0")
                    dipSet = false;
                else
                    dipSet = true;

                line = in.readLine();
                dipNumber = line.toUInt ();

                line = in.readLine();
                hcpNumber = line.toUInt ();

                AddLightGun(tempIsDefaultGun, tempDefaultGunNum, tempLightGunName, tenpLightGunNum, tempComPortNum, tempComPortName, *p_tempComPortInfo, tempComPortBaud, tempComPortDataBits, tempComPortParity, tempComPortStopBits, tempComPortFlow, dipSet, dipNumber, hcpNumber, recoilPriority, reloadNR, reloadDis);
            }
            else if(tempIsDefaultGun && (tempDefaultGunNum==JBGUN4IR || tempDefaultGunNum==OPENFIRE))
            {
                //Is Analog Strength Set
                line = in.readLine();
                if(line == "0")
                    analSet = false;
                else
                    analSet = true;

                //Analog Strength, If Set Above
                line = in.readLine();

                if(analSet)
                {
                    analNumber = line.toUInt ();
                    AddLightGun(tempIsDefaultGun, tempDefaultGunNum, tempLightGunName, tenpLightGunNum, tempComPortNum, tempComPortName, *p_tempComPortInfo, tempComPortBaud, tempComPortDataBits, tempComPortParity, tempComPortStopBits, tempComPortFlow, analNumber, recoilPriority, reloadNR, reloadDis);
                }
                else
                {
                    AddLightGun(tempIsDefaultGun, tempDefaultGunNum, tempLightGunName, tenpLightGunNum, tempComPortNum, tempComPortName, *p_tempComPortInfo, tempComPortBaud, tempComPortDataBits, tempComPortParity, tempComPortStopBits, tempComPortFlow, recoilPriority, reloadNR, reloadDis);
                }

            }
            else
            {
                AddLightGun(tempIsDefaultGun, tempDefaultGunNum, tempLightGunName, tenpLightGunNum, tempComPortNum, tempComPortName, *p_tempComPortInfo, tempComPortBaud, tempComPortDataBits, tempComPortParity, tempComPortStopBits, tempComPortFlow, recoilPriority, reloadNR, reloadDis);
            }


            delete p_tempComPortInfo;
            p_tempComPortInfo = nullptr;

        }
        else
        {
            //This is For USB Light Guns
            HIDInfo tempHIDInfo;

            line = in.readLine();
            tempHIDInfo.path = line;

            line = in.readLine();
            tempHIDInfo.vendorID = line.toUShort ();

            line = in.readLine();
            tempHIDInfo.vendorIDString = line;

            line = in.readLine();
            tempHIDInfo.productID = line.toUShort ();

            line = in.readLine();
            tempHIDInfo.productIDString = line;

            line = in.readLine();
            tempHIDInfo.serialNumber = line;

            line = in.readLine();
            tempHIDInfo.releaseNumber = line.toUShort ();

            line = in.readLine();
            tempHIDInfo.releaseString = line;

            line = in.readLine();
            tempHIDInfo.manufacturer = line;

            line = in.readLine();
            tempHIDInfo.productDiscription = line;

            line = in.readLine();
            tempHIDInfo.usagePage = line.toUShort ();

            line = in.readLine();
            tempHIDInfo.usage = line.toUShort ();

            line = in.readLine();
            tempHIDInfo.usageString = line;

            line = in.readLine();
            tempHIDInfo.interfaceNumber = line.toUShort ();

            tempHIDInfo.displayPath = tempHIDInfo.path;
            tempHIDInfo.displayPath.remove(0,ALIENUSBFRONTPATHREM);

            if(tempDefaultGunNum == ALIENUSB)
                AddLightGun(tempIsDefaultGun, tempDefaultGunNum, tempLightGunName, tenpLightGunNum, tempHIDInfo, recoilPriority, reloadNR, reloadDis);
            else if(tempDefaultGunNum == AIMTRAK)
                AddLightGun(tempIsDefaultGun, tempDefaultGunNum, tempLightGunName, tenpLightGunNum, tempHIDInfo, AIMTRAKDELAYDFLT, recoilPriority, reloadNR, reloadDis);
        }



    }

    //Next up is the players light gun assignments
    line = in.readLine();

    if(line != ENDOFFILE)
    {
        //qDebug() << line;
        QMessageBox::critical (nullptr, "File Error", "Light gun save data file is corrupted. The file did not end correctly. Please try to reload file, or re-enter the light guns again.", QMessageBox::Ok);
    }

    //Close the File
    loadLGData.close();

    LoadPlayersAss();
}

void ComDeviceList::SavePlayersAss()
{
    bool removedFile = true;
    bool openFile;
    quint8 i;
    QFile savePlayersAss(playersAssSaveFile);


    //Erase Old Save File, if it exisits
    if(savePlayersAss.exists ())
        removedFile = savePlayersAss.remove ();

    if(!removedFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not remove old players assignment save data file. Failed to remove. Please close program and solve file problem. Might be open or permissions changed.", QMessageBox::Ok);
        return;
    }

    openFile = savePlayersAss.open(QIODeviceBase::WriteOnly | QIODevice::Text);

    if(!openFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not create player assignment save data file. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream out(&savePlayersAss);

    out << PLAYERSASSIGNMENTS << "\n";

    for(i=0; i < MAXPLAYERLIGHTGUNS; i++)
    {
        out << playersLightGun[i] << "\n";
    }

    out << ENDOFFILE;


    //Close File
    savePlayersAss.close ();

}

void ComDeviceList::LoadPlayersAss()
{
    bool fileExists;
    bool openFile;
    QString line;
    quint8 i;

    QFile loadPlayerAss(playersAssSaveFile);

    //Check if the File Exists, as it might not be created yet. If no File, then exit out of member function
    if(loadPlayerAss.exists() == false)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not open player assignment save data file. It does not exists. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    openFile = loadPlayerAss.open (QIODeviceBase::ReadOnly | QIODevice::Text);

    if(!openFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not open player assignment save data file to read. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&loadPlayerAss);

    //Next up is the players light gun assignments
    line = in.readLine();

    if(line != PLAYERSASSIGNMENTS)
    {
        //qDebug() << line;
        QMessageBox::critical (nullptr, "File Error", "Players Assignment save data file is corrupted. Please try to reload file, or re-enter the light guns again.", QMessageBox::Ok);
    }

    for(i=0; i < MAXPLAYERLIGHTGUNS; i++)
    {
        line = in.readLine();
        playersLightGun[i] = line.toUInt ();
    }


    //Last Line is the END_OF_FILE
    line = in.readLine();


    if(line != ENDOFFILE)
    {
        //qDebug() << line;
        QMessageBox::critical (nullptr, "File Error", "Players Assignment save data file is corrupted. The file did not end correctly. Please try to reload file, or re-enter the light guns again.", QMessageBox::Ok);
    }

    //Close the File
    loadPlayerAss.close();
}


//Save & Load COM Devices from/to File
void ComDeviceList::SaveComDeviceList()
{
    bool removedFile = true;
    bool openFile;
    quint8 i;
    QFile saveCDData(comDevicesSaveFile);

    //If COM Device Number is Zero, then erase the old save file & done
    if(numberComPortDevices == 0)
    {
        if(saveCDData.exists ())
            saveCDData.remove ();
        return;
    }

    //Erase Old Save Data
    if(saveCDData.exists ())
        removedFile = saveCDData.remove ();

    if(!removedFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not remove old COM device save data file. Failed to remove. Please close program and solve file problem. Might be open or permissions changed.", QMessageBox::Ok);
        return;
    }

    openFile = saveCDData.open(QIODeviceBase::WriteOnly | QIODevice::Text);

    if(!openFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not create COM device save data file. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream out(&saveCDData);

    out << STARTCOMDEVICESAVEFILE << "\n";
    out << numberComPortDevices << "\n";


    for(i = 0; i < numberComPortDevices; i++)
    {

        out << COMDEVICENUMBERFILE << i << "\n";

        //COM Device Number
        out << p_comPortDeviceList[i]->GetComPortDeviceNumber()<< "\n";

        //Light Gun Name
        out << p_comPortDeviceList[i]->GetComPortDeviceName ()<< "\n";


        //COM Port Data
        out << p_comPortDeviceList[i]->GetComPortNumber() << "\n";
        out << p_comPortDeviceList[i]->GetComPortString() << "\n";
        out << p_comPortDeviceList[i]->GetComPortBaud() << "\n";
        out << p_comPortDeviceList[i]->GetComPortDataBits() << "\n";
        out << p_comPortDeviceList[i]->GetComPortParity() << "\n";
        out << p_comPortDeviceList[i]->GetComPortStopBits() << "\n";
        out << p_comPortDeviceList[i]->GetComPortFlow() << "\n";


    }

    out << ENDOFFILE;


    //Close File
    saveCDData.close ();

}


void ComDeviceList::LoadComDeviceList()
{
    bool fileExists;
    bool openFile;
    quint8 i;

    quint8 tempNumComDevices;
    quint8 tenpComDeviceNum;
    QString tempComDeviceName;

    quint8 tempComPortNum;
    QString tempComPortName;
    QSerialPortInfo *p_tempComPortInfo;
    quint32 tempComPortBaud;
    quint16 tempComPortDataBits;
    quint16 tempComPortParity;
    quint16 tempComPortStopBits;
    quint16 tempComPortFlow;

    QString line, cmpLine;

    QFile loadCDData(comDevicesSaveFile);

    //Check if the File Exists, as it might not be created yet. If no File, then exit out of member function
    if(loadCDData.exists() == false)
    {
        //QMessageBox::critical (nullptr, "File Error", "Can not open COM device save data file. It does not exists. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    openFile = loadCDData.open (QIODeviceBase::ReadOnly | QIODevice::Text);

    if(!openFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not open COM device save data file to read. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&loadCDData);

    //Check First Line for title
    line = in.readLine();


    if(line != STARTCOMDEVICESAVEFILE)
    {
        //qDebug() << line;
        QMessageBox::critical (nullptr, "File Error", "COM device save data file is corrupted. Please try to reload file, or re-enter the COM devices again.", QMessageBox::Ok);
    }

    //Next Line is Number of Com Devices
    line = in.readLine();
    tempNumComDevices = line.toUInt ();

    for(i = 0; i < tempNumComDevices; i++)
    {
        //Get New Line From File
        line = in.readLine();

        //Line Should be "Light Gun #i"
        cmpLine = COMDEVICENUMBERFILE + QString::number (i);

        if(line != cmpLine)
        {
            //qDebug() << line;
            QMessageBox::critical (nullptr, "File Error", "COM device save data file is corrupted. Please try to reload file, or re-enter the COM devices again.", QMessageBox::Ok);
        }

        //Next Line is the Light Gun Number
        line = in.readLine();
        tenpComDeviceNum = line.toUInt ();

        //Light Gun Name
        tempComDeviceName = in.readLine();


        //COM Port Number
        line = in.readLine();
        tempComPortNum = line.toUInt ();

        //COM Port Name
        tempComPortName = in.readLine();

        //COM Port Info
        p_tempComPortInfo = new QSerialPortInfo(tempComPortName);

        //COM Port BAUD
        line = in.readLine();
        tempComPortBaud = line.toUInt ();

        //COM Port Data Bits
        line = in.readLine();
        tempComPortDataBits = line.toUInt ();

        //COM Port Parity
        line = in.readLine();
        tempComPortParity = line.toUInt ();

        //COM Port Stop Bits
        line = in.readLine();
        tempComPortStopBits = line.toUInt ();

        //COM Port Flow
        line = in.readLine();
        tempComPortFlow = line.toUInt ();


        AddComPortDevice(tempComDeviceName, tenpComDeviceNum, tempComPortNum, tempComPortName, *p_tempComPortInfo, tempComPortBaud, tempComPortDataBits, tempComPortParity, tempComPortStopBits, tempComPortFlow);


    }


    //Last Line is the END_OF_FILE
    line = in.readLine();


    if(line != ENDOFFILE)
    {
        //qDebug() << line;
        QMessageBox::critical (nullptr, "File Error", "COM device save data file is corrupted. The file did not end correctly. Please try to reload file, or re-enter the COM devices again.", QMessageBox::Ok);
    }

    //Close the File
    loadCDData.close();

}







//Save & Load Settings from/to File
void ComDeviceList::SaveSettings()
{
    QString rtDisplay;
    bool removedFile = true;
    bool openFile;
    quint8 i;
    QFile saveSetData(settingsSaveFile);


    //Erase Old Save Data
    if(saveSetData.exists ())
        removedFile = saveSetData.remove ();

    if(!removedFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not remove old settings save data file. Failed to remove. Please close program and solve file problem. Might be open or permissions changed.", QMessageBox::Ok);
        return;
    }

    openFile = saveSetData.open(QIODeviceBase::WriteOnly | QIODevice::Text);

    if(!openFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not create settings save data file. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream out(&saveSetData);

    out << STARTSETTINGSFILE << "\n";

    if(useDefaultLGFirst)
        out << "1\n";
    else
        out << "0\n";

    if(useMultiThreading)
        out << "1\n";
    else
        out << "0\n";

    //Output the Refresh Time of the Display
    rtDisplay = QString::number(refreshTimeDisplay)+"\n";
    out << rtDisplay;

    if(closeComPortGameExit)
        out << "1\n";
    else
        out << "0\n";

    if(ignoreUselessDLGGF)
        out << "1\n";
    else
        out << "0\n";

    if(bypassSerialWriteChecks)
        out << "1\n";
    else
        out << "0\n";

    if(disbleReaperLEDs)
        out << "1\n";
    else
        out << "0\n";

    if(displayAmmoPriority)
        out << "1\n";
    else
        out << "0\n";

    if(displayLifePriority)
        out << "1\n";
    else
        out << "0\n";

    if(displayOtherPriority)
        out << "1\n";
    else
        out << "0\n";

    if(displayAmmoLife)
        out << "1\n";
    else
        out << "0\n";

    if(displayAmmoLifeGlyphs)
        out << "1\n";
    else
        out << "0\n";

    if(displayAmmoLifeBar)
        out << "1\n";
    else
        out << "0\n";

    if(displayAmmoLifeNumber)
        out << "1\n";
    else
        out << "0\n";

    if(enableNewGameFileCreation)
        out << "1\n";
    else
        out << "0\n";

    if(enableReaperAmmo0Delay)
        out << "1\n";
    else
        out << "0\n";

    //Output the Reaper Ammo 0 Delay Value
    rtDisplay = QString::number(repearAmmo0Delay)+"\n";
    out << rtDisplay;

    //Output the Reaper Hold Slide Time
    rtDisplay = QString::number(reaperHoldSlideTime)+"\n";
    out << rtDisplay;


    out << ENDOFFILE;


    //Close File
    saveSetData.close ();

}


void ComDeviceList::LoadSettings()
{
    QString line;
    bool openFile;
    bool isNumber;
    quint8 i;
    QFile loadSetData(settingsSaveFile);
    bool tempLEDDisable;
    bool tempAmmoDisplay, tempLifeDisplay, tempOtherDisplay;

    //Check if the File Exists, as it might not be created yet. If no File, then exit out of member function
    if(loadSetData.exists() == false)
    {
        //QMessageBox::critical (nullptr, "File Error", "Can not open COM device save data file. It does not exists. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }


    openFile = loadSetData.open(QIODeviceBase::ReadOnly | QIODevice::Text);

    if(!openFile)
    {
        QMessageBox::critical (nullptr, "File Error", "Can not create settings save data file. Please close program and solve file problem. Might be permissions problem.", QMessageBox::Ok);
        return;
    }

    //Create a Text Stream, to Stream in the Data Easier
    QTextStream in(&loadSetData);

    //Check First Line for title
    line = in.readLine();


    if(!line.startsWith (STARTSETTINGSFILE))
    {
        QMessageBox::critical (nullptr, "File Error", "Settings save data file is corrupted. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is Use Default Light gun First
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        useDefaultLGFirst = true;
    }
    else if(line.startsWith ("0"))
    {
        useDefaultLGFirst = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at first setting. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }


    //Next Line is Use Multi-Threading
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        useMultiThreading = true;
    }
    else if(line.startsWith ("0"))
    {
        useMultiThreading = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at second setting. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is the Refresh Time for the Display
    line = in.readLine();

    refreshTimeDisplay = line.toUInt (&isNumber);

    if(!isNumber)
    {
        refreshTimeDisplay = DEFAULTREFRESHDISPLAY;
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at third setting. Refresh time display is not a number, setting to default.", QMessageBox::Ok);
    }

    //Next Line is Close COM Port when Game Exits
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        closeComPortGameExit = true;
    }
    else if(line.startsWith ("0"))
    {
        closeComPortGameExit = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at fourth setting. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is Ignore Useless Default LG Game File
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        ignoreUselessDLGGF = true;
    }
    else if(line.startsWith ("0"))
    {
        ignoreUselessDLGGF = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at fifth setting. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is to Bypass the Serial Port Write Checks
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        bypassSerialWriteChecks = true;
    }
    else if(line.startsWith ("0"))
    {
        bypassSerialWriteChecks = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Bypass Serial Port Write Checks. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is to Disable 5 LEDs on Reaper
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        disbleReaperLEDs = true;
    }
    else if(line.startsWith ("0"))
    {
        disbleReaperLEDs = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Bypass Serial Port Write Checks. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }


    //Next Line is End of File or Display Ammo Priority
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        displayAmmoPriority = true;
    }
    else if(line.startsWith ("0"))
    {
        displayAmmoPriority = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Display Ammo Priority. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is Display Life Priority
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        displayLifePriority = true;
    }
    else if(line.startsWith ("0"))
    {
        displayLifePriority = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Display Life Priority. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }


    //Next Line is Display Other Priority
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        displayOtherPriority = true;
    }
    else if(line.startsWith ("0"))
    {
        displayOtherPriority = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Display Other Priority. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is Display Ammo & Life for OpenFire LG
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        displayAmmoLife = true;
    }
    else if(line.startsWith ("0"))
    {
        displayAmmoLife = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Display Ammo & Life for OpenFire. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is Display Ammo & Life Glyphs for OpenFire LG
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        displayAmmoLifeGlyphs = true;
    }
    else if(line.startsWith ("0"))
    {
        displayAmmoLifeGlyphs = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Display Ammo & Life Glyphs for OpenFire. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is Display Ammo & Life Bars for OpenFire LG
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        displayAmmoLifeBar = true;
    }
    else if(line.startsWith ("0"))
    {
        displayAmmoLifeBar = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Display Ammo & Life Bars for OpenFire. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is Display Ammo & Life Number for OpenFire LG
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        displayAmmoLifeNumber = true;
    }
    else if(line.startsWith ("0"))
    {
        displayAmmoLifeNumber = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Display Ammo & Life Number for OpenFire. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Setting for New Game File Creation
    line = in.readLine();

    if(line.startsWith ("1"))
    {
        enableNewGameFileCreation = true;
    }
    else if(line.startsWith ("0"))
    {
        enableNewGameFileCreation = false;
    }
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Enable New Game File Creation. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }


    //Setting for New Game File Creation
    line = in.readLine();

    if(line.startsWith (ENDOFFILE))
    {
        enableReaperAmmo0Delay = true;
        repearAmmo0Delay = DEFAULTAMMO0DELAY;
        reaperHoldSlideTime = REAPERHOLDSLIDETIME;
        loadSetData.close ();
        this->SaveSettings();
        this->UpdateLightGunWithSettings ();
        return;
    }
    else if(line.startsWith ("1"))
        enableReaperAmmo0Delay = true;
    else if(line.startsWith ("0"))
        enableReaperAmmo0Delay = false;
    else
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at Enable Reaper Ammo 0 Delay. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    //Next Line is the Reaper Ammo 0 Delay
    line = in.readLine();

    repearAmmo0Delay = line.toUInt (&isNumber);

    if(!isNumber || repearAmmo0Delay == 0)
    {
        repearAmmo0Delay = DEFAULTAMMO0DELAY;
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted. Reaper Ammo 0 Delay is not a number, setting to default.", QMessageBox::Ok);
    }

    //Next Line is the Reaper Hold Slide Back Time
    line = in.readLine();

    reaperHoldSlideTime = line.toUInt (&isNumber);

    if(!isNumber || reaperHoldSlideTime < REAPERHOLDSLIDEMIN || reaperHoldSlideTime > REAPERHOLDSLIDEMAX)
    {
        reaperHoldSlideTime = REAPERHOLDSLIDETIME;
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted. Reaper's Hold Slide Back Timing is not a number or out of range, setting to default.", QMessageBox::Ok);
    }



    //Next Line is End of File
    line = in.readLine();

    if(!line.startsWith (ENDOFFILE))
    {
        QMessageBox::critical (nullptr, "Settings File Error", "Settings save data file is corrupted at the end. Please close program and solve file problem.", QMessageBox::Ok);
        return;
    }

    loadSetData.close ();

    //Update Setting to Light Guns
    this->UpdateLightGunWithSettings ();
}

//Get & Set of the Settings
bool ComDeviceList::GetUseDefaultLGFirst()
{
    return useDefaultLGFirst;
}

void ComDeviceList::SetUseDefaultLGFirst(bool udlgFirst)
{
    useDefaultLGFirst = udlgFirst;
}


bool ComDeviceList::GetUseMultiThreading()
{
    return useMultiThreading;
}

void ComDeviceList::SetUseMultiThreading(bool umThreading)
{
    useMultiThreading = umThreading;
}

quint32 ComDeviceList::GetRefreshTimeDisplay()
{
    return refreshTimeDisplay;
}

void ComDeviceList::SetRefreshTimeDisplay(quint32 rtDisplay)
{
    refreshTimeDisplay = rtDisplay;
}

bool ComDeviceList::GetCloseComPortGameExit()
{
    return closeComPortGameExit;
}

void ComDeviceList::SetCloseComPortGameExit(bool ccpGameExit)
{
    closeComPortGameExit = ccpGameExit;
}


bool ComDeviceList::GetIgnoreUselessDFLGGF()
{
    return ignoreUselessDLGGF;
}

void ComDeviceList::SetIgnoreUselessDFLGGF(bool ignoreUDFLGGF)
{
    ignoreUselessDLGGF = ignoreUDFLGGF;
}

bool ComDeviceList::GetSerialPortWriteCheckBypass()
{
    return bypassSerialWriteChecks;
}

void ComDeviceList::SetSerialPortWriteCheckBypass(bool spwCB)
{
    bypassSerialWriteChecks = spwCB;
}

bool ComDeviceList::GetDisableReaperLEDs()
{
    return disbleReaperLEDs;
}

void ComDeviceList::SetDisableReaperLEDs(bool drLED)
{
    for(quint8 x = 0; x < numberLightGuns; x++)
    {
        if(p_lightGunList[x]->GetDefaultLightGun() && p_lightGunList[x]->GetDefaultLightGunNumber() == RS3_REAPER)
        {
            p_lightGunList[x]->SetDisableReaperLEDs(drLED);
            p_lightGunList[x]->LoadDefaultLGCommands();
        }
    }

    disbleReaperLEDs = drLED;
}

void ComDeviceList::GetDisplayPriority(bool *ammo, bool *life)
{
    *ammo = displayAmmoPriority;
    *life = displayLifePriority;
}

void ComDeviceList::SetDisplayPriority(bool ammo, bool life)
{
    if(ammo != life)
    {
        displayAmmoPriority = ammo;
        displayLifePriority = life;

        for(quint8 x = 0; x < numberLightGuns; x++)
            p_lightGunList[x]->SetDisplayPriority (ammo, life);
    }
}

bool ComDeviceList::GetDisplayOtherPriority()
{
    return displayOtherPriority;
}

void ComDeviceList::SetDisplayOtherPriority(bool other)
{
    displayOtherPriority = other;

    for(quint8 x = 0; x < numberLightGuns; x++)
        p_lightGunList[x]->SetDisplayOtherPriority (other);
}

bool ComDeviceList::GetDisplayAmmoAndLife(bool *displayLG, bool *displayLB, bool *displayLN)
{
    *displayLG = displayAmmoLifeGlyphs;
    *displayLB = displayAmmoLifeBar;
    *displayLN = displayAmmoLifeNumber;

    return displayAmmoLife;
}

void ComDeviceList::SetDisplayAmmoAndLife(bool displayAAL, bool displayLG, bool displayLB, bool displayLN)
{
    displayAmmoLife = displayAAL;
    displayAmmoLifeGlyphs = displayLG;
    displayAmmoLifeBar = displayLB;
    displayAmmoLifeNumber = displayLN;

    for(quint8 x = 0; x < numberLightGuns; x++)
    {
        if(p_lightGunList[x]->GetDefaultLightGun() && p_lightGunList[x]->GetDefaultLightGunNumber() == OPENFIRE)
        {
            p_lightGunList[x]->SetDisplayAmmoAndLife(displayAmmoLife, displayAmmoLifeGlyphs, displayAmmoLifeBar, displayAmmoLifeNumber);
            p_lightGunList[x]->LoadDefaultLGCommands();
        }
    }

}

bool ComDeviceList::GetEnableNewGameFileCreation()
{
    return enableNewGameFileCreation;
}

void ComDeviceList::SetEnableNewGameFileCreation(bool enableNGFC)
{
    enableNewGameFileCreation = enableNGFC;
}

quint8 ComDeviceList::GetReaperAmmo0Delay(bool *isAmmo0DelayEnabled, quint16 *reaperHST)
{
    *isAmmo0DelayEnabled = enableReaperAmmo0Delay;
    *reaperHST = reaperHoldSlideTime;
    return repearAmmo0Delay;
}

void ComDeviceList::SetReaperAmmo0Delay(bool isAmmo0DelayEnabled, quint8 delayTime, quint16 reaperHST)
{
    enableReaperAmmo0Delay = isAmmo0DelayEnabled;
    repearAmmo0Delay = delayTime;
    reaperHoldSlideTime = reaperHST;

    for(quint8 x = 0; x < numberLightGuns; x++)
    {
        if(p_lightGunList[x]->GetDefaultLightGun() && p_lightGunList[x]->GetDefaultLightGunNumber() == RS3_REAPER)
        {
            p_lightGunList[x]->SetReaperAmmo0Delay(enableReaperAmmo0Delay, repearAmmo0Delay, reaperHoldSlideTime);
        }
    }
}




void ComDeviceList::UpdateLightGunWithSettings()
{
    bool isDefaultLG;
    quint8 defaultLGNumber;


    for(quint8 x = 0; x < numberLightGuns; x++)
    {
        isDefaultLG = p_lightGunList[x]->GetDefaultLightGun();
        defaultLGNumber = p_lightGunList[x]->GetDefaultLightGunNumber();

        //Update RS3 Reaper with the 5 LED
        if(isDefaultLG && defaultLGNumber == RS3_REAPER)
        {
            p_lightGunList[x]->SetDisableReaperLEDs(disbleReaperLEDs);
            p_lightGunList[x]->SetReaperAmmo0Delay(enableReaperAmmo0Delay, repearAmmo0Delay, reaperHoldSlideTime);
            p_lightGunList[x]->LoadDefaultLGCommands();
        }

        //Update on Display Priority & Display Other
        p_lightGunList[x]->SetDisplayPriority (displayAmmoPriority, displayLifePriority);
        p_lightGunList[x]->SetDisplayOtherPriority (displayOtherPriority);

        //Update OpenFire on Display Ammo & Life
        if(isDefaultLG && defaultLGNumber == OPENFIRE)
        {
            p_lightGunList[x]->SetDisplayAmmoAndLife(displayAmmoLife, displayAmmoLifeGlyphs, displayAmmoLifeBar, displayAmmoLifeNumber);
            p_lightGunList[x]->LoadDefaultLGCommands();
        }
    }
}

void ComDeviceList::CopyUsedDipPlayersArray(bool *targetArray, quint8 size, quint8 hubComPort)
{
    if(usedHubComDipPlayers.contains (hubComPort))
    {
        for (quint8 i = 0; i < size; i++)
        {
            *(targetArray + i) = usedHubComDipPlayers[hubComPort][i];
        }
    }
    else
    {
        for (quint8 i = 0; i < size; i++)
        {
            *(targetArray + i) = false;
        }
    }
}

bool ComDeviceList::IsComDipPlayer(quint8 comNum)
{
    if(usedHubComDipPlayers.contains (comNum))
        return true;
    else
        return false;
}


void ComDeviceList::ChangeUsedDipPlayersArray(quint8 hubComPort, quint8 dipPN, bool value)
{
    if(usedHubComDipPlayers.contains (hubComPort))
    {
        usedHubComDipPlayers[hubComPort][dipPN] = value;

        quint8 count = 0;

        for (quint8 i = 0; i < usedHubComDipPlayers[hubComPort].size (); i++)
        {
            if(!usedHubComDipPlayers[hubComPort][i])
                count++;
        }

        if(count == DIPSWITCH_NUMBER)
        {
            usedHubComDipPlayers.remove (hubComPort);
            availableComPorts[hubComPort] = true;
        }
    }
    else if(value)
    {
        QList<bool> tempDP;

        for (quint8 i = 0; i < DIPSWITCH_NUMBER; i++)
        {
            if(dipPN == i)
                tempDP << value;
            else
                tempDP << false;
        }

        usedHubComDipPlayers.insert (hubComPort, tempDP);
    }
}

void ComDeviceList::ResetLightgun()
{
    for(quint8 i = 0; i < numberLightGuns; i++)
    {
        p_lightGunList[i]->ResetLightGun ();
    }
}

void ComDeviceList::ResetLightGun(quint8 lgNeedReset)
{
    p_lightGunList[lgNeedReset]->ResetLightGun ();
}


bool ComDeviceList::CheckUSBPath(QString lgPath)
{
    if(numberLightGuns == 0)
        return false;

    bool foundMatch = false;

    for(quint8 i = 0; i < numberLightGuns; i++)
    {
        if(p_lightGunList[i]->IsLightGunUSB () && !foundMatch)
            foundMatch = p_lightGunList[i]->CheckUSBPath(lgPath);
    }

    return foundMatch;
}

bool ComDeviceList::CheckUSBPath(QString lgPath, quint8 lgNumber)
{
    if(numberLightGuns == 0)
        return false;

    bool foundMatch = false;

    for(quint8 i = 0; i < numberLightGuns; i++)
    {
        //Don't Check Yoourself, Before You Wreck Yourself
        if(i != lgNumber)
        {
            if(p_lightGunList[i]->IsLightGunUSB () && !foundMatch)
                foundMatch = p_lightGunList[i]->CheckUSBPath(lgPath);
        }
    }

    return foundMatch;
}

QList<HIDInfo> ComDeviceList::GetLightGunHIDInfo()
{
    QList<HIDInfo> lgHIDInfo;

    if(numberLightGuns == 0)
        return lgHIDInfo;

    for(quint8 i = 0; i < numberLightGuns; i++)
    {
        if(p_lightGunList[i]->IsLightGunUSB ())
            lgHIDInfo << p_lightGunList[i]->GetUSBHIDInfo ();
    }

    return lgHIDInfo;
}



QString ComDeviceList::ProcessHIDUsage(quint16 usagePage, quint16 usage)
{
    if(usagePage == 1)
    {
        if(usage == 0)
            return "Undefined";
        else if(usage == 1)
            return "Pointer";
        else if(usage == 2)
            return "Mouse";
        else if(usage == 3)
            return "Reserved";
        else if(usage == 4)
            return "Joystick";
        else if(usage == 5)
            return "GamePad";
        else if(usage == 6)
            return "Keybaord";
        else if(usage == 7)
            return "Keypad";
        else if(usage == 8)
            return "Multi-Axis Controller";
        else if(usage == 9)
            return "Tablet PC System Controls";
        else if(usage == 0x0A)
            return "Water Cooling Device";
        else if(usage == 0x0B)
            return "Computer Chassis Device";
        else if(usage == 0x0C)
            return "Wireless Radio Controls";
        else if(usage == 0x0D)
            return "Portable Device Control";
        else if(usage == 0x0E)
            return "System Multi-Axis Controller";
        else if(usage == 0x0F)
            return "Spatial Controller";
        else if(usage == 0x10)
            return "Assistive Control";
        else if(usage == 0x11)
            return "Device Dock";
        else if(usage == 0x12)
            return "Dockable Device";
        else if(usage == 0x13)
            return "Call State Management Control";
        else if(usage == 0x3A)
            return "Counted Buffer";
        else if(usage == 0x80)
            return "System Control";
        else if(usage == 0x96)
            return "Thumbstick";
        else if(usage == 0xC5)
            return "Chassis Enclosure";
        else
            return "";
    }
    else if(usagePage == 0x05)
    {
        if(usage == 0)
            return "Undefined";
        else if(usage == 1)
            return "3D Game Controller";
        else if(usage == 2)
            return "Pinball Device";
        else if(usage == 3)
            return "Gun Device";
        else if(usage == 20)
            return "Point of View";
        else if(usage == 32)
            return "Gun Selector";
        else
            return "";
    }
    else if(usagePage == 0x0C)
    {
        if(usage == 1)
            return "Consumer Control";
        else if(usage == 2)
            return "Numeric Key Pad";
        else if(usage == 3)
            return "Programmable Buttons";
        else if(usage == 4)
            return "Microphone";
        else if(usage == 5)
            return "Headphone";
        else if(usage == 6)
            return "Graphic Equalizer";
        else if(usage == 0x36)
            return "Function Buttons";
        else if(usage == 0x80)
            return "Selection";
        else
            return "";
    }
    else if(usagePage >= 0xFF00)
        return "Vendor Defined";

    return "";
}

quint8* ComDeviceList::GetRecoilPriority()
{
    userRecoilPriority[0] = 3;
    userRecoilPriority[1] = 2;
    userRecoilPriority[2] = 1;
    userRecoilPriority[3] = 0;

    return userRecoilPriority;
}



































