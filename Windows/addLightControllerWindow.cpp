#include "addLightControllerWindow.h"
#include "ui_addLightControllerWindow.h"
#include "../Global.h"


addLightControllerWindow::addLightControllerWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addLightControllerWindow)
{
    ui->setupUi(this);

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports & Used Dip Players
    p_comDeviceList = cdList;

    //Get Number of Saved Light Controllers
    numberSaveLightCntlrs = p_comDeviceList->GetNumberLightControllers ();

    //If No New Controllers are found, Starts off as true, then false when new controller is found
    noControllers = true;

    //Varibles
    quint8 i;

    //Set Line Edits to Read Only
    ui->deviceLineEdit->setReadOnly(true);
    ui->vendorIDLineEdit->setReadOnly(true);
    ui->productIDLineEdit->setReadOnly(true);
    ui->vendorNameLineEdit->setReadOnly(true);
    ui->productNameLineEdit->setReadOnly(true);
    ui->versionLineEdit->setReadOnly(true);
    ui->versionLineEdit->setReadOnly(true);
    ui->serialNumberLineEdit->setReadOnly(true);
    ui->devicePathLineEdit->setReadOnly(true);

    //Init Ultimarc Light Controllers and Find How Many They Are
    numberUltimarcDevices = p_comDeviceList->p_pacDrive->GetNumberUltimarcDevices ();

    if(numberUltimarcDevices > 0)
    {
        CollectUltimarcData();
        noControllers = false;

        if(numberSaveLightCntlrs > 0)
        {
            //Check Saved Light Controller(s) Against Found Light Controller
            //If they Match, don't list them
            CheckSaveLightCntlrsWithNew();
        }

        if(!noControllers)
        {
            DisplayUltimarcData(0);

            for(i = 0; i < numberUltimarcDevices; i++)
            {
                QString controllerInfo = QString::number(i) + " - " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName + " ID: " + QString::number(dataUltimarc[i].id);

                ui->controllerComboBox->insertItem(i, controllerInfo);
            }
        }
        else
        {
            ui->controllerComboBox->insertItem(0, "No Light Controllers Found");
        }
    }

}

addLightControllerWindow::~addLightControllerWindow()
{
    delete ui;
}


//Private Slots

void addLightControllerWindow::on_pushButton_clicked()
{
    qint8 index = ui->controllerComboBox->currentIndex ();

    //Open File Dialog
    QString fileAndPath = QFileDialog::getOpenFileName(
        this, "Light Controller Group File", "lightCntlr", "Light Controller Group Files (*.txt)");


    ui->groupFileLineEdit->insert(fileAndPath);

    if(index >= 0)
        dataUltimarc[index].groupFile = fileAndPath;
}

void addLightControllerWindow::on_controllerComboBox_currentIndexChanged(int index)
{
    if(numberUltimarcDevices > 0 && index < numberUltimarcDevices)
    {
        DisplayUltimarcData(index);
    }
}


void addLightControllerWindow::on_addPushButton_clicked()
{
    //If No New Controller, then Do Nothing
    if(noControllers)
    {
        QMessageBox::critical (nullptr, "Add Light Controller Error", "Couldn't add light controller as there is no light controller to add", QMessageBox::Ok);
        return;
    }

    //Check if Group File Exists
    QString fileAndPath = ui->groupFileLineEdit->text ();
    QFile groupData(fileAndPath);
    qint8 index = ui->controllerComboBox->currentIndex ();

    if(groupData.exists() && (index >= 0 && index < ULTIMARCMAXDEVICES))
    {
        dataUltimarc[index].groupFile = fileAndPath;

        //Add Light Controller to Light Controller List
        //qDebug() << "Adding Light Controller with Group File:" << fileAndPath;

        p_comDeviceList->AddLightController(dataUltimarc[index]);

        //Remove Light Controller Data that was Added to the List
        RemoveUltimarcData(index);

        //Remove the ComboBox Display Name
        RemoveComboBoxDisplay(index);

        DisplayUltimarcData(0);

        ui->groupFileLineEdit->clear ();
    }
    else
    {
        QMessageBox::critical (nullptr, "Add Light Controller Error", "Couldn't add light controller to the list. Something wrong with light controller or group file", QMessageBox::Ok);
    }

}

void addLightControllerWindow::on_addClosePushButton_clicked()
{
    //If No New Controller, then Do Nothing and
    if(noControllers)
    {
        QMessageBox::critical (nullptr, "Add Light Controller Error", "Couldn't add light controller as there is no light controller to add", QMessageBox::Ok);
        return;
    }

    //Check if Group File Exists
    QString fileAndPath = ui->groupFileLineEdit->text ();
    QFile groupData(fileAndPath);
    qint8 index = ui->controllerComboBox->currentIndex ();

    if(groupData.exists() && (index >= 0 && index < ULTIMARCMAXDEVICES))
    {
        dataUltimarc[index].groupFile = fileAndPath;

        //Add Light Controller to Light Controller List
        //qDebug() << "Adding Light Controller with Group File:" << fileAndPath;

        p_comDeviceList->AddLightController(dataUltimarc[index]);

        //Remove Light Controller Data that was Added to the List
        RemoveUltimarcData(index);

        //Remove the ComboBox Display Name
        RemoveComboBoxDisplay(index);

        //Close and Destroy the Window
        accept ();
    }
    else
    {
        QMessageBox::critical (nullptr, "Add Light Controller Error", "Couldn't add light controller to the list. Something wrong with light controller or group file", QMessageBox::Ok);
    }
}


void addLightControllerWindow::on_closePushButton_clicked()
{
    //Close and Destroy the Window
    accept ();
}


//Private Functions

void addLightControllerWindow::CollectUltimarcData()
{
    if(numberUltimarcDevices > 0)
    {
        quint8 i;

        for(i = 0; i < numberUltimarcDevices; i++)
        {
            dataUltimarc[i] = p_comDeviceList->p_pacDrive->GetUltimarcData (i);
        }
    }
}

void addLightControllerWindow::DisplayUltimarcData(quint8 index)
{
    ui->deviceLineEdit->clear();
    ui->vendorIDLineEdit->clear();
    ui->productIDLineEdit->clear();
    ui->vendorNameLineEdit->clear();
    ui->productNameLineEdit->clear();
    ui->versionLineEdit->clear();
    ui->serialNumberLineEdit->clear();
    ui->devicePathLineEdit->clear();

    if(numberUltimarcDevices > index)
    {
        ui->deviceLineEdit->insert(dataUltimarc[index].typeName);
        ui->vendorIDLineEdit->insert(dataUltimarc[index].vendorIDS);
        ui->productIDLineEdit->insert(dataUltimarc[index].productIDS);
        ui->vendorNameLineEdit->insert(dataUltimarc[index].vendorName);
        ui->productNameLineEdit->insert(dataUltimarc[index].productName);
        ui->versionLineEdit->insert(dataUltimarc[index].versionS);
        ui->serialNumberLineEdit->insert(dataUltimarc[index].serialNumber);
        ui->devicePathLineEdit->insert(dataUltimarc[index].devicePath);
        ui->devicePathLineEdit->setCursorPosition (0);
    }
}

void addLightControllerWindow::CheckSaveLightCntlrsWithNew()
{
    quint8 i, j;
    QList<quint8> removeUltimarcDevices;
    qint8 newNumberUltimarcDevices = numberUltimarcDevices;
    UltimarcData savedData;

    for(i = 0; i < numberSaveLightCntlrs; i++)
    {
        savedData = p_comDeviceList->p_lightCntlrList[i]->GetUltimarcData ();

        for(j = 0; j < numberUltimarcDevices; j++)
        {
            if(savedData == dataUltimarc[j] && newNumberUltimarcDevices > 0)
            {
                //Saved Light Controller Match Found
                removeUltimarcDevices << j;
                newNumberUltimarcDevices--;
            }
        }
    }

    //Remove the Found Ultimarc Devices
    for(i = 0; i < removeUltimarcDevices.length(); i++)
        RemoveUltimarcData(removeUltimarcDevices[i]);

}

void addLightControllerWindow::RemoveUltimarcData(quint8 index)
{
    UltimarcData tempData;

    //If only 1 Ultimarc Device in Data, then Remove it
    if(numberUltimarcDevices == 1 && index == 0)
    {
        dataUltimarc[index] = tempData;
        noControllers = true;
    }
    else if(numberUltimarcDevices > 1 && index == (numberUltimarcDevices-1))
    {
        //Device is the last in the List
        dataUltimarc[index] = tempData;
    }
    else
    {
        //Device is First or Somewhere in the Middle, and have to Move things Around
        quint8 i;
        for(i = index; i < numberUltimarcDevices; i++)
        {
            if(i != (numberUltimarcDevices-1))
                dataUltimarc[i] = dataUltimarc[i+1];
            else
                dataUltimarc[i] = tempData;
        }
    }

    numberUltimarcDevices--;
}

void addLightControllerWindow::RemoveComboBoxDisplay(quint8 index)
{
    qint8 count = ui->controllerComboBox->count ();

    //If Only Displaying 1 Item, then Set Text to Blank
    if(count == 1 && index == 0)
    {
        ui->controllerComboBox->setItemText(0, "No Light Controllers Found");
    }
    else if(count > 1 && index == (count-1))
    {
        //Last in the List
        ui->controllerComboBox->removeItem (index);
    }
    else
    {
        //Display is First or Somewhere in the Middle
        quint8 i;
        for(i = index; i < count; i++)
        {
            if(i != (count-1))
            {
                QString controllerInfo = QString::number(i) + " - " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName + " ID: " + QString::number(dataUltimarc[i].id);
                ui->controllerComboBox->setItemText(i, controllerInfo);
            }
            else
                ui->controllerComboBox->removeItem (i);
        }
    }
}








