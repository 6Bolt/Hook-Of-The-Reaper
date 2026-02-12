#include "editLightGunWindowV2.h"
#include "Windows/ui_editLightGunWindowV2.h"
#include "../Global.h"

editLightGunWindowV2::editLightGunWindowV2(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::editLightGunWindowV2)
{
    QString tempQS;
    quint8 i;
    numberHIDDevices = 0;
    getAllHIDs = false;
    isHIDInit = false;

    isInitDone = false;

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

    //Make the Reaper Large Ammo only accept 3 numbers
    ui->reaperLargeAmmoLineEdit->setInputMask (REAPERAMMO0DELAYMASK);

    //Make the Reaper Ammo 0 Delay only accepted 3 Numbers
    ui->reaperDelayTimeLineEdit->setInputMask (REAPERAMMO0DELAYMASK);

    //Make the Reaper Hold Slide Time only accepted Numbers (0.0 to 9.9)
    ui->reaperSlideHoldTimeLineEdit->setInputMask (REAPERHOLDSLIDEMASK);

    //Set Mask for Analog Strength
    //ui->analogLineEdit->setInputMask (ANALOGSTRENGTHMASK);


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
        outputConnection = p_comDeviceList->p_lightGunList[i]->GetOutputConnection();
        //qDebug() << "Light Gun Number" << i << "Output Connection:" << outputConnect;
        tempQS = QString::number(i+1);
        tempQS.append (": ");
        tempQS.append (p_comDeviceList->p_lightGunList[i]->GetLightGunName());
        tempQS.append (" on ");
        if(outputConnection == SERIALPORT)
        {
            quint8 cpNum = p_comDeviceList->p_lightGunList[i]->GetComPortNumber();
            tempQS.append ("COM");
            tempQS.append (QString::number (cpNum));
        }
        else if(outputConnection == USBHID)
            tempQS.append ("USB");
        else if(outputConnection == BTLE)
            tempQS.append ("BTLE");
        else if(outputConnection == TCP)
        {
            tempQS.append ("TCP(");
            quint16 tempTCP = p_comDeviceList->p_lightGunList[i]->GetTCPPort ();
            tempQS.append(QString::number (tempTCP));
            tempQS.append (")");
        }
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
    ui->defaultLightGunComboBox->insertItem(XGUNNER,XGUNNERNAME);
    ui->defaultLightGunComboBox->insertItem(AIMTRAK,AIMTRAKNAME);
    ui->defaultLightGunComboBox->insertItem(XENAS,XENASNAME);
    ui->defaultLightGunComboBox->insertItem(XENASBTLE,XENASBTLENAME);
    ui->defaultLightGunComboBox->insertItem(SINDEN,SINDENNAME);

    //Check if the First Light Gun is a Default Light Gun
    defaultLightGun = p_comDeviceList->p_lightGunList[0]->GetDefaultLightGun();
    outputConnection = p_comDeviceList->p_lightGunList[0]->GetOutputConnection ();

    if(defaultLightGun)
    {
        defaultLightGunNum = p_comDeviceList->p_lightGunList[0]->GetDefaultLightGunNumber();

        ChangeLabels(defaultLightGunNum);

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
    else
        ChangeLabels(0);

    ui->dipSwitchComboBox->setEnabled(true);

    for(quint8 comPortIndx=0;comPortIndx<DIPSWITCH_NUMBER;comPortIndx++)
    {
        if(usedDipPlayers[comPortIndx] && dipSwitchNumber != comPortIndx)
            tempQS = "";
        else
            tempQS = "P"+QString::number(comPortIndx+1) + " USB";
        ui->dipSwitchComboBox->insertItem(comPortIndx,tempQS);
    }

    //Get USB HID Devices
    FillUSBDevicesComboBox();

    //Fill TCP Players
    if(defaultLightGun && defaultLightGunNum == SINDEN)
    {
        ui->tcpPlayerComboBox->insertItem(0, TCPPLAYER1NAME);
        ui->tcpPlayerComboBox->insertItem(1, TCPPLAYER2NAME);
    }
    else
    {
        ui->tcpPlayerComboBox->insertItem(0,"");
        ui->tcpPlayerComboBox->insertItem(1,"");
    }

    ui->tcpPlayerComboBox->setCurrentIndex (0);

    //Fill in TCP Recoil Voltage
    for(quint8 i=0;i<RECOILVOLTAGESETNUM;i++)
        ui->tcpRecoilVoltComboBox->insertItem(i,QString::number(i));

    ui->tcpRecoilVoltComboBox->setCurrentIndex (RECOILVOLTDEFAULT);
    ui->tcpRecoilVoltComboBox->setEnabled (false);


    if(defaultLightGun)
    {
        ui->defaultLightGunComboBox->setCurrentIndex (defaultLightGunNum);
        SetEnableComboBoxes(false);
        if(defaultLightGunNum == MX24)
        {
            ui->dipSwitchComboBox->setCurrentIndex(dipSwitchNumber);
            ui->hubComComboBox->setEnabled(true);
            //ui->analogLineEdit->setEnabled(false);
            ui->comPortComboBox->setEnabled(false);
        }
        else if(defaultLightGunNum == JBGUN4IR || defaultLightGunNum == OPENFIRE)
        {
            ui->comPortComboBox->setEnabled(true);
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            //ui->analogLineEdit->setEnabled(true);
            //analogStrength = p_comDeviceList->p_lightGunList[0]->GetAnalogStrength (&analogStrengthSet);
            //if(analogStrengthSet)
            //    ui->analogLineEdit->setText(QString::number (analogStrength));
        }
        else if(defaultLightGunNum == ALIENUSB || defaultLightGunNum == AIMTRAK)
        {
            ui->comPortComboBox->setEnabled(false);
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            //ui->analogLineEdit->setEnabled(false);

            if(defaultLightGunNum == AIMTRAK)
                ui->allHIDDevicesCheckBox->setEnabled (false);

            //First Light Gun is an USB HID Light Gun
            qint16 hidIndex = FindLightGunHID(0);

            //qDebug() << "hidIndex:" << hidIndex;
            //If Found, then load up that HID, if not Load LG Info into Line Edits
            if(hidIndex != -1)
                on_usbDevicesComboBox_currentIndexChanged(hidIndex);
            else
            {
                //HID Device Not Found in HID List. Fill in VendorID, ProductID, and Serial Number
                HIDInfo tempHIDInfo = p_comDeviceList->p_lightGunList[0]->GetUSBHIDInfo ();
            }

            if(defaultLightGunNum == ALIENUSB)
            {
                bool n2DDisplay = p_comDeviceList->p_lightGunList[0]->GetNoDisplay ();
                ui->no2DigitCheckBox->setChecked(n2DDisplay);
            }
            else
                ui->no2DigitCheckBox->setEnabled (false);

        }
        else if(defaultLightGunNum == SINDEN)
        {
            ui->tcpPortLineEdit->setEnabled (true);
            ui->tcpPlayerComboBox->setEnabled (true);
            ui->tcpRecoilVoltComboBox->setEnabled (true);

            quint16 tempTCP = p_comDeviceList->p_lightGunList[0]->GetTCPPort ();
            //ui->tcpPortLineEdit->setText (QString::number (tempTCP));
            ui->tcpPortLineEdit->setText ("0");
            ui->tcpPortLineEdit->setText (QString::number(tempTCP));

            quint8 tempRecVolt = p_comDeviceList->p_lightGunList[0]->GetRecoilVoltage ();
            ui->tcpRecoilVoltComboBox->setCurrentIndex (tempRecVolt);


            ui->comPortComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            //ui->analogLineEdit->setEnabled(false);
            ui->comPortComboBox->setEnabled(false);
        }
        else
        {
            ui->comPortComboBox->setEnabled(true);
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            //ui->analogLineEdit->setEnabled(false);
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
        //ui->analogLineEdit->setEnabled(false);
    }

    //COM Port Combo Box - List Unused COM Port and Select Light Gun COM Port
    if(outputConnection == SERIALPORT && defaultLightGunNum != MX24)
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

    if(outputConnection == SERIALPORT && defaultLightGunNum != MX24)
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

    //Recoil Priority
    //Fill Recoil Combo Boxes
    for(quint8 index=0;index<NUMBEROFRECOILS;index++)
    {
        ui->recoilComboBox->insertItem(index, QString::number(index));
        ui->ammoValueComboBox->insertItem(index, QString::number(index));
        ui->recoilR2SComboBox->insertItem(index, QString::number(index));
        ui->recoilValueComboBox->insertItem(index, QString::number(index));
    }

    p_recoilPriority =  p_comDeviceList->p_lightGunList[0]->GetRecoilPriority ();

    ui->ammoValueComboBox->setCurrentIndex (p_recoilPriority[0]);
    ui->recoilComboBox->setCurrentIndex (p_recoilPriority[1]);
    ui->recoilR2SComboBox->setCurrentIndex (p_recoilPriority[2]);
    ui->recoilValueComboBox->setCurrentIndex (p_recoilPriority[3]);

    if(defaultLightGunNum == FUSION || defaultLightGunNum == JBGUN4IR || defaultLightGunNum == OPENFIRE)
    {
        ui->recoilValueComboBox->setEnabled(true);
    }
    else
    {
        ui->recoilValueComboBox->setCurrentIndex(3);
        ui->recoilValueComboBox->setEnabled(false);
        ui->recoilComboBox->setItemText (3, "");
        ui->ammoValueComboBox->setItemText (3, "");
        ui->recoilR2SComboBox->setItemText (3, "");
    }


    LightGunSettings lgSet = p_comDeviceList->p_lightGunList[0]->GetLightGunSettings ();

    if(lgSet.reload == 1)
        ui->reloadRadioButton->setChecked (true);
    else if(lgSet.reload == 2)
        ui->reloadJustShakeRadioButton->setChecked (true);
    else if(lgSet.reload == 3)
        ui->reloadJustLEDRadioButton->setChecked (true);
    else
        ui->disableReloadradioButton->setChecked (true);

    if(lgSet.damage == 1)
        ui->damageRadioButton->setChecked (true);
    else if(lgSet.damage == 2)
        ui->damageJustShakeRadioButton->setChecked (true);
    else if(lgSet.damage == 3)
        ui->damageJustLEDRadioButton->setChecked (true);
    else
        ui->disableDamageradioButton->setChecked (true);

    if(lgSet.death == 1)
        ui->deathRadioButton->setChecked (true);
    else if(lgSet.death == 2)
        ui->deathJustShakeRadioButton->setChecked (true);
    else if(lgSet.death == 3)
        ui->deathJustLEDRadioButton->setChecked (true);
    else
        ui->disableDeathradioButton->setChecked (true);

    if(lgSet.shake)
        ui->shakeRadioButton->setChecked (true);
    else
        ui->disableShakeradioButton->setChecked (true);



    if(defaultLightGun && (defaultLightGunNum==FUSION || defaultLightGunNum==JBGUN4IR || defaultLightGunNum==OPENFIRE))
        ui->recoilValueComboBox->setEnabled(true);
    else
        ui->recoilValueComboBox->setEnabled(false);

    if(defaultLightGun && (defaultLightGunNum == ALIENUSB || defaultLightGunNum == AIMTRAK || defaultLightGunNum == MX24))
    {
        ui->reloadRadioButton->setEnabled(false);
        ui->reloadJustLEDRadioButton->setEnabled(false);
        ui->reloadJustShakeRadioButton->setEnabled(false);
        ui->disableReloadradioButton->setEnabled(false);

        ui->damageRadioButton->setEnabled(false);
        ui->damageJustLEDRadioButton->setEnabled(false);
        ui->damageJustShakeRadioButton->setEnabled(false);
        ui->disableDamageradioButton->setEnabled(false);

        ui->deathRadioButton->setEnabled(false);
        ui->deathJustLEDRadioButton->setEnabled(false);
        ui->deathJustShakeRadioButton->setEnabled(false);
        ui->disableDeathradioButton->setEnabled(false);

        ui->shakeRadioButton->setEnabled(false);
        ui->disableShakeradioButton->setEnabled(false);


    }
    else if(defaultLightGun && (defaultLightGunNum == XGUNNER || defaultLightGunNum == RS3_REAPER || defaultLightGunNum == SINDEN))
    {
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
    }

    isInitDone = true;
}

editLightGunWindowV2::~editLightGunWindowV2()
{
    delete ui;

    //Exit the USB HID
    hid_exit ();
}

void editLightGunWindowV2::on_savedLightGunsComboBox_currentIndexChanged(int index)
{
    //Loads Save Light Gun Settings
    LoadSavedLightGun(index);
}


void editLightGunWindowV2::on_defaultLightGunComboBox_currentIndexChanged(int index)
{
    if(isInitDone)
    {
        quint8 lgIndex = ui->savedLightGunsComboBox->currentIndex ();
        quint8 dlgNum = p_comDeviceList->p_lightGunList[lgIndex]->GetDefaultLightGunNumber ();

        if(dlgNum != index)
        {
            QMessageBox::warning (this, "Cannot Change Default Light Gun", "The light gun cannot change from one type to anoother. Please delete the light gun and enter it under the correct default light gun");
            ui->defaultLightGunComboBox->setCurrentIndex (dlgNum);
            return;
        }

        //Change Labeles to Red or Black
        ChangeLabels(index);

        /*
        quint8 lgIndex = ui->savedLightGunsComboBox->currentIndex ();
        outputConnection = p_comDeviceList->p_lightGunList[lgIndex]->GetOutputConnection ();

        bool isUSBLG = p_comDeviceList->p_lightGunList[lgIndex]->IsLightGunUSB ();
        quint8 dlgNum = p_comDeviceList->p_lightGunList[lgIndex]->GetDefaultLightGunNumber ();

        if(isUSBLG && (index != ALIENUSB && index != AIMTRAK))
        {
            QMessageBox::warning (this, "Cannot Change an USB HID to Serial Port", "An USB HID light gun cannot be changed to a Serial COM Port light gun. Please delete the USB HID light gun and then add a light gun");
            ui->defaultLightGunComboBox->setCurrentIndex (dlgNum);
            return;
        }
        else if((dlgNum == AIMTRAK && index == ALIENUSB) || (dlgNum == ALIENUSB && index == AIMTRAK))
        {
            QMessageBox::warning (this, "Cannot Change an USB HID", "An AimTrak light gun cannot be changed to an Alien light gun, and vice versa. Please delete the USB HID light gun and then add a light gun");
            ui->defaultLightGunComboBox->setCurrentIndex (dlgNum);
            return;
        }
        else if(!isUSBLG && (index == ALIENUSB || index == AIMTRAK))
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
        else if(outputConnection == TCP && index != SINDEN)
        {
            QMessageBox::warning (this, "Cannot Change a TCP to a Non-TCP", "A TCP light gun cannot be changed to a Non-TCP light gun. Please delete the TCP light gun and then add a light gun");
            ui->defaultLightGunComboBox->setCurrentIndex (dlgNum);
            return;
        }
        else if(outputConnection != TCP && index == SINDEN)
        {
            QMessageBox::warning (this, "Cannot Change a Non-TCP to a TCP", "A Non-TCP light gun cannot be changed to a TCP light gun. Please delete the light gun and then add a light gun");
            ui->defaultLightGunComboBox->setCurrentIndex (dlgNum);
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

        */

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
            else if(outputConnection == SERIALPORT)
            {
                ui->dipSwitchComboBox->setEnabled(false);
                ui->hubComComboBox->setEnabled(false);
                ui->comPortComboBox->setEnabled(true);

                quint8 comPortIndex = ui->comPortComboBox->currentIndex ();

                FillSerialPortInfo(comPortIndex);
            }

            //if(index == JBGUN4IR || index == OPENFIRE)
            //    ui->analogLineEdit->setEnabled(true);
            //else
            //{
            //    ui->analogLineEdit->clear();
            //    ui->analogLineEdit->setEnabled(false);
            //}

            //if(index == ALIENUSB)
            //    ui->no2DigitCheckBox->setEnabled (true);
            //else
            //    ui->no2DigitCheckBox->setEnabled (false);


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
                ui->reloadJustShakeRadioButton->setEnabled(false);
                ui->disableReloadradioButton->setEnabled(false);

                ui->damageRadioButton->setEnabled(false);
                ui->damageJustLEDRadioButton->setEnabled(false);
                ui->damageJustShakeRadioButton->setEnabled(false);
                ui->disableDamageradioButton->setEnabled(false);

                ui->deathRadioButton->setEnabled(false);
                ui->deathJustLEDRadioButton->setEnabled(false);
                ui->deathJustShakeRadioButton->setEnabled(false);
                ui->disableDeathradioButton->setEnabled(false);

                ui->shakeRadioButton->setEnabled(false);
                ui->disableShakeradioButton->setEnabled(false);
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
            }

            if(index == SINDEN)
            {
                ui->tcpPortLineEdit->setEnabled (true);
                ui->tcpPlayerComboBox->setEnabled (true);
                ui->tcpRecoilVoltComboBox->setEnabled (true);

                quint16 tempTCP = p_comDeviceList->p_lightGunList[lgIndex]->GetTCPPort ();
                ui->tcpPortLineEdit->setText (QString::number (tempTCP));

                quint8 tempRecVolt = p_comDeviceList->p_lightGunList[0]->GetRecoilVoltage ();
                ui->tcpRecoilVoltComboBox->setCurrentIndex (tempRecVolt);
            }
            else
            {
                ui->tcpPortLineEdit->setEnabled (false);
                ui->tcpPlayerComboBox->setEnabled (false);
                ui->tcpRecoilVoltComboBox->setEnabled (false);
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
            //ui->analogLineEdit->clear();
            //ui->analogLineEdit->setEnabled(false);

            //TCP
            ui->tcpPortLineEdit->setEnabled (false);
            ui->tcpPlayerComboBox->setEnabled (false);
            ui->tcpRecoilVoltComboBox->setEnabled (false);

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

    }
}


void editLightGunWindowV2::on_comPortComboBox_currentIndexChanged(int index)
{
    if(isInitDone)
    {
        //Fills out Serial Port Info on right Side, when a different COM Port is Selected
        FillSerialPortInfo(index);
    }
}


void editLightGunWindowV2::on_deletePushButton_clicked()
{
    quint8 dlgIndex = ui->savedLightGunsComboBox->currentIndex ();
    quint8 i;
    QString tempQS;

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
        //Delay to hear sound
        QThread::sleep(1);
        accept ();
    }
    else
    {
        //Remove Saved Light Gun from Combo Box
        for(i = 0; i<numberLightGuns; i++)
        {
            outputConnection = p_comDeviceList->p_lightGunList[i]->GetOutputConnection();
            tempQS = QString::number(i+1);
            tempQS.append (": ");
            tempQS.append (p_comDeviceList->p_lightGunList[i]->GetLightGunName());
            tempQS.append (" on ");
            if(outputConnection == SERIALPORT)
            {
                quint8 cpNum = p_comDeviceList->p_lightGunList[i]->GetComPortNumber();
                tempQS.append ("COM");
                tempQS.append (QString::number (cpNum));
            }
            else if(outputConnection == USBHID)
                tempQS.append ("USB");
            else if(outputConnection == BTLE)
                tempQS.append ("BTLE");
            else if(outputConnection == TCP)
            {
                tempQS.append ("TCP(");
                quint16 tempTCP = p_comDeviceList->p_lightGunList[i]->GetTCPPort ();
                tempQS.append(QString::number (tempTCP));
                tempQS.append (")");
            }

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


void editLightGunWindowV2::on_editPushButton_clicked()
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

        outputConnection = p_comDeviceList->p_lightGunList[lightGunNum]->GetOutputConnection();

        if(defaultLightGun && defaultLightGunNum == MX24)
        {
            ui->hubComComboBox->setCurrentIndex(hubComPortNumber);
            FillSerialPortInfo(hubComPortNumber);
        }
        else if(outputConnection == SERIALPORT)
        {
            ui->comPortComboBox->setCurrentIndex(comPortNum);
            FillSerialPortInfo(comPortNum);
        }

        tempQS = QString::number(lightGunNum+1);
        tempQS.append (": ");
        tempQS.append (lightGunName);
        tempQS.append (" on ");
        if(outputConnection == SERIALPORT)
            tempQS.append (comPortName);
        else if(outputConnection == USBHID)
            tempQS.append ("USB");
        else if(outputConnection == BTLE)
            tempQS.append ("BTLE");
        else if(outputConnection == TCP)
        {
            tempQS.append ("TCP(");
            quint16 tempTCP = p_comDeviceList->p_lightGunList[lightGunNum]->GetTCPPort ();
            tempQS.append(QString::number (tempTCP));
            tempQS.append (")");
        }

        ui->savedLightGunsComboBox->setItemText (lightGunNum,tempQS);
    }
}


void editLightGunWindowV2::on_okPushButton_clicked()
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

        //Play Edit Sound
        editLightGunSound.play ();

        QThread::sleep(1);

        //Close and Destroy the Window
        accept ();
    }
}


void editLightGunWindowV2::on_cancelPushButton_clicked()
{
    //Close and Destroy the Window
    this->accept ();
}


void editLightGunWindowV2::on_hubComComboBox_currentIndexChanged(int index)
{
    QString tempQS;
    p_comDeviceList->CopyUsedDipPlayersArray(usedDipPlayers, DIPSWITCH_NUMBER, index);

    for(quint8 comPortIndx=0;comPortIndx<DIPSWITCH_NUMBER;comPortIndx++)
    {
        if(usedDipPlayers[comPortIndx] && dipSwitchNumber != comPortIndx)
            tempQS = "";
        else
            tempQS = "P"+QString::number(comPortIndx+1) + " USB";
        ui->dipSwitchComboBox->setItemText(comPortIndx,tempQS);
    }


    FillSerialPortInfo(index);
}


void editLightGunWindowV2::on_usbDevicesComboBox_currentIndexChanged(int index)
{
    if(index >= 0)
    {
        //qDebug() << "index:" << index << "numberHIDDevices:" << numberHIDDevices;

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
        ui->usageLineEdit->setEnabled (true);
        ui->releaseNumLineEdit->setEnabled (true);

        //Fill in New Data
        ui->locationLlineEdit->insert(hidInfoList[index].path);
        ui->locationLlineEdit->setCursorPosition (0);

        //qDebug() << "path:" << hidInfoList[index].path;

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


void editLightGunWindowV2::on_allHIDDevicesCheckBox_checkStateChanged(const Qt::CheckState &arg1)
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


void editLightGunWindowV2::on_tcpPortLineEdit_textChanged(const QString &arg1)
{
    bool isNumber;
    quint16 tempPort = arg1.toUInt (&isNumber);
    qint8 tempPlayer = p_comDeviceList->GetTCPPortPlayerInfo(tempPort);
    quint8 currentIndex = ui->savedLightGunsComboBox->currentIndex ();
    quint8 lgPlayer = p_comDeviceList->p_lightGunList[currentIndex]->GetTCPPlayer ();
    quint16 lgPort = p_comDeviceList->p_lightGunList[currentIndex]->GetTCPPort ();

    //qDebug() << "tempPort" << tempPort << "tempPlayer" << tempPlayer << "currentIndex" << currentIndex << "lgPlayer" << lgPlayer;

    if(isNumber)
    {
        //If LG TCP Port is the Same Port in Line Edit
        if(tempPort == lgPort)
        {

            if(tempPlayer == -1)
            {
                ui->tcpPlayerComboBox->setItemText(0, TCPPLAYER1NAME);
                ui->tcpPlayerComboBox->setItemText(1, TCPPLAYER2NAME);
                ui->tcpPlayerComboBox->setCurrentIndex (0);

            }
            else if(tempPlayer == TCPPLAYER1 && lgPlayer == 0)
            {
                ui->tcpPlayerComboBox->setItemText(0,TCPPLAYER1NAME);
                ui->tcpPlayerComboBox->setItemText(1, TCPPLAYER2NAME);
                ui->tcpPlayerComboBox->setCurrentIndex (0);
            }
            else if(tempPlayer == TCPPLAYER2 && lgPlayer == 1)
            {
                ui->tcpPlayerComboBox->setItemText(0, TCPPLAYER1NAME);
                ui->tcpPlayerComboBox->setItemText(1, TCPPLAYER2NAME);
                ui->tcpPlayerComboBox->setCurrentIndex (1);
            }
            else if(tempPlayer == 0)
            {
                if(lgPlayer == 0)
                {
                    ui->tcpPlayerComboBox->setItemText(0, TCPPLAYER1NAME);
                    ui->tcpPlayerComboBox->setItemText(1,"");
                    ui->tcpPlayerComboBox->setCurrentIndex (0);
                }
                else if(lgPlayer == 1)
                {
                    ui->tcpPlayerComboBox->setItemText(0, "");
                    ui->tcpPlayerComboBox->setItemText(1, TCPPLAYER2NAME);
                    ui->tcpPlayerComboBox->setCurrentIndex (1);
                }
            }
        }
        else
        {
            //Different TCP Port than the LG
            //-1 = TCP Port Not Used Yet, 0 = Both Players are Used
            if(tempPlayer == -1)
            {
                ui->tcpPlayerComboBox->setItemText(0, TCPPLAYER1NAME);
                ui->tcpPlayerComboBox->setItemText(1, TCPPLAYER2NAME);
                ui->tcpPlayerComboBox->setCurrentIndex (0);

            }
            else if(tempPlayer == TCPPLAYER1)
            {
                ui->tcpPlayerComboBox->setItemText(0, "");
                ui->tcpPlayerComboBox->setItemText(1, TCPPLAYER2NAME);
                ui->tcpPlayerComboBox->setCurrentIndex (1);
            }
            else if(tempPlayer == TCPPLAYER2)
            {
                ui->tcpPlayerComboBox->setItemText(0, TCPPLAYER1NAME);
                ui->tcpPlayerComboBox->setItemText(1, "");
                ui->tcpPlayerComboBox->setCurrentIndex (0);
            }
            else if(tempPlayer == 0)
            {
                ui->tcpPlayerComboBox->setItemText(0, "");
                ui->tcpPlayerComboBox->setItemText(1, "");
                ui->tcpPlayerComboBox->setCurrentIndex (0);
            }
        }
    }
}



// Private


bool editLightGunWindowV2::IsValidData()
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
    bool recoilCheck = false;
    bool badTCPPort = false;
    bool badTCPPlayer = false;
    bool reaperDelayTime = false;
    bool reaperHoldTime = false;
    bool reaperLargeAmmo = false;
    quint8 numLightGuns = p_comDeviceList->GetNumberLightGuns();
    quint8 i;
    quint8 currentIndex = ui->savedLightGunsComboBox->currentIndex ();
    quint8 defaultLGIndex = ui->defaultLightGunComboBox->currentIndex ();
    //quint8 comPortIndex =  ui->comPortComboBox->currentIndex ();
    quint8 hubComIndex = ui->hubComComboBox->currentIndex ();

    outputConnection = p_comDeviceList->p_lightGunList[currentIndex]->GetOutputConnection();

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
    else if(outputConnection == SERIALPORT)
    {
        //Check If the Com Port Name is Empty for the Combo Box
        ivTemp = ui->comPortComboBox->currentText ();
        if(ivTemp.isEmpty ())
            comPortNumEmpty = true;
    }

    //if(defaultLGIndex == JBGUN4IR || defaultLGIndex == OPENFIRE)
    //{
    //    bool isNumber;
    //    QString analString = ui->analogLineEdit->text();
    //    quint16 analStrengthBig = analString.toUInt (&isNumber);
    //    if(!isNumber)
    //        analNotNumber = true;
    //    else
    //    {
    //        if(analStrengthBig > 255)
    //            analNotRange = true;
    //    }
    //}

    if(defaultLGIndex == ALIENUSB || defaultLGIndex == AIMTRAK)
    {
        QString tempDP;
        //HIDInfo tempHIDInfo = p_comDeviceList->p_lightGunList[currentIndex]->GetUSBHIDInfo ();
        quint8 hidIndex = ui->usbDevicesComboBox->currentIndex ();

        //tempDP = tempHIDInfo.displayPath;
        tempDP = hidInfoList[hidIndex].displayPath;
        usbDisPMatch = p_comDeviceList->CheckUSBPath(tempDP, currentIndex);
    }

    if(defaultLGIndex == SINDEN)
    {
        //Check Port Number
        bool isNumber;
        QString tcpPortS = ui->tcpPortLineEdit->text();
        quint32 tcpPort = tcpPortS.toUInt (&isNumber);

        if(!isNumber || tcpPort > MAXTCPPORT || tcpPort == 0)
            badTCPPort = true;

        QString tcpPlayer = ui->tcpPlayerComboBox->currentText ();

        if(tcpPlayer.isEmpty ())
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

        QMessageBox::warning (this, "Can Not Edit Light Gun", message);
        return false;
    }
    return false;
}


bool editLightGunWindowV2::IsDefaultLightGun()
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


void editLightGunWindowV2::EditLightGun()
{
    quint8 oldComPort;
    bool reloadNR = false;
    bool reloadDisable = false;
    SupportedRecoils recoilOptions;
    LightGunSettings lgSet;


    //Collect Light Gun Name, Number, COM Port Number & Name, and Serial Port Info
    lightGunNum = ui->savedLightGunsComboBox->currentIndex ();
    lightGunName = ui->lightGunNameLineEdit->text();
    outputConnection = p_comDeviceList->p_lightGunList[lightGunNum]->GetOutputConnection();

    if(outputConnection != SERIALPORT || defaultLightGunNum == MX24)
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

    //Get Recoil Options
    recoilOptions.recoil = ui->recoilComboBox->currentIndex();
    recoilOptions.ammoValue = ui->ammoValueComboBox->currentIndex();
    recoilOptions.recoilR2S = ui->recoilR2SComboBox->currentIndex();
    recoilOptions.recoilValue = ui->recoilValueComboBox->currentIndex();

    //Get Reload Options
    if(defaultLightGun && (defaultLightGunNum == MX24 || defaultLightGunNum == ALIENUSB || defaultLightGunNum == AIMTRAK))
    {
        lgSet.reload = 0;
        lgSet.damage = 0;
        lgSet.death = 0;
        lgSet.shake = false;
    }
    else if(defaultLightGun && (defaultLightGunNum == RS3_REAPER || defaultLightGunNum == XGUNNER || defaultLightGunNum == SINDEN))
    {
        if(ui->reloadRadioButton->isChecked ())
            lgSet.reload = 1;
        else
            lgSet.reload = 0;

        if(ui->damageRadioButton->isChecked ())
            lgSet.damage = 1;
        else
            lgSet.damage = 0;

        if(ui->deathRadioButton->isChecked ())
            lgSet.death = 1;
        else
            lgSet.death = 0;

        if(ui->shakeRadioButton->isChecked ())
            lgSet.shake = true;
        else
            lgSet.shake = false;
    }
    else
    {
        if(ui->reloadRadioButton->isChecked ())
            lgSet.reload = 1;
        else if(ui->reloadJustShakeRadioButton->isChecked ())
            lgSet.reload = 2;
        else if(ui->reloadJustLEDRadioButton->isChecked ())
            lgSet.reload = 3;
        else
            lgSet.reload = 0;

        if(ui->damageRadioButton->isChecked ())
            lgSet.damage = 1;
        else if(ui->damageJustShakeRadioButton->isChecked ())
            lgSet.damage = 2;
        else if(ui->damageJustLEDRadioButton->isChecked ())
            lgSet.damage = 3;
        else
            lgSet.damage = 0;

        if(ui->deathRadioButton->isChecked ())
            lgSet.death = 1;
        else if(ui->deathJustShakeRadioButton->isChecked ())
            lgSet.death = 2;
        else if(ui->deathJustLEDRadioButton->isChecked ())
            lgSet.death = 3;
        else
            lgSet.death = 0;

        if(ui->shakeRadioButton->isChecked ())
            lgSet.shake = true;
        else
            lgSet.shake = false;
    }

    //Set Recoil Priority
    p_comDeviceList->p_lightGunList[lightGunNum]->SetRecoilPriority (recoilOptions);


    //Edit Serial Port Light Gun Stuff
    if(outputConnection == SERIALPORT)
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
            bool disableLEDs = ui->reaperDisableLEDsCheckBox->isChecked ();
            QString largeAmmo = ui->reaperLargeAmmoLineEdit->text();

            ReaperSlideData slideData;

            slideData.disableHoldBack = ui->reaperDisableSlideCheckBox->isChecked ();

            if(!slideData.disableHoldBack)
                slideData.enableHoldDelay = ui->reaperEnableSlideDelayCheckBox->isChecked ();
            else
                slideData.enableHoldDelay = false;

            if(!slideData.disableHoldBack && slideData.enableHoldDelay)
            {
                QString holdDelayS = ui->reaperDelayTimeLineEdit->text();
                slideData.holdDelay = holdDelayS.toUInt ();

                QString tempRHSBT = ui->reaperSlideHoldTimeLineEdit->text ();
                float reaperHoldSlideTimeSec = tempRHSBT.toFloat ();
                slideData.slideHoldTime = static_cast<quint16>(reaperHoldSlideTimeSec*1000.0f);

                //qDebug() << "Reaper Hold Time" << tempRHSBT << reaperHoldSlideTimeSec << slideData.slideHoldTime;
            }
            else
            {
                slideData.holdDelay = DEFAULTAMMO0DELAY;
                slideData.slideHoldTime = REAPERHOLDSLIDETIME;
            }

            p_comDeviceList->p_lightGunList[lightGunNum]->SetDisableReaperLEDs(disableLEDs);
            p_comDeviceList->p_lightGunList[lightGunNum]->SetReaperLargeAmmo(largeAmmo.toUInt ());
            p_comDeviceList->p_lightGunList[lightGunNum]->SetReaperSlideData(slideData);

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
        else if(defaultLightGun && defaultLightGunNum == OPENFIRE)
        {
            DisplayPriority displayP;
            displayP.ammo = ui->ammoRadioButton->isChecked ();
            displayP.life = ui->lifeRadioButton->isChecked ();
            displayP.other = ui->displayOtherCheckBox->isChecked ();

            p_comDeviceList->p_lightGunList[lightGunNum]->SetDisplayPriority (displayP);

            bool noDisplay = ui->openFireNoDisplayCheckBox->isChecked ();
            p_comDeviceList->p_lightGunList[lightGunNum]->SetNoDisplay (noDisplay);

            DisplayOpenFire displayOF;

            if(noDisplay)
            {
                displayOF.ammoAndLifeDisplay = false;
                displayOF.lifeGlyphs = true;
                displayOF.lifeBar = false;
                displayOF.lifeNumber = false;
            }
            else
            {
                displayOF.ammoAndLifeDisplay = ui->openFireBothCheckBox->isChecked ();
                displayOF.lifeGlyphs = ui->openFireGlyphRadioButton->isChecked ();
                displayOF.lifeBar = ui->openFireBarRadioButton->isChecked ();
                displayOF.lifeNumber = ui->openFireNumberRadioButton->isChecked ();
            }

            p_comDeviceList->p_lightGunList[lightGunNum]->SetDisplayOpenFire (displayOF);
        }
        else if(defaultLightGun && defaultLightGunNum == BLAMCON)
        {
            if(ui->blamconDisplayCheckBox->isChecked ())
                p_comDeviceList->p_lightGunList[lightGunNum]->SetNoDisplay (false);
            else
                p_comDeviceList->p_lightGunList[lightGunNum]->SetNoDisplay (true);

            DisplayPriority displayP;
            displayP.ammo = ui->ammoRadioButton->isChecked ();
            displayP.life = ui->lifeRadioButton->isChecked ();
            displayP.other = ui->displayOtherCheckBox->isChecked ();

            p_comDeviceList->p_lightGunList[lightGunNum]->SetDisplayPriority (displayP);
        }
        //else if(defaultLightGun && (defaultLightGunNum == JBGUN4IR || defaultLightGunNum == OPENFIRE))
        //{
        //    QString analString = ui->analogLineEdit->text();
        //    quint8 analStrength = analString.toUInt ();
        //    p_comDeviceList->p_lightGunList[lightGunNum]->SetAnalogStrength (analStrength);
        //}

        //Load in Commands After the Edit
        //p_comDeviceList->p_lightGunList[lightGunNum]->LoadDefaultLGCommands ();

        delete p_comPortInfo;
        p_comPortInfo = nullptr;

    }
    else if(outputConnection == USBHID)
    {
        //Need to Get the HIDInfo -So Much Easier
        qint16 hidIndex = ui->usbDevicesComboBox->currentIndex ();
        p_comDeviceList->p_lightGunList[lightGunNum]->SetHIDInfo (hidInfoList[hidIndex]);

        if(defaultLightGun && defaultLightGunNum == ALIENUSB)
        {
            if(ui->no2DigitCheckBox->isChecked ())
                p_comDeviceList->p_lightGunList[lightGunNum]->SetNoDisplay (true);
            else
                p_comDeviceList->p_lightGunList[lightGunNum]->SetNoDisplay (false);

            DisplayPriority displayP;
            displayP.ammo = ui->ammoRadioButton->isChecked ();
            displayP.life = ui->lifeRadioButton->isChecked ();
            displayP.other = ui->displayOtherCheckBox->isChecked ();

            p_comDeviceList->p_lightGunList[lightGunNum]->SetDisplayPriority (displayP);
        }
    }
    else if(outputConnection == TCP)
    {
        QString tempTCPPortS = ui->tcpPortLineEdit->text();
        quint16 tempTCPPort = tempTCPPortS.toUInt ();
        quint8 tempTCPPlayer = ui->tcpPlayerComboBox->currentIndex ();
        quint16 currentPort = p_comDeviceList->p_lightGunList[lightGunNum]->GetTCPPort();
        quint8 currentPlayer = p_comDeviceList->p_lightGunList[lightGunNum]->GetTCPPlayer();
        p_comDeviceList->RemoveTCPPortPlayer(currentPort, currentPlayer);
        p_comDeviceList->p_lightGunList[lightGunNum]->SetTCPPort (tempTCPPort);
        p_comDeviceList->p_lightGunList[lightGunNum]->SetTCPPlayer (tempTCPPlayer);
        qint8 comPlayer = p_comDeviceList->GetTCPPortPlayerInfo(tempTCPPort);

        if(comPlayer == -1)
        {
            if(tempTCPPlayer == 0)
                p_comDeviceList->SetTCPPortPlayerInfo(tempTCPPort, TCPPLAYER1);
            else if(tempTCPPlayer == 1)
                p_comDeviceList->SetTCPPortPlayerInfo(tempTCPPort, TCPPLAYER2);
        }
        else if(comPlayer == TCPPLAYER1 && tempTCPPlayer == 1)
            p_comDeviceList->SetTCPPortPlayerInfo(tempTCPPort, 0);
        else if(comPlayer == TCPPLAYER2 && tempTCPPlayer == 0)
            p_comDeviceList->SetTCPPortPlayerInfo(tempTCPPort, 0);

        quint8 recVolt = ui->tcpRecoilVoltComboBox->currentIndex ();
        p_comDeviceList->p_lightGunList[lightGunNum]->SetRecoilVoltage (recVolt);
    }

    //Set the General Light Gun Settings, as they will Reload the Commands
    p_comDeviceList->p_lightGunList[lightGunNum]->SetLightGunSettings (lgSet);

}


void editLightGunWindowV2::FindBaudRate()
{
    quint8 indexTemp;

    indexTemp = ui->baudSpeedComboBox->currentIndex();

    comPortBaud = BAUDDATA_ARRAY[indexTemp];
}

void editLightGunWindowV2::FindDataBits()
{
    quint8 indexTemp;

    indexTemp = ui->dataBitsComboBox->currentIndex();

    comPortDataBits = DATABITSDATA_ARRAY[indexTemp];
}

void editLightGunWindowV2::FindParity()
{
    quint8 indexTemp;

    indexTemp = ui->parityComboBox->currentIndex();

    comPortParity = PARITYDATA_ARRAY[indexTemp];
}

void editLightGunWindowV2::FindStopBits()
{
    quint8 indexTemp;

    indexTemp = ui->stopBitsComboBox->currentIndex();

    comPortStopBits = STOPBITSDATA_ARRAY[indexTemp];
}

void editLightGunWindowV2::FindFlow()
{
    quint8 indexTemp;

    indexTemp = ui->flowControlComboBox->currentIndex();

    comPortFlow = FLOWDATA_ARRAY[indexTemp];
}


void editLightGunWindowV2::FillSerialPortInfo(quint8 index)
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

void editLightGunWindowV2::SetEnableComboBoxes(bool enableCB)
{
    ui->baudSpeedComboBox->setEnabled (enableCB);
    ui->dataBitsComboBox->setEnabled (enableCB);
    ui->parityComboBox->setEnabled (enableCB);
    ui->stopBitsComboBox->setEnabled (enableCB);
    ui->flowControlComboBox->setEnabled (enableCB);
}



void editLightGunWindowV2::LoadSavedLightGun(quint8 index)
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

    p_recoilPriority =  p_comDeviceList->p_lightGunList[index]->GetRecoilPriority ();

    ui->ammoValueComboBox->setCurrentIndex (p_recoilPriority[0]);
    ui->recoilComboBox->setCurrentIndex (p_recoilPriority[1]);
    ui->recoilR2SComboBox->setCurrentIndex (p_recoilPriority[2]);
    ui->recoilValueComboBox->setCurrentIndex (p_recoilPriority[3]);

    LightGunSettings lgSet = p_comDeviceList->p_lightGunList[index]->GetLightGunSettings ();

    if(lgSet.reload == 1)
        ui->reloadRadioButton->setChecked (true);
    else if(lgSet.reload == 2)
        ui->reloadJustShakeRadioButton->setChecked (true);
    else if(lgSet.reload == 3)
        ui->reloadJustLEDRadioButton->setChecked (true);
    else
        ui->disableReloadradioButton->setChecked (true);

    if(lgSet.damage == 1)
        ui->damageRadioButton->setChecked (true);
    else if(lgSet.damage == 2)
        ui->damageJustShakeRadioButton->setChecked (true);
    else if(lgSet.damage == 3)
        ui->damageJustLEDRadioButton->setChecked (true);
    else
        ui->disableDamageradioButton->setChecked (true);

    if(lgSet.death == 1)
        ui->deathRadioButton->setChecked (true);
    else if(lgSet.death == 2)
        ui->deathJustShakeRadioButton->setChecked (true);
    else if(lgSet.death == 3)
        ui->deathJustLEDRadioButton->setChecked (true);
    else
        ui->disableDeathradioButton->setChecked (true);

    if(lgSet.shake)
        ui->shakeRadioButton->setChecked (true);
    else
        ui->disableShakeradioButton->setChecked (true);


    ui->tcpPortLineEdit->setEnabled (false);
    ui->tcpPlayerComboBox->setEnabled (false);
    ui->tcpRecoilVoltComboBox->setEnabled (false);

    outputConnection = p_comDeviceList->p_lightGunList[index]->GetOutputConnection();

    //Check if the Light Gun is a Default Light Gun
    if(defaultLightGun)
    {
        defaultLightGunNum = p_comDeviceList->p_lightGunList[index]->GetDefaultLightGunNumber();
        ui->defaultLightGunComboBox->setCurrentIndex (defaultLightGunNum);
        defaultLightGun = true;
        SetEnableComboBoxes(false);

        if(defaultLightGunNum == MX24)
        {
            //ui->analogLineEdit->clear();
            //ui->analogLineEdit->setEnabled(false);
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

            ui->usbDevicesComboBox->setEnabled(false);
        }
        else if(defaultLightGunNum == JBGUN4IR || defaultLightGunNum == OPENFIRE)
        {
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            //ui->analogLineEdit->setEnabled(true);
            //analogStrength = p_comDeviceList->p_lightGunList[index]->GetAnalogStrength (&analogStrengthSet);
            //if(analogStrengthSet)
            //    ui->analogLineEdit->setText(QString::number (analogStrength));

            ui->comPortComboBox->setEnabled(true);
            ui->usbDevicesComboBox->setEnabled(false);

            if(defaultLightGunNum == OPENFIRE)
            {
                DisplayPriority displayP = p_comDeviceList->p_lightGunList[index]->GetDisplayPriority ();
                ui->ammoRadioButton->setChecked (displayP.ammo);
                ui->lifeRadioButton->setChecked (displayP.life);
                ui->displayOtherCheckBox->setChecked (displayP.other);

                bool noDisplay = p_comDeviceList->p_lightGunList[index]->GetNoDisplay ();
                ui->openFireNoDisplayCheckBox->setChecked(noDisplay);

                DisplayOpenFire displayOF = p_comDeviceList->p_lightGunList[index]->GetDisplayOpenFire ();
                ui->openFireBothCheckBox->setChecked (displayOF.ammoAndLifeDisplay);
                ui->openFireGlyphRadioButton->setChecked (displayOF.lifeGlyphs);
                ui->openFireBarRadioButton->setChecked (displayOF.lifeBar);
                ui->openFireNumberRadioButton->setChecked (displayOF.lifeNumber);
            }
        }
        else if(defaultLightGunNum == RS3_REAPER)
        {
            bool disableLEDs = p_comDeviceList->p_lightGunList[index]->GetDisableReaperLEDs ();
            quint8 largeAmmo = p_comDeviceList->p_lightGunList[index]->GetReaperLargeAmmo ();
            ReaperSlideData slideData = p_comDeviceList->p_lightGunList[index]->GetReaperSlideData ();

            ui->reaperDisableLEDsCheckBox->setChecked (disableLEDs);
            ui->reaperLargeAmmoLineEdit->setText (QString::number(largeAmmo));

            ui->reaperDisableSlideCheckBox->setChecked (slideData.disableHoldBack);
            ui->reaperEnableSlideDelayCheckBox->setChecked (slideData.enableHoldDelay);
            ui->reaperDelayTimeLineEdit->setText (QString::number(slideData.holdDelay));

            float reaperHoldSlideTimeSec = static_cast<float>(static_cast<float>(slideData.slideHoldTime) / 1000.0f);
            QString reaperHoldSlideTimeSecString = QString::number (reaperHoldSlideTimeSec).leftJustified (2, '0');
            ui->reaperSlideHoldTimeLineEdit->setText (reaperHoldSlideTimeSecString);

            //qDebug() << "Repear Hold Time" << reaperHoldSlideTimeSec << reaperHoldSlideTimeSecString;
        }
        else if(defaultLightGunNum == BLAMCON)
        {
            bool n2DDisplay = p_comDeviceList->p_lightGunList[index]->GetNoDisplay ();

            if(n2DDisplay)
                ui->blamconDisplayCheckBox->setChecked(false);
            else
                ui->blamconDisplayCheckBox->setChecked(true);

            DisplayPriority displayP = p_comDeviceList->p_lightGunList[index]->GetDisplayPriority ();

            ui->ammoRadioButton->setChecked (displayP.ammo);
            ui->lifeRadioButton->setChecked (displayP.life);

            ui->displayOtherCheckBox->setChecked (displayP.other);

            //qDebug() << "Display Priority" << displayP.ammo << displayP.life << displayP.other;
        }
        else if(outputConnection == USBHID)
        {
            ui->comPortComboBox->setEnabled(false);
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            //ui->analogLineEdit->setEnabled(false);


            ui->usbDevicesComboBox->setEnabled(true);

            if(defaultLightGunNum == AIMTRAK)
            {
                ui->allHIDDevicesCheckBox->setChecked (false);
                ui->allHIDDevicesCheckBox->setEnabled (false);
            }
            else
                ui->allHIDDevicesCheckBox->setEnabled (true);

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
            }

            if(defaultLightGunNum == ALIENUSB)
            {
                ui->no2DigitCheckBox->setEnabled (true);
                bool n2DDisplay = p_comDeviceList->p_lightGunList[index]->GetNoDisplay ();
                ui->no2DigitCheckBox->setChecked(n2DDisplay);

                DisplayPriority displayP = p_comDeviceList->p_lightGunList[index]->GetDisplayPriority ();

                ui->ammoRadioButton->setChecked (displayP.ammo);
                ui->lifeRadioButton->setChecked (displayP.life);

                ui->displayOtherCheckBox->setChecked (displayP.other);
            }
            else
                ui->no2DigitCheckBox->setEnabled (false);

        }
        else if(defaultLightGunNum == SINDEN)
        {
            quint16 tcpPort = p_comDeviceList->p_lightGunList[index]->GetTCPPort ();

            ui->tcpPortLineEdit->setEnabled (true);
            ui->tcpPlayerComboBox->setEnabled (true);
            ui->tcpRecoilVoltComboBox->setEnabled (true);

            //Set Line Edit to 0 and then Port Number to make sure it Updates
            ui->tcpPortLineEdit->setText ("0");
            ui->tcpPortLineEdit->setText (QString::number(tcpPort));

            //Set Recoil Voltage to right number
            quint8 recVolt = p_comDeviceList->p_lightGunList[index]->GetRecoilVoltage ();
            ui->tcpRecoilVoltComboBox->setCurrentIndex (recVolt);

            ui->comPortComboBox->setEnabled(false);
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            //ui->analogLineEdit->setEnabled(false);

            ui->usbDevicesComboBox->setEnabled(false);
        }
        else
        {
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            //ui->analogLineEdit->clear();
            //ui->analogLineEdit->setEnabled(false);
            ui->comPortComboBox->setEnabled(true);
            ui->usbDevicesComboBox->setEnabled(false);
        }

        if(defaultLightGun && (defaultLightGunNum==FUSION || defaultLightGunNum==JBGUN4IR || defaultLightGunNum==OPENFIRE))
            ui->recoilValueComboBox->setEnabled(true);
        else
            ui->recoilValueComboBox->setEnabled(false);

        if(defaultLightGun && (defaultLightGunNum == ALIENUSB || defaultLightGunNum == AIMTRAK || defaultLightGunNum == MX24))
        {
            ui->reloadRadioButton->setEnabled(false);
            ui->reloadJustLEDRadioButton->setEnabled(false);
            ui->reloadJustShakeRadioButton->setEnabled(false);
            ui->disableReloadradioButton->setEnabled(false);

            ui->damageRadioButton->setEnabled(false);
            ui->damageJustLEDRadioButton->setEnabled(false);
            ui->damageJustShakeRadioButton->setEnabled(false);
            ui->disableDamageradioButton->setEnabled(false);

            ui->deathRadioButton->setEnabled(false);
            ui->deathJustLEDRadioButton->setEnabled(false);
            ui->deathJustShakeRadioButton->setEnabled(false);
            ui->disableDeathradioButton->setEnabled(false);

            ui->shakeRadioButton->setEnabled(false);
            ui->disableShakeradioButton->setEnabled(false);

        }
        else if(defaultLightGun && (defaultLightGunNum == XGUNNER || defaultLightGunNum == RS3_REAPER || defaultLightGunNum == SINDEN))
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
        //ui->analogLineEdit->clear();
        //ui->analogLineEdit->setEnabled(false);
        ui->comPortComboBox->setEnabled(true);
        ui->usbDevicesComboBox->setEnabled(false);

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

    if(outputConnection == SERIALPORT)
    {
        for(quint8 comPortIndx=0;comPortIndx<DIPSWITCH_NUMBER;comPortIndx++)
        {
            if(usedDipPlayers[comPortIndx] && dipSwitchNumber != comPortIndx)
                tempQS = "";
            else
                tempQS = "P"+QString::number(comPortIndx+1) + " USB";
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


void editLightGunWindowV2::FillUSBDevicesComboBox()
{
    quint8 currentIndex = ui->savedLightGunsComboBox->currentIndex ();
    HIDInfo tempHIDInfo;

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
        quint8 defaultLGNum = p_comDeviceList->p_lightGunList[currentIndex]->GetDefaultLightGunNumber ();

        //qDebug() << "defaultLGNum:" << defaultLGNum;

        if(defaultLGNum == AIMTRAK)
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
    }
    //Release HID enumeration
    hid_free_enumeration(devs);


    //if(p_comDeviceList->p_lightGunList[currentIndex]->IsLightGunUSB ())
    if(p_comDeviceList->p_lightGunList[currentIndex]->GetOutputConnection() == USBHID)
    {
        tempHIDInfo = p_comDeviceList->p_lightGunList[currentIndex]->GetUSBHIDInfo ();
        if(!hidInfoList.contains (tempHIDInfo))
        {
            hidInfoList << tempHIDInfo;
            numberHIDDevices++;
        }
    }

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

        //if(p_comDeviceList->p_lightGunList[currentIndex]->IsLightGunUSB ())
        if(p_comDeviceList->p_lightGunList[currentIndex]->GetOutputConnection() == USBHID)
            ui->usbDevicesComboBox->setCurrentIndex (FindLightGunHID(currentIndex));
        else
            ui->usbDevicesComboBox->setCurrentIndex (0);
    }
    else
    {
        ui->usbDevicesComboBox->setCurrentIndex (-1);
    }

}


void editLightGunWindowV2::ProcessHIDInfo()
{
    HIDInfo tempHIDInfo;
    quint8 currentIndex = ui->savedLightGunsComboBox->currentIndex ();
    quint8 defaultLGNum = p_comDeviceList->p_lightGunList[currentIndex]->GetDefaultLightGunNumber ();


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

    if(defaultLGNum == AIMTRAK && !getAllHIDs)
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


qint16 editLightGunWindowV2::FindLightGunHID(quint8 lgNum)
{
    //Check if Light Gun is Not USB, if so return -1
    if(p_comDeviceList->p_lightGunList[lgNum]->GetOutputConnection() != USBHID || numberHIDDevices == 0)
        return -1;


    qint16 matchNum = -1;
    quint8 numberMatches = 0;

    HIDInfo tempHIDInfo = p_comDeviceList->p_lightGunList[lgNum]->GetUSBHIDInfo ();

    //qDebug() << " numberHIDDevices: " << numberHIDDevices << " tempHIDInfo.serialNumber: " << tempHIDInfo.serialNumber;

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


bool editLightGunWindowV2::CheckRecoilComboBoxes()
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


void editLightGunWindowV2::ChangeLabels(int index)
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


void editLightGunWindowV2::on_reaperDisableSlideCheckBox_checkStateChanged(const Qt::CheckState &arg1)
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


void editLightGunWindowV2::on_openFireBothCheckBox_checkStateChanged(const Qt::CheckState &arg1)
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


void editLightGunWindowV2::on_reaperEnableSlideDelayCheckBox_checkStateChanged(const Qt::CheckState &arg1)
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


void editLightGunWindowV2::on_openFireNoDisplayCheckBox_checkStateChanged(const Qt::CheckState &arg1)
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

