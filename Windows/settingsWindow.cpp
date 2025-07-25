#include "settingsWindow.h"
#include "ui_settingsWindow.h"

//Constructor
settingsWindow::settingsWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::settingsWindow)
{
    //Sets Up Window
    ui->setupUi(this);

    //Cannot Expand the Dialog window
    this->setFixedSize(this->size());

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports
    p_comDeviceList = cdList;

    //Get The First Two Settings
    useDefaultLGFirst = p_comDeviceList->GetUseDefaultLGFirst();
    useMultiThreading = p_comDeviceList->GetUseMultiThreading ();

    //If Set. then Check the Box
    if(useDefaultLGFirst)
        ui->useDefaultLGCheckBox->setCheckState (Qt::Checked);
    else
        ui->useDefaultLGCheckBox->setCheckState (Qt::Unchecked);


    if(useMultiThreading)
        ui->useMultiThreadCheckBox->setCheckState (Qt::Checked);
    else
        ui->useMultiThreadCheckBox->setCheckState (Qt::Unchecked);


    //Make it Only Accept Numbers in Line Edit
    ui->refreshDisplayLineEdit->setInputMask (REFRESHDISPLAYMASK);

    //Get Refresh Display Time
    refreshDisplayTime = p_comDeviceList->GetRefreshTimeDisplay();

    //Turn into String
    refreshDisplayTimeString = QString::number (refreshDisplayTime);

    //Display
    ui->refreshDisplayLineEdit->setText (refreshDisplayTimeString);

    //Get Fourth Setting
    closeComPortGameExit = p_comDeviceList->GetCloseComPortGameExit ();

    //If Set. then Check the Box
    if(closeComPortGameExit)
        ui->closeComCheckBox->setCheckState (Qt::Checked);
    else
        ui->closeComCheckBox->setCheckState (Qt::Unchecked);

    //Get Fifth Setting
    ignoreUselessDLGGF = p_comDeviceList->GetIgnoreUselessDFLGGF ();

    //If Set. then Check the Box
    if(ignoreUselessDLGGF)
        ui->ignoreUDLGGFCheckBox->setCheckState (Qt::Checked);
    else
        ui->ignoreUDLGGFCheckBox->setCheckState (Qt::Unchecked);


    bypassSerialWriteChecks = p_comDeviceList->GetSerialPortWriteCheckBypass ();

    //If Set. then Check the Box
    if(bypassSerialWriteChecks)
        ui->bypassSerialWriteCheckBox->setCheckState (Qt::Checked);
    else
        ui->bypassSerialWriteCheckBox->setCheckState (Qt::Unchecked);


    disbleReaperLEDs = p_comDeviceList->GetDisableReaperLEDs ();

    //If Set. then Check the Box
    if(disbleReaperLEDs)
        ui->reaperLEDCheckBox->setCheckState (Qt::Checked);
    else
        ui->reaperLEDCheckBox->setCheckState (Qt::Unchecked);


    ui->ammoRadioButton->setAutoExclusive (false);
    ui->lifeRadioButton->setAutoExclusive (false);

    p_comDeviceList->GetDisplayPriority (&displayAmmoPriority, &displayLifePriority);

    if(displayAmmoPriority == displayLifePriority)
    {
        displayAmmoPriority = true;
        displayLifePriority = false;
    }

    ui->ammoRadioButton->setChecked (displayAmmoPriority);
    ui->lifeRadioButton->setChecked (displayLifePriority);

    displayOtherPriority = p_comDeviceList->GetDisplayOtherPriority ();

    //If Set. then Check the Box
    if(displayOtherPriority)
        ui->displayOtherCheckBox->setCheckState (Qt::Checked);
    else
        ui->displayOtherCheckBox->setCheckState (Qt::Unchecked);


    ui->glyphsRadioButton->setAutoExclusive (false);
    ui->lifeBarRadioButton->setAutoExclusive (false);
    ui->glyphsRadioButton->setEnabled (true);
    ui->lifeBarRadioButton->setEnabled (true);
    ui->glyphsRadioButton->setCheckable(true);
    ui->lifeBarRadioButton->setCheckable(true);

    displayAmmoLife = p_comDeviceList->GetDisplayAmmoAndLife (&displayAmmoLifeGlyphs, &displayAmmoLifeBar, &displayAmmoLifeNumber);

    if(displayAmmoLife)
        ui->ammoLifeCheckBox->setCheckState (Qt::Checked);
    else
        ui->ammoLifeCheckBox->setCheckState (Qt::Unchecked);

    if(displayAmmoLife)
    {
        if(displayAmmoLifeNumber)
        {
            displayAmmoLifeGlyphs = true;
            displayAmmoLifeBar = false;
            displayAmmoLifeNumber = false;
        }

        ui->glyphsRadioButton->setChecked (displayAmmoLifeGlyphs);
        ui->lifeBarRadioButton->setChecked (displayAmmoLifeBar);
        ui->lifeNumRadioButton->setChecked (displayAmmoLifeNumber);

        ui->lifeNumRadioButton->setEnabled (false);

    }
    else
    {
        ui->lifeNumRadioButton->setEnabled (true);
        ui->glyphsRadioButton->setChecked (displayAmmoLifeGlyphs);
        ui->lifeBarRadioButton->setChecked (displayAmmoLifeBar);
        ui->lifeNumRadioButton->setChecked (displayAmmoLifeNumber);
    }

    enableNewGameFileCreation = p_comDeviceList->GetEnableNewGameFileCreation ();

    //If Set. then Check the Box
    if(enableNewGameFileCreation)
        ui->enableNewGFCheckBox->setCheckState (Qt::Checked);
    else
        ui->enableNewGFCheckBox->setCheckState (Qt::Unchecked);


    reaperAmmo0Delay = p_comDeviceList->GetReaperAmmo0Delay(&enableReaperAmmo0Delay, &reaperHoldSlideTime);

    if(enableReaperAmmo0Delay)
        ui->ammo0CheckBox->setCheckState (Qt::Checked);
    else
        ui->ammo0CheckBox->setCheckState (Qt::Unchecked);

    //Make the Reaper Ammo 0 Delay only accepted 3 Numbers
    ui->ammo0DelayLineEdit->setInputMask (REAPERAMMO0DELAYMASK);

    //Make the Reaper Hold Slide Time only accepted 1 Numbers
    ui->holdSlideLineEdit->setInputMask (REAPERHOLDSLIDEMASK);

    //Turn into String
    reaperAmmo0DelayString = QString::number (reaperAmmo0Delay);

    //Display Reaper Ammo 0 Delay
    ui->ammo0DelayLineEdit->setText (reaperAmmo0DelayString);

    //Convert ms to s, and then convert to QString
    reaperHoldSlideTimeSec = static_cast<float>(static_cast<float>(reaperHoldSlideTime) / 1000.0f);
    reaperHoldSlideTimeSecString = QString::number (reaperHoldSlideTimeSec).leftJustified (2, '0');

    //Display to Hold Slide Time
    ui->holdSlideLineEdit->setText (reaperHoldSlideTimeSecString);


    finishedInit = true;

}

//Deconstructor
settingsWindow::~settingsWindow()
{
    delete ui;
}


//private slots

//Collect Display Data, and then Save Data to List
void settingsWindow::on_updatePushButton_clicked()
{
    CheckAndSaveSetting();
}

//Collect Display Data, and then Save Data to List, and Then Close Window
void settingsWindow::on_okPushButton_clicked()
{
    CheckAndSaveSetting();

    //Close Dialog Window using accept
    accept();
}

//Close Window
void settingsWindow::on_cancelPushButton_clicked()
{
    //Close Dialog Window using accept
    accept();
}

void settingsWindow::CheckAndSaveSetting()
{
    QString tempRTD, tempRA0D, tempRHSBT;
    bool isNumber, isNumberAmmo, isNumberSlide;

    defaultLG = ui->useDefaultLGCheckBox->checkState ();
    multiThreading = ui->useMultiThreadCheckBox->checkState ();
    closeComPort = ui->closeComCheckBox->checkState ();
    ignoreUDLGGF = ui->ignoreUDLGGFCheckBox->checkState ();
    bypassSWC = ui->bypassSerialWriteCheckBox->checkState ();
    disableRLED = ui->reaperLEDCheckBox->checkState ();
    otherDisplay = ui->displayOtherCheckBox->checkState ();
    displayAL = ui->ammoLifeCheckBox->checkState ();
    enableNGFC = ui->enableNewGFCheckBox->checkState ();
    enableRA0D = ui->ammo0CheckBox->checkState ();

    if(defaultLG == Qt::Checked)
        useDefaultLGFirst = true;
    else
        useDefaultLGFirst = false;

    if(multiThreading == Qt::Checked)
        useMultiThreading = true;
    else
        useMultiThreading = false;

    if(closeComPort == Qt::Checked)
        closeComPortGameExit = true;
    else
        closeComPortGameExit = false;


    tempRTD = ui->refreshDisplayLineEdit->text ();
    refreshDisplayTime = tempRTD.toUInt (&isNumber);

    if(ignoreUDLGGF == Qt::Checked)
        ignoreUselessDLGGF = true;
    else
        ignoreUselessDLGGF = false;

    if(bypassSWC == Qt::Checked)
        bypassSerialWriteChecks = true;
    else
        bypassSerialWriteChecks = false;

    if(disableRLED == Qt::Checked)
        disbleReaperLEDs = true;
    else
        disbleReaperLEDs = false;

    if(otherDisplay == Qt::Checked)
        displayOtherPriority = true;
    else
        displayOtherPriority = false;

    if(displayAL == Qt::Checked)
        displayAmmoLife = true;
    else
        displayAmmoLife = false;

    if(enableNGFC == Qt::Checked)
        enableNewGameFileCreation = true;
    else
        enableNewGameFileCreation = false;

    if(enableRA0D == Qt::Checked)
        enableReaperAmmo0Delay = true;
    else
        enableReaperAmmo0Delay = false;

    tempRA0D = ui->ammo0DelayLineEdit->text ();
    reaperAmmo0Delay = tempRA0D.toUInt (&isNumberAmmo);

    if(!isNumberAmmo || reaperAmmo0Delay == 0)
    {
        reaperAmmo0Delay = DEFAULTAMMO0DELAY;
        reaperAmmo0DelayString = QString::number (reaperAmmo0Delay);
        ui->ammo0DelayLineEdit->setText (reaperAmmo0DelayString);
    }

    tempRHSBT = ui->holdSlideLineEdit->text ();
    reaperHoldSlideTimeSec = tempRHSBT.toFloat (&isNumberSlide);

    if(!isNumberSlide || reaperHoldSlideTimeSec < 1.0f || reaperHoldSlideTimeSec > 9.0f)
    {
        reaperHoldSlideTimeSec = REAPERHOLDSLIDETIMEF;
        reaperHoldSlideTime = REAPERHOLDSLIDETIME;
        reaperHoldSlideTimeSecString = QString::number (reaperHoldSlideTimeSec).leftJustified (2, '0');
        ui->holdSlideLineEdit->setText (reaperHoldSlideTimeSecString);
    }
    else
        reaperHoldSlideTime = static_cast<quint16>(reaperHoldSlideTimeSec*1000.0f);

    p_comDeviceList->SetUseDefaultLGFirst(useDefaultLGFirst);
    p_comDeviceList->SetUseMultiThreading (useMultiThreading);
    p_comDeviceList->SetCloseComPortGameExit (closeComPortGameExit);
    p_comDeviceList->SetIgnoreUselessDFLGGF (ignoreUselessDLGGF);
    p_comDeviceList->SetSerialPortWriteCheckBypass (bypassSerialWriteChecks);
    p_comDeviceList->SetDisableReaperLEDs (disbleReaperLEDs);
    p_comDeviceList->SetDisplayPriority (displayAmmoPriority, displayLifePriority);
    p_comDeviceList->SetDisplayOtherPriority (displayOtherPriority);

    p_comDeviceList->SetDisplayAmmoAndLife(displayAmmoLife, displayAmmoLifeGlyphs, displayAmmoLifeBar, displayAmmoLifeNumber);

    p_comDeviceList->SetEnableNewGameFileCreation (enableNewGameFileCreation);

    if(isNumber)
    {
        p_comDeviceList->SetRefreshTimeDisplay(refreshDisplayTime);
        refreshDisplayTimeString = QString::number (refreshDisplayTime);
    }

    p_comDeviceList->SetReaperAmmo0Delay(enableReaperAmmo0Delay, reaperAmmo0Delay, reaperHoldSlideTime);



    p_comDeviceList->SaveSettings();
}


void settingsWindow::on_bypassSerialWriteCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked && finishedInit)
    {
        //If Box is Checked, then Display the Warning About turning it on. Only if clicked 'Yes' then box remains checked. Default button is No
        QString qes = "This option is only for debug, and should not be used for normal use. It can be used to find hardware fails, for example if the USB port or wires are bad and corrupting the data.";
        qes.append (" It is higly recommanded not to enable this setting in normal use.");
        int ret = QMessageBox::question (this, "Bypass Serial Port Write Checks", qes, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if(ret != QMessageBox::Yes)
            ui->bypassSerialWriteCheckBox->setCheckState (Qt::Unchecked);
    }
}


void settingsWindow::on_ammoRadioButton_clicked()
{
    displayAmmoPriority = true;
    displayLifePriority = false;
    ui->ammoRadioButton->setChecked (displayAmmoPriority);
    ui->lifeRadioButton->setChecked (displayLifePriority);
}


void settingsWindow::on_lifeRadioButton_clicked()
{
    displayAmmoPriority = false;
    displayLifePriority = true;
    ui->ammoRadioButton->setChecked (displayAmmoPriority);
    ui->lifeRadioButton->setChecked (displayLifePriority);
}


void settingsWindow::on_ammoLifeCheckBox_checkStateChanged(const Qt::CheckState &arg1)
{
    if(arg1 == Qt::Checked)
    {
        displayAmmoLife = true;

        if(displayAmmoLifeNumber)
        {
            displayAmmoLifeGlyphs = true;
            displayAmmoLifeBar = false;
            displayAmmoLifeNumber = false;
            ui->glyphsRadioButton->setChecked (displayAmmoLifeGlyphs);
            ui->lifeBarRadioButton->setChecked (displayAmmoLifeBar);
            ui->lifeNumRadioButton->setChecked (displayAmmoLifeNumber);
        }

        ui->lifeNumRadioButton->setEnabled (false);
    }
    else
    {
        displayAmmoLife = false;
        ui->lifeNumRadioButton->setEnabled (true);
    }
}




void settingsWindow::on_glyphsRadioButton_clicked()
{
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    ui->glyphsRadioButton->setChecked (displayAmmoLifeGlyphs);
    ui->lifeBarRadioButton->setChecked (displayAmmoLifeBar);
    ui->lifeNumRadioButton->setChecked (displayAmmoLifeNumber);
}


void settingsWindow::on_lifeBarRadioButton_clicked()
{
    displayAmmoLifeGlyphs = false;
    displayAmmoLifeBar = true;
    displayAmmoLifeNumber = false;
    ui->glyphsRadioButton->setChecked (displayAmmoLifeGlyphs);
    ui->lifeBarRadioButton->setChecked (displayAmmoLifeBar);
    ui->lifeNumRadioButton->setChecked (displayAmmoLifeNumber);
}


void settingsWindow::on_lifeNumRadioButton_clicked()
{
    displayAmmoLifeGlyphs = false;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = true;
    ui->glyphsRadioButton->setChecked (displayAmmoLifeGlyphs);
    ui->lifeBarRadioButton->setChecked (displayAmmoLifeBar);
    ui->lifeNumRadioButton->setChecked (displayAmmoLifeNumber);
}

