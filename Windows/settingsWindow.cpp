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


    p_comDeviceList->SetUseDefaultLGFirst(useDefaultLGFirst);
    p_comDeviceList->SetUseMultiThreading (useMultiThreading);
    p_comDeviceList->SetCloseComPortGameExit (closeComPortGameExit);

    if(isNumber)
    {
        p_comDeviceList->SetRefreshTimeDisplay(refreshDisplayTime);
        refreshDisplayTimeString = QString::number (refreshDisplayTime);
    }

    p_comDeviceList->SaveSettings();
}

