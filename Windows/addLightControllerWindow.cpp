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

    ultimarcArrayNumber = 0;

    //Set Line Edits to Read Only
    ui->deviceLineEdit->setReadOnly(true);
    ui->deviceIDLineEdit->setReadOnly(true);
    ui->vendorIDLineEdit->setReadOnly(true);
    ui->productIDLineEdit->setReadOnly(true);
    ui->vendorNameLineEdit->setReadOnly(true);
    ui->productNameLineEdit->setReadOnly(true);
    ui->versionLineEdit->setReadOnly(true);
    ui->versionLineEdit->setReadOnly(true);
    ui->serialNumberLineEdit->setReadOnly(true);
    ui->devicePathLineEdit->setReadOnly(true);

    //Get Ultimarc Light Controllers Count & Number of Devices in the
    numberUltimarcDevices = p_comDeviceList->p_pacDrive->GetNumberUltimarcDevices ();
    devicesInList = p_comDeviceList->p_pacDrive->GetNumberDevicesInList();

    if(numberUltimarcDevices > devicesInList)
    {
        noControllers = false;
        CollectUltimarcData();

        if(!noControllers)
        {
            for(i = 0; i < ultimarcArrayNumber; i++)
            {
                QString controllerInfo = "ID: "+QString::number(dataUltimarc[i].id)+ " - " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName;

                ui->controllerComboBox->insertItem(i, controllerInfo);
            }

            ui->controllerComboBox->setCurrentIndex (0);
        }
        else
            ui->controllerComboBox->insertItem(0, "No Light Controllers Found");
    }
    else
        ui->controllerComboBox->insertItem(0, "No Light Controllers Found");

}

addLightControllerWindow::~addLightControllerWindow()
{
    delete ui;
}


//Private Slots

void addLightControllerWindow::on_pushButton_clicked()
{
    //Open File Dialog
    QString fileAndPath = QFileDialog::getOpenFileName(
        this, "Light Controller Group File", "lightCntlr", "Light Controller Group Files (*.txt)");


    ui->groupFileLineEdit->insert(fileAndPath);
}

void addLightControllerWindow::on_controllerComboBox_currentIndexChanged(int index)
{
    if(ultimarcArrayNumber > 0 && index < ultimarcArrayNumber)
    {
        DisplayUltimarcData(index);
    }
}


void addLightControllerWindow::on_addPushButton_clicked()
{
    AddLightController();
}

void addLightControllerWindow::on_addClosePushButton_clicked()
{
    bool goodAdd = AddLightController();

    //If Light Controller was Added, then Close Window
    if(goodAdd)
        accept ();

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
        UltimarcData tempData;

        for(i = 0; i < numberUltimarcDevices; i++)
        {
            tempData = p_comDeviceList->p_pacDrive->GetUltimarcData (i);

            if(!tempData.inList && tempData.valid)
            {
                dataUltimarc[ultimarcArrayNumber] = tempData;
                ultimarcArrayNumber++;
            }
        }

        if(ultimarcArrayNumber == 0)
            noControllers = true;
    }
}

void addLightControllerWindow::DisplayUltimarcData(quint8 index)
{
    ui->deviceLineEdit->clear();
    ui->deviceIDLineEdit->clear();
    ui->vendorIDLineEdit->clear();
    ui->productIDLineEdit->clear();
    ui->vendorNameLineEdit->clear();
    ui->productNameLineEdit->clear();
    ui->versionLineEdit->clear();
    ui->serialNumberLineEdit->clear();
    ui->devicePathLineEdit->clear();

    if(index < ultimarcArrayNumber)
    {
        ui->deviceLineEdit->insert(dataUltimarc[index].typeName);
        ui->deviceIDLineEdit->insert(QString::number(dataUltimarc[index].deviceID));
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



void addLightControllerWindow::RemoveUltimarcData(quint8 index)
{
    if(ultimarcArrayNumber > 0)
    {

        UltimarcData tempData;

        //If only 1 Ultimarc Device in Data, then Remove it
        if(ultimarcArrayNumber == 1 && index == 0)
        {
            dataUltimarc[index] = tempData;
            noControllers = true;
        }
        else if(ultimarcArrayNumber > 1 && index == (ultimarcArrayNumber-1))
        {
            //Device is the last in the List
            dataUltimarc[index] = tempData;
        }
        else
        {
            //Device is First or Somewhere in the Middle, and have to Move things Around
            quint8 i;
            for(i = index; i < ultimarcArrayNumber; i++)
            {
                if(i != (ultimarcArrayNumber-1))
                    dataUltimarc[i] = dataUltimarc[i+1];
                else
                    dataUltimarc[i] = tempData;
            }
        }

        ultimarcArrayNumber--;
    }
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
        //Rebuild Text in Combo Box
        quint8 i;
        for(i = index; i < count; i++)
        {
            if(i != (count-1))
            {
                QString controllerInfo = "ID: "+QString::number(dataUltimarc[i].id)+ " - " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName;
                ui->controllerComboBox->setItemText(i, controllerInfo);
            }
            else
                ui->controllerComboBox->removeItem (i);
        }
    }
}


bool addLightControllerWindow::AddLightController()
{
    //If No New Controller, then Do Nothing
    if(noControllers)
    {
        QMessageBox::critical (nullptr, "Add Light Controller Error", "Couldn't add light controller as there is no light controller to add", QMessageBox::Ok);
        return false;
    }

    //Check if Group File Exists
    QString fileAndPath = ui->groupFileLineEdit->text ();
    QFile groupData(fileAndPath);
    qint8 index = ui->controllerComboBox->currentIndex ();

    if(groupData.exists() && index >= 0 && ultimarcArrayNumber > 0)
    {
        //Group File Exists, Now Trail Load it
        LightController tempLC(0, dataUltimarc[index]);

        connect(&tempLC,&LightController::ShowErrorMessage, this, &addLightControllerWindow::ErrorMessage);

        bool didFileLoad = tempLC.SetGroupFile(fileAndPath);

        disconnect(&tempLC,&LightController::ShowErrorMessage, this, &addLightControllerWindow::ErrorMessage);

        //Add Light Controller to Light Controller List
        //qDebug() << "Adding Light Controller with Group File:" << fileAndPath;

        if(didFileLoad)
        {
            dataUltimarc[index].groupFile = fileAndPath;

            p_comDeviceList->AddLightController(dataUltimarc[index]);

            //Remove Light Controller Data that was Added to the List
            RemoveUltimarcData(index);

            //Remove the ComboBox Display Name
            RemoveComboBoxDisplay(index);

            ui->controllerComboBox->setCurrentIndex (0);

            DisplayUltimarcData(0);

            ui->groupFileLineEdit->clear ();

            return true;
        }
        else
            QMessageBox::critical (nullptr, "Group File Error", "Couldn't add light controller to the list,because the group file failed to load", QMessageBox::Ok);
    }
    else
    {
        QMessageBox::critical (nullptr, "Add Light Controller Error", "Couldn't add light controller to the list. Something is wrong with the group file", QMessageBox::Ok);
    }

    return false;
}



//Public Slots

void addLightControllerWindow::ErrorMessage(const QString &title, const QString &message)
{
    QMessageBox::critical (this, title, message, QMessageBox::Ok);
}








