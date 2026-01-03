#include "addComDeviceWindow.h"
#include "ui_addComDeviceWindow.h"
#include "../Global.h"

//Constructor
addComDeviceWindow::addComDeviceWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addComDeviceWindow)
{
    QString tempQS;

    //Set Up This Windows
    ui->setupUi(this);

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports
    p_comDeviceList = cdList;
    p_comDeviceList->CopyAvailableComPortsArray(unusedComPort, MAXCOMPORTS);

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

    /*
    //Init Ultimarc Light Controllers
    PacInitialize();

    //bool setIntensity = Pac64SetLEDIntensity(0, 0, 0x0f);

    //qDebug() << "setIntensity" << setIntensity;


    bool setState = Pac64SetLEDState(0, 0, 0, true);

    qDebug() << "setState" << setState;

    setState = Pac64SetLEDState(0, 0, 1, true);

    qDebug() << "setState" << setState;

    setState = Pac64SetLEDState(0, 0, 2, true);

    qDebug() << "setState" << setState;
    */

    //bool setIntensity = Pac64SetLEDIntensity(0, 0, 0x0f);

    //qDebug() << "setIntensity" << setIntensity;

    //setIntensity = Pac64SetLEDIntensity(0, 1, 0x0f);

    //qDebug() << "setIntensity" << setIntensity;

    //bool setIntensity = Pac64SetLEDIntensity(0, 0, 0x0f);

    //qDebug() << "setIntensity" << setIntensity;


    //bool setStates = Pac64SetLEDStates(0, 0, 0x05);

    //qDebug() << "setStates" << setStates;

    //Pac64SetLEDIntensity(0, 2, 128);

    /*

    wchar_t buffer[256];

    PWCHAR ledString = buffer;

    PWCHAR ledString2 = new wchar_t[256];

    bool isNumber;

    int type = PacGetDeviceType(0);
    qDebug() << "Type" << type;

    int vendorID = PacGetVendorId(0);
    int productID = PacGetProductId(0);

    QString formattedHexVendor = QString("0x%1").arg(vendorID, 4, 16, QChar('0')).toUpper(); // "Value: 0x00FF"
    QString formattedHexProduct = QString("0x%1").arg(productID, 4, 16, QChar('0')).toUpper();

    qDebug() << "VendorID:" << formattedHexVendor;
    qDebug() << "ProductID:" << formattedHexProduct;
    qDebug() << "VersionNumber:" << PacGetVersionNumber(0);

    PacGetVendorName(0, ledString2);
    qDebug() << "VendorName:" << QString::fromWCharArray(ledString2);

    PacGetProductName(0, ledString);
    qDebug() << "ProductName:" << QString::fromWCharArray(ledString);

    PacGetSerialNumber(0, ledString);
    qDebug() << "SerialNumber:" << QString::fromWCharArray(ledString);

    PacGetDevicePath(0, ledString);
    qDebug() << "DevicePath:" << QString::fromWCharArray(ledString);



    std::array<std::uint8_t, 64> modernArray;
    std::uint8_t* pModernData = modernArray.data();

    // Initialize (e.g., set all bytes to 0xFF)
    std::fill(modernArray.begin(), modernArray.end(), 0x0F);

    bool intensites = Pac64SetLEDIntensities(0, pModernData);
    qDebug() << "intensites" << intensites;

    quint8 i, j, count;
    //j = 1;
    count = 1;

    for(i = 0; i < 32; i++)
    {
        j = (1 << count);

        Pac64SetLEDStates(0, 2, j);

        qDebug() << "data" << j;

        QThread::msleep(1000);

        count++;
        if(count > 3)
        {
            //j = 1;
            count = 1;
        }
    }

    */

}

//Deconstructor
addComDeviceWindow::~addComDeviceWindow()
{
    //Delete Window
    delete ui;
    //Delete temp Com Device pointer
    if(p_cpdTemp != nullptr)
        delete p_cpdTemp;
    //Delete temp ComPortDevice pointer
    if(p_comPortInfo != nullptr)
        delete p_comPortInfo;

    //bool setState = Pac64SetLEDState(0, 0, 1, false);

    //qDebug() << "setState" << setState;

    //bool setStates = Pac64SetLEDStates(0, 0, 0x00);

    //qDebug() << "setStates" << setStates;

    //Shut Down Ultimarc Light Controllers
    //PacShutdown();
}

//private slots

void addComDeviceWindow::on_addPushButton_clicked()
{
    bool dataValid = false;
    QString tempQS;

    //Check Input Value of the New COM Device Data
    dataValid = IsValidData();

    if(dataValid)
    {
        //Add New COM Device to ComDeviceList
        AddComPortDevice();

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
    }
}


void addComDeviceWindow::on_okPushButton_clicked()
{
    bool dataValid = false;

    //Check Input Value of the New COM Device Data
    dataValid = IsValidData();

    if(dataValid)
    {
        //Add New COM Device to ComDeviceList
        AddComPortDevice();
        //Close and Destroy the Window
        accept ();
    }
}


void addComDeviceWindow::on_cancelPushButton_clicked()
{
    //Close and Destroy the Window
    this->accept ();
}

void addComDeviceWindow::on_comPortComboBox_currentIndexChanged(int index)
{
    FillSerialPortInfo(index);
}

//private

bool addComDeviceWindow::IsValidData()
{
    QString ivTemp, message;
    bool cdnIsEmpty = false;
    bool comPortNumEmpty = false;
    bool unusedName = false;
    quint8 numComDevices = p_comDeviceList->GetNumberComPortDevices();
    quint8 i;


    //Check If COM Device Name Line Edit Box is Empty
    ivTemp = ui->comDeviceNameLineEdit->text();
    if(ivTemp.isEmpty ())
        cdnIsEmpty = true;


    if(numComDevices > 0)
    {
        for(i = 0; i < numComDevices; i++)
        {
            if(ivTemp == p_comDeviceList->p_comPortDeviceList[i]->GetComPortDeviceName ())
                unusedName = true;
        }
    }


    //Check If the Com Port Name is Empty for the Combo Box
    ivTemp = ui->comPortComboBox->currentText ();
    if(ivTemp.isEmpty ())
        comPortNumEmpty = true;


    //Check All the bools, If They are false, data is good and return true.
    //If false, then make a Warning string, and Pop Up a Warning Message Box on What is Wrong and return false
    if(cdnIsEmpty == false && comPortNumEmpty == false && unusedName == false)
    {
        return true;
    }
    else
    {
        if(cdnIsEmpty == true)
            message.append ("COM Device Name is empty. Please enter a name for the COM Device Name. ");
        if(comPortNumEmpty == true)
            message.append ("COM Port is empty. Please select a valid COM Port. ");
        if(unusedName == true)
            message.append ("COM device name is already used. Please pick a different name.");

        QMessageBox::warning (this, "Can Not Add COM Device", message);

        return false;
    }
    return false;
}

void addComDeviceWindow::AddComPortDevice()
{
    //Collect COM Device Name, Number, COM Port Number & Name, and Serial Port Info
    comPortDeviceName = ui->comDeviceNameLineEdit->text();
    comPortDeviceNum = p_comDeviceList->GetNumberComPortDevices ();
    comPortNum = ui->comPortComboBox->currentIndex ();
    comPortName = BEGINCOMPORTNAME+QString::number(comPortNum);
    p_comPortInfo = new QSerialPortInfo(comPortName);

    //Collect COM Port Settings: BAUD, Data Bits, Parity, Stop Bits, and Flow Control
    FindBaudRate();
    FindDataBits();
    FindParity();
    FindStopBits();
    FindFlow();

    //If COM Device is a RS3 Reaper, then Collect Max Ammo and Reload Value
    //Make a New ComPortDevice class
    p_cpdTemp = new ComPortDevice(comPortDeviceName, comPortDeviceNum, comPortNum, comPortName, *p_comPortInfo, comPortBaud, comPortDataBits, comPortParity, comPortStopBits, comPortFlow);


    //Copy the New Com Device to the ComDeviceList
    p_comDeviceList->AddComPortDevice (*p_cpdTemp);

    //Delete pointer, since it has been copied over. Then set it to nullptr
    delete p_cpdTemp;
    p_cpdTemp = nullptr;
}

void addComDeviceWindow::FindBaudRate()
{
    quint8 indexTemp;

    indexTemp = ui->baudSpeedComboBox->currentIndex();

    comPortBaud = BAUDDATA_ARRAY[indexTemp];
}

void addComDeviceWindow::FindDataBits()
{
    quint8 indexTemp;

    indexTemp = ui->dataBitsComboBox->currentIndex();

    comPortDataBits = DATABITSDATA_ARRAY[indexTemp];
}

void addComDeviceWindow::FindParity()
{
    quint8 indexTemp;

    indexTemp = ui->parityComboBox->currentIndex();

    comPortParity = PARITYDATA_ARRAY[indexTemp];
}

void addComDeviceWindow::FindStopBits()
{
    quint8 indexTemp;

    indexTemp = ui->stopBitsComboBox->currentIndex();

    comPortStopBits = STOPBITSDATA_ARRAY[indexTemp];
}

void addComDeviceWindow::FindFlow()
{
    quint8 indexTemp;

    indexTemp = ui->flowControlComboBox->currentIndex();

    comPortFlow = FLOWDATA_ARRAY[indexTemp];
}

void addComDeviceWindow::FillSerialPortInfo(quint8 index)
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


