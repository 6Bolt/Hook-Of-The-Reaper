#include "editLightGunWindow.h"
#include "ui_editLightGunWindow.h"
#include "../Global.h"

//Constructor
editLightGunWindow::editLightGunWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::editLightGunWindow)
{
    QString tempQS;
    quint8 i;
    numberHIDDevices = 0;
    getAllHIDs = false;
    isHIDInit = false;

    //Setup the Window Functions, like Combo Box
    ui->setupUi(this);

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());

    editLightGunSound.setSource (QUrl::fromLocalFile(":/sounds/editLightGun.wav"));
    deleteLightGunSound.setSource (QUrl::fromLocalFile(":/sounds/deleteLightGun.wav"));
    editLightGunSound.setVolume(0.7);
    deleteLightGunSound.setVolume(0.7);

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
        if(!p_comDeviceList->p_lightGunList[i]->IsLightGunUSB ())
        {
            quint8 cpNum = p_comDeviceList->p_lightGunList[i]->GetComPortNumber();
            tempQS.append ("COM");
            tempQS.append (QString::number (cpNum));
        }
        else
            tempQS.append ("USB");

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
    ui->defaultLightGunComboBox->insertItem(ALIENUSB,ALIENUSBNAME);

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


    //USB Input Mask
    //ui->vendorIDUSBLineEdit->setInputMask (USBINPUTMASKHEX);
    //ui->productIDUSBLineEdit->setInputMask (USBINPUTMASKHEX);
    ui->recoilDelayLineEdit->setInputMask (USBRECOILDELAYMASK);

    //Get USB HID Devices
    FillUSBDevicesComboBox();



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
        else if(defaultLightGunNum == ALIENUSB)
        {
            ui->comPortComboBox->setEnabled(false);
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->analogLineEdit->setEnabled(false);

            //First Light Gun is an USB HID Light Gun
            qint16 hidIndex = FindLightGunHID(0);

            //If Found, then load up that HID, if not Load LG Info into Line Edits
            if(hidIndex != -1)
                on_usbDevicesComboBox_currentIndexChanged(hidIndex);
            else
            {
                //HID Device Not Found in HID List. Fill in VendorID, ProductID, and Serial Number
                HIDInfo tempHIDInfo = p_comDeviceList->p_lightGunList[0]->GetUSBHIDInfo ();
                quint16 rcDelay = p_comDeviceList->p_lightGunList[0]->GetRecoilDelay ();

                //Clear Out Line Edits
                ui->vendorIDUSBLineEdit->clear ();
                ui->productIDUSBLineEdit->clear ();
                ui->displayPathUSBLineEdit->clear ();

                QString tempVID = tempHIDInfo.vendorIDString;
                tempVID.remove (0,2);
                ui->vendorIDUSBLineEdit->insert (tempVID);

                QString tempPID = tempHIDInfo.productIDString;
                tempPID.remove (0,2);
                ui->productIDUSBLineEdit->insert (tempPID);

                ui->displayPathUSBLineEdit->insert(tempHIDInfo.displayPath);

                ui->recoilDelayLineEdit->clear();
                ui->recoilDelayLineEdit->setEnabled (true);
                ui->recoilDelayLineEdit->insert (QString::number(rcDelay));
            }
        }
        else
        {
            ui->comPortComboBox->setEnabled(true);
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->analogLineEdit->setEnabled(false);
            dipSwitchNumber = UNASSIGN;
            ui->recoilDelayLineEdit->setEnabled (false);
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
    if((defaultLightGun && defaultLightGunNum != MX24 && defaultLightGunNum != ALIENUSB) || !defaultLightGun)
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

    if((defaultLightGun && defaultLightGunNum != MX24 && defaultLightGunNum != ALIENUSB) || !defaultLightGun)
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


    ui->lgLCDNumber->setSegmentStyle(QLCDNumber::Flat);
    numberLightGuns = p_comDeviceList->GetNumberLightGuns ();
    ui->lgLCDNumber->setDigitCount(3);
    ui->lgLCDNumber->display (numberLightGuns);

}

//Deconstructor
editLightGunWindow::~editLightGunWindow()
{
    delete ui;

    //Exit the USB HID
    hid_exit ();
}

//private slots

void editLightGunWindow::on_savedLightGunsComboBox_currentIndexChanged(int index)
{
    //Loads Save Light Gun Settings
    LoadSavedLightGun(index);
}


void editLightGunWindow::on_defaultLightGunComboBox_currentIndexChanged(int index)
{
    quint8 lgIndex = ui->savedLightGunsComboBox->currentIndex ();

    bool isUSBLG = p_comDeviceList->p_lightGunList[lgIndex]->IsLightGunUSB ();

    if(isUSBLG && index != ALIENUSB)
    {
        QMessageBox::warning (this, "Cannot Change an USB HID to Serial Port", "An USB HID light gun cannot be changed to a Serial COM Port light gun. Please delete the USB HID light gun and then add a light gun");
        ui->defaultLightGunComboBox->setCurrentIndex (ALIENUSB);
        return;
    }
    else if(!isUSBLG && index == ALIENUSB)
    {
        QMessageBox::warning (this, "Cannot Change a Serial Port to USB HID", "A Serial Port light gun cannot be changed to a USB HID light gun. Please delete the Serial Port light gun and then add a light gun");
        bool isdefaultLG = p_comDeviceList->p_lightGunList[lgIndex]->GetDefaultLightGun ();
        quint8 defaultLGIndex;
        if(isdefaultLG)
            defaultLGIndex = p_comDeviceList->p_lightGunList[lgIndex]->GetDefaultLightGunNumber ();
        else
            defaultLGIndex = 0;
        ui->defaultLightGunComboBox->setCurrentIndex (defaultLGIndex);
        return;
    }



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
        else if(index != ALIENUSB)
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
    else  //For Non-Default Light Guns
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

    deleteLightGunSound.play();
    numberLightGuns = p_comDeviceList->GetNumberLightGuns ();
    ui->lgLCDNumber->display (numberLightGuns);

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
            if(!p_comDeviceList->p_lightGunList[i]->IsLightGunUSB ())
            {
                quint8 cpNum = p_comDeviceList->p_lightGunList[i]->GetComPortNumber();
                tempQS.append ("COM");
                tempQS.append (QString::number (cpNum));
            }
            else
                tempQS.append ("USB");

            ui->savedLightGunsComboBox->setItemText(i,tempQS);
            //Reset Light Gun Numbers, to match Array
            p_comDeviceList->p_lightGunList[i]->SetLightGunNumber (i);
        }
        ui->savedLightGunsComboBox->removeItem (numberLightGuns);
        ui->savedLightGunsComboBox->setCurrentIndex (0);

        //Rebuild COM Port Combo Box, Now with the Used COM port Removed
        p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);
        comPortNum = p_comDeviceList->p_lightGunList[0]->GetComPortNumber();
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

        if(comPortNum != UNASSIGN)
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

        editLightGunSound.play ();

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
        else if((defaultLightGun && defaultLightGunNum != ALIENUSB) || !defaultLightGun)
        {
            ui->comPortComboBox->setCurrentIndex(comPortNum);
            FillSerialPortInfo(comPortNum);
        }

        tempQS = QString::number(lightGunNum+1);
        tempQS.append (": ");
        tempQS.append (lightGunName);
        tempQS.append (" on ");
        if(!p_comDeviceList->p_lightGunList[lightGunNum]->IsLightGunUSB ())
            tempQS.append (comPortName);
        else
            tempQS.append ("USB");

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
    bool usbDisPMatch = false;
    bool emptyUSBdelay = false;
    bool usbDelayNotNum = false;
    bool usbDelayRange = false;
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
    else if(defaultLGIndex != ALIENUSB)
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

    if(defaultLGIndex == ALIENUSB)
    {
        QString tempDP;
        bool isNumber;

        ivTemp = ui->recoilDelayLineEdit->text();
        if(ivTemp.isEmpty ())
            emptyUSBdelay = true;

        quint16 tempDelay = ivTemp.toUShort (&isNumber);

        if(!isNumber)
            usbDelayNotNum = true;
        else if(tempDelay < ALIENUSBMINDELAY)
            usbDelayRange = true;

        tempDP = ui->displayPathUSBLineEdit->text();
        usbDisPMatch = p_comDeviceList->CheckUSBPath(tempDP, currentIndex);
    }


    //Check All the bools, If They are false, data is good and return true.
    //If false, then make a Warning string, and Pop Up a Warning Message Box on What is Wrong and return false
    if(lgnIsEmpty == false && comPortNumEmpty == false && unusedName == false && analNotNumber == false && analNotRange == false && badDipIndex == false && hubComLG == false && usbDisPMatch == false && emptyUSBdelay == false && usbDelayRange == false)
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
        if(usbDisPMatch == true)
            message.append ("An Exsiting USB Light Gun already has that path/location. There cannot be duplicates of the path/location. As it is used to send data to the device.");
        if(emptyUSBdelay == true)
            message.append ("The USB Recoil Delay is empty. It needs at least 40ms or higher");
        if(usbDelayRange == true)
            message.append ("The USB Recoil Delay is too small. It needs at least 40ms or higher");

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

    if(defaultLightGun && (defaultLightGunNum == MX24 || defaultLightGunNum == ALIENUSB))
        comPortNum = UNASSIGN;
    else
        comPortNum = ui->comPortComboBox->currentIndex ();

    if(comPortNum != UNASSIGN)
    {
        comPortName = BEGINCOMPORTNAME+QString::number(comPortNum);
        p_comPortInfo = new QSerialPortInfo(comPortName);
    }

    //Edit Light Gun Stuff for Serial Port & USB
    p_comDeviceList->p_lightGunList[lightGunNum]->SetLightGunNumber (lightGunNum);
    p_comDeviceList->p_lightGunList[lightGunNum]->SetDefaultLightGun (defaultLightGun);
    if(defaultLightGun)
        p_comDeviceList->p_lightGunList[lightGunNum]->SetDefaultLightGunNumer (defaultLightGunNum);
    else
        p_comDeviceList->p_lightGunList[lightGunNum]->SetDefaultLightGunNumer (0);
    p_comDeviceList->p_lightGunList[lightGunNum]->SetLightGunName (lightGunName);


    //Edit Serial Port Light Gun Stuff
    if((defaultLightGun && defaultLightGunNum != ALIENUSB) || !defaultLightGun)
    {
        //Check if COM Port Changed, if so update unusedComPorts list
        oldComPort = p_comDeviceList->p_lightGunList[lightGunNum]->GetComPortNumber ();
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
    else
    {
        //Need to Get the HIDInfo -So Much Easier
        qint16 hidIndex = ui->usbDevicesComboBox->currentIndex ();
        p_comDeviceList->p_lightGunList[lightGunNum]->SetHIDInfo (hidInfoList[hidIndex]);

        //Recoil Delay
        QString rcDelayS = ui->recoilDelayLineEdit->text ();
        quint16 rcDelay = rcDelayS.toUShort ();
        p_comDeviceList->p_lightGunList[lightGunNum]->SetRecoilDelay (rcDelay);
    }

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
    ui->usageLineEdit->clear ();
    ui->releaseNumLineEdit->clear ();

    ui->usageLineEdit->setEnabled (false);
    ui->releaseNumLineEdit->setEnabled (false);

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

            ui->vendorIDUSBLineEdit->setEnabled(false);
            ui->productIDUSBLineEdit->setEnabled(false);
            ui->displayPathUSBLineEdit->setEnabled(false);
            ui->usbDevicesComboBox->setEnabled(false);
            ui->recoilDelayLineEdit->setEnabled (false);
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
            ui->vendorIDUSBLineEdit->setEnabled(false);
            ui->productIDUSBLineEdit->setEnabled(false);
            ui->displayPathUSBLineEdit->setEnabled(false);
            ui->usbDevicesComboBox->setEnabled(false);
            ui->recoilDelayLineEdit->setEnabled (false);
        }
        else if(defaultLightGunNum == ALIENUSB)
        {
            ui->comPortComboBox->setEnabled(false);
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->analogLineEdit->setEnabled(false);

            ui->vendorIDUSBLineEdit->setEnabled(true);
            ui->productIDUSBLineEdit->setEnabled(true);
            ui->displayPathUSBLineEdit->setEnabled(true);
            ui->usbDevicesComboBox->setEnabled(true);

            quint16 rcDelay = p_comDeviceList->p_lightGunList[index]->GetRecoilDelay ();
            ui->recoilDelayLineEdit->clear();
            ui->recoilDelayLineEdit->setEnabled (true);
            ui->recoilDelayLineEdit->insert (QString::number(rcDelay));

            FillUSBDevicesComboBox();

            //First Light Gun is an USB HID Light Gun
            qint16 hidIndex = FindLightGunHID(index);

            //qDebug() << "hidIndex: " << hidIndex;


            //If Found, then load up that HID, if not Load LG Info into Line Edits
            if(hidIndex != -1)
            {
                ui->usbDevicesComboBox->setCurrentIndex (hidIndex);
            }
            else
            {
                QMessageBox::warning (this, "Cannot Find Light Gun USB HID", "Cannot find the USB HID device, that was saved to the light gun. Please make sure that the light gun is plugged in. Will Load in the values");

                //HID Device Not Found in HID List. Fill in VendorID, ProductID, and Serial Number
                HIDInfo tempHIDInfo = p_comDeviceList->p_lightGunList[index]->GetUSBHIDInfo ();

                //Clear Out Line Edits
                ui->vendorIDUSBLineEdit->clear ();
                ui->productIDUSBLineEdit->clear ();
                ui->displayPathUSBLineEdit->clear ();

                QString tempVID = tempHIDInfo.vendorIDString;
                tempVID.remove (0,2);
                ui->vendorIDUSBLineEdit->insert (tempVID);

                QString tempPID = tempHIDInfo.productIDString;
                tempPID.remove (0,2);
                ui->productIDUSBLineEdit->insert (tempPID);

                ui->displayPathUSBLineEdit->insert(tempHIDInfo.displayPath);


            }

        }
        else
        {
            ui->analogLineEdit->clear();
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->analogLineEdit->clear();
            ui->analogLineEdit->setEnabled(false);
            ui->comPortComboBox->setEnabled(true);
            ui->vendorIDUSBLineEdit->setEnabled(false);
            ui->productIDUSBLineEdit->setEnabled(false);
            ui->displayPathUSBLineEdit->setEnabled(false);
            ui->usbDevicesComboBox->setEnabled(false);
            ui->recoilDelayLineEdit->setEnabled (false);
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
        ui->vendorIDUSBLineEdit->setEnabled(false);
        ui->productIDUSBLineEdit->setEnabled(false);
        ui->displayPathUSBLineEdit->setEnabled(false);
        ui->usbDevicesComboBox->setEnabled(false);
        ui->recoilDelayLineEdit->setEnabled (false);
    }

    if((defaultLightGun && defaultLightGunNum != ALIENUSB) || !defaultLightGun)
    {


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


void editLightGunWindow::FillUSBDevicesComboBox()
{
    if(!isHIDInit)
    {
        if(hid_init ())
        {
            QMessageBox::critical (this, "HID API Failed to Init", "The USB HID API failed to init. Something is really wrong");
            return;
        }
        isHIDInit = true;
    }

    if(numberHIDDevices > 0)
    {
        //Clear Combo Box of HIDs
        ui->usbDevicesComboBox->clear ();

        //Clear List of HID Info
        hidInfoList.clear ();

        numberHIDDevices = 0;
    }

    if(!getAllHIDs)
    {
        //Get a list of Alien Gun HIDs
        devs = hid_enumerate(ALIENUSBVENDORID, ALIENUSBPRODUCTID);
    }
    else
    {
        //Get a list of all the HIDs
        devs = hid_enumerate(0x0, 0x0);
    }

    lgHIDInfoList = p_comDeviceList->GetLightGunHIDInfo ();

    //Process System HID Device Info
    for (; devs; devs = devs->next)
    {
        this->ProcessHIDInfo();
    }
    //Release HID enumeration
    hid_free_enumeration(devs);

    //quint8 numberLG = ui->savedLightGunsComboBox->count ();
    quint8 currentIndex = ui->savedLightGunsComboBox->currentIndex ();
    HIDInfo tempHIDInfo;
    //qint16 lgUSBIndex;
    //bool isUSB;

    if(p_comDeviceList->p_lightGunList[currentIndex]->IsLightGunUSB ())
    {
        tempHIDInfo = p_comDeviceList->p_lightGunList[currentIndex]->GetUSBHIDInfo ();
        if(!hidInfoList.contains (tempHIDInfo))
        {
            hidInfoList << tempHIDInfo;
            numberHIDDevices++;
        }
    }

    /*
    for(quint8 i = 0; i < numberLG; i++)
    {
        isUSB = p_comDeviceList->p_lightGunList[i]->IsLightGunUSB ();

        if(isUSB)
        {
            lgUSBIndex = FindLightGunHID(i);

            if(lgUSBIndex == -1)
            {
                tempHIDInfo = p_comDeviceList->p_lightGunList[i]->GetUSBHIDInfo ();
                hidInfoList << tempHIDInfo;
                numberHIDDevices++;
            }
        }
    }
    */

    if(numberHIDDevices > 0)
    {
        for(quint16 i = 0; i < numberHIDDevices; i++)
        {
            QString tempS;
            QTextStream tempTS(&tempS);
            tempTS << "VID: " << hidInfoList[i].vendorIDString << " PID: " << hidInfoList[i].productIDString << " " << hidInfoList[i].displayPath.first(ALIENUSBPATHFIRST);
            ui->usbDevicesComboBox->insertItem(i,tempS);
            //qDebug() << "Item: " << i << " is: " << tempS;
        }

        if(p_comDeviceList->p_lightGunList[currentIndex]->IsLightGunUSB ())
            ui->usbDevicesComboBox->setCurrentIndex (FindLightGunHID(currentIndex));
        else
            ui->usbDevicesComboBox->setCurrentIndex (0);
    }
    else
    {
        ui->usbDevicesComboBox->setCurrentIndex (-1);
    }

}


void editLightGunWindow::ProcessHIDInfo()
{
    HIDInfo tempHIDInfo;

    tempHIDInfo.vendorID = devs->vendor_id;
    tempHIDInfo.vendorIDString = QString::number(devs->vendor_id, 16).rightJustified(4, '0');
    tempHIDInfo.vendorIDString = tempHIDInfo.vendorIDString.toUpper ();
    tempHIDInfo.vendorIDString.prepend ("0x");

    tempHIDInfo.productID = devs->product_id;
    tempHIDInfo.productIDString = QString::number(devs->product_id, 16).rightJustified(4, '0');
    tempHIDInfo.productIDString = tempHIDInfo.productIDString.toUpper ();
    tempHIDInfo.productIDString.prepend ("0x");

    tempHIDInfo.path = QString::fromLatin1(devs->path);
    tempHIDInfo.displayPath = tempHIDInfo.path;
    tempHIDInfo.displayPath.remove(0,ALIENUSBFRONTPATHREM);

    tempHIDInfo.serialNumber = QString::fromWCharArray(devs->serial_number);
    tempHIDInfo.releaseNumber = devs->release_number;
    tempHIDInfo.releaseString = QString::number(devs->release_number, 16).rightJustified(4, '0');
    tempHIDInfo.releaseString = tempHIDInfo.releaseString.toUpper ();
    tempHIDInfo.releaseString.prepend ("0x");

    tempHIDInfo.manufacturer = QString::fromWCharArray(devs->manufacturer_string);
    tempHIDInfo.productDiscription = QString::fromWCharArray(devs->product_string);
    tempHIDInfo.interfaceNumber = devs->interface_number;
    tempHIDInfo.usagePage = devs->usage_page;
    tempHIDInfo.usage = devs->usage;
    tempHIDInfo.usageString = p_comDeviceList->ProcessHIDUsage(tempHIDInfo.usagePage, tempHIDInfo.usage);

    if(!lgHIDInfoList.contains (tempHIDInfo))
    {
        hidInfoList << tempHIDInfo;
        numberHIDDevices++;
    }
}





void editLightGunWindow::on_usbDevicesComboBox_currentIndexChanged(int index)
{
    if(index >= 0)
    {
        //Clear Out Old Data
        ui->locationLlineEdit->clear ();
        ui->descriptionLineEdit->clear ();
        ui->manufactureLineEdit->clear ();
        ui->serialNumLineEdit->clear ();
        ui->vendorIDLineEdit->clear ();
        ui->productIDLineEdit->clear ();
        ui->usageLineEdit->clear ();
        ui->releaseNumLineEdit->clear ();
        ui->vendorIDUSBLineEdit->clear ();
        ui->productIDUSBLineEdit->clear ();
        ui->displayPathUSBLineEdit->clear ();

        //Turn On The 2 USB Info Line Edits
        ui->usageLineEdit->setEnabled (true);
        ui->releaseNumLineEdit->setEnabled (true);

        //Fill in New Data
        ui->locationLlineEdit->insert(hidInfoList[index].path);
        ui->locationLlineEdit->setCursorPosition (0);

        ui->descriptionLineEdit->insert(hidInfoList[index].productDiscription);

        ui->manufactureLineEdit->insert(hidInfoList[index].manufacturer);

        ui->serialNumLineEdit->insert(hidInfoList[index].serialNumber);

        ui->vendorIDLineEdit->insert(hidInfoList[index].vendorIDString);

        ui->productIDLineEdit->insert(hidInfoList[index].productIDString);

        ui->usageLineEdit->insert(hidInfoList[index].usageString);

        ui->releaseNumLineEdit->insert(hidInfoList[index].releaseString);

        QString tempVID = hidInfoList[index].vendorIDString;
        //tempVID.remove (0,2);
        ui->vendorIDUSBLineEdit->insert (tempVID);

        QString tempPID = hidInfoList[index].productIDString;
        //tempPID.remove (0,2);
        ui->productIDUSBLineEdit->insert (tempPID);

        ui->displayPathUSBLineEdit->insert(hidInfoList[index].displayPath);
        ui->displayPathUSBLineEdit->setCursorPosition (0);

    }
    else if(index == -1)
    {
        //Clear Out Old Data
        ui->locationLlineEdit->clear ();
        ui->descriptionLineEdit->clear ();
        ui->manufactureLineEdit->clear ();
        ui->serialNumLineEdit->clear ();
        ui->vendorIDLineEdit->clear ();
        ui->productIDLineEdit->clear ();
        ui->usageLineEdit->clear ();
        ui->releaseNumLineEdit->clear ();
        ui->vendorIDUSBLineEdit->clear ();
        ui->productIDUSBLineEdit->clear ();
        ui->displayPathUSBLineEdit->clear ();
    }
}


qint16 editLightGunWindow::FindLightGunHID(quint8 lgNum)
{
    //Check if Light Gun is Not USB, if so return -1
    if(!p_comDeviceList->p_lightGunList[lgNum]->IsLightGunUSB () || numberHIDDevices == 0)
        return -1;

    bool isSerialNumber;
    qint16 matchNum = -1;
    quint8 numberMatches = 0;

    HIDInfo tempHIDInfo = p_comDeviceList->p_lightGunList[lgNum]->GetUSBHIDInfo ();

    //qDebug() << "isSerialNumber: " << isSerialNumber << " numberHIDDevices: " << numberHIDDevices << " tempHIDInfo.serialNumber: " << tempHIDInfo.serialNumber;

    for(quint16 i = 0; i < numberHIDDevices; i++)
    {

        if(tempHIDInfo == hidInfoList[i])
        {
            if(matchNum == -1)
                matchNum = i;
            numberMatches++;
        }

    }

    //qDebug() << "numberMatches: " << numberMatches;

    if(numberMatches == 1)
        return matchNum;
    else if(numberMatches > 1)
    {
        QMessageBox::warning (this, "Multiple Matches for USB HID Device", "There shouldn't multiple matches for the USB HID Light Gun. Returning the first match.");
        return matchNum;
    }
    else
        return -1;

}

void editLightGunWindow::on_allHIDDevicesCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked)
    {
        getAllHIDs = true;
        FillUSBDevicesComboBox();
    }
    else
    {
        getAllHIDs = false;
        FillUSBDevicesComboBox();
    }
}

