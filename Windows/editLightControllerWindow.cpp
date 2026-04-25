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

    numberSaveUltimarcCntlrs = p_comDeviceList->GetNumberUltimarcControllers();
    numberALEDStripCntlrs = p_comDeviceList->GetNumberALEDStripControllers ();
    noControllers = false;

    if(numberSaveUltimarcCntlrs > 0)
        ultimarcPotitions = p_comDeviceList->GetUltimarcPotitions();
    else
        noControllers = true;

    if(numberALEDStripCntlrs > 0)
    {
        aledStripPotitions = p_comDeviceList->GetALEDPotitions();
        aledCntlrCOMNames = p_comDeviceList->GetALEDCOMNames();
        aledCntlrNumber = p_comDeviceList->GetALEDCOMNumbers();
    }

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

    //Set LED Strip Number Boxes to Digits Only
    ui->ledsStrip1LineEdit->setInputMask (ALEDNUMBERLINEMASK);
    ui->ledsStrip2LineEdit->setInputMask (ALEDNUMBERLINEMASK);
    ui->ledsStrip3LineEdit->setInputMask (ALEDNUMBERLINEMASK);
    ui->ledsStrip4LineEdit->setInputMask (ALEDNUMBERLINEMASK);


    //Fill Types Combo Box
    ui->typeComboBox->insertItem(0, ULTIMARCTYPE);
    ui->typeComboBox->insertItem(1, ALEDSTRIPTYPE);
    ui->typeComboBox->setCurrentIndex (0);
    typeSet = 0;

    //Fill ALED Strip Patterns Combo Box
    ui->ledPatternComboBox->insertItem (0, NOPATTERN);
    ui->ledPatternComboBox->insertItem (1, SNAKESPAT);
    ui->ledPatternComboBox->insertItem (2, RANDOMPAT);
    ui->ledPatternComboBox->insertItem (3, SPARKLESPAT);
    ui->ledPatternComboBox->insertItem (4, GREYSPAT);

    SetALED(false);

    //Clear Out Combo Box
    ui->controllerComboBox->clear ();

    if(numberSaveUltimarcCntlrs > 0)
    {
        //Add Light Controllers to the ComboBox
        quint8 i;
        for(i = 0; i < numberSaveUltimarcCntlrs; i++)
        {
            quint8 index = ultimarcPotitions[i];

            quint8 templightCntlrNumber = p_comDeviceList->p_lightCntlrList[index]->GetLightCntlrNumber();
            dataUltimarc[i] = p_comDeviceList->p_lightCntlrList[index]->GetUltimarcData();

            QString controllerInfo = QString::number(templightCntlrNumber) + " - " + "ID: "+QString::number(dataUltimarc[i].id)+ " " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName;

            ui->controllerComboBox->insertItem(i, controllerInfo);
        }
    }
    else
        ui->controllerComboBox->insertItem(0, "No Light Controllers Found");

    ui->controllerComboBox->setCurrentIndex (0);

    if(numberSaveUltimarcCntlrs == 0 && numberALEDStripCntlrs > 0)
        ui->typeComboBox->setCurrentIndex (1);
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
    if(typeSet == 0)
    {
        if(numberSaveUltimarcCntlrs > 0 && index < numberSaveUltimarcCntlrs)
            DisplayUltimarcData(index);
    }
    else if(typeSet == 1)
    {
        if(numberALEDStripCntlrs > 0)
            DisplayALEDStripData(index);
    }
}

//Delete the Light Controller
void editLightControllerWindow::on_deletePushButton_clicked()
{
    //Get Current Index
    quint8 index = ui->controllerComboBox->currentIndex ();

    if(typeSet == 0)
    {
        quint8 potition = ultimarcPotitions[index];

        p_comDeviceList->DeleteLightController (potition);

        RemoveUltimarcData(index);

        RemoveComboBoxDisplay(index);

        if(numberSaveUltimarcCntlrs > 0)
            ultimarcPotitions = p_comDeviceList->GetUltimarcPotitions();
        else
            ultimarcPotitions.clear();

        if(numberALEDStripCntlrs > 0)
        {
            aledStripPotitions = p_comDeviceList->GetALEDPotitions();
            aledCntlrCOMNames = p_comDeviceList->GetALEDCOMNames();
            aledCntlrNumber = p_comDeviceList->GetALEDCOMNumbers();
        }

        if(numberSaveUltimarcCntlrs == 0 && numberALEDStripCntlrs > 0)
            ui->typeComboBox->setCurrentIndex (1);
    }
    else if(typeSet == 1)
    {
        quint8 potition = aledStripPotitions[index];

        p_comDeviceList->DeleteLightController (potition);

        numberALEDStripCntlrs--;

        if(numberSaveUltimarcCntlrs > 0)
            ultimarcPotitions = p_comDeviceList->GetUltimarcPotitions();

        ui->controllerComboBox->clear ();

        if(numberALEDStripCntlrs > 0)
        {
            aledStripPotitions = p_comDeviceList->GetALEDPotitions();
            aledCntlrCOMNames = p_comDeviceList->GetALEDCOMNames();
            aledCntlrNumber = p_comDeviceList->GetALEDCOMNumbers();

            quint8 i;
            for(i = 0; i < numberALEDStripCntlrs; i++)
            {
                quint8 index = aledStripPotitions[i];
                quint8 templightCntlrNumber = p_comDeviceList->p_lightCntlrList[index]->GetLightCntlrNumber();

                //QString controllerInfo = QString::number(templightCntlrNumber) + " - LED Hook: Strip - " + aledCntlrCOMNames[i];
                QString controllerInfo = QString::number(templightCntlrNumber) + " - " + aledCntlrCOMNames[i];

                ui->controllerComboBox->insertItem(i, controllerInfo);
            }
        }
        else
        {
            ui->controllerComboBox->insertItem(0, "No Strip Controllers Found");
            aledStripPotitions.clear();
            aledCntlrCOMNames.clear();
            aledCntlrNumber.clear();
        }

        ui->controllerComboBox->setCurrentIndex (0);
        SetALED(true);
        DisplayALEDStripData(0);

        if(numberALEDStripCntlrs == 0 && numberSaveUltimarcCntlrs > 0)
            ui->typeComboBox->setCurrentIndex (0);
    }

    if(noControllers && numberALEDStripCntlrs == 0)
    {
        //Close and Destroy the Window, Since No More Light Controllers
        accept ();
    }

}

void editLightControllerWindow::on_editPushButton_clicked()
{
    //Collect Current Index
    quint8 index = ui->controllerComboBox->currentIndex ();

    if(typeSet == 0)
    {
        bool fileExists = CollectUltimarcData();

        quint8 potition = ultimarcPotitions[index];

        if(fileExists)
        {
            bool didGroupFileLoad = p_comDeviceList->p_lightCntlrList[potition]->SetGroupFile (dataUltimarc[index].groupFile);

            //qDebug() << "didGroupFileLoad" << didGroupFileLoad;

            if(!didGroupFileLoad)
                QMessageBox::critical (nullptr, "Group File Error", "The group file for light controller didn't load properly. Please fix errors in group file", QMessageBox::Ok);
        }
        else
            QMessageBox::critical (nullptr, "Ultimarc Group File Error", "The group file for light controller doesn't exist at the current path. Please select the file with the Set button.", QMessageBox::Ok);
    }
    else if(typeSet == 1)
    {
        bool getData = CollectALEDStripData();

        if(getData)
        {
            quint8 potition = aledStripPotitions[index];

            if(aledStripDataChanged)
                p_comDeviceList->p_lightCntlrList[potition]->SetALEDStrip(aledNumberStrings, aledElements);

            if(aledPatChanged)
                p_comDeviceList->p_lightCntlrList[potition]->SetALEDPattern(aledPattern);

            if(aledStripDataChanged)
                p_comDeviceList->p_lightCntlrList[potition]->RedoSetUpALEDStrips();
            else if(aledPatChanged && !aledStripDataChanged)
                p_comDeviceList->p_lightCntlrList[potition]->UpdateALEDPattern();

            bool didGroupFileLoad = p_comDeviceList->p_lightCntlrList[potition]->SetGroupFile (fileAndPath);

            if(!didGroupFileLoad)
                QMessageBox::critical (nullptr, "LED Hook: Strip Group File Error", "The group file for light controller didn't load properly. Please fix errors in group file", QMessageBox::Ok);
        }
    }
}

void editLightControllerWindow::on_editClosePushButton_clicked()
{
    //Collect Current Index
    quint8 index = ui->controllerComboBox->currentIndex ();

    if(typeSet == 0)
    {

        bool fileExists = CollectUltimarcData();

        quint8 potition = ultimarcPotitions[index];

        if(fileExists)
        {
            bool didGroupFileLoad = p_comDeviceList->p_lightCntlrList[potition]->SetGroupFile (dataUltimarc[index].groupFile);

            if(!didGroupFileLoad)
                QMessageBox::critical (nullptr, "Group File Error", "The group file for light controller didn't load properly. Please fix errors in group file", QMessageBox::Ok);
            else
                //Close and Destroy the Window
                accept ();
        }
        else
            QMessageBox::critical (nullptr, "Group File Error", "The group file for light controller doesn't exist at the current path. Please select the file with the Set button.", QMessageBox::Ok);
    }
    else if(typeSet == 1)
    {
        bool getData = CollectALEDStripData();

        if(getData)
        {
            quint8 potition = aledStripPotitions[index];

            p_comDeviceList->p_lightCntlrList[potition]->SetALEDStrip(aledNumberStrings, aledElements);
            p_comDeviceList->p_lightCntlrList[potition]->SetALEDPattern(aledPattern);

            p_comDeviceList->p_lightCntlrList[potition]->RedoSetUpALEDStrips();

            bool didGroupFileLoad = p_comDeviceList->p_lightCntlrList[potition]->SetGroupFile (fileAndPath);

            if(didGroupFileLoad)
            {
                //Close and Destroy the Window
                accept ();
            }
            else
                QMessageBox::critical (nullptr, "LED Hook: Strip Group File Error", "The group file for light controller didn't load properly. Please fix errors in group file", QMessageBox::Ok);
        }
    }
}

void editLightControllerWindow::on_closePushButton_clicked()
{
    //Close and Destroy the Window
    accept ();
}

void editLightControllerWindow::on_typeComboBox_currentIndexChanged(int index)
{
    typeSet = index;

    //Clear Out Combo Box
    ui->controllerComboBox->clear ();

    if(typeSet == 0)
    {
        //Re-fill in Data
        if(numberSaveUltimarcCntlrs > 0)
        {
            quint8 i;
            for(i = 0; i < numberSaveUltimarcCntlrs; i++)
            {
                quint8 index = ultimarcPotitions[i];

                quint8 templightCntlrNumber = p_comDeviceList->p_lightCntlrList[index]->GetLightCntlrNumber();
                dataUltimarc[i] = p_comDeviceList->p_lightCntlrList[index]->GetUltimarcData();

                QString controllerInfo = QString::number(templightCntlrNumber) + " - " + "ID: "+QString::number(dataUltimarc[i].id)+ " " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName;

                ui->controllerComboBox->insertItem(i, controllerInfo);
            }
        }
        else
            ui->controllerComboBox->insertItem(0, "No Light Controllers Found");

        ui->controllerComboBox->setCurrentIndex (0);
        SetALED(false);
        DisplayUltimarcData(0);
    }
    else if(typeSet == 1)
    {
        if(numberALEDStripCntlrs > 0)
        {
            quint8 i;
            for(i = 0; i < numberALEDStripCntlrs; i++)
            {
                quint8 index = aledStripPotitions[i];
                quint8 templightCntlrNumber = p_comDeviceList->p_lightCntlrList[index]->GetLightCntlrNumber();

                //QString controllerInfo = QString::number(templightCntlrNumber) + " - LED Hook: Strip - " + aledCntlrCOMNames[i];
                QString controllerInfo = QString::number(templightCntlrNumber) + " - " + aledCntlrCOMNames[i];

                ui->controllerComboBox->insertItem(i, controllerInfo);
            }
        }
        else
            ui->controllerComboBox->insertItem(0, "No Strip Controllers Found");

        ui->controllerComboBox->setCurrentIndex (0);
        SetALED(true);
        DisplayALEDStripData(0);
    }
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

    if(numberSaveUltimarcCntlrs > index)
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

void editLightControllerWindow::DisplayALEDStripData(quint8 index)
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


    if(index < numberALEDStripCntlrs && numberALEDStripCntlrs > 0)
    {
        quint8 potition = aledStripPotitions[index];

        SerialPortInfo tempSPI = p_comDeviceList->p_lightCntlrList[potition]->GetCOMPortInfo ();

        ui->deviceLineEdit->insert(ALEDSTRIPTYPE);
        ui->deviceIDLineEdit->insert(tempSPI.productDiscription);

        ui->vendorIDLineEdit->insert(tempSPI.vendorIDString);
        ui->productIDLineEdit->insert(tempSPI.productIDString);

        ui->vendorNameLineEdit->insert(tempSPI.manufacturer);
        ui->productNameLineEdit->insert(ALEDSTRIPTYPE);
        ui->serialNumberLineEdit->insert(tempSPI.serialNumber);
        ui->devicePathLineEdit->insert(tempSPI.path);
        ui->devicePathLineEdit->setCursorPosition (0);

        fileAndPath = p_comDeviceList->p_lightCntlrList[potition]->GetGroupFile();

        ui->groupFileLineEdit->clear();
        ui->groupFileLineEdit->insert(fileAndPath);

        aledNumberStrings = p_comDeviceList->p_lightCntlrList[potition]->GetNumberStrips();
        aledElements = p_comDeviceList->p_lightCntlrList[potition]->GetElementCounts();
        aledPattern = p_comDeviceList->p_lightCntlrList[potition]->GetALEDPattern();

        //Display Strip Elements
        if(aledNumberStrings == 1)
            ui->ledsStrip1LineEdit->setText (QString::number(aledElements[0]));
        else if(aledNumberStrings == 2)
        {
            ui->ledsStrip1LineEdit->setText (QString::number(aledElements[0]));
            ui->ledsStrip2LineEdit->setText (QString::number(aledElements[1]));
        }
        else if(aledNumberStrings == 3)
        {
            ui->ledsStrip1LineEdit->setText (QString::number(aledElements[0]));
            ui->ledsStrip2LineEdit->setText (QString::number(aledElements[1]));
            ui->ledsStrip3LineEdit->setText (QString::number(aledElements[2]));
        }
        else if(aledNumberStrings == 4)
        {
            ui->ledsStrip1LineEdit->setText (QString::number(aledElements[0]));
            ui->ledsStrip2LineEdit->setText (QString::number(aledElements[1]));
            ui->ledsStrip3LineEdit->setText (QString::number(aledElements[2]));
            ui->ledsStrip4LineEdit->setText (QString::number(aledElements[3]));
        }

        //Display Pattern
        ui->ledPatternComboBox->setCurrentIndex (aledPattern);
    }
    else
    {
        ui->groupFileLineEdit->clear();
        ui->ledsStrip1LineEdit->clear();
        ui->ledsStrip2LineEdit->clear();
        ui->ledsStrip3LineEdit->clear();
        ui->ledsStrip4LineEdit->clear();
        ui->ledPatternComboBox->setCurrentIndex(0);
    }
}



void editLightControllerWindow::RemoveUltimarcData(quint8 index)
{
    UltimarcData tempData;

    //If only 1 Ultimarc Device in Data, then Remove it
    if(numberSaveUltimarcCntlrs == 1 && index == 0)
    {
        dataUltimarc[index] = tempData;
        noControllers = true;
    }
    else if(numberSaveUltimarcCntlrs > 1 && index == (numberSaveUltimarcCntlrs-1))
    {
        //Device is the last in the List
        dataUltimarc[index] = tempData;
    }
    else
    {
        //Device is First or Somewhere in the Middle, and have to Move things Around
        quint8 i;
        for(i = index; i < numberSaveUltimarcCntlrs; i++)
        {
            if(i != (numberSaveUltimarcCntlrs-1))
                dataUltimarc[i] = dataUltimarc[i+1];
            else
                dataUltimarc[i] = tempData;
        }
    }

    numberSaveUltimarcCntlrs--;
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


void editLightControllerWindow::SetALED(bool enable)
{
    ui->ledsStrip1LineEdit->setEnabled (enable);
    ui->ledsStrip2LineEdit->setEnabled (enable);
    ui->ledsStrip3LineEdit->setEnabled (enable);
    ui->ledsStrip4LineEdit->setEnabled (enable);
    ui->ledPatternComboBox->setEnabled (enable);

    if(!enable)
    {
        ui->ledsStrip1LineEdit->clear ();
        ui->ledsStrip2LineEdit->clear ();
        ui->ledsStrip3LineEdit->clear ();
        ui->ledsStrip4LineEdit->clear ();
    }

}


bool editLightControllerWindow::CollectALEDStripData()
{
    bool isStrip0, isStrip1, isStrip2;
    QList<quint16> ledElementsList;
    quint8 stripCount = 0;
    QString strip0 = ui->ledsStrip1LineEdit->text ();
    QString strip1 = ui->ledsStrip2LineEdit->text ();
    QString strip2 = ui->ledsStrip3LineEdit->text ();
    QString strip3 = ui->ledsStrip4LineEdit->text ();

    qint8 index = ui->controllerComboBox->currentIndex ();

    if(index < 0 || typeSet == 0)
        return false;

    if(strip0.isEmpty ())
    {
        QMessageBox::critical (nullptr, "Add ALED Strip Light Controller Error", "Couldn't add light controller as the first strip is empty", QMessageBox::Ok);
        return false;
    }
    else
    {
        isStrip0 = true;
        quint16 temp0Num = strip0.toUInt ();
        if(temp0Num < 0)
        {
            QMessageBox::critical (nullptr, "Add ALED Strip Light Controller Error", "Couldn't add light controller as the first strip is negative", QMessageBox::Ok);
            return false;
        }
        ledElementsList << temp0Num;
        stripCount++;
    }

    if(isStrip0 && !strip1.isEmpty())
    {
        isStrip1 = true;
        quint16 temp1Num = strip1.toUInt ();
        if(temp1Num < 0)
        {
            QMessageBox::critical (nullptr, "Add ALED Strip Light Controller Error", "Couldn't add light controller as the second strip is negative", QMessageBox::Ok);
            return false;
        }
        ledElementsList << temp1Num;
        stripCount++;
    }

    if(isStrip0 && isStrip1 && !strip2.isEmpty())
    {
        isStrip2 = true;
        quint16 temp2Num = strip2.toUInt ();
        if(temp2Num < 0)
        {
            QMessageBox::critical (nullptr, "Add ALED Strip Light Controller Error", "Couldn't add light controller as the third strip is negative", QMessageBox::Ok);
            return false;
        }
        ledElementsList << temp2Num;
        stripCount++;
    }


    if(isStrip0 && isStrip1 && isStrip2 && !strip3.isEmpty ())
    {
        quint16 temp3Num = strip3.toUInt ();
        if(temp3Num < 0)
        {
            QMessageBox::critical (nullptr, "Add ALED Strip Light Controller Error", "Couldn't add light controller as the fourth strip is negative", QMessageBox::Ok);
            return false;
        }
        ledElementsList << temp3Num;
        stripCount++;
    }


    if(aledNumberStrings != stripCount)
    {
        aledNumberStrings = stripCount;
        aledElements = ledElementsList;
        aledStripDataChanged = true;
    }
    else
    {
        bool change = false;
        for(quint8 i = 0; i < aledNumberStrings; i++)
        {
            if(aledElements[i] != ledElementsList[i])
                change = true;
        }

        if(change)
        {
            aledNumberStrings = stripCount;
            aledElements = ledElementsList;
            aledStripDataChanged = true;
        }
        else
            aledStripDataChanged = false;
    }

    quint8 patNum = ui->ledPatternComboBox->currentIndex ();

    if(aledPattern != patNum)
    {
        aledPattern = patNum;
        aledPatChanged = true;
    }
    else
        aledPatChanged = false;

    //Check the group file
    QString filePath = ui->groupFileLineEdit->text ();
    QFile groupData(filePath);

    if(!groupData.exists())
        return false;

    if(fileAndPath != filePath)
    {
        fileAndPath = filePath;
        fileChanged = true;
    }
    else
        fileChanged = false;

    return true;
}










