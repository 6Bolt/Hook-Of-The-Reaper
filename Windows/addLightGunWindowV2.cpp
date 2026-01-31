#include "addLightGunWindowV2.h"
#include "ui_addLightGunWindowV2.h"
#include "../Global.h"

addLightGunWindowV2::addLightGunWindowV2(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addLightGunWindowV2)
{
    ui->setupUi(this);

    QString tempQS;

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());

    numberHIDDevices = 0;
    getAllHIDs = false;
    isHIDInit = false;


    addLightGunSound.setSource (QUrl::fromLocalFile(":/sounds/addLightGun.wav"));
    addLightGunSound.setVolume(0.7);

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports & Used Dip Players
    p_comDeviceList = cdList;
    p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);
    p_comDeviceList->CopyUsedDipPlayersArray(usedDipPlayers, DIPSWITCH_NUMBER, 0);


    //Default Light Gun Combo Box - Adding Default Light Guns
    ui->defaultLightGunComboBox->insertItem(0,"");
    ui->defaultLightGunComboBox->insertItem(RS3_REAPER,REAPERNAME);
    ui->defaultLightGunComboBox->insertItem(MX24,MX24NAME);
    ui->defaultLightGunComboBox->insertItem(JBGUN4IR,JBGUN4IRNAME);
    ui->defaultLightGunComboBox->insertItem(FUSION,FUSIONNAME);
    ui->defaultLightGunComboBox->insertItem(BLAMCON,BLAMCONNAME);
    ui->defaultLightGunComboBox->insertItem(OPENFIRE,OPENFIRENAME);
    ui->defaultLightGunComboBox->insertItem(ALIENUSB,ALIENUSBNAME);
    ui->defaultLightGunComboBox->insertItem(XGUNNER,XGUNNERNAME);
    ui->defaultLightGunComboBox->insertItem(AIMTRAK,AIMTRAKNAME);
    ui->defaultLightGunComboBox->insertItem(XENAS,XENASNAME);
    ui->defaultLightGunComboBox->insertItem(XENASBTLE,XENASBTLENAME);
    ui->defaultLightGunComboBox->insertItem(SINDEN,SINDENNAME);
    ui->defaultLightGunComboBox->setCurrentIndex (0);

    //COM Port Combo Box - Adding Available COM Ports
    for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
    {
        if(unusedComPort[comPortIndx])
            tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
        else
            tempQS = "";
        ui->comPortComboBox->insertItem(comPortIndx,tempQS);
    }


    //Baud Speed Combo Box - Adding in Speed Rates
    for(quint8 comPortIndx=0;comPortIndx<BAUD_NUMBER;comPortIndx++)
    {
        ui->baudSpeedComboBox->insertItem(comPortIndx,BAUDNAME_ARRAY[comPortIndx]);
    }

    //Data Bits Combo Box - Adding in Data Bit Options
    for(quint8 comPortIndx=0;comPortIndx<DATABITS_NUMBER;comPortIndx++)
    {
        ui->dataBitsComboBox->insertItem(comPortIndx,DATABITSNAME_ARRAY[comPortIndx]);
    }


    //Parity Combo Box - Adding Parity Options
    for(quint8 comPortIndx=0;comPortIndx<PARITY_NUMBER;comPortIndx++)
    {
        ui->parityComboBox->insertItem(comPortIndx,PARITYNAME_ARRAY[comPortIndx]);
    }


    //Stop Bits Combo Box - Adding in Stop Bit Options
    for(quint8 comPortIndx=0;comPortIndx<STOPBITS_NUMBER;comPortIndx++)
    {
        ui->stopBitsComboBox->insertItem(comPortIndx,STOPBITSNAME_ARRAY[comPortIndx]);
    }


    //Flow Control Combo Box - Adding in Flow Options
    for(quint8 comPortIndx=0;comPortIndx<FLOW_NUMBER;comPortIndx++)
    {
        ui->flowControlComboBox->insertItem(comPortIndx,FLOWNAME_ARRAY[comPortIndx]);
    }

    //Dip Switch Players - Based on usedDipPlayers
    for(quint8 comPortIndx=0;comPortIndx<DIPSWITCH_NUMBER;comPortIndx++)
    {
        if(usedDipPlayers[comPortIndx])
            tempQS = "";
        else
            tempQS = "P"+QString::number(comPortIndx+1) + " USB";
        ui->dipSwitchComboBox->insertItem(comPortIndx,tempQS);
    }

    //Hub COM Port Combo Box - Adding All COM Ports
    for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
    {
        tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
        ui->hubComComboBox->insertItem(comPortIndx,tempQS);
    }

    //Fill Recoil Combo Boxes
    for(quint8 index=0;index<NUMBEROFRECOILS;index++)
    {
        ui->recoilComboBox->insertItem(index, QString::number(index));
        ui->ammoValueComboBox->insertItem(index, QString::number(index));
        ui->recoilR2SComboBox->insertItem(index, QString::number(index));
        ui->recoilValueComboBox->insertItem(index, QString::number(index));
    }

    ui->recoilComboBox->setCurrentIndex(0);
    ui->ammoValueComboBox->setCurrentIndex(1);
    ui->recoilR2SComboBox->setCurrentIndex(2);
    ui->recoilValueComboBox->setCurrentIndex(3);

    ui->reloadRadioButton->setChecked(true);

    //Only for MX24 Light Gun
    ui->dipSwitchComboBox->setEnabled(false);
    ui->hubComComboBox->setEnabled(false);
    //Only for JB Gun4IR & OpenFire
    //ui->analogLineEdit->setInputMask (ANALOGSTRENGTHMASK);
    //ui->analogLineEdit->setEnabled(false);

    this->FillUSBDevicesComboBox();

    //ui->vendorIDUSBLineEdit->setEnabled(false);
    //ui->productIDUSBLineEdit->setEnabled(false);
    //ui->displayPathUSBLineEdit->setEnabled(false);
    ui->usbDevicesComboBox->setEnabled(false);

    //Make the Reaper Large Ammo only accept 3 numbers
    ui->reaperLargeAmmoLineEdit->setInputMask (REAPERAMMO0DELAYMASK);

    //Make the Reaper Ammo 0 Delay only accepted 3 Numbers
    ui->reaperDelayTimeLineEdit->setInputMask (REAPERAMMO0DELAYMASK);

    //Make the Reaper Hold Slide Time only accepted Numbers (0.0 to 9.9)
    ui->reaperSlideHoldTimeLineEdit->setInputMask (REAPERHOLDSLIDEMASK);



    ui->reaperDelayTimeLineEdit->setText(DEFAULTAMMODELAYS);
    ui->reaperSlideHoldTimeLineEdit->setText(DEFAULTHOLDTIMES);

    ui->reaperLargeAmmoLineEdit->setText(LARGEAMMOVALUEDEFAULTS);

    //Fill out Serial Port 0 Info
    FillSerialPortInfo(0);

    //Change Needed Labels to Red
    ChangeLabels(0);

    ui->lgLCDNumber->setSegmentStyle(QLCDNumber::Flat);
    numberLightGuns = p_comDeviceList->GetNumberLightGuns ();
    ui->lgLCDNumber->setDigitCount(3);
    ui->lgLCDNumber->display (numberLightGuns);

    //Fill in for TCP Server

    //Only Digits Go Into TCP Server Line Edit
    ui->tcpPortLineEdit->setInputMask (TCPSERVERMASK);
    ui->tcpPortLineEdit->setText (TCPSERVERDEFAULT);
    ui->tcpPortLineEdit->setEnabled (false);

    tcpPort = TCPSERVERDEFAULTNUM;
    tcpPlayer = p_comDeviceList->GetTCPPortPlayerInfo(tcpPort);

    if(tcpPlayer == -1)
    {
        ui->tcpPlayerComboBox->insertItem(0,"Player 1");
        ui->tcpPlayerComboBox->insertItem(1,"Player 2");
        ui->tcpPlayerComboBox->setCurrentIndex (0);

    }
    else if(tcpPlayer == TCPPLAYER1)
    {
        ui->tcpPlayerComboBox->insertItem(0,"");
        ui->tcpPlayerComboBox->insertItem(1,"Player 2");
        ui->tcpPlayerComboBox->setCurrentIndex (1);
    }
    else if(tcpPlayer == TCPPLAYER2)
    {
        ui->tcpPlayerComboBox->insertItem(0,"Player 1");
        ui->tcpPlayerComboBox->insertItem(1,"");
        ui->tcpPlayerComboBox->setCurrentIndex (0);
    }
    else if(tcpPlayer == 0)
    {
        ui->tcpPlayerComboBox->insertItem(0,"");
        ui->tcpPlayerComboBox->insertItem(1,"");
        ui->tcpPlayerComboBox->setCurrentIndex (0);
    }

    ui->tcpPlayerComboBox->setEnabled (false);

    //Fill the TCP Recoil Voltage Combo Box: 0-10
    //tcpRecoilVoltComboBox
    for(quint8 i=0;i<RECOILVOLTAGESETNUM;i++)
        ui->tcpRecoilVoltComboBox->insertItem(i,QString::number(i));

    ui->tcpRecoilVoltComboBox->setCurrentIndex (RECOILVOLTDEFAULT);
    ui->tcpRecoilVoltComboBox->setEnabled (false);

}

//Deconstructor
addLightGunWindowV2::~addLightGunWindowV2()
{
    delete ui;

    //Delete temp ComPortDevice pointer
    if(p_comPortInfo != nullptr)
        delete p_comPortInfo;

    //Exit the USB HID
    hid_exit ();
}

//private slots
void addLightGunWindowV2::on_defaultLightGunComboBox_currentIndexChanged(int index)
{
    //Set Data for Default Guns
    if(index > 0)
    {
        //Sets Default Light Gun Settings
        ui->baudSpeedComboBox->setCurrentIndex (DEFAULTLG_ARRAY[index].BAUD);
        ui->dataBitsComboBox->setCurrentIndex (DEFAULTLG_ARRAY[index].DATA);
        ui->parityComboBox->setCurrentIndex (DEFAULTLG_ARRAY[index].PARITY);
        ui->stopBitsComboBox->setCurrentIndex (DEFAULTLG_ARRAY[index].STOP);
        ui->flowControlComboBox->setCurrentIndex (DEFAULTLG_ARRAY[index].FLOW);

        //Disable Combo Boxes for Default Light Gun
        SetEnableComboBoxes(false);


        if(index == MX24)
        {
            ui->dipSwitchComboBox->setEnabled(true);
            ui->hubComComboBox->setEnabled(true);
            ui->comPortComboBox->setEnabled(false);

            quint8 hubIndex = ui->hubComComboBox->currentIndex ();

            FillSerialPortInfo(hubIndex);
        }
        else if(index != ALIENUSB && index != AIMTRAK && index != SINDEN)
        {
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->comPortComboBox->setEnabled(true);

            quint8 comPortIndex = ui->comPortComboBox->currentIndex ();
            FillSerialPortInfo(comPortIndex);
        }
        else
        {
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->comPortComboBox->setEnabled(false);
        }

        //if(index == JBGUN4IR || index == OPENFIRE)
        //    ui->analogLineEdit->setEnabled(true);
        //else
        //{
        //    ui->analogLineEdit->clear ();
        //    ui->analogLineEdit->setEnabled(false);
        //}

        //USB Light Gun, Not Serial Port
        if(index == ALIENUSB || index == AIMTRAK)
        {
            //Turn Off COM Port Combo Box
            //ui->comPortComboBox->setEnabled(false);

            //Turn On USB Combo Box
            ui->usbDevicesComboBox->setEnabled(true);

            //Get USB Combo Box Index, and then Display USB data of the Index
            qint16 usbIndex = ui->usbDevicesComboBox->currentIndex ();
            on_usbDevicesComboBox_currentIndexChanged(usbIndex);

            if(index == AIMTRAK)
            {
                ui->allHIDDevicesCheckBox->setEnabled (false);
            }
            else
            {
                ui->allHIDDevicesCheckBox->setEnabled (true);
            }

            ui->allHIDDevicesCheckBox->setChecked (false);

            this->FillUSBDevicesComboBox();
        }
        else
        {
            ui->usbDevicesComboBox->setEnabled(false);
        }

        //For Recoil Options
        if(index == FUSION || index == JBGUN4IR || index == OPENFIRE)
        {
            ui->recoilValueComboBox->setEnabled(true);
            ui->recoilComboBox->setItemText (3, "3");
            ui->ammoValueComboBox->setItemText (3, "3");
            ui->recoilR2SComboBox->setItemText (3, "3");
        }
        else
        {
            ui->recoilValueComboBox->setCurrentIndex(3);
            ui->recoilValueComboBox->setEnabled(false);
            ui->recoilComboBox->setItemText (3, "");
            ui->ammoValueComboBox->setItemText (3, "");
            ui->recoilR2SComboBox->setItemText (3, "");
        }


        //For Reload Options
        if(index == ALIENUSB || index == AIMTRAK || index == MX24)
        {
            ui->reloadRadioButton->setEnabled(false);
            ui->reloadJustLEDRadioButton->setEnabled(false);
            ui->disableReloadradioButton->setEnabled(false);
            ui->reloadJustShakeRadioButton->setEnabled(false);

            ui->damageRadioButton->setEnabled(false);
            ui->damageJustLEDRadioButton->setEnabled(false);
            ui->disableDamageradioButton->setEnabled(false);
            ui->damageJustShakeRadioButton->setEnabled(false);

            ui->deathRadioButton->setEnabled(false);
            ui->deathJustLEDRadioButton->setEnabled(false);
            ui->disableDeathradioButton->setEnabled(false);
            ui->deathJustShakeRadioButton->setEnabled(false);

            ui->shakeRadioButton->setEnabled(false);
            ui->disableShakeradioButton->setEnabled(false);

            //ui->recoilComboBox->setCurrentIndex(0);
            //ui->ammoValueComboBox->setCurrentIndex(1);
            //ui->recoilR2SComboBox->setCurrentIndex(2);
        }
        else if(index == XGUNNER || index == RS3_REAPER || index == SINDEN)
        {
            if(ui->reloadJustLEDRadioButton->isChecked () || ui->reloadJustShakeRadioButton->isChecked ())
                ui->reloadRadioButton->setChecked(true);

            if(ui->damageJustLEDRadioButton->isChecked () || ui->damageJustShakeRadioButton->isChecked ())
                ui->damageRadioButton->setChecked(true);

            if(ui->deathJustLEDRadioButton->isChecked () || ui->deathJustShakeRadioButton->isChecked ())
                ui->deathRadioButton->setChecked(true);

            ui->reloadRadioButton->setEnabled(true);
            ui->reloadJustLEDRadioButton->setEnabled(false);
            ui->reloadJustShakeRadioButton->setEnabled(false);
            ui->disableReloadradioButton->setEnabled(true);

            ui->damageRadioButton->setEnabled(true);
            ui->damageJustLEDRadioButton->setEnabled(false);
            ui->damageJustShakeRadioButton->setEnabled(false);
            ui->disableDamageradioButton->setEnabled(true);

            ui->deathRadioButton->setEnabled(true);
            ui->deathJustLEDRadioButton->setEnabled(false);
            ui->deathJustShakeRadioButton->setEnabled(false);
            ui->disableDeathradioButton->setEnabled(true);

            ui->shakeRadioButton->setEnabled(true);
            ui->disableShakeradioButton->setEnabled(true);
        }
        else
        {
            ui->reloadRadioButton->setEnabled(true);
            ui->reloadJustLEDRadioButton->setEnabled(true);
            ui->reloadJustShakeRadioButton->setEnabled(true);
            ui->disableReloadradioButton->setEnabled(true);

            ui->damageRadioButton->setEnabled(true);
            ui->damageJustLEDRadioButton->setEnabled(true);
            ui->damageJustShakeRadioButton->setEnabled(true);
            ui->disableDamageradioButton->setEnabled(true);

            ui->deathRadioButton->setEnabled(true);
            ui->deathJustLEDRadioButton->setEnabled(true);
            ui->deathJustShakeRadioButton->setEnabled(true);
            ui->disableDeathradioButton->setEnabled(true);

            ui->shakeRadioButton->setEnabled(true);
            ui->disableShakeradioButton->setEnabled(true);

            //ui->reloadRadioButton->setChecked(true);
            //ui->damageRadioButton->setChecked(true);
            //ui->deathRadioButton->setChecked(true);
            //ui->shakeRadioButton->setChecked(true);
        }

        //For Default Recoil Priority
        if(index == RS3_REAPER || index == SINDEN)
        {
            ui->recoilComboBox->setCurrentIndex(1);
            ui->ammoValueComboBox->setCurrentIndex(0);
            ui->recoilR2SComboBox->setCurrentIndex(2);
        }
        else
        {
            ui->recoilComboBox->setCurrentIndex(0);
            ui->ammoValueComboBox->setCurrentIndex(1);
            ui->recoilR2SComboBox->setCurrentIndex(2);
        }



        if(index == SINDEN)
        {
            ui->tcpPortLineEdit->setEnabled (true);
            ui->tcpPlayerComboBox->setEnabled (true);
            ui->tcpRecoilVoltComboBox->setEnabled (true);
            //ui->comPortComboBox->setEnabled(false);
        }
        else
        {
            ui->tcpPortLineEdit->setEnabled (false);
            ui->tcpPlayerComboBox->setEnabled (false);
            ui->tcpRecoilVoltComboBox->setEnabled (false);
        }

    }
    else
    {
        //Re-enable Combo Boxes when No Default Light is Selected
        SetEnableComboBoxes(true);

        ui->comPortComboBox->setEnabled(true);

        quint8 comPortIndex = ui->comPortComboBox->currentIndex ();

        FillSerialPortInfo(comPortIndex);

        ui->dipSwitchComboBox->setEnabled(false);
        //ui->analogLineEdit->clear ();
        //ui->analogLineEdit->setEnabled(false);
        ui->hubComComboBox->setEnabled(false);

        ui->usbDevicesComboBox->setEnabled(false);

        //Recoil Options
        ui->recoilValueComboBox->setEnabled(true);
        ui->recoilComboBox->setItemText (3, "3");
        ui->ammoValueComboBox->setItemText (3, "3");
        ui->recoilR2SComboBox->setItemText (3, "3");

        //Reload Options
        ui->reloadRadioButton->setEnabled(true);
        ui->reloadJustLEDRadioButton->setEnabled(true);
        ui->reloadJustShakeRadioButton->setEnabled(true);
        ui->disableReloadradioButton->setEnabled(true);

        ui->damageRadioButton->setEnabled(true);
        ui->damageJustLEDRadioButton->setEnabled(true);
        ui->damageJustShakeRadioButton->setEnabled(true);
        ui->disableDamageradioButton->setEnabled(true);

        ui->deathRadioButton->setEnabled(true);
        ui->deathJustLEDRadioButton->setEnabled(true);
        ui->deathJustShakeRadioButton->setEnabled(true);
        ui->disableDeathradioButton->setEnabled(true);

        ui->shakeRadioButton->setEnabled(true);
        ui->disableShakeradioButton->setEnabled(true);
    }

    ChangeLabels(index);
}


void addLightGunWindowV2::on_comPortComboBox_currentIndexChanged(int index)
{
    FillSerialPortInfo(index);
}


void addLightGunWindowV2::on_addPushButton_clicked()
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
        AddLightGun();

        //Rebuild COM Port Combo Box, Now with the Used COM port Removed
        p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);

        //COM Port Combo Box
        for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
        {
            if(unusedComPort[comPortIndx])
                tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
            else
                tempQS = "";
            ui->comPortComboBox->setItemText (comPortIndx,tempQS);
        }

        for(quint8 comPortIndx=0;comPortIndx<DIPSWITCH_NUMBER;comPortIndx++)
        {
            if(usedDipPlayers[comPortIndx])
                tempQS = "";
            else
                tempQS = "P"+QString::number(comPortIndx+1) + " USB";
            ui->dipSwitchComboBox->setItemText(comPortIndx,tempQS);
        }
    }
}


void addLightGunWindowV2::on_okPushButton_clicked()
{
    bool dataValid;

    //Check Input Value of the New Light gun Data
    dataValid = IsValidData();

    if(dataValid)
    {
        //Check if it is Default Light gun
        IsDefaultLightGun();

        //Add New Light Gun to ComDeviceList
        AddLightGun();

        QThread::sleep(1);

        //Close and Destroy the Window
        accept ();
    }
}


void addLightGunWindowV2::on_cancelPushButton_clicked()
{
    //Close and Destroy the Window
    accept ();
}


void addLightGunWindowV2::on_hubComComboBox_currentIndexChanged(int index)
{
    QString tempQS;
    p_comDeviceList->CopyUsedDipPlayersArray(usedDipPlayers, DIPSWITCH_NUMBER, index);

    for(quint8 comPortIndx=0;comPortIndx<DIPSWITCH_NUMBER;comPortIndx++)
    {
        if(usedDipPlayers[comPortIndx])
            tempQS = "";
        else
            tempQS = "P"+QString::number(comPortIndx+1) + " USB";
        ui->dipSwitchComboBox->setItemText(comPortIndx,tempQS);
    }


    FillSerialPortInfo(index);
}


void addLightGunWindowV2::on_usbDevicesComboBox_currentIndexChanged(int index)
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


        //Turn On The 2 USB Info Line Edits
        //ui->usageLineEdit->setEnabled (true);
        //ui->releaseNumLineEdit->setEnabled (true);

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
    }
}


void addLightGunWindowV2::on_allHIDDevicesCheckBox_checkStateChanged(const Qt::CheckState &arg1)
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


void addLightGunWindowV2::on_tcpPortLineEdit_textChanged(const QString &arg1)
{
    //qDebug() << arg1;

    bool isNumber;
    quint16 tempPort = arg1.toUInt (&isNumber);
    qint8 tempPlayer = p_comDeviceList->GetTCPPortPlayerInfo(tempPort);

    if(isNumber)
    {
        if(tempPlayer == -1)
        {
            ui->tcpPlayerComboBox->setItemText (0, TCPPLAYER1NAME);
            ui->tcpPlayerComboBox->setItemText(1, TCPPLAYER2NAME);
            ui->tcpPlayerComboBox->setCurrentIndex (0);

        }
        else if(tempPlayer == TCPPLAYER1)
        {
            ui->tcpPlayerComboBox->setItemText(0,"");
            ui->tcpPlayerComboBox->setItemText(1, TCPPLAYER2NAME);
            ui->tcpPlayerComboBox->setCurrentIndex (1);
        }
        else if(tempPlayer == TCPPLAYER2)
        {
            ui->tcpPlayerComboBox->setItemText(0, TCPPLAYER1NAME);
            ui->tcpPlayerComboBox->setItemText(1,"");
            ui->tcpPlayerComboBox->setCurrentIndex (0);
        }
        else if(tempPlayer == 0)
        {
            ui->tcpPlayerComboBox->setItemText(0,"");
            ui->tcpPlayerComboBox->setItemText(1,"");
            ui->tcpPlayerComboBox->setCurrentIndex (0);
        }
    }
}


//private

bool addLightGunWindowV2::IsValidData()
{
    QString ivTemp, message;
    bool lgnIsEmpty = false;
    bool comPortNumEmpty = false;
    bool unusedName = false;
    bool analNotNumber = false;
    bool analNotRange = false;
    bool badDipIndex = false;
    bool hubComLG = false;
    bool emptyUSBdelay = false;
    bool usbDisPMatch = false;
    bool recoilCheck = false;
    bool badTCPPort = false;
    bool badTCPPlayer = false;
    bool reaperDelayTime = false;
    bool reaperHoldTime = false;
    bool reaperLargeAmmo = false;
    quint8 numLightGuns = p_comDeviceList->GetNumberLightGuns();
    quint8 i;
    quint8 defaultLGIndex = ui->defaultLightGunComboBox->currentIndex ();
    quint8 hubComIndex = ui->hubComComboBox->currentIndex ();
    //quint8 comPortIndex =  ui->comPortComboBox->currentIndex ();

    //Check If Light Gun Name Line Edit Box is Empty
    ivTemp = ui->lightGunNameLineEdit->text();
    if(ivTemp.isEmpty ())
        lgnIsEmpty = true;


    if(numLightGuns > 0)
    {
        for(i = 0; i < numLightGuns; i++)
        {
            if(ivTemp == p_comDeviceList->p_lightGunList[i]->GetLightGunName ())
                unusedName = true;
        }
    }



    if(defaultLGIndex == MX24)
    {
        quint8 dipIndex = ui->dipSwitchComboBox->currentIndex ();
        if(usedDipPlayers[dipIndex])
            badDipIndex = true;


        for(i = 0; i < numLightGuns; i++)
        {
            if(hubComIndex == p_comDeviceList->p_lightGunList[i]->GetComPortNumberBypass ())
                hubComLG = true;
        }

    }
    else if(defaultLGIndex != ALIENUSB && defaultLGIndex != AIMTRAK && defaultLGIndex != XENASBTLE && defaultLGIndex != SINDEN)
    {
        //Check If the Com Port Name is Empty for the Combo Box
        ivTemp = ui->comPortComboBox->currentText ();
        if(ivTemp.isEmpty ())
            comPortNumEmpty = true;
    }

    if(defaultLGIndex == JBGUN4IR || defaultLGIndex == OPENFIRE)
    {
        //bool isNumber;
        //QString analString = ui->analogLineEdit->text();
        quint16 analStrengthBig = DEFAULTANALOGSTRENGTH;  //analString.toUInt (&isNumber);
        //if(!isNumber)
        //    analNotNumber = true;
        //else
        //{
        //    if(analStrengthBig > 255)
        //        analNotRange = true;
        //}
    }

    if(defaultLGIndex == ALIENUSB || defaultLGIndex == AIMTRAK)
    {
        QString tempDP;
        quint8 hidIndex = ui->usbDevicesComboBox->currentIndex ();


        //Check is the Display Path Already Exist
        tempDP = hidInfoList[hidIndex].displayPath;
        usbDisPMatch = p_comDeviceList->CheckUSBPath(tempDP);

    }

    if(defaultLGIndex == SINDEN)
    {
        //Check Port Number
        bool isNumber;
        QString tcpPortS = ui->tcpPortLineEdit->text();
        quint32 tcpPort = tcpPortS.toUInt (&isNumber);

        if(!isNumber || tcpPort > MAXTCPPORT || tcpPort == 0)
            badTCPPort = true;

        //Now Check if Player Availible
        quint8 playerInfo = ui->tcpPlayerComboBox->currentIndex ();
        bool goodTCPPort = p_comDeviceList->CheckTCPPortPlayer(tcpPort, playerInfo);

        if(!goodTCPPort)
            badTCPPlayer = true;

    }

    if(CheckRecoilComboBoxes() == false)
        recoilCheck = true;

    //Check Reaper Hold Slide Settings
    if(defaultLGIndex == RS3_REAPER)
    {
        //Large Ammo
        QString largeAmmo = ui->reaperLargeAmmoLineEdit->text ();
        quint16 largeAmmoNum = largeAmmo.toUInt();

        if(largeAmmoNum > 255 || largeAmmoNum < 10)
            reaperLargeAmmo = true;

        if(!ui->reaperDisableSlideCheckBox->isChecked() && ui->reaperEnableSlideDelayCheckBox->isChecked())
        {
            //Get delay time
            QString delayTime = ui->reaperDelayTimeLineEdit->text ();
            quint16 delayTimeNum = delayTime.toUInt();

            //Get Hold Time
            QString tempRHSBT = ui->reaperSlideHoldTimeLineEdit->text ();
            float reaperHoldSlideTimeSec = tempRHSBT.toFloat ();

            if(delayTimeNum > 255 || delayTimeNum == 0)
                reaperDelayTime = true;

            if(reaperHoldSlideTimeSec < 1.0f || reaperHoldSlideTimeSec > 9.0f)
                reaperHoldTime = true;            
        }
    }


    //Check All the bools, If They are false, data is good and return true.
    //If false, then make a Warning string, and Pop Up a Warning Message Box on What is Wrong and return false
    if(lgnIsEmpty == false && comPortNumEmpty == false && unusedName == false && analNotNumber == false && analNotRange == false && badDipIndex == false && hubComLG == false && usbDisPMatch == false && recoilCheck == false && badTCPPort == false && badTCPPlayer == false && reaperDelayTime == false && reaperHoldTime == false && reaperLargeAmmo == false)
    {
        return true;
    }
    else
    {
        if(lgnIsEmpty == true)
            message.append ("Light Gun Name is empty. Please enter a name for the Light Gun Name. ");
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
            message.append ("An Exsiting USB Light Gun already has the same path/location. There cannot be duplicates that have the same path/location. As it is used to send data to the device.");
        if(recoilCheck == true)
            message.append ("Two or more of the numbers match for the Recoil Priority. None of the numbers can match");
        if(badTCPPort == true)
            message.append ("TCP Port number is not a number or not in range of 1-65,535. Port cannot be 0 or larger than 65,535.");
        if(badTCPPlayer == true)
            message.append ("TCP Port number has that player assigned already. Try different player number, or you have to delete light gun");
        if(reaperDelayTime == true)
            message.append ("Reaper delay needs to be in the range of 1ms to 255ms.");
        if(reaperHoldTime == true)
            message.append ("Reaper hold time needs to be in the range of 1.0s to 9.0s.");
        if(reaperLargeAmmo == true)
            message.append ("Reaper large ammo needs to be in the range of 10 to 255.");

        QMessageBox::warning (this, "Can Not Add Light Gun", message);

        return false;
    }
    return false;
}


bool addLightGunWindowV2::IsDefaultLightGun()
{
    quint8 dlgIndex = ui->defaultLightGunComboBox->currentIndex ();

    if(dlgIndex > 0)
    {
        defaultLightGun = true;
        defaultLightGunNum = dlgIndex;
        return true;
    }

    defaultLightGun = false;
    defaultLightGunNum = 0;
    return false;
}


void addLightGunWindowV2::AddLightGun()
{
    quint8 tempHub;
    bool reloadNR = false;
    bool reloadDisable = false;
    SupportedRecoils recoilOptions;
    quint8 reloadSet, damageSet, deathSet;
    bool shakeEn;
    LightGunSettings lgSet;

    //Collect Light Gun Name, Number, COM Port Number & Name, and Serial Port Info
    lightGunName = ui->lightGunNameLineEdit->text();
    lightGunNum = p_comDeviceList->GetNumberLightGuns ();

    if(defaultLightGun && defaultLightGunNum == MX24)
    {
        comPortNum = UNASSIGN;
        tempHub = ui->hubComComboBox->currentIndex ();
        comPortName = BEGINCOMPORTNAME+QString::number(tempHub);
    }
    else if(!defaultLightGun || (defaultLightGun && defaultLightGunNum != ALIENUSB))
    {
        comPortNum = ui->comPortComboBox->currentIndex ();
        comPortName = BEGINCOMPORTNAME+QString::number(comPortNum);
    }

    if(!defaultLightGun || (defaultLightGun && (defaultLightGunNum != ALIENUSB || defaultLightGunNum != AIMTRAK)))
    {
        p_comPortInfo = new QSerialPortInfo(comPortName);

        //Collect COM Port Settings: BAUD, Data Bits, Parity, Stop Bits, and Flow Control
        FindBaudRate();
        FindDataBits();
        FindParity();
        FindStopBits();
        FindFlow();
    }

    //Get Recoil Options
    recoilOptions.recoil = ui->recoilComboBox->currentIndex();
    recoilOptions.ammoValue = ui->ammoValueComboBox->currentIndex();
    recoilOptions.recoilR2S = ui->recoilR2SComboBox->currentIndex();
    recoilOptions.recoilValue = ui->recoilValueComboBox->currentIndex();

    //Get Reload Options
    if(defaultLightGun && (defaultLightGunNum == MX24 || defaultLightGunNum == ALIENUSB || defaultLightGunNum == AIMTRAK))
    {
        reloadSet = 0;
        damageSet = 0;
        deathSet = 0;
        shakeEn = false;
    }
    else if(defaultLightGun && (defaultLightGunNum == RS3_REAPER || defaultLightGunNum == XGUNNER || defaultLightGunNum == SINDEN))
    {
        if(ui->reloadRadioButton->isChecked ())
            reloadSet = 1;
        else
            reloadSet = 0;

        if(ui->damageRadioButton->isChecked ())
            damageSet = 1;
        else
            damageSet = 0;

        if(ui->deathRadioButton->isChecked ())
            deathSet = 1;
        else
            deathSet = 0;

        if(ui->shakeRadioButton->isChecked ())
            shakeEn = true;
        else
            shakeEn = false;
    }
    else
    {
        if(ui->reloadRadioButton->isChecked ())
            reloadSet = 1;
        else if(ui->reloadJustShakeRadioButton->isChecked ())
            reloadSet = 2;
        else if(ui->reloadJustLEDRadioButton->isChecked ())
            reloadSet = 3;
        else
            reloadSet = 0;

        if(ui->damageRadioButton->isChecked ())
            damageSet = 1;
        else if(ui->damageJustShakeRadioButton->isChecked ())
            damageSet = 2;
        else if(ui->damageJustLEDRadioButton->isChecked ())
            damageSet = 3;
        else
            damageSet = 0;

        if(ui->deathRadioButton->isChecked ())
            deathSet = 1;
        else if(ui->deathJustShakeRadioButton->isChecked ())
            deathSet = 2;
        else if(ui->deathJustLEDRadioButton->isChecked ())
            deathSet = 3;
        else
            deathSet = 0;

        if(ui->shakeRadioButton->isChecked ())
            shakeEn = true;
        else
            shakeEn = false;
    }

    lgSet.reload = reloadSet;
    lgSet.damage = damageSet;
    lgSet.death = deathSet;
    lgSet.shake = shakeEn;


    //Set Player Dip Switch for MX24 Light Gun, since it is needed for commands
    if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        bool disableLEDs;
        quint8 largeAmmoNum;
        ReaperSlideData slideData;

        if(ui->reaperDisableLEDsCheckBox->isChecked ())
            disableLEDs = true;
        else
            disableLEDs = false;

        QString largeAmmo = ui->reaperLargeAmmoLineEdit->text ();
        largeAmmoNum = largeAmmo.toUInt();

        if(ui->reaperDisableSlideCheckBox->isChecked())
            slideData.disableHoldBack = true;
        else
            slideData.disableHoldBack = false;

        if(ui->reaperEnableSlideDelayCheckBox->isChecked())
            slideData.enableHoldDelay = true;
        else
            slideData.enableHoldDelay = false;

        if(!slideData.disableHoldBack && slideData.enableHoldDelay)
        {
            //Get delay time
            QString delayTime = ui->reaperDelayTimeLineEdit->text ();
            slideData.holdDelay = delayTime.toUInt();

            //Get Hold Time
            QString tempRHSBT = ui->reaperSlideHoldTimeLineEdit->text ();
            float reaperHoldSlideTimeSec = tempRHSBT.toFloat ();
            slideData.slideHoldTime = static_cast<quint16>(reaperHoldSlideTimeSec*1000.0f);

        }
        else
        {
            slideData.holdDelay = DEFAULTAMMO0DELAY;
            slideData.slideHoldTime = REAPERHOLDSLIDETIME;
        }

        //Create a New Reaper Light Gun Class
        p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, comPortNum, comPortName, *p_comPortInfo, comPortBaud, comPortDataBits, comPortParity, comPortStopBits, comPortFlow, recoilOptions, lgSet, disableLEDs, largeAmmoNum, slideData);
    }
    else if(defaultLightGun && defaultLightGunNum == MX24)
    {
        quint8 tempDS = ui->dipSwitchComboBox->currentIndex ();

        tempHub = ui->hubComComboBox->currentIndex ();

        usedDipPlayers[tempDS] = true;

        //Create a New MX24 Light Gun Class
        p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, comPortNum, comPortName, *p_comPortInfo, comPortBaud, comPortDataBits, comPortParity, comPortStopBits, comPortFlow, true, tempDS, tempHub, recoilOptions);
    }
    else if(defaultLightGun && defaultLightGunNum == OPENFIRE)
    {
        bool noDisplay;
        DisplayPriority displayPri;
        DisplayOpenFire displayOF;

        if(ui->ammoRadioButton->isChecked ())
        {
            displayPri.ammo = true;
            displayPri.life = false;
        }
        else
        {
            displayPri.ammo = false;
            displayPri.life = true;
        }

        if(ui->displayOtherCheckBox->isChecked ())
            displayPri.other = true;
        else
            displayPri.other = false;

        noDisplay = ui->openFireNoDisplayCheckBox->isChecked ();

        if(noDisplay)
        {
            displayOF.ammoAndLifeDisplay = false;
            displayOF.lifeGlyphs = true;
            displayOF.lifeBar = false;
            displayOF.lifeNumber = false;
        }
        else
        {

            if(ui->openFireBothCheckBox->isChecked ())
                displayOF.ammoAndLifeDisplay = true;
            else
                displayOF.ammoAndLifeDisplay = false;

            if(ui->openFireGlyphRadioButton->isChecked ())
            {
                displayOF.lifeGlyphs = true;
                displayOF.lifeBar = false;
                displayOF.lifeNumber = false;
            }
            else if(ui->openFireBarRadioButton->isChecked ())
            {
                displayOF.lifeGlyphs = false;
                displayOF.lifeBar = true;
                displayOF.lifeNumber = false;
            }
            else
            {
                displayOF.lifeGlyphs = false;
                displayOF.lifeBar = false;
                displayOF.lifeNumber = true;
            }
        }

        //Create a New OpenFire Light Gun Class
        p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, comPortNum, comPortName, *p_comPortInfo, comPortBaud, comPortDataBits, comPortParity, comPortStopBits, comPortFlow, recoilOptions, lgSet, noDisplay, displayPri, displayOF);
    }
    else if(defaultLightGun && (defaultLightGunNum == ALIENUSB || defaultLightGunNum == AIMTRAK))
    {
        quint16 usbIndex = ui->usbDevicesComboBox->currentIndex ();

        if(defaultLightGunNum == ALIENUSB)
        {
            bool n2DDisplay;
            DisplayPriority displayPri;

            if(ui->no2DigitCheckBox->isChecked ())
                n2DDisplay = true;
            else
                n2DDisplay = false;

            if(ui->ammoRadioButton->isChecked ())
            {
                displayPri.ammo = true;
                displayPri.life = false;
            }
            else
            {
                displayPri.ammo = false;
                displayPri.life = true;
            }

            if(ui->displayOtherCheckBox->isChecked ())
                displayPri.other = true;
            else
                displayPri.other = false;


            //Create a New Alien USB or Ultimarc Aimtrak Light Gun
            p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, hidInfoList[usbIndex], recoilOptions, n2DDisplay, displayPri);
        }
        else if(defaultLightGunNum == AIMTRAK)
        {
            p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, hidInfoList[usbIndex], AIMTRAKDELAYDFLT, recoilOptions);
        }

        FillUSBDevicesComboBox();
    }
    else if(defaultLightGun && (defaultLightGunNum == SINDEN))
    {
        //Get TCP Port Number and Player Number. Player 1 = 0, Player 2 = 1
        QString tcpPortS = ui->tcpPortLineEdit->text();
        quint16 tcpPort = tcpPortS.toUInt ();
        quint8 tcpPlayer = ui->tcpPlayerComboBox->currentIndex ();
        quint8 recVolt = ui->tcpRecoilVoltComboBox->currentIndex ();

        p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, tcpPort, tcpPlayer, recVolt, recoilOptions, lgSet);

        on_tcpPortLineEdit_textChanged(tcpPortS);
    }
    else if(defaultLightGun && defaultLightGunNum == BLAMCON)
    {
        bool has2DigitDisply;
        DisplayPriority displayPri;

        if(ui->blamconDisplayCheckBox->isChecked ())
            has2DigitDisply = true;
        else
            has2DigitDisply = false;

        if(ui->ammoRadioButton->isChecked ())
        {
            displayPri.ammo = true;
            displayPri.life = false;
        }
        else
        {
            displayPri.ammo = false;
            displayPri.life = true;
        }

        if(ui->displayOtherCheckBox->isChecked ())
            displayPri.other = true;
        else
            displayPri.other = false;

        //Create a New Blamcon Light Gun Class
        p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, comPortNum, comPortName, *p_comPortInfo, comPortBaud, comPortDataBits, comPortParity, comPortStopBits, comPortFlow, recoilOptions, lgSet, has2DigitDisply, displayPri);
    }
    else
    {
        //Create a New Light Gun Class
        p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, comPortNum, comPortName, *p_comPortInfo, comPortBaud, comPortDataBits, comPortParity, comPortStopBits, comPortFlow, recoilOptions, lgSet);
    }

    addLightGunSound.play();
    numberLightGuns = p_comDeviceList->GetNumberLightGuns ();
    ui->lgLCDNumber->display (numberLightGuns);

    if(!defaultLightGun || (defaultLightGun && defaultLightGunNum != ALIENUSB))
    {
        //Since Data was Passed onto the COM Port Device List
        delete p_comPortInfo;
        p_comPortInfo = nullptr;
    }
}


void addLightGunWindowV2::FindBaudRate()
{
    quint8 indexTemp;

    indexTemp = ui->baudSpeedComboBox->currentIndex();

    comPortBaud = BAUDDATA_ARRAY[indexTemp];
}

void addLightGunWindowV2::FindDataBits()
{
    quint8 indexTemp;

    indexTemp = ui->dataBitsComboBox->currentIndex();

    comPortDataBits = DATABITSDATA_ARRAY[indexTemp];
}

void addLightGunWindowV2::FindParity()
{
    quint8 indexTemp;

    indexTemp = ui->parityComboBox->currentIndex();

    comPortParity = PARITYDATA_ARRAY[indexTemp];
}

void addLightGunWindowV2::FindStopBits()
{
    quint8 indexTemp;

    indexTemp = ui->stopBitsComboBox->currentIndex();

    comPortStopBits = STOPBITSDATA_ARRAY[indexTemp];
}

void addLightGunWindowV2::FindFlow()
{
    quint8 indexTemp;

    indexTemp = ui->flowControlComboBox->currentIndex();

    comPortFlow = FLOWDATA_ARRAY[indexTemp];
}

void addLightGunWindowV2::FillSerialPortInfo(quint8 index)
{
    QString tempQS;

    //Get Serial Port Data
    tempQS = BEGINCOMPORTNAME+QString::number(index);
    p_comPortInfo = new QSerialPortInfo(tempQS);

    //Clear Out Old Data
    ui->locationLlineEdit->clear ();
    ui->descriptionLineEdit->clear ();
    ui->manufactureLineEdit->clear ();
    ui->serialNumLineEdit->clear ();
    ui->vendorIDLineEdit->clear ();
    ui->productIDLineEdit->clear ();
    ui->usageLineEdit->clear ();
    ui->releaseNumLineEdit->clear ();

    //Turn Off The 2 USB Info Line Edits
    //ui->usageLineEdit->setEnabled (false);
    //ui->releaseNumLineEdit->setEnabled (false);

    //Fill in New Data
    ui->locationLlineEdit->insert(p_comPortInfo->systemLocation());

    ui->descriptionLineEdit->insert(p_comPortInfo->description());

    ui->manufactureLineEdit->insert(p_comPortInfo->manufacturer());

    ui->serialNumLineEdit->insert(p_comPortInfo->serialNumber());

    if(p_comPortInfo->hasVendorIdentifier())
    {
        QString tempVID = QString::number(p_comPortInfo->vendorIdentifier(), 16).rightJustified(4, '0');
        tempVID = tempVID.toUpper ();
        tempVID.prepend ("0x");
        ui->vendorIDLineEdit->insert(tempVID);
    }

    if(p_comPortInfo->hasProductIdentifier())
    {
        QString tempPID = QString::number(p_comPortInfo->productIdentifier(), 16).rightJustified(4, '0');
        tempPID = tempPID.toUpper ();
        tempPID.prepend ("0x");
        ui->productIDLineEdit->insert(tempPID);
    }

    //Clean Up Pointer
    delete p_comPortInfo;
    p_comPortInfo = nullptr;
}


void addLightGunWindowV2::SetEnableComboBoxes(bool enableCB)
{
    ui->baudSpeedComboBox->setEnabled (enableCB);
    ui->dataBitsComboBox->setEnabled (enableCB);
    ui->parityComboBox->setEnabled (enableCB);
    ui->stopBitsComboBox->setEnabled (enableCB);
    ui->flowControlComboBox->setEnabled (enableCB);
}


void addLightGunWindowV2::FillUSBDevicesComboBox()
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
        quint8 defaultIndex = ui->defaultLightGunComboBox->currentIndex ();

        if(defaultIndex == AIMTRAK)
        {
            devs = hid_enumerate(AIMTRAKVENDORID, 0x0);
        }
        else
        {
            //Get a list of Alien Gun HIDs
            devs = hid_enumerate(ALIENUSBVENDORID, ALIENUSBPRODUCTID);
        }
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
        //this->PrintHIDInfo();
    }
    //Release HID enumeration
    hid_free_enumeration(devs);

    if(numberHIDDevices > 0)
    {
        for(quint16 i = 0; i < numberHIDDevices; i++)
        {
            QString tempS;
            QTextStream tempTS(&tempS);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            tempTS << "VID: " << hidInfoList[i].vendorIDString << " PID: " << hidInfoList[i].productIDString << " " << hidInfoList[i].displayPath.first(ALIENUSBPATHFIRST);
#else
            tempTS << "VID: " << hidInfoList[i].vendorIDString << " PID: " << hidInfoList[i].productIDString << " " << hidInfoList[i].displayPath.chopped(hidInfoList[i].displayPath.size() - ALIENUSBPATHFIRST);
#endif

            ui->usbDevicesComboBox->insertItem(i,tempS);
            //qDebug() << "Item: " << i << " is: " << tempS;
        }
    }
    else
        ui->usbDevicesComboBox->setCurrentIndex (-1);


}


void addLightGunWindowV2::ProcessHIDInfo()
{
    quint8 defaultIndex = ui->defaultLightGunComboBox->currentIndex ();
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

    //qDebug() << "Usage Page: " << tempHIDInfo.usagePage << " Usage: " << tempHIDInfo.usage;

    if(defaultIndex == AIMTRAK && !getAllHIDs)
    {
        if(tempHIDInfo.productIDString.startsWith (AIMTRAKPRODUCTIDS) && tempHIDInfo.displayPath.startsWith (AIMTRAKPATHFRONT))
        {
            if(!lgHIDInfoList.contains (tempHIDInfo))
            {
                hidInfoList << tempHIDInfo;
                numberHIDDevices++;
            }
        }
    }
    else
    {
        if(!tempHIDInfo.productIDString.startsWith (AIMTRAKPRODUCTIDS) && !tempHIDInfo.displayPath.startsWith (AIMTRAKPATHFRONT))
        {
            if(!lgHIDInfoList.contains (tempHIDInfo))
            {
                hidInfoList << tempHIDInfo;
                numberHIDDevices++;
            }
        }
    }

}


bool addLightGunWindowV2::CheckRecoilComboBoxes()
{
    quint8 rIndex = ui->recoilComboBox->currentIndex();
    quint8 avIndex = ui->ammoValueComboBox->currentIndex();
    quint8 rr2sIndex = ui->recoilR2SComboBox->currentIndex();
    quint8 rvIndex = ui->recoilValueComboBox->currentIndex();
    bool check = true;

    if(avIndex == rIndex || avIndex == rr2sIndex || avIndex == rvIndex)
        check = false;

    if(rIndex == rr2sIndex || rIndex == rvIndex)
        check = false;

    if(rr2sIndex == rvIndex)
        check = false;

    return check;
}



void addLightGunWindowV2::ChangeLabels(int index)
{
    //Always Red, Name and Recoil Method
    ui->nameLabel->setStyleSheet("QLabel { color: red; }");
    ui->recoilLabel->setStyleSheet("QLabel { color: red; }");
    ui->ammoLabel->setStyleSheet("QLabel { color: red; }");
    ui->r2sLabel->setStyleSheet("QLabel { color: red; }");

    //Serial Port Info
    //For Unsupported Serial Port Light Gun
    if(index == 0 || index == -1)
    {
        ui->comLabel->setStyleSheet("QLabel { color: red; }");
        ui->baudLabel->setStyleSheet("QLabel { color: red; }");
        ui->dataLabel->setStyleSheet("QLabel { color: red; }");
        ui->parityLabel->setStyleSheet("QLabel { color: red; }");
        ui->stopLabel->setStyleSheet("QLabel { color: red; }");
        ui->flowLabel->setStyleSheet("QLabel { color: red; }");
    }
    else if((index >= 1 && index <= 6) || index == XGUNNER || index == XENAS)  //All Supported Serial Port Light Guns
    {
        ui->comLabel->setStyleSheet("QLabel { color: red; }");
        ui->baudLabel->setStyleSheet("QLabel { color: black; }");
        ui->dataLabel->setStyleSheet("QLabel { color: black; }");
        ui->parityLabel->setStyleSheet("QLabel { color: black; }");
        ui->stopLabel->setStyleSheet("QLabel { color: black; }");
        ui->flowLabel->setStyleSheet("QLabel { color: black; }");
    }
    else
    {
        ui->comLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->baudLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->dataLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->parityLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->stopLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->flowLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
    }

    //For MX24 Hub Player USB (DipSwitch) & Hub COM Port
    if(index == MX24)
    {
        ui->dipLabel->setStyleSheet("QLabel { color: red; }");
        ui->hubLabel->setStyleSheet("QLabel { color: red; }");

        //Regular COM Port
        ui->comLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
    }
    else
    {
        ui->dipLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->hubLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
    }

    //For the USB HID Light Guns
    if(index == ALIENUSB || index == AIMTRAK)
    {
        ui->usbLabel->setStyleSheet("QLabel { color: red; }");

        if(index == AIMTRAK)
            ui->allHIDDevicesCheckBox->setEnabled (false);
        else
            ui->allHIDDevicesCheckBox->setEnabled (true);

        ui->usageLabel->setStyleSheet("QLabel { color: black }");
        ui->releaseNumLabel->setStyleSheet("QLabel { color: black; }");

        //Turn On The 2 USB Info Line Edits
        ui->usageLineEdit->setEnabled (true);
        ui->releaseNumLineEdit->setEnabled (true);
    }
    else
    {
        ui->usbLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->allHIDDevicesCheckBox->setEnabled (false);
        ui->no2DigitCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
        ui->no2DigitCheckBox->setEnabled (false);

        ui->usageLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->releaseNumLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");

        //Turn On The 2 USB Info Line Edits
        ui->usageLineEdit->setEnabled (false);
        ui->releaseNumLineEdit->setEnabled (false);
    }

    //For Recoil Value
    if(index == JBGUN4IR || index == FUSION || index == OPENFIRE)
    {
        ui->valueLabel->setStyleSheet("QLabel { color: red; }");
    }
    else
    {
        ui->valueLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
    }


    //For Reload, Damage, Death, and Shake
    if(index < 1 || (index >= 3 && index <= 6) || index == XENAS || index == XENASBTLE)
    {
        //Everything On Rumble Motor and LED
        ui->reloadRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->reloadJustLEDRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->reloadJustShakeRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->disableReloadradioButton->setStyleSheet("QRadioButton { color: red; }");

        ui->damageRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->damageJustLEDRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->damageJustShakeRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->disableDamageradioButton->setStyleSheet("QRadioButton { color: red; }");

        ui->deathRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->deathJustLEDRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->deathJustShakeRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->disableDeathradioButton->setStyleSheet("QRadioButton { color: red; }");

        ui->shakeRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->disableShakeradioButton->setStyleSheet("QRadioButton { color: red; }");
    }
    else if(index == RS3_REAPER || index == XGUNNER || index == SINDEN)
    {
        //Just Rumble
        ui->reloadRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->reloadJustLEDRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->reloadJustShakeRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->disableReloadradioButton->setStyleSheet("QRadioButton { color: red; }");

        ui->damageRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->damageJustLEDRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->damageJustShakeRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->disableDamageradioButton->setStyleSheet("QRadioButton { color: red; }");

        ui->deathRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->deathJustLEDRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->deathJustShakeRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->disableDeathradioButton->setStyleSheet("QRadioButton { color: red; }");

        ui->shakeRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->disableShakeradioButton->setStyleSheet("QRadioButton { color: red; }");
    }
    else
    {
        ui->reloadRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->reloadJustLEDRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->reloadJustShakeRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->disableReloadradioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");

        ui->damageRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->damageJustLEDRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->damageJustShakeRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->disableDamageradioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");

        ui->deathRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->deathJustLEDRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->deathJustShakeRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->disableDeathradioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");

        ui->shakeRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->disableShakeradioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
    }

    //TCP Server Interface
    if(index == SINDEN)
    {
        ui->tcpPortLabel->setStyleSheet("QLabel { color: red; }");
        ui->tcpPlayerLabel->setStyleSheet("QLabel { color: red; }");
        ui->tcpRecoilVoltLabel->setStyleSheet("QLabel { color: red; }");
    }
    else
    {
        ui->tcpPortLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->tcpPlayerLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->tcpRecoilVoltLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
    }

    //For Reaper LED, and Slide Hold Control
    if(index == RS3_REAPER)
    {
        //RS3 Reaper Settings
        ui->reaperDisableLEDsCheckBox->setEnabled (true);
        ui->reaperDisableLEDsCheckBox->setStyleSheet("QCheckBox { color: red; }");
        ui->reaperLargeAmmoLineEdit->setEnabled (true);
        //ui->reaperLargeAmmoLineEdit->setStyleSheet("QLabel { color: red; }");
        ui->reaperLargeAmmoLabel->setStyleSheet("QLabel { color: red; }");

        ui->reaperDisableSlideCheckBox->setEnabled (true);
        ui->reaperDisableSlideCheckBox->setStyleSheet("QCheckBox { color: red; }");

        if(ui->reaperDisableSlideCheckBox->isChecked ())
        {
            ui->reaperEnableSlideDelayCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
            ui->reaperEnableSlideDelayCheckBox->setEnabled (false);

            ui->reaperDelayTimeLineEdit->setEnabled (false);
            ui->reaperDelayLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");

            ui->reaperSlideHoldTimeLineEdit->setEnabled (false);
            ui->reaperHoldTimeLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        }
        else
        {
            ui->reaperEnableSlideDelayCheckBox->setEnabled (true);
            ui->reaperEnableSlideDelayCheckBox->setStyleSheet("QCheckBox { color: red; }");

            ui->reaperDelayTimeLineEdit->setEnabled (true);
            ui->reaperDelayLabel->setStyleSheet("QLabel { color: red; }");

            ui->reaperSlideHoldTimeLineEdit->setEnabled (true);
            ui->reaperHoldTimeLabel->setStyleSheet("QLabel { color: red; }");
        }
    }
    else
    {
        ui->reaperDisableLEDsCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
        ui->reaperDisableLEDsCheckBox->setEnabled (false);
        ui->reaperLargeAmmoLineEdit->setEnabled (false);
        //ui->reaperLargeAmmoLineEdit->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->reaperDisableSlideCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
        ui->reaperLargeAmmoLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->reaperDisableSlideCheckBox->setEnabled (false);
        ui->reaperEnableSlideDelayCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
        ui->reaperEnableSlideDelayCheckBox->setEnabled (false);

        ui->reaperDelayTimeLineEdit->setEnabled (false);
        ui->reaperDelayLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");

        ui->reaperSlideHoldTimeLineEdit->setEnabled (false);
        ui->reaperHoldTimeLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
    }

    //Display Priority
    if(index < 1 || index == ALIENUSB || index == OPENFIRE || index == BLAMCON || index == XENAS || index == XENASBTLE)
    {
        //Display Settings
        ui->ammoRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->lifeRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->displayPriorityLabel->setStyleSheet("QLabel { color: black; }");

        ui->displayOtherCheckBox->setEnabled (true);
        ui->displayOtherCheckBox->setStyleSheet("QCheckBox { color: red; }");

        if(index == ALIENUSB)
        {
            ui->no2DigitCheckBox->setEnabled (true);
            ui->no2DigitCheckBox->setStyleSheet("QCheckBox { color: red; }");
        }
        else
        {
            ui->no2DigitCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
            ui->no2DigitCheckBox->setEnabled (false);
        }

        if(index == OPENFIRE)
        {


            //Display Settings - OpenFire
            ui->openFireNoDisplayCheckBox->setStyleSheet("QCheckBox { color: red; }");
            ui->openFireNoDisplayCheckBox->setEnabled (true);

            if(ui->openFireNoDisplayCheckBox->isChecked ())
            {
                ui->openFireBothCheckBox->setEnabled (false);
                ui->openFireBothCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");

                ui->openFireDisplayLifeLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
                ui->openFireGlyphRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
                ui->openFireBarRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
                ui->openFireNumberRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
            }
            else
            {
                ui->openFireBothCheckBox->setEnabled (true);
                ui->openFireBothCheckBox->setStyleSheet("QCheckBox { color: red; }");

                ui->openFireDisplayLifeLabel->setStyleSheet("QLabel { color:black; }");
                ui->openFireGlyphRadioButton->setStyleSheet("QRadioButton { color: red; }");
                ui->openFireBarRadioButton->setStyleSheet("QRadioButton { color: red; }");
                ui->openFireNumberRadioButton->setStyleSheet("QRadioButton { color: red; }");
            }
        }
        else
        {

            //Display Settings - OpenFire
            ui->openFireNoDisplayCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
            ui->openFireNoDisplayCheckBox->setEnabled (false);

            ui->openFireBothCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
            ui->openFireBothCheckBox->setEnabled (false);

            ui->openFireDisplayLifeLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
            ui->openFireGlyphRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
            ui->openFireBarRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
            ui->openFireNumberRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        }


        if(index == BLAMCON)
        {
            //Display Settings - Blamocon
            ui->blamconDisplayCheckBox->setEnabled (true);
            ui->blamconDisplayCheckBox->setStyleSheet("QCheckBox { color: red; }");
        }
        else
        {
            ui->blamconDisplayCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
            ui->blamconDisplayCheckBox->setEnabled (false);
        }
    }
    else
    {
        //No Display
        ui->ammoRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->lifeRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->displayPriorityLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");

        ui->displayOtherCheckBox->setEnabled (false);
        ui->displayOtherCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");

        //Alien
        ui->no2DigitCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
        ui->no2DigitCheckBox->setEnabled (false);

        //OpenFire
        ui->openFireNoDisplayCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
        ui->openFireNoDisplayCheckBox->setEnabled (false);

        ui->openFireBothCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
        ui->openFireBothCheckBox->setEnabled (false);

        ui->openFireDisplayLifeLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->openFireGlyphRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->openFireBarRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->openFireNumberRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");

        //Blamcon
        ui->blamconDisplayCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
        ui->blamconDisplayCheckBox->setEnabled (false);

    }
}




void addLightGunWindowV2::on_reaperDisableSlideCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked)
    {
        //ui->reaperEnableSlideDelayCheckBox->setChecked (false);
        ui->reaperEnableSlideDelayCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");
        ui->reaperEnableSlideDelayCheckBox->setEnabled (false);
        ui->reaperEnableSlideDelayCheckBox->setChecked (false);

        ui->reaperDelayTimeLineEdit->setEnabled (false);
        ui->reaperDelayLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");

        ui->reaperSlideHoldTimeLineEdit->setEnabled (false);
        ui->reaperHoldTimeLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
    }
    else
    {
        ui->reaperEnableSlideDelayCheckBox->setStyleSheet("QCheckBox { color: red; }");
        ui->reaperEnableSlideDelayCheckBox->setEnabled (true);
        ui->reaperEnableSlideDelayCheckBox->setChecked (true);
    }
}

void addLightGunWindowV2::on_openFireBothCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked)
    {
        if(ui->openFireNumberRadioButton->isChecked ())
            ui->openFireGlyphRadioButton->setChecked (true);

        ui->openFireNumberRadioButton->setEnabled (false);
        ui->openFireNumberRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");

    }
    else
    {
        ui->openFireNumberRadioButton->setEnabled (true);
        ui->openFireNumberRadioButton->setStyleSheet("QRadioButton { color: red; }");
    }
}


void addLightGunWindowV2::on_reaperEnableSlideDelayCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked)
    {
        ui->reaperDelayTimeLineEdit->setEnabled (true);
        ui->reaperDelayLabel->setStyleSheet("QLabel { color: red; }");

        ui->reaperSlideHoldTimeLineEdit->setEnabled (true);
        ui->reaperHoldTimeLabel->setStyleSheet("QLabel { color: red; }");
    }
    else
    {
        ui->reaperDelayTimeLineEdit->setEnabled (false);
        ui->reaperDelayLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");

        ui->reaperSlideHoldTimeLineEdit->setEnabled (false);
        ui->reaperHoldTimeLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
    }
}


void addLightGunWindowV2::on_openFireNoDisplayCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked)
    {
        ui->openFireBothCheckBox->setChecked (false);
        ui->openFireGlyphRadioButton->setChecked (true);

        ui->openFireBothCheckBox->setEnabled (false);
        ui->openFireBothCheckBox->setStyleSheet("QCheckBox { color: rgba(0, 0, 0, 84); }");

        ui->openFireDisplayLifeLabel->setStyleSheet("QLabel { color: rgba(0, 0, 0, 84); }");
        ui->openFireGlyphRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->openFireBarRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
        ui->openFireNumberRadioButton->setStyleSheet("QRadioButton { color: rgba(0, 0, 0, 84); }");
    }
    else
    {
        ui->openFireBothCheckBox->setEnabled (true);
        ui->openFireBothCheckBox->setStyleSheet("QCheckBox { color: red; }");

        ui->openFireDisplayLifeLabel->setStyleSheet("QLabel { color: red; }");
        ui->openFireGlyphRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->openFireBarRadioButton->setStyleSheet("QRadioButton { color: red; }");
        ui->openFireNumberRadioButton->setStyleSheet("QRadioButton { color: red; }");
    }
}

