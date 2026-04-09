#include "addLightControllerWindow.h"
#include "Windows/ui_addLightControllerWindow.h"
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

    //Number of Found ALED Strip Controllers
    numberALEDCntlrs = 0;

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


    //Get Ultimarc Light Controllers Count & Number of Devices in the
    numberUltimarcDevices = p_comDeviceList->p_pacDrive->GetNumberUltimarcDevices ();
    devicesInList = p_comDeviceList->p_pacDrive->GetNumberDevicesInList();

    //qDebug() << "numberUltimarcDevices" << numberUltimarcDevices << "devicesInList" << devicesInList;

    //Clear Out Combo Box
    ui->controllerComboBox->clear ();

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

    //Search for ALED Strip Controllers
    SearchSerialPorts();

    if(noControllers && numberALEDCntlrs > 0)
        ui->typeComboBox->setCurrentIndex (1);

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

    //Clear Out Line Edit, and Add In File and Path
    ui->groupFileLineEdit->clear ();
    ui->groupFileLineEdit->insert(fileAndPath);
}

void addLightControllerWindow::on_controllerComboBox_currentIndexChanged(int index)
{
    if(typeSet == 0)
    {
        if(ultimarcArrayNumber > 0 && index < ultimarcArrayNumber)
            DisplayUltimarcData(index);
    }
    else if(typeSet == 1)
    {
        if(numberALEDCntlrs > 0)
            DisplayALEDStripData(index);
    }
}


void addLightControllerWindow::on_addPushButton_clicked()
{
    if(typeSet == 0)
        AddLightController();
    else if(typeSet == 1)
        AddALEDStripController();
}

void addLightControllerWindow::on_addClosePushButton_clicked()
{
    bool goodAdd;

    if(typeSet == 0)
        goodAdd = AddLightController();
    else if(typeSet == 1)
        goodAdd = AddALEDStripController();
    else
        goodAdd = false;

    //If Light Controller was Added, then Close Window
    if(goodAdd)
        accept ();

}


void addLightControllerWindow::on_closePushButton_clicked()
{
    //Close and Destroy the Window
    accept ();
}

void addLightControllerWindow::on_typeComboBox_currentIndexChanged(int index)
{
    typeSet = index;

    //qDebug() << "Updating Types" << index;

    //Clear Out Combo Box
    ui->controllerComboBox->clear ();

    if(typeSet == 0)
    {
        //Re-fill in Data
        if(!noControllers)
        {
            quint8 i;
            for(i = 0; i < ultimarcArrayNumber; i++)
            {
                QString controllerInfo = "ID: "+QString::number(dataUltimarc[i].id)+ " - " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName;

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
        if(numberALEDCntlrs > 0)
        {
            quint8 i;
            for(i = 0; i < numberALEDCntlrs; i++)
            {
                QString controllerInfo = "LED Hook: Strip - " + foundALEDCntlr[i];

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


void addLightControllerWindow::DisplayALEDStripData(quint8 index)
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

    if(index < numberALEDCntlrs)
    {
        ui->deviceLineEdit->insert(ALEDSTRIPTYPE);
        ui->deviceIDLineEdit->insert(foundALEDInfo[index].productDiscription);

        ui->vendorIDLineEdit->insert(foundALEDInfo[index].vendorIDString);
        ui->productIDLineEdit->insert(foundALEDInfo[index].productIDString);

        ui->vendorNameLineEdit->insert(foundALEDInfo[index].manufacturer);
        ui->productNameLineEdit->insert(ALEDSTRIPTYPE);
        ui->serialNumberLineEdit->insert(foundALEDInfo[index].serialNumber);
        ui->devicePathLineEdit->insert(foundALEDInfo[index].path);
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
        UltimarcLC tempLC(0, dataUltimarc[index]);

        connect(&tempLC,&UltimarcLC::ShowErrorMessage, this, &addLightControllerWindow::ErrorMessage);

        bool didFileLoad = tempLC.SetGroupFile(fileAndPath);

        disconnect(&tempLC,&UltimarcLC::ShowErrorMessage, this, &addLightControllerWindow::ErrorMessage);

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


void addLightControllerWindow::SearchSerialPorts()
{
    QString tempSP;
    quint8 i;

    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : serialPortInfos)
    {
        quint16 vid, pid;


        if(portInfo.hasVendorIdentifier() && portInfo.hasProductIdentifier())
        {
            vid = portInfo.vendorIdentifier();
            pid = portInfo.productIdentifier();

            if(vid == ALEDCNTLRVID && pid == ALEDCNTLRPID)
            {
                //qDebug() << "Port Name:" << portInfo.portName();
                QString comNam = portInfo.portName();
                QString comN = comNam;
                comN.remove ("COM");
                //qDebug() << "Port Number" << comN;
                quint8 comNum = comN.toUInt ();
                //Check if it is in the List Already
                bool check = p_comDeviceList->CheckALEDStripCOM (comNum);
                //qDebug() << "Check is" << check;

                if(!check)
                {
                    numberALEDCntlrs++;
                    foundALEDCntlr << comNam;
                    foundALEDCntlrNumber << comNum;

                    SerialPortInfo tempSPI;
                    tempSPI.hasVendorID = true;
                    tempSPI.hasProductID = true;
                    tempSPI.portName = comNam;
                    tempSPI.portNumber = comNum;
                    tempSPI.path = portInfo.systemLocation();
                    tempSPI.productDiscription = portInfo.description();
                    tempSPI.manufacturer = portInfo.manufacturer();
                    tempSPI.serialNumber = portInfo.serialNumber();
                    tempSPI.vendorID = portInfo.vendorIdentifier();
                    tempSPI.productID = portInfo.productIdentifier();

                    QString tempVID = QString::number(tempSPI.vendorID, 16).rightJustified(4, '0');
                    tempVID = tempVID.toUpper ();
                    tempVID.prepend ("0x");

                    QString tempPID = QString::number(tempSPI.productID, 16).rightJustified(4, '0');
                    tempVID = tempVID.toUpper ();
                    tempVID.prepend ("0x");

                    tempSPI.vendorIDString = tempVID;
                    tempSPI.productIDString = tempPID;

                    foundALEDInfo << tempSPI;

                    //qDebug() << "Found ALED Strip Controller on " << tempSP;
                }
            }
        }
    }
}

void addLightControllerWindow::SetALED(bool enable)
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


bool addLightControllerWindow::AddALEDStripController()
{
    bool isStrip0 = false;
    bool isStrip1 = false;
    bool isStrip2 = false;
    QList<quint16> ledElementsList;
    quint8 stripCount = 0;
    QString strip0 = ui->ledsStrip1LineEdit->text ();
    QString strip1 = ui->ledsStrip2LineEdit->text ();
    QString strip2 = ui->ledsStrip3LineEdit->text ();
    QString strip3 = ui->ledsStrip4LineEdit->text ();

    qint8 index = ui->controllerComboBox->currentIndex ();

    if(index < 0)
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

    //Get Pattern
    quint8 patNum = ui->ledPatternComboBox->currentIndex ();

    //Get Path
    QString path = ui->devicePathLineEdit->text();

    //Check the group file
    QString fileAndPath = ui->groupFileLineEdit->text ();
    QFile groupData(fileAndPath);

    if(!groupData.exists())
    {
        QMessageBox::critical (nullptr, "Add ALED Strip Light Controller Error", "Couldn't add light controller as the path to group file doesn't exsist", QMessageBox::Ok);
        return false;
    }

    p_comDeviceList->AddALEDStripController(foundALEDCntlrNumber[index], foundALEDCntlr[index], foundALEDInfo[index], stripCount, ledElementsList, patNum, fileAndPath);

    ui->groupFileLineEdit->clear ();

    RemoveALEDStrip(index);

    return true;
}

void addLightControllerWindow::RemoveALEDStrip(quint8 index)
{
    if(numberALEDCntlrs == 1)
    {
        foundALEDCntlrNumber.clear();
        foundALEDCntlr.clear();
        numberALEDCntlrs = 0;
    }
    else
    {
        QStringList tempSL;
        QList<quint8> tempNum;

        for(quint8 i = 0; i < numberALEDCntlrs; i++)
        {
            if(i != index)
            {
                tempSL << foundALEDCntlr[i];
                tempNum << foundALEDCntlrNumber[i];
            }
        }

        foundALEDCntlr = tempSL;
        foundALEDCntlrNumber = tempNum;
        numberALEDCntlrs--;
    }

    if(numberALEDCntlrs > 0)
    {
        for(quint8 i = 0; i < numberALEDCntlrs; i++)
        {
            QString controllerInfo = "LED Hook: Strip - " + foundALEDCntlr[i];

            ui->controllerComboBox->insertItem(i, controllerInfo);
        }
    }
    else
        ui->controllerComboBox->insertItem(0, "No Strip Controllers Found");

    ui->controllerComboBox->setCurrentIndex (0);
    SetALED(true);
    DisplayALEDStripData(0);
}

//Public Slots

void addLightControllerWindow::ErrorMessage(const QString &title, const QString &message)
{
    QMessageBox::critical (this, title, message, QMessageBox::Ok);
}










