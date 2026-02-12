#include "testLightController.h"
#include "Windows/ui_testLightController.h"
//#include "ui_testLightController.h"

testLightController::testLightController(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::testLightController)
{
    ui->setupUi(this);

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports
    p_comDeviceList = cdList;

    numberLightCntlrs = p_comDeviceList->GetNumberLightControllers ();

    regularLights = p_comDeviceList->p_lightCntlrList[0]->IsRegularGroups ();
    rgbLights = p_comDeviceList->p_lightCntlrList[0]->IsRGBGroups ();

    //Do Masks on Line Edits
    ui->timeOnLineEdit->setInputMask(LIGHTCNTLRTIMEMASK);
    ui->timeOffLineEdit->setInputMask(LIGHTCNTLRTIMEMASK);
    ui->timeDelayLineEdit->setInputMask(LIGHTCNTLRTIMEMASK);

    //Add Light Controllers to the ComboBox
    quint8 i;
    for(i = 0; i < numberLightCntlrs; i++)
    {
        quint8 templightCntlrNumber = p_comDeviceList->p_lightCntlrList[i]->GetLightCntlrNumber();
        dataUltimarc[i] = p_comDeviceList->p_lightCntlrList[i]->GetUltimarcData();

        QString controllerInfo = QString::number(templightCntlrNumber) + " - " + "ID: "+QString::number(dataUltimarc[i].id)+ " " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName;

        ui->lightCntlrComboBox->insertItem(i, controllerInfo);
    }

    cntlrsNumber = 0;

    ui->lightCntlrComboBox->setCurrentIndex (0);
}

testLightController::~testLightController()
{
    delete ui;
}


void testLightController::LoadCommands()
{
    QString tempCmd;
    numberCommands = 0;

    ui->commandsComboBox->clear ();

    if(regularLights)
    {
        tempCmd = FLASHREG;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;

        tempCmd = RANDOMFLASHREG;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;

        tempCmd = SEQUENCEREG;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;
    }

    if(rgbLights)
    {
        tempCmd = FLASHRGB;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;

        tempCmd = RANDOMFLASHRGB;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;

        tempCmd = RANDOMFLASHRGB2C;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;

        tempCmd = RANDOMFLASHRGBCM;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;

        tempCmd = SEQUENCERGB;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;

        tempCmd = SEQUENCERGBCM;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;
    }
}

void testLightController::SetUpArguments(quint8 index)
{
    if(regularLights && rgbLights)
    {
        if(index == 0)
            SetUpRegularFlash();
        else if(index == 1)
            SetUpRegularFlash();
        else if(index == 2)
            SetUpRegularSequence();
        else if(index == 3)
            SetUpRGBFlash();
        else if(index == 4)
            SetUpRGBFlash();
        else if(index == 5)
            SetUpRGBFlash2C();
        else if(index == 6)
            SetUpRGBFlashCM();
        else if(index == 7)
            SetUpRGBSequence();
        else if(index == 8)
            SetUpRGBSequenceCM();
    }
    else if(regularLights)
    {
        if(index == 0)
            SetUpRegularFlash();
        else if(index == 1)
            SetUpRegularFlash();
        else if(index == 2)
            SetUpRegularSequence();
    }
    else if(rgbLights)
    {
        if(index == 0)
            SetUpRGBFlash();
        else if(index == 1)
            SetUpRGBFlash();
        else if(index == 2)
            SetUpRGBFlash2C();
        else if(index == 3)
            SetUpRGBFlashCM();
        else if(index == 4)
            SetUpRGBSequence();
        else if(index == 5)
            SetUpRGBSequenceCM();
    }
}


void testLightController::SetUpRegularFlash()
{
    ui->timeOnLineEdit->setEnabled (true);
    ui->timeOffLineEdit->setEnabled (true);
    ui->numFlashesLineEdit->setEnabled (true);

    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    //ui->colorLineEdit->clear();
    ui->colorLineEdit->setEnabled (false);

    //ui->sideColorLineEdit->clear();
    ui->sideColorLineEdit->setEnabled (false);

    //ui->colorMapLineEdit->clear();
    ui->colorMapLineEdit->setEnabled (false);
}

void testLightController::SetUpRegularSequence()
{
    ui->timeDelayLineEdit->setEnabled (true);

    //ui->timeOnLineEdit->clear();
    ui->timeOnLineEdit->setEnabled (false);

    //ui->timeOffLineEdit->clear();
    ui->timeOffLineEdit->setEnabled (false);

    //ui->numFlashesLineEdit->clear();
    ui->numFlashesLineEdit->setEnabled (false);

    //ui->colorLineEdit->clear();
    ui->colorLineEdit->setEnabled (false);

    //ui->sideColorLineEdit->clear();
    ui->sideColorLineEdit->setEnabled (false);

    //ui->colorMapLineEdit->clear();
    ui->colorMapLineEdit->setEnabled (false);
}

void testLightController::SetUpRGBFlash()
{
    ui->timeOnLineEdit->setEnabled (true);
    ui->timeOffLineEdit->setEnabled (true);
    ui->numFlashesLineEdit->setEnabled (true);

    ui->colorLineEdit->setEnabled (true);


    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    //ui->sideColorLineEdit->clear();
    ui->sideColorLineEdit->setEnabled (false);

    //ui->colorMapLineEdit->clear();
    ui->colorMapLineEdit->setEnabled (false);
}

void testLightController::SetUpRGBFlash2C()
{
    ui->timeOnLineEdit->setEnabled (true);
    ui->timeOffLineEdit->setEnabled (true);
    ui->numFlashesLineEdit->setEnabled (true);

    ui->colorLineEdit->setEnabled (true);
    ui->sideColorLineEdit->setEnabled (true);


    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    //ui->colorMapLineEdit->clear();
    ui->colorMapLineEdit->setEnabled (false);
}

void testLightController::SetUpRGBFlashCM()
{
    ui->timeOnLineEdit->setEnabled (true);
    ui->timeOffLineEdit->setEnabled (true);
    ui->numFlashesLineEdit->setEnabled (true);
    ui->colorMapLineEdit->setEnabled (true);


    //ui->colorLineEdit->clear();
    ui->colorLineEdit->setEnabled (false);

    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    //ui->sideColorLineEdit->clear();
    ui->sideColorLineEdit->setEnabled (false);
}

void testLightController::SetUpRGBSequence()
{
    ui->timeDelayLineEdit->setEnabled (true);
    ui->colorLineEdit->setEnabled (true);

    //ui->timeOnLineEdit->clear();
    ui->timeOnLineEdit->setEnabled (false);

    //ui->timeOffLineEdit->clear();
    ui->timeOffLineEdit->setEnabled (false);

    //ui->numFlashesLineEdit->clear();
    ui->numFlashesLineEdit->setEnabled (false);

    //ui->sideColorLineEdit->clear();
    ui->sideColorLineEdit->setEnabled (false);

    //ui->colorMapLineEdit->clear();
    ui->colorMapLineEdit->setEnabled (false);
}

void testLightController::SetUpRGBSequenceCM()
{
    ui->timeDelayLineEdit->setEnabled (true);
    ui->colorMapLineEdit->setEnabled (true);

    //ui->timeOnLineEdit->clear();
    ui->timeOnLineEdit->setEnabled (false);

    //ui->timeOffLineEdit->clear();
    ui->timeOffLineEdit->setEnabled (false);

    //ui->numFlashesLineEdit->clear();
    ui->numFlashesLineEdit->setEnabled (false);

    //ui->colorLineEdit->clear();
    ui->colorLineEdit->setEnabled (false);

    //ui->sideColorLineEdit->clear();
    ui->sideColorLineEdit->setEnabled (false);
}

bool testLightController::GetTimeOn()
{
    bool isNumber;
    QString tempTimeOn = ui->timeOnLineEdit->text ();

    quint16 tempOn = tempTimeOn.toUInt (&isNumber);

    if(!isNumber)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time On is not a number.\nTime On: "+tempTimeOn;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempOn == 0)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time On cannot be 0. Needs to be 1-65,535.\nTime On: "+tempTimeOn;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    timeOn = tempOn;

    return true;
}

bool testLightController::GetTimeOff()
{
    bool isNumber;
    QString tempTimeOff = ui->timeOffLineEdit->text ();

    quint16 tempOff = tempTimeOff.toUInt (&isNumber);

    if(!isNumber)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time Off is not a number.\nTime Off: "+tempTimeOff;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempOff == 0)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time Off cannot be 0. Needs to be 1-65,535.\nTime Off: "+tempTimeOff;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    timeOff = tempOff;

    return true;
}

bool testLightController::GetNumberFlashes()
{
    bool isNumber;
    QString tempFlashes = ui->numFlashesLineEdit->text ();

    quint16 tempFlash = tempFlashes.toUInt (&isNumber);

    if(!isNumber)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Number of Flashes is not a number.\nNumber of Flashes: "+tempFlashes;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempFlash == 0 || tempFlash > 255)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Number of Flashes is out of range. Needs to be 1-255.\nNumber of Flashes: "+tempFlashes;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    numberFlashes = tempFlash;
    return true;
}


bool testLightController::GetTimeDelay()
{
    bool isNumber;
    QString tempTimeDelay = ui->timeDelayLineEdit->text ();

    quint16 tempDelay = tempTimeDelay.toUInt (&isNumber);

    if(!isNumber)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time Delay is not a number.\nTime Delay: "+tempTimeDelay;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempDelay == 0)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time Delay cannot be 0. Needs to be 1-65,535.\nTime Delay: "+tempTimeDelay;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    timeDelay = tempDelay;

    return true;
}

bool testLightController::CheckColor()
{
    QString tempColor = ui->colorLineEdit->text ();

    bool colorChk = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->CheckColor (tempColor);

    if(!colorChk)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Color is not found in the light controller\nColor: "+tempColor;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    color = tempColor;
    return true;
}

bool testLightController::CheckSideColor()
{
    QString tempColor = ui->sideColorLineEdit->text ();

    bool colorChk = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->CheckColor (tempColor);

    if(!colorChk)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Side Color is not found in the light controller\nSide Color: "+tempColor;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    sideColor = tempColor;
    return true;
}

bool testLightController::CheckColorMap()
{
    QString tempColor = ui->colorMapLineEdit->text ();

    bool colorChk = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->CheckColorMap (tempColor);

    if(!colorChk)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Color Map is not found in the light controller\nColor Map: "+tempColor;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    colorMap = tempColor;
    return true;
}


bool testLightController::GetGroups()
{
    QString tempGroups = ui->groupsLineEdit->text ();

    //Split Up Line Data with a Space
    QStringList splitData = tempGroups.split(' ', Qt::SkipEmptyParts);

    quint8 i;
    bool isNumber, isRGB;
    groups.clear();

    for(i = 0; i < splitData.length(); i++)
    {
        quint8 tempGrp = splitData[i].toUInt(&isNumber);

        if(!isNumber)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Group Number is not a number.\nGroup Number: "+splitData[i];
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }

        groups << tempGrp;
    }

    if(regularLights && rgbLights)
    {
        if(commandIndex >= 0 && commandIndex <= 2)
            isRGB = false;
        else
            isRGB = true;
    }
    else if(regularLights)
        isRGB = false;
    else if(rgbLights)
        isRGB = true;

    for(i = 0; i < groups.count(); i++)
    {
        bool chkGrp;

        if(isRGB)
            chkGrp = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->CheckRGBGroupNumber (groups[i]);
        else
            chkGrp = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->CheckRegularGroupNumber (groups[i]);

        if(!chkGrp)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Group number doesn't exist in the light controller.\nFailing Group Number: "+QString::number(groups[i])+"\nFailing Controller Number: "+QString::number(cntlrsNumber);
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }
    }


    return true;
}






void testLightController::on_lightCntlrComboBox_currentIndexChanged(int index)
{
    //labelCntlrNum

    cntlrsNumber = index;

    QString newLabel = "!"+QString::number(index);

    ui->labelCntlrNum->clear();

    ui->labelCntlrNum->setText (newLabel);

    regularLights = p_comDeviceList->p_lightCntlrList[index]->IsRegularGroups ();
    rgbLights = p_comDeviceList->p_lightCntlrList[index]->IsRGBGroups ();


    LoadCommands();
}


void testLightController::on_commandsComboBox_currentIndexChanged(int index)
{
    commandIndex = index;

    //Set Up the Arguments
    SetUpArguments(index);

}


void testLightController::on_pushButton_clicked()
{
    bool chkArg;

    if(regularLights && rgbLights)
    {
        if(commandIndex == 0 || commandIndex == 1)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeOn();

            if(!chkArg)
                return;

            chkArg = GetTimeOff();

            if(!chkArg)
                return;

            chkArg = GetNumberFlashes();

            if(!chkArg)
                return;

            if(commandIndex == 0)
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRegularLights(groups, timeOn, timeOff, numberFlashes);
            else
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRandomRegularLights(groups, timeOn, timeOff, numberFlashes);
        }
        else if(commandIndex == 2)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeDelay();

            if(!chkArg)
                return;

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SequenceRegularLights(groups, timeDelay);
        }
        else if(commandIndex == 3 || commandIndex == 4)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeOn();

            if(!chkArg)
                return;

            chkArg = GetTimeOff();

            if(!chkArg)
                return;

            chkArg = GetNumberFlashes();

            if(!chkArg)
                return;

            chkArg = CheckColor();

            if(!chkArg)
                return;

            if(commandIndex == 3)
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRGBLights(groups, timeOn, timeOff, numberFlashes, color);
            else
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRandomRGBLights(groups, timeOn, timeOff, numberFlashes, color);
        }
        else if(commandIndex == 5)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeOn();

            if(!chkArg)
                return;

            chkArg = GetTimeOff();

            if(!chkArg)
                return;

            chkArg = GetNumberFlashes();

            if(!chkArg)
                return;

            chkArg = CheckColor();

            if(!chkArg)
                return;

            chkArg = CheckSideColor();

            if(!chkArg)
                return;

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRandomRGB2CLights(groups, timeOn, timeOff, numberFlashes, color, sideColor);
        }
        else if(commandIndex == 6)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeOn();

            if(!chkArg)
                return;

            chkArg = GetTimeOff();

            if(!chkArg)
                return;

            chkArg = GetNumberFlashes();

            if(!chkArg)
                return;

            chkArg = CheckColorMap();

            if(!chkArg)
                return;

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRandomRGBLightsCM(groups, timeOn, timeOff, numberFlashes, colorMap);
        }
        else if(commandIndex == 7)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeDelay();

            if(!chkArg)
                return;

            chkArg = CheckColor();

            if(!chkArg)
                return;

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SequenceRGBLights(groups, timeDelay, color);
        }
        else if(commandIndex == 8)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeDelay();

            if(!chkArg)
                return;

            chkArg = CheckColorMap();

            if(!chkArg)
                return;

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SequenceRGBLightsCM(groups, timeDelay, colorMap);
        }
    }
    else if(regularLights)
    {
        if(commandIndex == 0 || commandIndex == 1)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeOn();

            if(!chkArg)
                return;

            chkArg = GetTimeOff();

            if(!chkArg)
                return;

            chkArg = GetNumberFlashes();

            if(!chkArg)
                return;

            if(commandIndex == 0)
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRegularLights(groups, timeOn, timeOff, numberFlashes);
            else
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRandomRegularLights(groups, timeOn, timeOff, numberFlashes);
        }
        else if(commandIndex == 2)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeDelay();

            if(!chkArg)
                return;

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SequenceRegularLights(groups, timeDelay);
        }
    }
    else if(rgbLights)
    {
        if(commandIndex == 0 || commandIndex == 1)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeOn();

            if(!chkArg)
                return;

            chkArg = GetTimeOff();

            if(!chkArg)
                return;

            chkArg = GetNumberFlashes();

            if(!chkArg)
                return;

            chkArg = CheckColor();

            if(!chkArg)
                return;

            if(commandIndex == 0)
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRGBLights(groups, timeOn, timeOff, numberFlashes, color);
            else
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRandomRGBLights(groups, timeOn, timeOff, numberFlashes, color);
        }
        else if(commandIndex == 2)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeOn();

            if(!chkArg)
                return;

            chkArg = GetTimeOff();

            if(!chkArg)
                return;

            chkArg = GetNumberFlashes();

            if(!chkArg)
                return;

            chkArg = CheckColor();

            if(!chkArg)
                return;

            chkArg = CheckSideColor();

            if(!chkArg)
                return;

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRandomRGB2CLights(groups, timeOn, timeOff, numberFlashes, color, sideColor);
        }
        else if(commandIndex == 3)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeOn();

            if(!chkArg)
                return;

            chkArg = GetTimeOff();

            if(!chkArg)
                return;

            chkArg = GetNumberFlashes();

            if(!chkArg)
                return;

            chkArg = CheckColorMap();

            if(!chkArg)
                return;

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRandomRGBLightsCM(groups, timeOn, timeOff, numberFlashes, colorMap);
        }
        else if(commandIndex == 4)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeDelay();

            if(!chkArg)
                return;

            chkArg = CheckColor();

            if(!chkArg)
                return;

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SequenceRGBLights(groups, timeDelay, color);
        }
        else if(commandIndex == 5)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            chkArg = GetTimeDelay();

            if(!chkArg)
                return;

            chkArg = CheckColorMap();

            if(!chkArg)
                return;

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SequenceRGBLightsCM(groups, timeDelay, colorMap);
        }
    }
}

