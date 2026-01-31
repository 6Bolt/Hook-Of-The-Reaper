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


    enableNewGameFileCreation = p_comDeviceList->GetEnableNewGameFileCreation ();

    //If Set. then Check the Box
    if(enableNewGameFileCreation)
        ui->enableNewGFCheckBox->setCheckState (Qt::Checked);
    else
        ui->enableNewGFCheckBox->setCheckState (Qt::Unchecked);



    //Convert ms to s, and then convert to QString
    //reaperHoldSlideTimeSec = static_cast<float>(static_cast<float>(reaperHoldSlideTime) / 1000.0f);
    //reaperHoldSlideTimeSecString = QString::number (reaperHoldSlideTimeSec).leftJustified (2, '0');


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
    QString tempRTD;
    bool isNumber;

    defaultLG = ui->useDefaultLGCheckBox->checkState ();
    multiThreading = ui->useMultiThreadCheckBox->checkState ();
    closeComPort = ui->closeComCheckBox->checkState ();
    ignoreUDLGGF = ui->ignoreUDLGGFCheckBox->checkState ();
    bypassSWC = ui->bypassSerialWriteCheckBox->checkState ();
    enableNGFC = ui->enableNewGFCheckBox->checkState ();


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

    if(enableNGFC == Qt::Checked)
        enableNewGameFileCreation = true;
    else
        enableNewGameFileCreation = false;


    //reaperHoldSlideTime = static_cast<quint16>(reaperHoldSlideTimeSec*1000.0f);

    p_comDeviceList->SetUseDefaultLGFirst(useDefaultLGFirst);
    p_comDeviceList->SetUseMultiThreading (useMultiThreading);
    p_comDeviceList->SetCloseComPortGameExit (closeComPortGameExit);
    p_comDeviceList->SetIgnoreUselessDFLGGF (ignoreUselessDLGGF);
    p_comDeviceList->SetSerialPortWriteCheckBypass (bypassSerialWriteChecks);
    p_comDeviceList->SetEnableNewGameFileCreation (enableNewGameFileCreation);

    if(isNumber)
    {
        p_comDeviceList->SetRefreshTimeDisplay(refreshDisplayTime);
        refreshDisplayTimeString = QString::number (refreshDisplayTime);
    }

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


