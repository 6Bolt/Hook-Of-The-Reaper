#include "editLightControllerWindow.h"
#include "Windows/ui_editLightControllerWindow.h"

editLightControllerWindow::editLightControllerWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::editLightControllerWindow)
{
    ui->setupUi(this);

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports & Used Dip Players
    p_comDeviceList = cdList;

    numberSaveLightCntlrs = p_comDeviceList->GetNumberLightControllers();
    noControllers = false;

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

    //Add Light Controllers to the ComboBox
    quint8 i;
    for(i = 0; i < numberSaveLightCntlrs; i++)
    {
        quint8 templightCntlrNumber = p_comDeviceList->p_lightCntlrList[i]->GetLightCntlrNumber();
        dataUltimarc[i] = p_comDeviceList->p_lightCntlrList[i]->GetUltimarcData();

        QString controllerInfo = QString::number(templightCntlrNumber) + " - " + "ID: "+QString::number(dataUltimarc[i].id)+ " " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName;

        ui->controllerComboBox->insertItem(i, controllerInfo);
    }

    ui->controllerComboBox->setCurrentIndex (0);
}

editLightControllerWindow::~editLightControllerWindow()
{
    delete ui;
}


//Private Slots

void editLightControllerWindow::on_grpFilePushButton_clicked()
{
    //Open File Dialog
    QString fileAndPath = QFileDialog::getOpenFileName(
        this, "Light Controller Group File", "lightCntlr", "Light Controller Group Files (*.txt)");

    //Clear out Old Data
    ui->groupFileLineEdit->clear ();

    ui->groupFileLineEdit->insert(fileAndPath);
}

void editLightControllerWindow::on_controllerComboBox_currentIndexChanged(int index)
{

    DisplayUltimarcData(index);

}

//Delete the Light Controller
void editLightControllerWindow::on_deletePushButton_clicked()
{
    //Get Current Index
    quint8 index = ui->controllerComboBox->currentIndex ();

    p_comDeviceList->DeleteLightController (index);

    RemoveUltimarcData(index);

    RemoveComboBoxDisplay(index);

    if(noControllers)
    {
        //Close and Destroy the Window, Since No More Light Controllers
        accept ();
    }

}

void editLightControllerWindow::on_editPushButton_clicked()
{
    //Collect Current Index
    quint8 index = ui->controllerComboBox->currentIndex ();

    bool fileExists = CollectUltimarcData();

    if(fileExists)
    {
        bool didGroupFileLoad = p_comDeviceList->p_lightCntlrList[index]->SetGroupFile (dataUltimarc[index].groupFile);

        //qDebug() << "didGroupFileLoad" << didGroupFileLoad;

        if(!didGroupFileLoad)
            QMessageBox::critical (nullptr, "Group File Error", "The group file for light controller didn't load properly. Please fix errors in group file", QMessageBox::Ok);
    }
    else
        QMessageBox::critical (nullptr, "Group File Error", "The group file for light controller doesn't exist at the current path. Please select the file with the Set button.", QMessageBox::Ok);
}

void editLightControllerWindow::on_editClosePushButton_clicked()
{
    //Collect Current Index
    quint8 index = ui->controllerComboBox->currentIndex ();

    bool fileExists = CollectUltimarcData();

    if(fileExists)
    {
        bool didGroupFileLoad = p_comDeviceList->p_lightCntlrList[index]->SetGroupFile (dataUltimarc[index].groupFile);

        if(!didGroupFileLoad)
            QMessageBox::critical (nullptr, "Group File Error", "The group file for light controller didn't load properly. Please fix errors in group file", QMessageBox::Ok);
        else
            //Close and Destroy the Window
            accept ();
    }
    else
        QMessageBox::critical (nullptr, "Group File Error", "The group file for light controller doesn't exist at the current path. Please select the file with the Set button.", QMessageBox::Ok);
}

void editLightControllerWindow::on_closePushButton_clicked()
{
    //Close and Destroy the Window
    accept ();
}



void editLightControllerWindow::DisplayUltimarcData(quint8 index)
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
    ui->groupFileLineEdit->clear();

    if(numberSaveLightCntlrs > index)
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
        ui->groupFileLineEdit->insert(dataUltimarc[index].groupFile);
    }
}


void editLightControllerWindow::RemoveUltimarcData(quint8 index)
{
    UltimarcData tempData;

    //If only 1 Ultimarc Device in Data, then Remove it
    if(numberSaveLightCntlrs == 1 && index == 0)
    {
        dataUltimarc[index] = tempData;
        noControllers = true;
    }
    else if(numberSaveLightCntlrs > 1 && index == (numberSaveLightCntlrs-1))
    {
        //Device is the last in the List
        dataUltimarc[index] = tempData;
    }
    else
    {
        //Device is First or Somewhere in the Middle, and have to Move things Around
        quint8 i;
        for(i = index; i < numberSaveLightCntlrs; i++)
        {
            if(i != (numberSaveLightCntlrs-1))
                dataUltimarc[i] = dataUltimarc[i+1];
            else
                dataUltimarc[i] = tempData;
        }
    }

    numberSaveLightCntlrs--;
}


void editLightControllerWindow::RemoveComboBoxDisplay(quint8 index)
{
    qint8 count = ui->controllerComboBox->count ();

    //If Only Displaying 1 Item, then Set Text to Blank
    if(count == 1 && index == 0)
    {
        ui->controllerComboBox->setItemText(0, "No Light Controllers Saved");
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
                quint8 templightCntlrNumber = p_comDeviceList->p_lightCntlrList[i]->GetLightCntlrNumber();
                QString controllerInfo = QString::number(templightCntlrNumber) + " - " + "ID: "+QString::number(dataUltimarc[i].id)+ " " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName;
                ui->controllerComboBox->setItemText(i, controllerInfo);
            }
            else
                ui->controllerComboBox->removeItem (i);
        }
    }
}


bool editLightControllerWindow::CollectUltimarcData()
{
    quint8 index = ui->controllerComboBox->currentIndex ();
    QString filePath = ui->groupFileLineEdit->text ();

    QFile loadLCData(filePath);

    //Check if the File Exists, as it might not be created yet. If no File, then exit out of member function
    if(loadLCData.exists() == false)
        return false;

    dataUltimarc[index].groupFile = filePath;
    return true;
}










