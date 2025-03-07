#include "editLightGunWindow.h"
#include "ui_editLightGunWindow.h"

//Constructor
editLightGunWindow::editLightGunWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::editLightGunWindow)
{
    QString tempQS;
    quint8 i;

    //Setup the Window Functions, like Combo Box
    ui->setupUi(this);

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports & Used Dip Players
    p_comDeviceList = cdList;
    p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);
    p_comDeviceList->CopyUsedDipPlayersArray(usedDipPlayers, DIPSWITCH_NUMBER, 0);

    //Set Mask for Analog Strength
    ui->analogLineEdit->setInputMask (ANALOGSTRENGTHMASK);


    //Hub COM Port Combo Box - Adding All COM Ports
    for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
    {
        tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
        ui->hubComComboBox->insertItem(comPortIndx,tempQS);
    }


    //Load First Combo Box with the Saved Light Guns; Number, Name, and COM Port
    numberLightGuns = p_comDeviceList->GetNumberLightGuns();
    for(i = 0; i<numberLightGuns; i++)
    {
        tempQS = QString::number(i+1);
        tempQS.append (": ");
        tempQS.append (p_comDeviceList->p_lightGunList[i]->GetLightGunName());
        tempQS.append (" on ");
        quint8 cpNum = p_comDeviceList->p_lightGunList[i]->GetComPortNumber();
        tempQS.append ("COM");
        tempQS.append (QString::number (cpNum));
        ui->savedLightGunsComboBox->insertItem(i,tempQS);
    }

    //Put the Name of the First Light Gun into the Light Gun Name Line Edit
    lightGunName = p_comDeviceList->p_lightGunList[0]->GetLightGunName ();
    ui->lightGunNameLineEdit->setText (lightGunName);

    //Default Light Gun Combo Box - Adding Default Light Guns
    ui->defaultLightGunComboBox->insertItem(0,"");
    ui->defaultLightGunComboBox->insertItem(RS3_REAPER,REAPERNAME);
    ui->defaultLightGunComboBox->insertItem(MX24,MX24NAME);
    ui->defaultLightGunComboBox->insertItem(JBGUN4IR,JBGUN4IRNAME);
    ui->defaultLightGunComboBox->insertItem(FUSION,FUSIONNAME);
    ui->defaultLightGunComboBox->insertItem(BLAMCON,BLAMCONNAME);
    ui->defaultLightGunComboBox->insertItem(OPENFIRE,OPENFIRENAME);

    //Check if the First Light Gun is a Default Light Gun
    defaultLightGun = p_comDeviceList->p_lightGunList[0]->GetDefaultLightGun();

    if(defaultLightGun)
    {
        defaultLightGunNum = p_comDeviceList->p_lightGunList[0]->GetDefaultLightGunNumber();

        if(defaultLightGunNum == MX24)
        {
            dipSwitchNumber = p_comDeviceList->p_lightGunList[0]->GetDipSwitchPlayerNumber (&dipSwitchSet);
            if(!dipSwitchSet)
                dipSwitchNumber = UNASSIGN;

            hubComPortNumber = p_comDeviceList->p_lightGunList[0]->GetHubComPortNumber ();
            ui->hubComComboBox->setCurrentIndex(hubComPortNumber);
            ui->comPortComboBox->setEnabled(false);
            FillSerialPortInfo(hubComPortNumber);
            p_comDeviceList->CopyUsedDipPlayersArray(usedDipPlayers, DIPSWITCH_NUMBER, hubComPortNumber);
        }
    }

    ui->dipSwitchComboBox->setEnabled(true);

    for(quint8 comPortIndx=0;comPortIndx<DIPSWITCH_NUMBER;comPortIndx++)
    {
        if(usedDipPlayers[comPortIndx] && dipSwitchNumber != comPortIndx)
            tempQS = "";
        else
            tempQS = "P"+QString::number(comPortIndx+1);
        ui->dipSwitchComboBox->insertItem(comPortIndx,tempQS);
    }




    if(defaultLightGun)
    {

        ui->defaultLightGunComboBox->setCurrentIndex (defaultLightGunNum);
        SetEnableComboBoxes(false);
        if(defaultLightGunNum == MX24)
        {
            ui->dipSwitchComboBox->setCurrentIndex(dipSwitchNumber);
            ui->hubComComboBox->setEnabled(true);
            ui->analogLineEdit->setEnabled(false);
            ui->comPortComboBox->setEnabled(false);
        }
        else if(defaultLightGunNum == JBGUN4IR || defaultLightGunNum == OPENFIRE)
        {
            ui->comPortComboBox->setEnabled(true);
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->analogLineEdit->setEnabled(true);
            analogStrength = p_comDeviceList->p_lightGunList[0]->GetAnalogStrength (&analogStrengthSet);
            if(analogStrengthSet)
                ui->analogLineEdit->setText(QString::number (analogStrength));
        }
        else
        {
            ui->comPortComboBox->setEnabled(true);
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->analogLineEdit->setEnabled(false);
            dipSwitchNumber = UNASSIGN;
        }
    }
    else
    {
        ui->defaultLightGunComboBox->setCurrentIndex(0);
        defaultLightGunNum = 0;
        SetEnableComboBoxes(true);
        ui->comPortComboBox->setEnabled(true);
        ui->dipSwitchComboBox->setEnabled(false);
        ui->hubComComboBox->setEnabled(false);
        ui->analogLineEdit->setEnabled(false);
    }



    //COM Port Combo Box - List Unused COM Port and Select Light Gun COM Port
    if((defaultLightGun && defaultLightGunNum != MX24) || !defaultLightGun)
    {
        comPortName = p_comDeviceList->p_lightGunList[0]->GetComPortString();
        comPortNum = p_comDeviceList->p_lightGunList[0]->GetComPortNumberBypass();
        unusedComPort[comPortNum] = true;
    }


    for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
    {
        if(unusedComPort[comPortIndx])
            tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
        else
            tempQS = "";
        ui->comPortComboBox->insertItem(comPortIndx,tempQS);

    }

    if((defaultLightGun && defaultLightGunNum != MX24) || !defaultLightGun)
    {
        ui->comPortComboBox->setCurrentIndex(comPortNum);
        FillSerialPortInfo(comPortNum);
    }

    comPortBaud = p_comDeviceList->p_lightGunList[0]->GetComPortBaud ();

    //Baud Speed Combo Box - Adding in Speed Rates
    for(quint8 comPortIndx=0;comPortIndx<BAUD_NUMBER;comPortIndx++)
    {
        ui->baudSpeedComboBox->insertItem(comPortIndx,BAUDNAME_ARRAY[comPortIndx]);
        if(comPortBaud == BAUDDATA_ARRAY[comPortIndx])
            baudIndex = comPortIndx;
    }
    ui->baudSpeedComboBox->setCurrentIndex(baudIndex);


    comPortDataBits = p_comDeviceList->p_lightGunList[0]->GetComPortDataBits ();

    //Data Bits Combo Box - Adding in Data Bit Options
    for(quint8 comPortIndx=0;comPortIndx<DATABITS_NUMBER;comPortIndx++)
    {
        ui->dataBitsComboBox->insertItem(comPortIndx,DATABITSNAME_ARRAY[comPortIndx]);
        if(comPortDataBits == DATABITSDATA_ARRAY[comPortIndx])
            dataBitsIndex = comPortIndx;
    }
    ui->dataBitsComboBox->setCurrentIndex(dataBitsIndex);


    comPortParity = p_comDeviceList->p_lightGunList[0]->GetComPortParity ();

    //Parity Combo Box - Adding Parity Options
    for(quint8 comPortIndx=0;comPortIndx<PARITY_NUMBER;comPortIndx++)
    {
        ui->parityComboBox->insertItem(comPortIndx,PARITYNAME_ARRAY[comPortIndx]);
        if(comPortParity == PARITYDATA_ARRAY[comPortIndx])
            parityIndex = comPortIndx;
    }
    ui->parityComboBox->setCurrentIndex(parityIndex);


    comPortStopBits = p_comDeviceList->p_lightGunList[0]->GetComPortStopBits ();

    //Stop Bits Combo Box - Adding in Stop Bit Options
    for(quint8 comPortIndx=0;comPortIndx<STOPBITS_NUMBER;comPortIndx++)
    {
        ui->stopBitsComboBox->insertItem(comPortIndx,STOPBITSNAME_ARRAY[comPortIndx]);
        if(comPortStopBits == STOPBITSDATA_ARRAY[comPortIndx])
            stopBitsIndex = comPortIndx;
    }
    ui->stopBitsComboBox->setCurrentIndex(stopBitsIndex);


    comPortFlow = p_comDeviceList->p_lightGunList[0]->GetComPortFlow ();

    //Flow Control Combo Box - Adding in Flow Options
    for(quint8 comPortIndx=0;comPortIndx<FLOW_NUMBER;comPortIndx++)
    {
        ui->flowControlComboBox->insertItem(comPortIndx,FLOWNAME_ARRAY[comPortIndx]);
        if(comPortFlow == FLOWDATA_ARRAY[comPortIndx])
            flowIndex = comPortIndx;
    }
    ui->flowControlComboBox->setCurrentIndex(flowIndex);

}

//Deconstructor
editLightGunWindow::~editLightGunWindow()
{
    delete ui;
}

//private slots

void editLightGunWindow::on_savedLightGunsComboBox_currentIndexChanged(int index)
{
    //Loads Save Light Gun Settings
    LoadSavedLightGun(index);
}


void editLightGunWindow::on_defaultLightGunComboBox_currentIndexChanged(int index)
{
    if(defaultLightGun && defaultLightGunNum == MX24)
    {
        QString tempQS;

        p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);

        for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
        {
            if(unusedComPort[comPortIndx])
                tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
            else
                tempQS = "";
            ui->comPortComboBox->setItemText (comPortIndx,tempQS);
        }
    }


    //Set Data for Default Guns. Currently only Retro Shooters: RS3 Reaper Gun
    if(index > 0)
    {
        //Set Combo Boxes to Default Light Gun Settings
        ui->baudSpeedComboBox->setCurrentIndex (DEFAULTLG_ARRAY[index].BAUD);
        ui->dataBitsComboBox->setCurrentIndex (DEFAULTLG_ARRAY[index].DATA);
        ui->parityComboBox->setCurrentIndex (DEFAULTLG_ARRAY[index].PARITY);
        ui->stopBitsComboBox->setCurrentIndex (DEFAULTLG_ARRAY[index].STOP);
        ui->flowControlComboBox->setCurrentIndex (DEFAULTLG_ARRAY[index].FLOW);

        //Disable Combo Boxes Affect by Default Light Gun
        SetEnableComboBoxes(false);

        if(index == MX24)
        {
            ui->dipSwitchComboBox->setEnabled(true);
            ui->hubComComboBox->setEnabled(true);
            ui->comPortComboBox->setEnabled(false);

            quint8 hubIndex = ui->hubComComboBox->currentIndex ();

            FillSerialPortInfo(hubIndex);
        }
        else
        {
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->comPortComboBox->setEnabled(true);

            quint8 comPortIndex = ui->comPortComboBox->currentIndex ();

            FillSerialPortInfo(comPortIndex);
        }

        if(index == JBGUN4IR || index == OPENFIRE)
            ui->analogLineEdit->setEnabled(true);
        else
        {
            ui->analogLineEdit->clear();
            ui->analogLineEdit->setEnabled(false);
        }
    }
    else
    {
        //Enable Combo Boxes Affect by Default Light Gun
        SetEnableComboBoxes(true);

        ui->comPortComboBox->setEnabled(true);

        quint8 comPortIndex = ui->comPortComboBox->currentIndex ();

        FillSerialPortInfo(comPortIndex);

        ui->dipSwitchComboBox->setEnabled(false);
        ui->hubComComboBox->setEnabled(false);
        ui->analogLineEdit->clear();
        ui->analogLineEdit->setEnabled(false);
    }
}



void editLightGunWindow::on_comPortComboBox_currentIndexChanged(int index)
{
    //Fills out Serial Port Info on right Side, when a different COM Port is Selected
    FillSerialPortInfo(index);
}



void editLightGunWindow::on_deletePushButton_clicked()
{
    quint8 dlgIndex = ui->savedLightGunsComboBox->currentIndex ();
    quint8 i;
    QString tempQS;
    quint8 hupComPort;


    //Delete Selected Light Gun
    p_comDeviceList->DeleteLightGun (dlgIndex);

    //Load First Combo Box with the Saved Light Guns; Number, Name, and COM Port
    numberLightGuns = p_comDeviceList->GetNumberLightGuns();

    //If No Light Guns are in the List After Deletion of a Light Gun, Then Close the Edit Window
    if(numberLightGuns == 0)
    {
        accept ();
    }
    else
    {
        //Remove Saved Light Gun from Combo Box
        for(i = 0; i<numberLightGuns; i++)
        {
            tempQS = QString::number(i+1);
            tempQS.append (": ");
            tempQS.append (p_comDeviceList->p_lightGunList[i]->GetLightGunName());
            tempQS.append (" on ");
            quint8 cpNum = p_comDeviceList->p_lightGunList[i]->GetComPortNumber();
            tempQS.append ("COM");
            tempQS.append (QString::number (cpNum));
            ui->savedLightGunsComboBox->setItemText(i,tempQS);
            //Reset Light Gun Numbers, to match Array
            p_comDeviceList->p_lightGunList[i]->SetLightGunNumber (i);
        }
        ui->savedLightGunsComboBox->removeItem (numberLightGuns);
        ui->savedLightGunsComboBox->setCurrentIndex (0);

        //Rebuild COM Port Combo Box, Now with the Used COM port Removed
        p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);
        comPortNum = p_comDeviceList->p_lightGunList[0]->GetComPortNumber();
        unusedComPort[comPortNum] = true;

        //COM Port Combo Box
        for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
        {
            if(unusedComPort[comPortIndx])
                tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
            else
                tempQS = "";
            ui->comPortComboBox->setItemText (comPortIndx,tempQS);
        }
        ui->comPortComboBox->setCurrentIndex(comPortNum);

        LoadSavedLightGun(0);
    }
}


void editLightGunWindow::on_editPushButton_clicked()
{
    bool dataValid = false;
    QString tempQS;

    //Check Input Value of the New Light gun Data
    dataValid = IsValidData();

    if(dataValid)
    {
        //Check if it is Default Light gun
        IsDefaultLightGun();

        //Add New Light Gun to ComDeviceList
        EditLightGun();

        //Rebuild COM Port Combo Box, Now with the Used COM port Removed
        p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);
        if(comPortNum != UNASSIGN)
            unusedComPort[comPortNum] = true;

        //COM Port Combo Box
        for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
        {
            if(unusedComPort[comPortIndx])
                tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
            else
                tempQS = "";
            ui->comPortComboBox->setItemText (comPortIndx,tempQS);
        }

        if(defaultLightGun && defaultLightGunNum == MX24)
        {
            ui->hubComComboBox->setCurrentIndex(hubComPortNumber);
            FillSerialPortInfo(hubComPortNumber);
        }
        else
        {
            ui->comPortComboBox->setCurrentIndex(comPortNum);
            FillSerialPortInfo(comPortNum);
        }

        tempQS = QString::number(lightGunNum+1);
        tempQS.append (": ");
        tempQS.append (lightGunName);
        tempQS.append (" on ");
        tempQS.append (comPortName);
        ui->savedLightGunsComboBox->setItemText (lightGunNum,tempQS);
    }
}


void editLightGunWindow::on_okPushButton_clicked()
{
    bool dataValid = false;

    //Check Input Value of the New Light gun Data
    dataValid = IsValidData();

    if(dataValid)
    {
        //Check if it is Default Light gun
        IsDefaultLightGun();

        //Add New Light Gun to ComDeviceList
        EditLightGun();
        //Close and Destroy the Window
        accept ();
    }
}


void editLightGunWindow::on_cancelPushButton_clicked()
{
    //Close and Destroy the Window
    this->accept ();
}


//private

bool editLightGunWindow::IsValidData()
{
    QString ivTemp, message;
    bool lgnIsEmpty = false;
    bool comPortNumEmpty = false;
    bool unusedName = false;
    bool analNotNumber = false;
    bool analNotRange = false;
    bool badDipIndex = false;
    bool hubComLG = false;
    quint8 numLightGuns = p_comDeviceList->GetNumberLightGuns();
    quint8 i;
    quint8 currentIndex = ui->savedLightGunsComboBox->currentIndex ();
    quint8 defaultLGIndex = ui->defaultLightGunComboBox->currentIndex ();
    //quint8 comPortIndex =  ui->comPortComboBox->currentIndex ();
    quint8 hubComIndex = ui->hubComComboBox->currentIndex ();


    //Check If Light Gun Name Line Edit Box is Empty
    ivTemp = ui->lightGunNameLineEdit->text();
    if(ivTemp.isEmpty ())
        lgnIsEmpty = true;

    if(numLightGuns > 0)
    {
        for(i = 0; i < numLightGuns; i++)
        {
            //Don't Compare its Own Name
            if(i != currentIndex)
            {
                if(ivTemp == p_comDeviceList->p_lightGunList[i]->GetLightGunName ())
                    unusedName = true;
            }
        }
    }


    if(defaultLGIndex == MX24)
    {
        ivTemp = ui->dipSwitchComboBox->currentText ();
        if(ivTemp.isEmpty ())
            badDipIndex = true;


        for(i = 0; i < numLightGuns; i++)
        {
            if(hubComIndex == p_comDeviceList->p_lightGunList[i]->GetComPortNumberBypass ())
                hubComLG = true;
        }
    }
    else
    {
        //Check If the Com Port Name is Empty for the Combo Box
        ivTemp = ui->comPortComboBox->currentText ();
        if(ivTemp.isEmpty ())
            comPortNumEmpty = true;
    }

    if(defaultLGIndex == JBGUN4IR || defaultLGIndex == OPENFIRE)
    {
        bool isNumber;
        QString analString = ui->analogLineEdit->text();
        quint16 analStrengthBig = analString.toUInt (&isNumber);
        if(!isNumber)
            analNotNumber = true;
        else
        {
            if(analStrengthBig > 255)
                analNotRange = true;
        }
    }



    //Check All the bools, If They are false, data is good and return true.
    //If false, then make a Warning string, and Pop Up a Warning Message Box on What is Wrong and return false
    if(lgnIsEmpty == false && comPortNumEmpty == false && unusedName == false && analNotNumber == false && analNotRange == false && badDipIndex == false && hubComLG == false)
    {
        return true;
    }
    else
    {
        if(lgnIsEmpty == true)
            message.append ("Light Gun Name is empty. Please enter a name for the Light Gun. ");
        if(comPortNumEmpty == true)
            message.append ("COM Port is empty. Please select a valid COM Port. ");
        if(unusedName == true)
            message.append ("Light gun name is already used. Please pick a different name.");
        if(badDipIndex == true)
            message.append ("A Used Dip Switch Player has been selected. Please change to a valid Dip Switch Player.");
        if(hubComLG == true)
            message.append ("The hub COM port cannot be the same as saved light gun(s). Please change it to the MX24 Hub COM port number.");
        if(analNotNumber == true)
            message.append ("Analog Strength is not a number.");
        if(analNotRange == true)
            message.append ("Analog Strength is out of range. Needs to be 0-255");

        QMessageBox::warning (this, "Can Not Edit Light Gun", message);

        return false;
    }
    return false;
}

bool editLightGunWindow::IsDefaultLightGun()
{
    quint8 dlgIndex = ui->defaultLightGunComboBox->currentIndex ();
    defaultLightGunNumChanged = false;

    if(defaultLightGun)
    {
        oldDefaultLightGunNum = defaultLightGunNum;
        oldDefaultLightGun = true;
    }
    else
        oldDefaultLightGun = false;

    if(dlgIndex > 0)
    {
        defaultLightGun = true;
        defaultLightGunNum = dlgIndex;

        //Went to Not a DefaultLG to a DefaultLG
        if(oldDefaultLightGun != defaultLightGun)
            defaultLightGunNumChanged = true;
        else
        {   //If Both are DefaultLG, then check number change
            if(oldDefaultLightGun)
            {
                if(oldDefaultLightGunNum != defaultLightGunNum)
                    defaultLightGunNumChanged = true;
            }
        }

        return true;
    }
    else
    {
        defaultLightGun = false;
        defaultLightGunNum = 0;
        return false;
    }


}

void editLightGunWindow::EditLightGun()
{
    quint8 oldComPort;
    quint8 hupComPort;


    //Collect Light Gun Name, Number, COM Port Number & Name, and Serial Port Info
    lightGunNum = ui->savedLightGunsComboBox->currentIndex ();
    lightGunName = ui->lightGunNameLineEdit->text();

    if(defaultLightGun && defaultLightGunNum == MX24)
        comPortNum = UNASSIGN;
    else
        comPortNum = ui->comPortComboBox->currentIndex ();

    if(comPortNum != UNASSIGN)
    {
        comPortName = BEGINCOMPORTNAME+QString::number(comPortNum);
        p_comPortInfo = new QSerialPortInfo(comPortName);
    }


    //Check if COM Port Changed, if so update unusedComPorts list
    oldComPort = p_comDeviceList->p_lightGunList[lightGunNum]->GetComPortNumberBypass ();
    if(oldComPort != comPortNum)
    {
        p_comDeviceList->SwitchComPortsInList(oldComPort, comPortNum);
    }

    //Collect COM Port Settings: BAUD, Data Bits, Parity, Stop Bits, and Flow Control
    FindBaudRate();
    FindDataBits();
    FindParity();
    FindStopBits();
    FindFlow();

    //Edit Light Gun
    p_comDeviceList->p_lightGunList[lightGunNum]->SetLightGunNumber (lightGunNum);
    p_comDeviceList->p_lightGunList[lightGunNum]->SetDefaultLightGun (defaultLightGun);
    if(defaultLightGun)
        p_comDeviceList->p_lightGunList[lightGunNum]->SetDefaultLightGunNumer (defaultLightGunNum);
    else
        p_comDeviceList->p_lightGunList[lightGunNum]->SetDefaultLightGunNumer (0);
    p_comDeviceList->p_lightGunList[lightGunNum]->SetLightGunName (lightGunName);
    //Only Happens on MX24
    if(comPortNum != UNASSIGN)
    {
        p_comDeviceList->p_lightGunList[lightGunNum]->SetComPortNumber (comPortNum);
        p_comDeviceList->p_lightGunList[lightGunNum]->SetComPortString (comPortName);
        p_comDeviceList->p_lightGunList[lightGunNum]->SetComPortInfo (*p_comPortInfo);
    }
    p_comDeviceList->p_lightGunList[lightGunNum]->SetComPortBaud (comPortBaud);
    p_comDeviceList->p_lightGunList[lightGunNum]->SetComPortDataBits (comPortDataBits);
    p_comDeviceList->p_lightGunList[lightGunNum]->SetComPortParity (comPortParity);
    p_comDeviceList->p_lightGunList[lightGunNum]->SetComPortStopBits (comPortStopBits);
    p_comDeviceList->p_lightGunList[lightGunNum]->SetComPortFlow (comPortFlow);

    if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        quint16 maxAmmo = REAPERMAXAMMONUM;
        quint16 reloadValue = REAPERRELOADNUM;
        p_comDeviceList->p_lightGunList[lightGunNum]->SetMaxAmmo (maxAmmo);
        p_comDeviceList->p_lightGunList[lightGunNum]->SetReloadValue (reloadValue);
    }
    else if(defaultLightGun && defaultLightGunNum == MX24)
    {
        dipSwitchNumber = ui->dipSwitchComboBox->currentIndex ();


        bool isSet;
        quint8 oldDipNumber = p_comDeviceList->p_lightGunList[lightGunNum]->GetDipSwitchPlayerNumber (&isSet);
        quint8 oldHubComPort = p_comDeviceList->p_lightGunList[lightGunNum]->GetHubComPortNumber ();

        hubComPortNumber = ui->hubComComboBox->currentIndex ();


        //Check if Dip Player is Set & Number Changed
        if((oldDipNumber != dipSwitchNumber && isSet) || (oldHubComPort != hubComPortNumber && isSet))
        {
            p_comDeviceList->ChangeUsedDipPlayersArray(hubComPortNumber, dipSwitchNumber, true);
            p_comDeviceList->ChangeUsedDipPlayersArray(oldHubComPort, oldDipNumber, false);
        }

        p_comDeviceList->p_lightGunList[lightGunNum]->SetDipSwitchPlayerNumber (dipSwitchNumber);
        p_comDeviceList->p_lightGunList[lightGunNum]->SetHubComPortNumber (hubComPortNumber);

        //Load up DIP Switches and redo DIP Switch Combo Box
        on_hubComComboBox_currentIndexChanged(hubComPortNumber);

        comPortName = BEGINCOMPORTNAME+QString::number(hubComPortNumber);
        p_comPortInfo = new QSerialPortInfo(comPortName);
        p_comDeviceList->p_lightGunList[lightGunNum]->SetComPortString (comPortName);
        p_comDeviceList->p_lightGunList[lightGunNum]->SetComPortInfo (*p_comPortInfo);

    }
    else if(defaultLightGun && (defaultLightGunNum == JBGUN4IR || defaultLightGunNum == OPENFIRE))
    {
        QString analString = ui->analogLineEdit->text();
        quint8 analStrength = analString.toUInt ();
        p_comDeviceList->p_lightGunList[lightGunNum]->SetAnalogStrength (analStrength);
    }

    //Load in Commands After the Edit
    p_comDeviceList->p_lightGunList[lightGunNum]->LoadDefaultLGCommands ();

    delete p_comPortInfo;
    p_comPortInfo = nullptr;


}

void editLightGunWindow::FindBaudRate()
{
    quint8 indexTemp;

    indexTemp = ui->baudSpeedComboBox->currentIndex();

    comPortBaud = BAUDDATA_ARRAY[indexTemp];
}

void editLightGunWindow::FindDataBits()
{
    quint8 indexTemp;

    indexTemp = ui->dataBitsComboBox->currentIndex();

    comPortDataBits = DATABITSDATA_ARRAY[indexTemp];
}

void editLightGunWindow::FindParity()
{
    quint8 indexTemp;

    indexTemp = ui->parityComboBox->currentIndex();

    comPortParity = PARITYDATA_ARRAY[indexTemp];
}

void editLightGunWindow::FindStopBits()
{
    quint8 indexTemp;

    indexTemp = ui->stopBitsComboBox->currentIndex();

    comPortStopBits = STOPBITSDATA_ARRAY[indexTemp];
}

void editLightGunWindow::FindFlow()
{
    quint8 indexTemp;

    indexTemp = ui->flowControlComboBox->currentIndex();

    comPortFlow = FLOWDATA_ARRAY[indexTemp];
}


void editLightGunWindow::FillSerialPortInfo(quint8 index)
{
    QString tempQS;
    QByteArray tempBA;

    tempQS = BEGINCOMPORTNAME+QString::number(index);
    p_comPortInfo = new QSerialPortInfo(tempQS);

    ui->locationLlineEdit->clear ();
    ui->descriptionLineEdit->clear ();
    ui->manufactureLineEdit->clear ();
    ui->serialNumLineEdit->clear ();
    ui->vendorIDLineEdit->clear ();
    ui->productIDLineEdit->clear ();


    ui->locationLlineEdit->insert(p_comPortInfo->systemLocation());

    ui->descriptionLineEdit->insert(p_comPortInfo->description());

    ui->manufactureLineEdit->insert(p_comPortInfo->manufacturer());

    ui->serialNumLineEdit->insert(p_comPortInfo->serialNumber());

    if(p_comPortInfo->hasVendorIdentifier())
    {
        tempBA = QByteArray::number(p_comPortInfo->vendorIdentifier(), 16);
        ui->vendorIDLineEdit->insert(tempBA);
    }

    if(p_comPortInfo->hasProductIdentifier())
    {
        tempBA = QByteArray::number(p_comPortInfo->productIdentifier(), 16);
        ui->productIDLineEdit->insert(tempBA);
    }

    delete p_comPortInfo;
    p_comPortInfo = nullptr;
}

void editLightGunWindow::SetEnableComboBoxes(bool enableCB)
{
    ui->baudSpeedComboBox->setEnabled (enableCB);
    ui->dataBitsComboBox->setEnabled (enableCB);
    ui->parityComboBox->setEnabled (enableCB);
    ui->stopBitsComboBox->setEnabled (enableCB);
    ui->flowControlComboBox->setEnabled (enableCB);
}


void editLightGunWindow::LoadSavedLightGun(quint8 index)
{
    QString tempQS;
    quint8 tempHCP = ui->hubComComboBox->currentIndex ();

    //Move Over a Clean Copy of the Unused COM Port Array & Used Dip Switch Players
    p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);
    p_comDeviceList->CopyUsedDipPlayersArray(usedDipPlayers, DIPSWITCH_NUMBER, tempHCP);

    //Put the Name of the Selected Light Gun into the Light Gun Name Line Edit
    lightGunName = p_comDeviceList->p_lightGunList[index]->GetLightGunName ();
    ui->lightGunNameLineEdit->setText (lightGunName);

    defaultLightGun = p_comDeviceList->p_lightGunList[index]->GetDefaultLightGun();


    //Check if the Light Gun is a Default Light Gun
    if(defaultLightGun)
    {
        defaultLightGunNum = p_comDeviceList->p_lightGunList[index]->GetDefaultLightGunNumber();
        ui->defaultLightGunComboBox->setCurrentIndex (defaultLightGunNum);
        defaultLightGun = true;
        SetEnableComboBoxes(false);

        if(defaultLightGunNum == MX24)
        {
            ui->analogLineEdit->clear();
            ui->analogLineEdit->setEnabled(false);
            ui->dipSwitchComboBox->setEnabled(true);
            dipSwitchNumber = p_comDeviceList->p_lightGunList[index]->GetDipSwitchPlayerNumber (&dipSwitchSet);
            if(!dipSwitchSet)
                dipSwitchNumber = UNASSIGN;
            else
                ui->dipSwitchComboBox->setCurrentIndex(dipSwitchNumber);

            hubComPortNumber = p_comDeviceList->p_lightGunList[index]->GetHubComPortNumber ();
            p_comDeviceList->CopyUsedDipPlayersArray(usedDipPlayers, DIPSWITCH_NUMBER, hubComPortNumber);
            ui->hubComComboBox->setEnabled(true);
            ui->hubComComboBox->setCurrentIndex (hubComPortNumber);

            ui->comPortComboBox->setEnabled(false);

            FillSerialPortInfo(hubComPortNumber);


        }
        else if(defaultLightGunNum == JBGUN4IR || defaultLightGunNum == OPENFIRE)
        {
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->analogLineEdit->setEnabled(true);
            analogStrength = p_comDeviceList->p_lightGunList[index]->GetAnalogStrength (&analogStrengthSet);
            if(analogStrengthSet)
                ui->analogLineEdit->setText(QString::number (analogStrength));

            ui->comPortComboBox->setEnabled(true);
        }
        else
        {
            ui->analogLineEdit->clear();
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->analogLineEdit->clear();
            ui->analogLineEdit->setEnabled(false);
            ui->comPortComboBox->setEnabled(true);
        }
    }
    else
    {
        ui->defaultLightGunComboBox->setCurrentIndex(0);
        defaultLightGun = false;
        defaultLightGunNum = 0;
        SetEnableComboBoxes(true);
        ui->dipSwitchComboBox->setEnabled(false);
        ui->hubComComboBox->setEnabled(false);
        ui->analogLineEdit->clear();
        ui->analogLineEdit->setEnabled(false);
        ui->comPortComboBox->setEnabled(true);
    }


    for(quint8 comPortIndx=0;comPortIndx<DIPSWITCH_NUMBER;comPortIndx++)
    {
        if(usedDipPlayers[comPortIndx] && dipSwitchNumber != comPortIndx)
            tempQS = "";
        else
            tempQS = "P"+QString::number(comPortIndx+1);
        ui->dipSwitchComboBox->setItemText(comPortIndx,tempQS);
    }


    if((defaultLightGun && defaultLightGunNum != MX24) || !defaultLightGun)
    {
        //COM Port Combo Box - List Unused COM Port and Select Light Gun COM Port
        comPortName = p_comDeviceList->p_lightGunList[index]->GetComPortString();
        comPortNum = p_comDeviceList->p_lightGunList[index]->GetComPortNumberBypass();
        unusedComPort[comPortNum] = true;

        for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
        {
            if(unusedComPort[comPortIndx])
                tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
            else
                tempQS = "";
            ui->comPortComboBox->setItemText (comPortIndx,tempQS);
        }

        ui->comPortComboBox->setCurrentIndex(comPortNum);

        FillSerialPortInfo(comPortNum);
    }


    comPortBaud = p_comDeviceList->p_lightGunList[index]->GetComPortBaud ();

    //Baud Speed Combo Box - Adding in Speed Rates
    for(quint8 comPortIndx=0;comPortIndx<BAUD_NUMBER;comPortIndx++)
    {
        if(comPortBaud == BAUDDATA_ARRAY[comPortIndx])
            baudIndex = comPortIndx;
    }
    ui->baudSpeedComboBox->setCurrentIndex(baudIndex);

    comPortDataBits = p_comDeviceList->p_lightGunList[index]->GetComPortDataBits ();

    //Data Bits Combo Box - Adding in Data Bit Options
    for(quint8 comPortIndx=0;comPortIndx<DATABITS_NUMBER;comPortIndx++)
    {
        if(comPortDataBits == DATABITSDATA_ARRAY[comPortIndx])
            dataBitsIndex = comPortIndx;
    }
    ui->dataBitsComboBox->setCurrentIndex(dataBitsIndex);


    comPortParity = p_comDeviceList->p_lightGunList[index]->GetComPortParity ();

    //Parity Combo Box - Adding Parity Options
    for(quint8 comPortIndx=0;comPortIndx<PARITY_NUMBER;comPortIndx++)
    {
        if(comPortParity == PARITYDATA_ARRAY[comPortIndx])
            parityIndex = comPortIndx;
    }
    ui->parityComboBox->setCurrentIndex(parityIndex);


    comPortStopBits = p_comDeviceList->p_lightGunList[index]->GetComPortStopBits ();

    //Stop Bits Combo Box - Adding in Stop Bit Options
    for(quint8 comPortIndx=0;comPortIndx<STOPBITS_NUMBER;comPortIndx++)
    {
        if(comPortStopBits == STOPBITSDATA_ARRAY[comPortIndx])
            stopBitsIndex = comPortIndx;
    }
    ui->stopBitsComboBox->setCurrentIndex(stopBitsIndex);


    comPortFlow = p_comDeviceList->p_lightGunList[index]->GetComPortFlow ();

    //Flow Control Combo Box - Adding in Flow Options
    for(quint8 comPortIndx=0;comPortIndx<FLOW_NUMBER;comPortIndx++)
    {
        if(comPortFlow == FLOWDATA_ARRAY[comPortIndx])
            flowIndex = comPortIndx;
    }
    ui->flowControlComboBox->setCurrentIndex(flowIndex);
}







void editLightGunWindow::on_hubComComboBox_currentIndexChanged(int index)
{
    QString tempQS;
    p_comDeviceList->CopyUsedDipPlayersArray(usedDipPlayers, DIPSWITCH_NUMBER, index);

    for(quint8 comPortIndx=0;comPortIndx<DIPSWITCH_NUMBER;comPortIndx++)
    {
        if(usedDipPlayers[comPortIndx] && dipSwitchNumber != comPortIndx)
            tempQS = "";
        else
            tempQS = "P"+QString::number(comPortIndx+1);
        ui->dipSwitchComboBox->setItemText(comPortIndx,tempQS);
    }


    FillSerialPortInfo(index);
}

