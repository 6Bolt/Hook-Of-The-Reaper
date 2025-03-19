#include "addLightGunWindow.h"
#include "ui_addLightGunWindow.h"
#include "../Global.h"


//Code to Pring out Serial Port Info
/*
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : serialPortInfos)
    {
        qDebug() << "\n"
                 << "Port:" << portInfo.portName() << "\n"
                 << "Location:" << portInfo.systemLocation() << "\n"
                 << "Description:" << portInfo.description() << "\n"
                 << "Manufacturer:" << portInfo.manufacturer() << "\n"
                 << "Serial number:" << portInfo.serialNumber() << "\n"
                 << "Vendor Identifier:"
                 << (portInfo.hasVendorIdentifier()
                         ? QByteArray::number(portInfo.vendorIdentifier(), 16)
                         : QByteArray()) << "\n"
                 << "Product Identifier:"
                 << (portInfo.hasProductIdentifier()
                         ? QByteArray::number(portInfo.productIdentifier(), 16)
                         : QByteArray());
    }
*/


//Constructor
addLightGunWindow::addLightGunWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addLightGunWindow)
{
    QString tempQS;

    //Set Up This Windows
    ui->setupUi(this);

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());


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

    //Fill out Serial Port 0 Info
    FillSerialPortInfo(0);


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
            tempQS = "P"+QString::number(comPortIndx+1);
        ui->dipSwitchComboBox->insertItem(comPortIndx,tempQS);
    }

    //Hub COM Port Combo Box - Adding All COM Ports
    for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
    {
        tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
        ui->hubComComboBox->insertItem(comPortIndx,tempQS);
    }


    //Only for MX24 Light Gun
    ui->dipSwitchComboBox->setEnabled(false);
    ui->hubComComboBox->setEnabled(false);
    //Only for JB Gun4IR & OpenFire
    ui->analogLineEdit->setInputMask (ANALOGSTRENGTHMASK);
    ui->analogLineEdit->setEnabled(false);
}

//Deconstructor
addLightGunWindow::~addLightGunWindow()
{
    //Delete Window
    delete ui;

    //Delete temp ComPortDevice pointer
    if(p_comPortInfo != nullptr)
        delete p_comPortInfo;

}


//private slots

void addLightGunWindow::on_defaultLightGunComboBox_currentIndexChanged(int index)
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
        else
        {
            ui->dipSwitchComboBox->setEnabled(false);
            ui->hubComComboBox->setEnabled(false);
            ui->comPortComboBox->setEnabled(true);

            quint8 comPortIndex = ui->comPortComboBox->currentIndex ();

            FillSerialPortInfo(comPortIndex);
        }

        if(index == JBGUN4IR || index == OPENFIRE)
            ui->analogLineEdit->setEnabled(true);
        else
        {
            ui->analogLineEdit->clear ();
            ui->analogLineEdit->setEnabled(false);
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
        ui->analogLineEdit->clear ();
        ui->analogLineEdit->setEnabled(false);
        ui->hubComComboBox->setEnabled(false);
    }
}

void addLightGunWindow::on_comPortComboBox_currentIndexChanged(int index)
{
    FillSerialPortInfo(index);
}


void addLightGunWindow::on_addPushButton_clicked()
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
                tempQS = "P"+QString::number(comPortIndx+1);
            ui->dipSwitchComboBox->setItemText(comPortIndx,tempQS);
        }
    }
}


void addLightGunWindow::on_okPushButton_clicked()
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
        //Close and Destroy the Window
        accept ();
    }
}

void addLightGunWindow::on_cancelPushButton_clicked()
{
    //Close and Destroy the Window
    accept ();
}


//private

bool addLightGunWindow::IsValidData()
{
    QString ivTemp, message;
    bool lgnIsEmpty = false;
    bool comPortNumEmpty = false;
    bool unusedName = false;
    bool analNotNumber = false;
    bool analNotRange = false;
    bool badDipIndex = false;
    bool hubComLG = false;
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
    else
    {
        //Check If the Com Port Name is Empty for the Combo Box
        ivTemp = ui->comPortComboBox->currentText ();
        if(ivTemp.isEmpty ())
            comPortNumEmpty = true;
    }

    if(defaultLGIndex == JBGUN4IR || defaultLGIndex == OPENFIRE)
    {
        bool isNumber;
        QString analString = ui->analogLineEdit->text();
        quint16 analStrengthBig = analString.toUInt (&isNumber);
        if(!isNumber)
            analNotNumber = true;
        else
        {
            if(analStrengthBig > 255)
                analNotRange = true;
        }
    }


    //Check All the bools, If They are false, data is good and return true.
    //If false, then make a Warning string, and Pop Up a Warning Message Box on What is Wrong and return false
    if(lgnIsEmpty == false && comPortNumEmpty == false && unusedName == false && analNotNumber == false && analNotRange == false && badDipIndex == false && hubComLG == false)
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


        QMessageBox::warning (this, "Can Not Add Light Gun", message);

        return false;
    }
    return false;
}

bool addLightGunWindow::IsDefaultLightGun()
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


void addLightGunWindow::AddLightGun()
{
    quint8 tempHub;

    //Collect Light Gun Name, Number, COM Port Number & Name, and Serial Port Info
    lightGunName = ui->lightGunNameLineEdit->text();
    lightGunNum = p_comDeviceList->GetNumberLightGuns ();

    if(defaultLightGun && defaultLightGunNum == MX24)
    {
        comPortNum = UNASSIGN;
        tempHub = ui->hubComComboBox->currentIndex ();
        comPortName = BEGINCOMPORTNAME+QString::number(tempHub);
    }
    else
    {
        comPortNum = ui->comPortComboBox->currentIndex ();    
        comPortName = BEGINCOMPORTNAME+QString::number(comPortNum);
    }
    p_comPortInfo = new QSerialPortInfo(comPortName);


    //Collect COM Port Settings: BAUD, Data Bits, Parity, Stop Bits, and Flow Control
    FindBaudRate();
    FindDataBits();
    FindParity();
    FindStopBits();
    FindFlow();

    //Set Player Dip Switch for MX24 Light Gun, since it is needed for commands
    if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        quint16 maxAmmo = REAPERMAXAMMONUM;
        quint16 reloadValue = REAPERRELOADNUM;

        //Create a New Reaper Light Gun Class
        p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, comPortNum, comPortName, *p_comPortInfo, comPortBaud, comPortDataBits, comPortParity, comPortStopBits, comPortFlow, maxAmmo, reloadValue);
    }
    else if(defaultLightGun && defaultLightGunNum == MX24)
    {
        quint8 tempDS = ui->dipSwitchComboBox->currentIndex ();

        tempHub = ui->hubComComboBox->currentIndex ();

        usedDipPlayers[tempDS] = true;

        //Create a New MX24 Light Gun Class
        p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, comPortNum, comPortName, *p_comPortInfo, comPortBaud, comPortDataBits, comPortParity, comPortStopBits, comPortFlow, true, tempDS, tempHub);
    }
    else if(defaultLightGun && (defaultLightGunNum == JBGUN4IR || defaultLightGunNum == OPENFIRE))
    {
        QString analString = ui->analogLineEdit->text();
        quint8 analStrength = analString.toUInt ();

        //Create a New Gun4IR Light Gun Class
        p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, comPortNum, comPortName, *p_comPortInfo, comPortBaud, comPortDataBits, comPortParity, comPortStopBits, comPortFlow, analStrength);
    }
    else
    {
        //Create a New Light Gun Class
        p_comDeviceList->AddLightGun(defaultLightGun, defaultLightGunNum, lightGunName, lightGunNum, comPortNum, comPortName, *p_comPortInfo, comPortBaud, comPortDataBits, comPortParity, comPortStopBits, comPortFlow);
    }


    //Since Data was Passed onto the COM Port Device List
    delete p_comPortInfo;
    p_comPortInfo = nullptr;
}

void addLightGunWindow::FindBaudRate()
{
    quint8 indexTemp;

    indexTemp = ui->baudSpeedComboBox->currentIndex();

    comPortBaud = BAUDDATA_ARRAY[indexTemp];
}

void addLightGunWindow::FindDataBits()
{
    quint8 indexTemp;

    indexTemp = ui->dataBitsComboBox->currentIndex();

    comPortDataBits = DATABITSDATA_ARRAY[indexTemp];
}

void addLightGunWindow::FindParity()
{
    quint8 indexTemp;

    indexTemp = ui->parityComboBox->currentIndex();

    comPortParity = PARITYDATA_ARRAY[indexTemp];
}

void addLightGunWindow::FindStopBits()
{
    quint8 indexTemp;

    indexTemp = ui->stopBitsComboBox->currentIndex();

    comPortStopBits = STOPBITSDATA_ARRAY[indexTemp];
}

void addLightGunWindow::FindFlow()
{
    quint8 indexTemp;

    indexTemp = ui->flowControlComboBox->currentIndex();

    comPortFlow = FLOWDATA_ARRAY[indexTemp];
}

void addLightGunWindow::FillSerialPortInfo(quint8 index)
{
    QString tempQS;
    QByteArray tempBA;

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

    //Fill in New Data
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

    //Clean Up Pointer
    delete p_comPortInfo;
    p_comPortInfo = nullptr;
}


void addLightGunWindow::SetEnableComboBoxes(bool enableCB)
{
    ui->baudSpeedComboBox->setEnabled (enableCB);
    ui->dataBitsComboBox->setEnabled (enableCB);
    ui->parityComboBox->setEnabled (enableCB);
    ui->stopBitsComboBox->setEnabled (enableCB);
    ui->flowControlComboBox->setEnabled (enableCB);
}







void addLightGunWindow::on_hubComComboBox_currentIndexChanged(int index)
{
    QString tempQS;
    p_comDeviceList->CopyUsedDipPlayersArray(usedDipPlayers, DIPSWITCH_NUMBER, index);

    for(quint8 comPortIndx=0;comPortIndx<DIPSWITCH_NUMBER;comPortIndx++)
    {
        if(usedDipPlayers[comPortIndx])
            tempQS = "";
        else
            tempQS = "P"+QString::number(comPortIndx+1);
        ui->dipSwitchComboBox->setItemText(comPortIndx,tempQS);
    }


    FillSerialPortInfo(index);
}

