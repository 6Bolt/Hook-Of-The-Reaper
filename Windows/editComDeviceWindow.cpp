#include "editComDeviceWindow.h"
#include "ui_editComDeviceWindow.h"

//Constructor
editComDeviceWindow::editComDeviceWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::editComDeviceWindow)
{
    QString tempQS;
    quint8 i;

    //Setup the Window Functions, like Combo Box
    ui->setupUi(this);

    //Setup the Window Functions, like Combo Box
    this->setFixedSize(this->size());

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports
    p_comDeviceList = cdList;
    p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);

    //Load First Combo Box with the Saved COM Devices; Number, Name, and COM Port
    numberComDevices = p_comDeviceList->GetNumberComPortDevices ();
    for(i = 0; i<numberComDevices; i++)
    {
        tempQS = QString::number(i+1);
        tempQS.append (": ");
        tempQS.append (p_comDeviceList->p_comPortDeviceList[i]->GetComPortDeviceName());
        tempQS.append (" on ");
        tempQS.append (p_comDeviceList->p_comPortDeviceList[i]->GetComPortString());
        ui->savedComDevicesComboBox->insertItem(i,tempQS);
    }

    //Put the Name of the First COM Device into the COM Device Name Line Edit
    comDeviceName = p_comDeviceList->p_comPortDeviceList[0]->GetComPortDeviceName ();
    ui->comDeviceNameLineEdit->setText (comDeviceName);

    //COM Port Combo Box - List Unused COM Port and Select COM Device COM Port
    comPortName = p_comDeviceList->p_comPortDeviceList[0]->GetComPortString();
    comPortNum = p_comDeviceList->p_comPortDeviceList[0]->GetComPortNumber();
    unusedComPort[comPortNum] = true;

    for(quint8 comPortIndx=0;comPortIndx<MAXCOMPORTS;comPortIndx++)
    {
        if(unusedComPort[comPortIndx])
            tempQS = BEGINCOMPORTNAME+QString::number(comPortIndx);
        else
            tempQS = "";
        ui->comPortComboBox->insertItem(comPortIndx,tempQS);

    }
    ui->comPortComboBox->setCurrentIndex(comPortNum);
    FillSerialPortInfo(comPortNum);


    comPortBaud = p_comDeviceList->p_comPortDeviceList[0]->GetComPortBaud ();

    //Baud Speed Combo Box - Adding in Speed Rates
    for(quint8 comPortIndx=0;comPortIndx<BAUD_NUMBER;comPortIndx++)
    {
        ui->baudSpeedComboBox->insertItem(comPortIndx,BAUDNAME_ARRAY[comPortIndx]);
        if(comPortBaud == BAUDDATA_ARRAY[comPortIndx])
            baudIndex = comPortIndx;
    }
    ui->baudSpeedComboBox->setCurrentIndex(baudIndex);


    comPortDataBits = p_comDeviceList->p_comPortDeviceList[0]->GetComPortDataBits ();

    //Data Bits Combo Box - Adding in Data Bit Options
    for(quint8 comPortIndx=0;comPortIndx<DATABITS_NUMBER;comPortIndx++)
    {
        ui->dataBitsComboBox->insertItem(comPortIndx,DATABITSNAME_ARRAY[comPortIndx]);
        if(comPortDataBits == DATABITSDATA_ARRAY[comPortIndx])
            dataBitsIndex = comPortIndx;
    }
    ui->dataBitsComboBox->setCurrentIndex(dataBitsIndex);


    comPortParity = p_comDeviceList->p_comPortDeviceList[0]->GetComPortParity ();

    //Parity Combo Box - Adding Parity Options
    for(quint8 comPortIndx=0;comPortIndx<PARITY_NUMBER;comPortIndx++)
    {
        ui->parityComboBox->insertItem(comPortIndx,PARITYNAME_ARRAY[comPortIndx]);
        if(comPortParity == PARITYDATA_ARRAY[comPortIndx])
            parityIndex = comPortIndx;
    }
    ui->parityComboBox->setCurrentIndex(parityIndex);


    comPortStopBits = p_comDeviceList->p_comPortDeviceList[0]->GetComPortStopBits ();

    //Stop Bits Combo Box - Adding in Stop Bit Options
    for(quint8 comPortIndx=0;comPortIndx<STOPBITS_NUMBER;comPortIndx++)
    {
        ui->stopBitsComboBox->insertItem(comPortIndx,STOPBITSNAME_ARRAY[comPortIndx]);
        if(comPortStopBits == STOPBITSDATA_ARRAY[comPortIndx])
            stopBitsIndex = comPortIndx;
    }
    ui->stopBitsComboBox->setCurrentIndex(stopBitsIndex);


    comPortFlow = p_comDeviceList->p_comPortDeviceList[0]->GetComPortFlow ();

    //Flow Control Combo Box - Adding in Flow Options
    for(quint8 comPortIndx=0;comPortIndx<FLOW_NUMBER;comPortIndx++)
    {
        ui->flowControlComboBox->insertItem(comPortIndx,FLOWNAME_ARRAY[comPortIndx]);
        if(comPortFlow == FLOWDATA_ARRAY[comPortIndx])
            flowIndex = comPortIndx;
    }
    ui->flowControlComboBox->setCurrentIndex(flowIndex);
}

//Deconstructor
editComDeviceWindow::~editComDeviceWindow()
{
    delete ui;
}

//private slots

void editComDeviceWindow::on_savedComDevicesComboBox_currentIndexChanged(int index)
{
    LoadSavedComDevice(index);
}


void editComDeviceWindow::on_comPortComboBox_currentIndexChanged(int index)
{
    FillSerialPortInfo(index);
}


void editComDeviceWindow::on_deletePushButton_clicked()
{
    quint8 dcdIndex = ui->savedComDevicesComboBox->currentIndex ();
    quint8 i;
    QString tempQS;

    p_comDeviceList->DeleteComDevice (dcdIndex);

    //Load First Combo Box with the Saved COM Devices; Number, Name, and COM Port
    numberComDevices = p_comDeviceList->GetNumberComPortDevices();

    //If No COM Devices are in the List After Deletion of a COM Device, Then Close the Edit Window
    if(numberComDevices == 0)
    {
        accept ();
    }
    else
    {
        for(i = 0; i<numberComDevices; i++)
        {
            tempQS = QString::number(i+1);
            tempQS.append (": ");
            tempQS.append (p_comDeviceList->p_comPortDeviceList[i]->GetComPortDeviceName());
            tempQS.append (" on ");
            tempQS.append (p_comDeviceList->p_comPortDeviceList[i]->GetComPortString());
            ui->savedComDevicesComboBox->setItemText(i,tempQS);
        }
        ui->savedComDevicesComboBox->removeItem (numberComDevices);
        ui->savedComDevicesComboBox->setCurrentIndex (0);

        //Rebuild COM Port Combo Box, Now with the Used COM port Removed
        p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);
        comPortNum = p_comDeviceList->p_comPortDeviceList[0]->GetComPortNumber();
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
        ui->comPortComboBox->setCurrentIndex(comPortNum);

        LoadSavedComDevice(0);
    }
}


void editComDeviceWindow::on_editPushButton_clicked()
{
    bool dataValid = false;
    QString tempQS;

    //Check Input Value of the New COM Device Data
    dataValid = IsValidData();

    if(dataValid)
    {
        //Edit COM Device in the ComDeviceList
        EditComDevice();

        //Rebuild COM Port Combo Box, Now with the Used COM port Removed
        p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);
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
        ui->comPortComboBox->setCurrentIndex(comPortNum);

        FillSerialPortInfo(comPortNum);

        tempQS = QString::number(comDeviceNum+1);
        tempQS.append (": ");
        tempQS.append (comDeviceName);
        tempQS.append (" on ");
        tempQS.append (comPortName);
        ui->savedComDevicesComboBox->setItemText (comDeviceNum,tempQS);
    }
}


void editComDeviceWindow::on_okPushButton_clicked()
{
    bool dataValid = false;

    //Check Input Value of the New COM Device Data
    dataValid = IsValidData();

    if(dataValid)
    {
        //Add New COM Device to ComDeviceList
        EditComDevice();
        //Close and Destroy the Window
        accept ();
    }
}


void editComDeviceWindow::on_cancelPushButton_clicked()
{
    //Close and Destroy the Window
    this->accept ();
}




//private


bool editComDeviceWindow::IsValidData()
{
    QString ivTemp, message;
    bool lgnIsEmpty = false;
    bool comPortNumEmpty = false;
    bool maLEEmpty = false;
    bool rvLEEmpty = false;
    bool unusedName = false;
    quint8 numComDevices = p_comDeviceList->GetNumberComPortDevices();
    quint8 i;
    quint8 currentIndex = ui->savedComDevicesComboBox->currentIndex ();

    //Check If COM Device Name Line Edit Box is Empty
    ivTemp = ui->comDeviceNameLineEdit->text();
    if(ivTemp.isEmpty ())
        lgnIsEmpty = true;

    if(numComDevices > 0)
    {
        for(i = 0; i < numComDevices; i++)
        {
            //Don't Compare its Own Name
            if(i != currentIndex)
            {
                if(ivTemp == p_comDeviceList->p_comPortDeviceList[i]->GetComPortDeviceName ())
                    unusedName = true;
            }
        }
    }


    //Check If the Com Port Name is Empty for the Combo Box
    ivTemp = ui->comPortComboBox->currentText ();
    if(ivTemp.isEmpty ())
        comPortNumEmpty = true;


    //Check All the bools, If They are false, data is good and return true.
    //If false, then make a Warning string, and Pop Up a Warning Message Box on What is Wrong and return false
    if(lgnIsEmpty == false && comPortNumEmpty == false && unusedName == false)
    {
        return true;
    }
    else
    {
        if(lgnIsEmpty == true)
            message.append ("COM Device Name is empty. Please enter a name for the COM Device. ");
        if(comPortNumEmpty == true)
            message.append ("COM Port is empty. Please select a valid COM Port. ");
        if(unusedName == true)
            message.append ("COM device name is already used. Please pick a different name.");

        QMessageBox::warning (this, "Can Not Edit COM Device", message);

        return false;
    }
    return false;
}

void editComDeviceWindow::EditComDevice()
{
    quint8 oldComPort;

    //Collect COM Device Name, Number, COM Port Number & Name, and Serial Port Info
    comDeviceNum = ui->savedComDevicesComboBox->currentIndex ();
    comDeviceName = ui->comDeviceNameLineEdit->text();
    comPortNum = ui->comPortComboBox->currentIndex ();
    comPortName = BEGINCOMPORTNAME+QString::number(comPortNum);
    p_comPortInfo = new QSerialPortInfo(comPortName);

    //Check if COM Port Changed, if so update unusedComPorts list
    oldComPort = p_comDeviceList->p_comPortDeviceList[comDeviceNum]->GetComPortNumber ();
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

    //Edit COM Device
    p_comDeviceList->p_comPortDeviceList[comDeviceNum]->SetComPortDeviceName (comDeviceName);
    p_comDeviceList->p_comPortDeviceList[comDeviceNum]->SetComPortNumber (comPortNum);
    p_comDeviceList->p_comPortDeviceList[comDeviceNum]->SetComPortString (comPortName);
    p_comDeviceList->p_comPortDeviceList[comDeviceNum]->SetComPortInfo (*p_comPortInfo);
    p_comDeviceList->p_comPortDeviceList[comDeviceNum]->SetComPortBaud (comPortBaud);
    p_comDeviceList->p_comPortDeviceList[comDeviceNum]->SetComPortDataBits (comPortDataBits);
    p_comDeviceList->p_comPortDeviceList[comDeviceNum]->SetComPortParity (comPortParity);
    p_comDeviceList->p_comPortDeviceList[comDeviceNum]->SetComPortStopBits (comPortStopBits);
    p_comDeviceList->p_comPortDeviceList[comDeviceNum]->SetComPortFlow (comPortFlow);

    delete p_comPortInfo;
    p_comPortInfo = nullptr;

    //p_comDeviceList->p_comPortDeviceList[comDeviceNum]->PrintSerialPortInfo ();

}

void editComDeviceWindow::FindBaudRate()
{
    quint8 indexTemp;

    indexTemp = ui->baudSpeedComboBox->currentIndex();

    comPortBaud = BAUDDATA_ARRAY[indexTemp];
}

void editComDeviceWindow::FindDataBits()
{
    quint8 indexTemp;

    indexTemp = ui->dataBitsComboBox->currentIndex();

    comPortDataBits = DATABITSDATA_ARRAY[indexTemp];
}

void editComDeviceWindow::FindParity()
{
    quint8 indexTemp;

    indexTemp = ui->parityComboBox->currentIndex();

    comPortParity = PARITYDATA_ARRAY[indexTemp];
}

void editComDeviceWindow::FindStopBits()
{
    quint8 indexTemp;

    indexTemp = ui->stopBitsComboBox->currentIndex();

    comPortStopBits = STOPBITSDATA_ARRAY[indexTemp];
}

void editComDeviceWindow::FindFlow()
{
    quint8 indexTemp;

    indexTemp = ui->flowControlComboBox->currentIndex();

    comPortFlow = FLOWDATA_ARRAY[indexTemp];
}


void editComDeviceWindow::FillSerialPortInfo(quint8 index)
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

void editComDeviceWindow::LoadSavedComDevice(quint8 index)
{
    QString tempQS;

    //Move Over a Clean Copy of the Unused COM Port Array
    p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);

    //Put the Name of the Selected COM Device into the COM Device Name Line Edit
    comDeviceName = p_comDeviceList->p_comPortDeviceList[index]->GetComPortDeviceName ();
    ui->comDeviceNameLineEdit->setText (comDeviceName);


    //COM Port Combo Box - List Unused COM Port and Select COM Device COM Port
    comPortName = p_comDeviceList->p_comPortDeviceList[index]->GetComPortString();
    comPortNum = p_comDeviceList->p_comPortDeviceList[index]->GetComPortNumber();
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

    comPortBaud = p_comDeviceList->p_comPortDeviceList[index]->GetComPortBaud ();

    //Baud Speed Combo Box - Adding in Speed Rates
    for(quint8 comPortIndx=0;comPortIndx<BAUD_NUMBER;comPortIndx++)
    {
        if(comPortBaud == BAUDDATA_ARRAY[comPortIndx])
            baudIndex = comPortIndx;
    }
    ui->baudSpeedComboBox->setCurrentIndex(baudIndex);

    comPortDataBits = p_comDeviceList->p_comPortDeviceList[index]->GetComPortDataBits ();

    //Data Bits Combo Box - Adding in Data Bit Options
    for(quint8 comPortIndx=0;comPortIndx<DATABITS_NUMBER;comPortIndx++)
    {
        if(comPortDataBits == DATABITSDATA_ARRAY[comPortIndx])
            dataBitsIndex = comPortIndx;
    }
    ui->dataBitsComboBox->setCurrentIndex(dataBitsIndex);


    comPortParity = p_comDeviceList->p_comPortDeviceList[index]->GetComPortParity ();

    //Parity Combo Box - Adding Parity Options
    for(quint8 comPortIndx=0;comPortIndx<PARITY_NUMBER;comPortIndx++)
    {
        if(comPortParity == PARITYDATA_ARRAY[comPortIndx])
            parityIndex = comPortIndx;
    }
    ui->parityComboBox->setCurrentIndex(parityIndex);


    comPortStopBits = p_comDeviceList->p_comPortDeviceList[index]->GetComPortStopBits ();

    //Stop Bits Combo Box - Adding in Stop Bit Options
    for(quint8 comPortIndx=0;comPortIndx<STOPBITS_NUMBER;comPortIndx++)
    {
        if(comPortStopBits == STOPBITSDATA_ARRAY[comPortIndx])
            stopBitsIndex = comPortIndx;
    }
    ui->stopBitsComboBox->setCurrentIndex(stopBitsIndex);


    comPortFlow = p_comDeviceList->p_comPortDeviceList[index]->GetComPortFlow ();

    //Flow Control Combo Box - Adding in Flow Options
    for(quint8 comPortIndx=0;comPortIndx<FLOW_NUMBER;comPortIndx++)
    {
        if(comPortFlow == FLOWDATA_ARRAY[comPortIndx])
            flowIndex = comPortIndx;
    }
    ui->flowControlComboBox->setCurrentIndex(flowIndex);

}



