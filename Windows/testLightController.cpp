#include "testLightController.h"
#include "Windows/ui_testLightController.h"
//#include "ui_testLightController.h"

#define REGULARLOW          0
#define REGULARHIGH         3

testLightController::testLightController(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::testLightController)
{
    ui->setupUi(this);

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports
    p_comDeviceList = cdList;

    isBGRunning = false;
    isBGRGB = false;
    isFirstController = true;
    isDRRunning = false;

    numberLightCntlrs = p_comDeviceList->GetNumberLightControllers ();

    numberSaveUltimarcCntlrs = p_comDeviceList->GetNumberUltimarcControllers();
    numberALEDStripCntlrs = p_comDeviceList->GetNumberALEDStripControllers ();

    if(numberSaveUltimarcCntlrs > 0)
        ultimarcPotitions = p_comDeviceList->GetUltimarcPotitions();

    if(numberALEDStripCntlrs > 0)
    {
        aledStripPotitions = p_comDeviceList->GetALEDPotitions();
        aledCntlrCOMNames = p_comDeviceList->GetALEDCOMNames();
    }


    //Fill Types Combo Box
    ui->typeComboBox->insertItem(0, ULTIMARCTYPE);
    ui->typeComboBox->insertItem(1, ALEDSTRIPTYPE);

    //Fill Probability Cobo Box 2-12 and Number of LEDs in Random Flash
    for(quint8 i = 0; i < 10; i++)
    {
        ui->propComboBox->insertItem(i, QString::number(i+2));

        ui->ledsFlashComboBox->insertItem(i, QString::number(i+1));
    }

    //10th Number of Probability 12
    ui->propComboBox->insertItem(10, QString::number(12));

    //Fill LEDs per Delay Combo Box
    ui->numLEDsComboBox->insertItem(0, "1");
    ui->numLEDsComboBox->insertItem(1, "2");
    ui->numLEDsComboBox->insertItem(2, "4");
    ui->numLEDsComboBox->insertItem(3, "8");


    //Do Masks on Line Edits
    ui->timeOnLineEdit->setInputMask(LIGHTCNTLRTIMEMASK);
    ui->timeOffLineEdit->setInputMask(LIGHTCNTLRTIMEMASK);
    ui->timeDelayLineEdit->setInputMask(LIGHTCNTLRTIMEMASK);
    ui->timeDelayBGLineEdit->setInputMask(LIGHTCNTLRTIMEMASK);
    ui->timeDelayBGRLineEdit->setInputMask(LIGHTCNTLRTIMEMASK);

    //Dis-enable the Background and Display Range Buttons
    ui->updateBG->setEnabled(false);
    ui->closeBG->setEnabled(false);


    if(numberSaveUltimarcCntlrs > 0)
        ui->typeComboBox->setCurrentIndex (ULTIMARC);
    else if(numberALEDStripCntlrs > 0)
        ui->typeComboBox->setCurrentIndex (HOTRALEDSTRIP);
    else
        accept();

}

testLightController::~testLightController()
{
    delete ui;

    if(isDRRunning)
        p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameEnded();

    //Turn Off Lights
    p_comDeviceList->p_lightCntlrList[cntlrsNumber]->TurnOffLights();
    p_comDeviceList->p_lightCntlrList[cntlrsNumber]->ResetLightController();
}


void testLightController::LoadCommands()
{
    //qDebug() << "Loading Commands for Test Light Controller";

    QString tempCmd;
    numberCommands = 0;

    ui->commandsComboBox->clear ();

    if(typeSet == ULTIMARC)
    {
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

            tempCmd = BACKGROUNDREG;
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

            tempCmd = SLASHRGB;
            ui->commandsComboBox->insertItem(numberCommands, tempCmd);
            numberCommands++;

            tempCmd = DOUBLESLASHRGB;
            ui->commandsComboBox->insertItem(numberCommands, tempCmd);
            numberCommands++;

            tempCmd = BACKGROUNDRGB;
            ui->commandsComboBox->insertItem(numberCommands, tempCmd);
            numberCommands++;
        }
    }
    else if(typeSet == HOTRALEDSTRIP)
    {
        tempCmd = FLASHALEDS;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;

        tempCmd = RANDOMFLASHALEDS;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;

        tempCmd = SEQUENCEALEDS;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;

        tempCmd = DISPLAYRANGEALEDS;
        ui->commandsComboBox->insertItem(numberCommands, tempCmd);
        numberCommands++;
    }

    ui->commandsComboBox->setCurrentIndex (0);
}

void testLightController::SetUpArguments(quint8 index)
{
    if(typeSet == ULTIMARC)
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
                SetUpRegularBackground();
            else if(index == 4)
                SetUpRGBFlash();
            else if(index == 5)
                SetUpRGBFlash();
            else if(index == 6)
                SetUpRGBFlash2C();
            else if(index == 7)
                SetUpRGBFlashCM();
            else if(index == 8)
                SetUpRGBSequence();
            else if(index == 9)
                SetUpRGBSequenceCM();
            else if(index == 10)
                SetUpRGBSlash();
            else if(index == 11)
                SetUpRGBDoubleSlash();
            else if(index == 12)
                SetUpRGBBackground();
        }
        else if(regularLights)
        {
            if(index == 0)
                SetUpRegularFlash();
            else if(index == 1)
                SetUpRegularFlash();
            else if(index == 2)
                SetUpRegularSequence();
            else if(index == 3)
                SetUpRegularBackground();
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
            else if(index == 6)
                SetUpRGBSlash();
            else if(index == 7)
                SetUpRGBDoubleSlash();
            else if(index == 8)
                SetUpRGBBackground();
        }
    }
    else if(typeSet == HOTRALEDSTRIP)
    {
        if(index != 3)
        {
            if(isDRRunning)
                CloseDisplayRange();
        }

        if(index == 0)
            SetUpALEDFlash();
        else if(index == 1)
            SetUpALEDRandomFlash();
        else if(index == 2)
            SetUpALEDSequential();
        else if(index == 3)
            SetUpALEDDisplayRange();
    }
}


void testLightController::SetUpRegularFlash()
{
    ui->timeOnLineEdit->setEnabled (true);
    ui->timeOffLineEdit->setEnabled (true);
    ui->numFlashesLineEdit->setEnabled (true);

    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    ui->colorComboBox->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);
    //ui->backgroundLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
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

    ui->colorComboBox->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);
    //ui->backgroundLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
}

void testLightController::SetUpRegularBackground()
{
    ui->playerNumberLineEdit->setEnabled (true);
    ui->timeDelayBGLineEdit->setEnabled (true);
    ui->timeDelayBGRLineEdit->setEnabled (true);
    ui->highCountLineEdit->setEnabled (true);
    ui->otherGroupsLineEdit->setEnabled (true);
    ui->backgroundLineEdit->setEnabled (true);

    ui->timeOnLineEdit->setEnabled (false);
    ui->timeOffLineEdit->setEnabled (false);
    ui->numFlashesLineEdit->setEnabled (false);

    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    ui->colorComboBox->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
}




void testLightController::SetUpRGBFlash()
{
    ui->timeOnLineEdit->setEnabled (true);
    ui->timeOffLineEdit->setEnabled (true);
    ui->numFlashesLineEdit->setEnabled (true);

    ui->colorComboBox->setEnabled (true);


    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);
    //ui->backgroundLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
}

void testLightController::SetUpRGBFlash2C()
{
    ui->timeOnLineEdit->setEnabled (true);
    ui->timeOffLineEdit->setEnabled (true);
    ui->numFlashesLineEdit->setEnabled (true);

    ui->colorComboBox->setEnabled (true);
    ui->sideColorComboBox->setEnabled (true);

    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);
    //ui->backgroundLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
}

void testLightController::SetUpRGBFlashCM()
{
    ui->timeOnLineEdit->setEnabled (true);
    ui->timeOffLineEdit->setEnabled (true);
    ui->numFlashesLineEdit->setEnabled (true);
    ui->colorMapComboBox->setEnabled (true);

    ui->colorComboBox->setEnabled (false);

    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);
    //ui->backgroundLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
}

void testLightController::SetUpRGBSequence()
{
    ui->timeDelayLineEdit->setEnabled (true);
    ui->colorComboBox->setEnabled (true);

    //ui->timeOnLineEdit->clear();
    ui->timeOnLineEdit->setEnabled (false);

    //ui->timeOffLineEdit->clear();
    ui->timeOffLineEdit->setEnabled (false);

    //ui->numFlashesLineEdit->clear();
    ui->numFlashesLineEdit->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);
    //ui->backgroundLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
}

void testLightController::SetUpRGBSequenceCM()
{
    ui->timeDelayLineEdit->setEnabled (true);
    ui->colorMapComboBox->setEnabled (true);

    //ui->timeOnLineEdit->clear();
    ui->timeOnLineEdit->setEnabled (false);

    //ui->timeOffLineEdit->clear();
    ui->timeOffLineEdit->setEnabled (false);

    //ui->numFlashesLineEdit->clear();
    ui->numFlashesLineEdit->setEnabled (false);

    ui->colorComboBox->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);
    //ui->backgroundLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
}

void testLightController::SetUpRGBSlash()
{
    ui->timeDelayLineEdit->setEnabled (true);
    ui->colorComboBox->setEnabled (true);

    //ui->timeOnLineEdit->clear();
    ui->timeOnLineEdit->setEnabled (false);

    //ui->timeOffLineEdit->clear();
    ui->timeOffLineEdit->setEnabled (false);

    //ui->numFlashesLineEdit->clear();
    ui->numFlashesLineEdit->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);
    //ui->backgroundLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
}

void testLightController::SetUpRGBDoubleSlash()
{
    ui->timeDelayLineEdit->setEnabled (true);
    ui->colorComboBox->setEnabled (true);

    //ui->timeOnLineEdit->clear();
    ui->timeOnLineEdit->setEnabled (false);

    //ui->timeOffLineEdit->clear();
    ui->timeOffLineEdit->setEnabled (true);

    //ui->numFlashesLineEdit->clear();
    ui->numFlashesLineEdit->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);
    //ui->backgroundLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
}



void testLightController::SetUpRGBBackground()
{
    ui->colorMapComboBox->setEnabled (true);
    ui->playerNumberLineEdit->setEnabled (true);
    ui->timeDelayBGLineEdit->setEnabled (true);
    ui->timeDelayBGRLineEdit->setEnabled (true);

    ui->highCountLineEdit->setEnabled (true);
    ui->otherGroupsLineEdit->setEnabled (true);
    ui->backgroundLineEdit->setEnabled (true);

    ui->timeOnLineEdit->setEnabled (false);
    ui->timeOffLineEdit->setEnabled (false);
    ui->numFlashesLineEdit->setEnabled (false);

    ui->timeDelayLineEdit->setEnabled (false);

    ui->colorComboBox->setEnabled (false);
    ui->sideColorComboBox->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);
}


void testLightController::SetUpALEDFlash()
{
    ui->timeOnLineEdit->setEnabled (true);
    ui->timeOffLineEdit->setEnabled (true);
    ui->numFlashesLineEdit->setEnabled (true);

    ui->colorComboBox->setEnabled (true);


    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);

    ui->backgroundLineEdit->setEnabled (false);
}

void testLightController::SetUpALEDRandomFlash()
{
    ui->timeOnLineEdit->setEnabled (true);
    ui->timeOffLineEdit->setEnabled (true);
    ui->numFlashesLineEdit->setEnabled (true);

    ui->colorComboBox->setEnabled (true);

    ui->color2ndCheckBox->setEnabled (true);
    ui->propComboBox->setEnabled (true);
    ui->sideColorComboBox->setEnabled (true);

    ui->ledsFlashComboBox->setEnabled (true);


    //ui->timeDelayLineEdit->clear();
    ui->timeDelayLineEdit->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->numLEDsComboBox->setEnabled (false);

    ui->backgroundLineEdit->setEnabled (false);
}

void testLightController::SetUpALEDSequential()
{
    ui->timeDelayLineEdit->setEnabled (true);
    ui->colorComboBox->setEnabled (true);

    ui->numLEDsComboBox->setEnabled (true);

    //ui->timeOnLineEdit->clear();
    ui->timeOnLineEdit->setEnabled (false);

    //ui->timeOffLineEdit->clear();
    ui->timeOffLineEdit->setEnabled (false);

    //ui->numFlashesLineEdit->clear();
    ui->numFlashesLineEdit->setEnabled (false);

    ui->sideColorComboBox->setEnabled (false);

    ui->colorMapComboBox->setEnabled (false);

    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeDelayBGLineEdit->setEnabled (false);
    ui->timeDelayBGRLineEdit->setEnabled (false);
    ui->highCountLineEdit->setEnabled (false);
    ui->otherGroupsLineEdit->setEnabled (false);
    //ui->backgroundLineEdit->setEnabled (false);

    ui->seqReloadCheckBox->setEnabled (false);
    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);

    ui->backgroundLineEdit->setEnabled (false);
}


void testLightController::SetUpALEDDisplayRange()
{
    ui->colorMapComboBox->setEnabled (true);
    ui->timeDelayBGLineEdit->setEnabled (true);
    ui->timeDelayBGRLineEdit->setEnabled (true);

    ui->highCountLineEdit->setEnabled (true);
    ui->otherGroupsLineEdit->setEnabled (true);
    //ui->backgroundLineEdit->setEnabled (true);

    ui->seqReloadCheckBox->setEnabled (true);
    ui->numLEDsComboBox->setEnabled (true);


    ui->playerNumberLineEdit->setEnabled (false);
    ui->timeOnLineEdit->setEnabled (false);
    ui->timeOffLineEdit->setEnabled (false);
    ui->numFlashesLineEdit->setEnabled (false);

    ui->timeDelayLineEdit->setEnabled (false);

    ui->colorComboBox->setEnabled (false);
    ui->sideColorComboBox->setEnabled (false);


    ui->ledsFlashComboBox->setEnabled (false);

    ui->color2ndCheckBox->setEnabled (false);
    ui->propComboBox->setEnabled (false);

    ui->backgroundLineEdit->setEnabled (false);
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
        QString message;

        if(typeSet == ULTIMARC)
            message = "Time On cannot be 0. Needs to be 1-65,535.\nTime On: "+tempTimeOn;
        else
            message = "Time On cannot be 0. Needs to be 1-9,999.\nTime On: "+tempTimeOn;

        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempOn > 9999 && typeSet == HOTRALEDSTRIP)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time On cannot be 10,000 or more. Needs to be 1-9,999.\nTime On: "+tempTimeOn;
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
        QString message;

        if(typeSet == ULTIMARC)
            message = "Time Off cannot be 0. Needs to be 1-65,535.\nTime Off: "+tempTimeOff;
        else
            message = "Time Off cannot be 0. Needs to be 1-9,999.\nTime Off: "+tempTimeOff;

        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempOff > 9999 && typeSet == HOTRALEDSTRIP)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time Off cannot be 10,000 or more. Needs to be 1-9,999.\nTime Off: "+tempTimeOff;
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

    if(typeSet == ULTIMARC)
    {
        if(tempFlash == 0 || tempFlash > 255)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Number of Flashes is out of range. Needs to be 1-255.\nNumber of Flashes: "+tempFlashes;
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }
    }
    else if(typeSet == HOTRALEDSTRIP)
    {
        if(tempFlash == 0 || tempFlash > 9)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Number of Flashes is out of range. Needs to be 1-9.\nNumber of Flashes: "+tempFlashes;
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }
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
        QString message;

        if(typeSet == ULTIMARC)
            message = "Time Delay cannot be 0. Needs to be 1-65,535.\nTime Delay: "+tempTimeDelay;
        else
            message = "Time Delay cannot be 0. Needs to be 1-9,999.\nTime Delay: "+tempTimeDelay;

        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempDelay > 9999 && typeSet == HOTRALEDSTRIP)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time Delay cannot be 10,000 or more. Needs to be 1-9,999.\nTime Delay: "+tempTimeDelay;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    timeDelay = tempDelay;

    return true;
}

bool testLightController::CheckColor()
{
    quint8 index = ui->colorComboBox->currentIndex ();

    QString tempColor = colors[index];

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
    quint8 index = ui->sideColorComboBox->currentIndex ();

    QString tempColor = colors[index];

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
    quint8 index = ui->colorMapComboBox->currentIndex ();

    QString tempColor = colorMaps[index];

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
    bool isNumber;


    if(typeSet == ULTIMARC)
    {
        bool isRGB;
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
            if(commandIndex >= REGULARLOW && commandIndex <= REGULARHIGH)
                isRGB = false;
            else
                isRGB = true;
        }
        else if(regularLights)
            isRGB = false;
        else if(rgbLights)
            isRGB = true;
        else
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
    }
    else
    {
        quint8 numStrips = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GetNumberStrips();

        groups.clear();

        for(i = 0; i < splitData.length(); i++)
        {
            quint8 tempGrp = splitData[i].toUInt(&isNumber);

            if(!isNumber)
            {
                QString title = "Test Light Controller Fail";
                QString message = "Strip Number is not a number.\nStrip Number: "+splitData[i];
                QMessageBox::critical (this, title, message, QMessageBox::Ok);
                return false;
            }

            if(tempGrp > numStrips)
            {
                QString title = "Test Light Controller Fail";
                QString message = "Strip Number is out of range. Number of strips is "+QString::number(numStrips)+"\nStrip Number: "+splitData[i];
                QMessageBox::critical (this, title, message, QMessageBox::Ok);
                return false;
            }

            groups << tempGrp;
        }
    }


    return true;
}


bool testLightController::GetTimeBG()
{
    if(isBGRunning)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Background is already running. Can only have one background open.";
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    bool isNumber;
    QString tempTimeDelay = ui->timeDelayBGLineEdit->text ();

    quint16 tempDelay = tempTimeDelay.toUInt (&isNumber);

    if(!isNumber)
    {
        QString title = "Test Light Controller Fail";
        QString message;

        if(typeSet == ULTIMARC)
            message = "Time Off Background is not a number.\nTime Off: "+tempTimeDelay;
        else
            message = "Time Off Display Range is not a number.\nTime Off: "+tempTimeDelay;

        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempDelay == 0)
    {
        QString title = "Test Light Controller Fail";
        QString message;

        if(typeSet == ULTIMARC)
            message = "Time Off Background cannot be 0. Needs to be 1-65,535.\nTime Off: "+tempTimeDelay;
        else
            message = "Time Off Display Range cannot be 0. Needs to be 1-9,999.\nTime Off: "+tempTimeDelay;

        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempDelay > 9999 && typeSet == HOTRALEDSTRIP)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time Off Display Range cannot be 10,000 or more. Needs to be 1-9,999.\nTime Off: "+tempTimeDelay;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    timeBG = tempDelay;

    return true;
}


bool testLightController::GetTimeBGR()
{
    if(isBGRunning)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Background is already running. Can only have one background open.";
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    bool isNumber;
    QString tempTimeDelay = ui->timeDelayBGRLineEdit->text ();

    quint16 tempDelay = tempTimeDelay.toUInt (&isNumber);

    if(!isNumber)
    {
        QString title = "Test Light Controller Fail";
        QString message;

        if(typeSet == ULTIMARC)
            message = "Time Delay Background is not a number.\nTime Delay: "+tempTimeDelay;
        else
            message = "Time Delay Display Range is not a number.\nTime Delay: "+tempTimeDelay;

        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempDelay == 0)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time Delay Background cannot be 0. Needs to be 1-65,535.\nTime Delay: "+tempTimeDelay;

        if(typeSet == ULTIMARC)
            message = "Time Delay Background cannot be 0. Needs to be 1-65,535.\nTime Delay: "+tempTimeDelay;
        else
            message = "Time Delay Display Range cannot be 0. Needs to be 1-9,999.\nTime Delay: "+tempTimeDelay;

        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempDelay > 9999 && typeSet == HOTRALEDSTRIP)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Time Delay Display Range cannot be 10,000 or more. Needs to be 1-9,999.\nTime Delay: "+tempTimeDelay;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    timeBGR = tempDelay;

    return true;
}


bool testLightController::GetHighCount()
{
    if(isBGRunning)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Background is already running. Can only have one background open.";
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    bool isNumber;
    QString tempTimeDelay = ui->highCountLineEdit->text ();

    quint16 tempCount = tempTimeDelay.toUInt (&isNumber);

    if(!isNumber)
    {
        QString title = "Test Light Controller Fail";
        QString message;

        if(typeSet == ULTIMARC)
            message = "Background's High Count is not a number.\nHigh Count: "+tempTimeDelay;
        else
            message = "Display Range's Max Range is not a number.\nMax Range: "+tempTimeDelay;

        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(typeSet == ULTIMARC)
    {
        if(isBGRGB)
            bgGroupPinCount = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GetRGBGroupPinCount(groups[0]);
        else
            bgGroupPinCount = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GetRegularGroupPinCount(groups[0]);

        if(tempCount == 0)
            highCount = bgGroupPinCount;
        else if(tempCount > bgGroupPinCount)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Background High Count is larger than the pins in the background group.\nHigh Count: "+tempTimeDelay+"\nBackground Group Pins Count: "+QString::number(bgGroupPinCount);
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }
        else
            highCount = tempCount;
    }
    else
    {
        if(tempCount == 0)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Display Range's Max Range cannot be 0. Can be 1 to 9,999.\nMax Range: "+tempTimeDelay;
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }
        else if(tempCount > 9999)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Display Range's Max Range cannot be 10,000 or more. Can be 1 to 9,999.\nMax Range: "+tempTimeDelay;
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }

        maxRange = tempCount;
    }

    return true;
}


bool testLightController::GetOtherGroups()
{
    if(typeSet == ULTIMARC)
    {
        if(isBGRunning)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Background is already running. Can only have one background open.";
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }

        QString tempGroups = ui->otherGroupsLineEdit->text ();

        //Split Up Line Data with a Space
        QStringList splitData = tempGroups.split(' ', Qt::SkipEmptyParts);

        quint8 i;
        bool isNumber, isRGB;
        otherGroups.clear();

        for(i = 0; i < splitData.length(); i++)
        {
            quint8 tempGrp = splitData[i].toUInt(&isNumber);

            if(!isNumber)
            {
                QString title = "Test Light Controller Fail";
                QString message = "Background's Other Group Number is not a number.\nGroup Number: "+splitData[i];
                QMessageBox::critical (this, title, message, QMessageBox::Ok);
                return false;
            }

            otherGroups << tempGrp;
        }

        if(regularLights && rgbLights)
        {
            if(commandIndex >= REGULARLOW && commandIndex <= REGULARHIGH)
                isRGB = false;
            else
                isRGB = true;
        }
        else if(regularLights)
            isRGB = false;
        else if(rgbLights)
            isRGB = true;
        else
            isRGB = true;

        for(i = 0; i < otherGroups.count(); i++)
        {
            bool chkGrp;

            if(isRGB)
                chkGrp = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->CheckRGBGroupNumber (otherGroups[i]);
            else
                chkGrp = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->CheckRegularGroupNumber (otherGroups[i]);

            if(!chkGrp)
            {
                QString title = "Test Light Controller Fail";
                QString message = "Background's Other Group number doesn't exist in the light controller.\nFailing Group Number: "+QString::number(otherGroups[i])+"\nFailing Controller Number: "+QString::number(cntlrsNumber);
                QMessageBox::critical (this, title, message, QMessageBox::Ok);
                return false;
            }
        }
    }
    else
    {
        QString tempStepsS = ui->otherGroupsLineEdit->text ();
        quint8 i;
        bool isNumber;

        quint8 tempSteps = tempStepsS.toUInt(&isNumber);

        if(!isNumber)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Display Ranges's Steps is not a number.\nSteps: "+tempStepsS;
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }

        if(tempSteps == 0)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Display Ranges's Steps cannot be 0. Can be 1 to 9.\nSteps: "+tempStepsS;
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }

        if(tempSteps > 9)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Display Ranges's Steps cannot be 10 or more. Can be 1 to 9.\nSteps: "+tempStepsS;
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }

        steps = tempSteps;
    }

    return true;
}


bool testLightController::GetPlayerNumber()
{

    if(isBGRunning)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Background is already running. Can only have one background open.";
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    bool isNumber;
    QString tempplayerNum = ui->playerNumberLineEdit->text ();

    quint8 tempPlayNum = tempplayerNum.toUShort (&isNumber);

    if(!isNumber)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Background Player Number is not a number.\nPlayer Number: "+tempplayerNum;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    if(tempPlayNum > MAXGAMEPLAYERS || tempPlayNum == 0)
    {
        QString title = "Test Light Controller Fail";
        QString message = "Background player number is out of range. Needs to be 1-4.\nPlayer Number: "+tempplayerNum;
        QMessageBox::critical (this, title, message, QMessageBox::Ok);
        return false;
    }

    playerNumber = tempPlayNum - 1;
    return true;
}

bool testLightController::GetProbability()
{
    if(typeSet == ULTIMARC)
        return false;
    else
    {
        bool enable2ndColor = ui->color2ndCheckBox->isChecked ();

        if(!enable2ndColor)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Then Enable 2nd Color is not checked.";
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return false;
        }

        quint8 probIndex = ui->propComboBox->currentIndex ();

        probability = probIndex + 2;
    }

    return true;
}

bool testLightController::GetLEDsPerDelay()
{
    if(typeSet == ULTIMARC)
        return false;
    else
    {
        quint8 ledIndex = ui->numLEDsComboBox->currentIndex ();

        if(ledIndex == 0)
            ledsPerDelay = 1;
        else if(ledIndex == 1)
            ledsPerDelay = 2;
        else if(ledIndex == 2)
            ledsPerDelay = 4;
        else if(ledIndex == 3)
            ledsPerDelay = 8;
        else
            return false;
    }

    return true;
}

bool testLightController::GetLEDsToFlash()
{
    if(typeSet == ULTIMARC)
        return false;
    else
    {
        quint8 ledIndex = ui->ledsFlashComboBox->currentIndex ();

        ledsToFlash = ledIndex + 1;
    }

    return true;
}



void testLightController::SetLabels()
{
    if(typeSet == ULTIMARC)
    {
        ui->labelGroups->clear();
        ui->labelGroups->setText (ULTIMARCGRPS);

        ui->sideColor->clear();
        ui->sideColor->setText (ULTIMARCSIDEC);

        ui->TimeDelayBG->clear();
        ui->TimeDelayBG->setText (ULTIMARCTIMEBG);

        ui->TimeDelayBGR->clear();
        ui->TimeDelayBGR->setText (ULTIMARCTIMEBGR);

        ui->highCount->clear();
        ui->highCount->setText (ULTIMARCHIGHC);

        ui->otherGroupsBG->clear();
        ui->otherGroupsBG->setText (ULTIMARCOTHER);

        ui->backgroundNumber->clear();
        ui->backgroundNumber->setText (ULTIMARCBGNUM);

        //ui->updateBG->clear();
        ui->updateBG->setText (ULTIMARCUPDATEBG);

        ui->closeBG->setText (ULTIMARCCLOSEBG);
    }
    else if(typeSet == HOTRALEDSTRIP)
    {
        ui->labelGroups->clear();
        ui->labelGroups->setText (ALEDSTRIPS);

        ui->sideColor->clear();
        ui->sideColor->setText (ALED2NDCOLOR);

        ui->TimeDelayBG->clear();
        ui->TimeDelayBG->setText (ALEDDRTIME);

        ui->TimeDelayBGR->clear();
        ui->TimeDelayBGR->setText (ALEDDRTIMER);

        ui->highCount->clear();
        ui->highCount->setText (ALEDMAXRANGE);

        ui->otherGroupsBG->clear();
        ui->otherGroupsBG->setText (ALEDNUMSTEPS);

        ui->backgroundNumber->clear();
        ui->backgroundNumber->setText (ALEDDRNUMBER);

        //ui->updateBG->clear();
        ui->updateBG->setText (ALEDDRUPDATE);

        ui->closeBG->setText (ALEDCLOSEDR);
    }
}


void testLightController::CloseDisplayRange()
{
    if(isDRRunning)
    {
        p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameEnded();
        p_comDeviceList->p_lightCntlrList[cntlrsNumber]->TurnOffLights();
        p_comDeviceList->p_lightCntlrList[cntlrsNumber]->ResetLightController();
        isDRRunning = false;
        ui->updateBG->setEnabled(false);
        ui->closeBG->setEnabled(false);

        ui->backgroundLineEdit->setEnabled(false);
    }
}



void testLightController::on_lightCntlrComboBox_currentIndexChanged(int index)
{
    //qDebug() << "Light Controller Index Changed for Test Light Controller";

    //If Switching to a New Light Controller, then Turn Off Lights in Old Light Controller
    if(!isFirstController)
    {
        p_comDeviceList->p_lightCntlrList[cntlrsNumber]->TurnOffLights();
        p_comDeviceList->p_lightCntlrList[cntlrsNumber]->ResetLightController();
        isBGRunning = false;
        isBGRunning = false;
    }

    if(typeSet == ULTIMARC)
        cntlrsNumber = ultimarcPotitions[index];
    else
        cntlrsNumber = aledStripPotitions[index];


    QString newLabel = "!"+QString::number(cntlrsNumber);

    ui->labelCntlrNum->clear();

    ui->labelCntlrNum->setText (newLabel);

    if(typeSet == ULTIMARC)
    {
        regularLights = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->IsRegularGroups ();
        rgbLights = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->IsRGBGroups ();
    }
    else
        rgbLights = true;

    LoadCommands();

    isFirstController = false;

    numberColors = 0;
    numberColorMaps = 0;

    ui->colorComboBox->clear();
    ui->sideColorComboBox->clear();
    ui->colorMapComboBox->clear();

    if(rgbLights)
    {
        rgbColorMap = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GetRGBColorMap();

        QMapIterator<QString, RGBColor> x(rgbColorMap);

        while (x.hasNext())
        {
            x.next();

            ui->colorComboBox->insertItem(numberColors, x.key());

            ui->sideColorComboBox->insertItem(numberColors, x.key());

            colors << x.key();

            numberColors++;
        }

        rgbColorMapMap = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GetRGBColorMapMap();

        QMapIterator<QString, QStringList> y(rgbColorMapMap);

        while (y.hasNext())
        {
            y.next();

            ui->colorMapComboBox->insertItem(numberColorMaps, y.key());

            colorMaps << y.key();

            numberColorMaps++;
        }
    }
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

    if(typeSet == ULTIMARC)
    {

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
            else if(commandIndex == 3 && !isBGRunning)
            {
                isBGRGB = false;

                chkArg = GetGroups();

                if(!chkArg)
                    return;

                if(groups.count() != 1)
                {
                    QString title = "Test Light Controller Fail";
                    QString message = "Background Groups can only be 1 group.\nNumber of Groups: "+QString::number(groups.count());
                    QMessageBox::critical (this, title, message, QMessageBox::Ok);
                    return;
                }

                chkArg = GetPlayerNumber();

                if(!chkArg)
                    return;

                chkArg = GetTimeBG();

                if(!chkArg)
                    return;

                chkArg = GetTimeBGR();

                if(!chkArg)
                    return;

                chkArg = GetHighCount();

                if(!chkArg)
                    return;

                chkArg = GetOtherGroups();

                if(!chkArg)
                    return;

                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SetUpBackgroundRegular(groups, playerNumber, timeBG, timeBGR, highCount, otherGroups);

                isBGRunning = true;
            }
            else if(commandIndex == 4 || commandIndex == 5)
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

                if(commandIndex == 4)
                    p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRGBLights(groups, timeOn, timeOff, numberFlashes, color);
                else
                    p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRandomRGBLights(groups, timeOn, timeOff, numberFlashes, color);
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

                chkArg = CheckColor();

                if(!chkArg)
                    return;

                chkArg = CheckSideColor();

                if(!chkArg)
                    return;

                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->FlashRandomRGB2CLights(groups, timeOn, timeOff, numberFlashes, color, sideColor);
            }
            else if(commandIndex == 7)
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
            else if(commandIndex == 8)
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
            else if(commandIndex == 9)
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
            else if(commandIndex == 10)
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

                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SlashRGBLights(groups, timeDelay, color);
            }
            else if(commandIndex == 11)
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

                chkArg = GetTimeOff();

                if(!chkArg)
                    return;

                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->DoubleSlashRGBLights(groups, timeDelay, timeOff, color);
            }
            else if(commandIndex == 12 && !isBGRunning)
            {
                isBGRGB = true;

                chkArg = GetGroups();

                if(!chkArg)
                    return;

                if(groups.count() != 1)
                {
                    QString title = "Test Light Controller Fail";
                    QString message = "Background Groups can only be 1 group.\nNumber of Groups: "+QString::number(groups.count());
                    QMessageBox::critical (this, title, message, QMessageBox::Ok);
                    return;
                }

                chkArg = CheckColorMap();

                if(!chkArg)
                    return;

                chkArg = GetPlayerNumber();

                if(!chkArg)
                    return;

                chkArg = GetTimeBG();

                if(!chkArg)
                    return;

                chkArg = GetTimeBGR();

                if(!chkArg)
                    return;

                chkArg = GetHighCount();

                if(!chkArg)
                    return;

                chkArg = GetOtherGroups();

                if(!chkArg)
                    return;

                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SetUpBackgroundRGB(groups, colorMap, playerNumber, timeBG, timeBGR, highCount, otherGroups);

                isBGRunning = true;
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
            else if(commandIndex == 3 && !isBGRunning)
            {
                isBGRGB = false;

                chkArg = GetGroups();

                if(!chkArg)
                    return;

                if(groups.count() != 1)
                {
                    QString title = "Test Light Controller Fail";
                    QString message = "Background Groups can only be 1 group.\nNumber of Groups: "+QString::number(groups.count());
                    QMessageBox::critical (this, title, message, QMessageBox::Ok);
                    return;
                }

                chkArg = GetPlayerNumber();

                if(!chkArg)
                    return;

                chkArg = GetTimeBG();

                if(!chkArg)
                    return;

                chkArg = GetTimeBGR();

                if(!chkArg)
                    return;

                chkArg = GetHighCount();

                if(!chkArg)
                    return;

                chkArg = GetOtherGroups();

                if(!chkArg)
                    return;

                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SetUpBackgroundRegular(groups, playerNumber, timeBG, timeBGR, highCount, otherGroups);

                isBGRunning = true;
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
            else if(commandIndex == 6)
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

                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SlashRGBLights(groups, timeDelay, color);
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

                chkArg = GetTimeOff();

                if(!chkArg)
                    return;

                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->DoubleSlashRGBLights(groups, timeDelay, timeOff, color);
            }
            else if(commandIndex == 8 && !isBGRunning)
            {
                isBGRGB = true;

                chkArg = GetGroups();

                if(!chkArg)
                    return;

                if(groups.count() != 1)
                {
                    QString title = "Test Light Controller Fail";
                    QString message = "Background Groups can only be 1 group.\nNumber of Groups: "+QString::number(groups.count());
                    QMessageBox::critical (this, title, message, QMessageBox::Ok);
                    return;
                }

                chkArg = CheckColorMap();

                if(!chkArg)
                    return;

                chkArg = GetPlayerNumber();

                if(!chkArg)
                    return;

                chkArg = GetTimeBG();

                if(!chkArg)
                    return;

                chkArg = GetTimeBGR();

                if(!chkArg)
                    return;

                chkArg = GetHighCount();

                if(!chkArg)
                    return;

                chkArg = GetOtherGroups();

                if(!chkArg)
                    return;

                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SetUpBackgroundRGB(groups, colorMap, playerNumber, timeBG, timeBGR, highCount, otherGroups);

                isBGRunning = true;
            }
        }
    }
    else if(typeSet == HOTRALEDSTRIP)
    {
        if(commandIndex == 0)
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

            for(quint8 i = 0; i < groups.length (); i++)
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SetUpStripFlash(i, groups[i], timeOn, timeOff, numberFlashes, color);


            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameStarted();


            for(quint8 i = 0; i < groups.length (); i++)
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->DoStripFlash(i);

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameEnded();
        }
        else if(commandIndex == 1)
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

            chkArg = GetLEDsToFlash();

            if(!chkArg)
                return;

            bool enable2ndColor = ui->color2ndCheckBox->isChecked ();

            if(enable2ndColor)
            {
                chkArg = GetProbability();

                if(!chkArg)
                    return;

                chkArg = CheckSideColor();

                if(!chkArg)
                    return;

                for(quint8 i = 0; i < groups.length (); i++)
                    p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SetUpStripRndFlash(i, groups[i], ledsToFlash, timeOn, timeOff, numberFlashes, color, true, probability, sideColor);
            }
            else
            {
                for(quint8 i = 0; i < groups.length (); i++)
                    p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SetUpStripRndFlash(i, groups[i], ledsToFlash, timeOn, timeOff, numberFlashes, color, false, 0, "");
            }

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameStarted();


            for(quint8 i = 0; i < groups.length (); i++)
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->DoStripRndFlash(i);

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameEnded();
        }
        else if(commandIndex == 2)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            //qDebug() << "Got Strips" << groups;

            chkArg = GetLEDsPerDelay();

            if(!chkArg)
                return;

            //qDebug() << "Got LEDs Per Delay" << ledsPerDelay;

            chkArg = GetTimeDelay();

            if(!chkArg)
                return;

            //qDebug() << "Got Time Delay" << timeDelay;

            chkArg = CheckColor();

            if(!chkArg)
                return;

            //qDebug() << "Got Color" << color;

            for(quint8 i = 0; i < groups.length (); i++)
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SetUpStripSequential(i, groups[i], timeDelay, color, ledsPerDelay);

            //qDebug() << "Done Set Up";

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameStarted();

            //qDebug() << "Game Started";

            for(quint8 i = 0; i < groups.length (); i++)
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->DoStripSequential(i);

            //qDebug() << "Do Sequential";

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameEnded();

            //qDebug() << "Game Ended";
        }
        else if(commandIndex == 3 && !isDRRunning)
        {
            chkArg = GetGroups();

            if(!chkArg)
                return;

            //Max Range
            chkArg = GetHighCount();

            if(!chkArg)
                return;

            //Steps
            chkArg = GetOtherGroups();

            if(!chkArg)
                return;

            //Time Off
            chkArg = GetTimeBG();

            if(!chkArg)
                return;

            chkArg = CheckColorMap();

            if(!chkArg)
                return;

            bool enableSeq = ui->seqReloadCheckBox->isChecked ();

            if(enableSeq)
            {
                //Time Off
                chkArg = GetTimeBGR();

                if(!chkArg)
                    return;

                chkArg = GetLEDsPerDelay();

                if(!chkArg)
                    return;

                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SetUpDisplayRange(groups, maxRange, steps, timeBG, colorMap, true, timeBGR, ledsPerDelay);
            }
            else
                p_comDeviceList->p_lightCntlrList[cntlrsNumber]->SetUpDisplayRange(groups, maxRange, steps, timeBG, colorMap, false, 0, 0);

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameStarted();

            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->UpdateDisplayRange(groups, 0);

            isDRRunning = true;

            ui->backgroundLineEdit->setEnabled(true);
        }
    }

    if(typeSet == ULTIMARC && isBGRunning)
    {
        ui->updateBG->setEnabled(true);
        ui->closeBG->setEnabled(true);
    }
    else if(typeSet == HOTRALEDSTRIP && isDRRunning)
    {
        ui->updateBG->setEnabled(true);
        ui->closeBG->setEnabled(true);
    }
}


void testLightController::on_updateBG_clicked()
{

    if(typeSet == ULTIMARC)
    {
        if(!isBGRunning)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Background is not running.";
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return;
        }

        bool isNumber;
        QString tempNumberS = ui->backgroundLineEdit->text ();

        quint16 tempNumber = tempNumberS.toUInt (&isNumber);

        if(!isNumber)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Background update is not a number.\nNumber: "+tempNumberS;
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return;
        }

        if(isBGRGB)
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->BackgroundRGB(playerNumber, tempNumber);
        else
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->BackgroundRegular(playerNumber, tempNumber);
    }
    else
    {
        if(!isDRRunning)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Display Range is not running.";
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return;
        }

        bool isNumber;
        QString tempNumberS = ui->backgroundLineEdit->text ();

        quint16 tempNumber = tempNumberS.toUInt (&isNumber);

        if(!isNumber)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Display Range update is not a number.\nNumber: "+tempNumberS;
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return;
        }

        if(tempNumber > maxRange)
        {
            QString title = "Test Light Controller Fail";
            QString message = "Display Range update is larger than Max Range.\nNumber: "+tempNumberS+"\nMaxRange: "+QString::number(maxRange);
            QMessageBox::critical (this, title, message, QMessageBox::Ok);
            return;
        }

        p_comDeviceList->p_lightCntlrList[cntlrsNumber]->UpdateDisplayRange(groups, tempNumber);
    }
}


void testLightController::on_typeComboBox_currentIndexChanged(int index)
{
    ui->lightCntlrComboBox->clear();

    if(index == ULTIMARC)
    {
        if(isDRRunning)
        {
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameEnded();
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->TurnOffLights();
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->ResetLightController();
            isDRRunning = false;
            ui->updateBG->setEnabled(false);
            ui->closeBG->setEnabled(false);
            ui->backgroundLineEdit->setEnabled(true);
        }

        if(numberSaveUltimarcCntlrs > 0)
        {
            typeSet = ULTIMARC;

            for(quint8 i = 0; i < numberSaveUltimarcCntlrs; i++)
            {
                quint8 templightCntlrNumber = ultimarcPotitions[i];
                dataUltimarc[i] = p_comDeviceList->p_lightCntlrList[i]->GetUltimarcData();

                QString controllerInfo = QString::number(templightCntlrNumber) + " - " + "ID: "+QString::number(dataUltimarc[i].id)+ " " + dataUltimarc[i].vendorName + " " + dataUltimarc[i].typeName;

                ui->lightCntlrComboBox->insertItem(i, controllerInfo);
            }

            cntlrsNumber = ultimarcPotitions[0];

            regularLights = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->IsRegularGroups ();
            rgbLights = p_comDeviceList->p_lightCntlrList[cntlrsNumber]->IsRGBGroups ();

            ui->backgroundLineEdit->setEnabled (true);

            ui->lightCntlrComboBox->setCurrentIndex (0);
        }
        else
        {
            if(numberALEDStripCntlrs > 0)
            {
                ui->typeComboBox->setCurrentIndex (HOTRALEDSTRIP);
            }
        }
    }
    else if(index == HOTRALEDSTRIP)
    {
        if(isBGRunning)
        {
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->TurnOffLights();
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->ResetLightController();
            isBGRunning = false;
            ui->updateBG->setEnabled(false);
            ui->closeBG->setEnabled(false);
            ui->backgroundLineEdit->setEnabled(false);
        }

        if(numberALEDStripCntlrs > 0)
        {
            typeSet = HOTRALEDSTRIP;

            for(quint8 i = 0; i < numberALEDStripCntlrs; i++)
            {
                quint8 templightCntlrNumber = aledStripPotitions[i];

                QString controllerInfo = QString::number(templightCntlrNumber) + " - LED Hook: Strip on " + aledCntlrCOMNames[i];
                ui->lightCntlrComboBox->insertItem(i, controllerInfo);
            }

            cntlrsNumber = aledStripPotitions[0];

            ui->typeComboBox->setCurrentIndex (HOTRALEDSTRIP);

            ui->backgroundLineEdit->setEnabled (false);

            ui->lightCntlrComboBox->setCurrentIndex (0);
        }
        else
        {
            ui->typeComboBox->setCurrentIndex (ULTIMARC);
        }
    }

    //Change the Lables
    SetLabels();
}


void testLightController::on_closeBG_clicked()
{
    if(typeSet == ULTIMARC)
    {
        if(isBGRunning)
        {
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->TurnOffLights();
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->ResetLightController();
            isBGRunning = false;
            ui->updateBG->setEnabled(false);
            ui->closeBG->setEnabled(false);
        }
    }
    else
    {
        if(isDRRunning)
        {
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->GameEnded();
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->TurnOffLights();
            p_comDeviceList->p_lightCntlrList[cntlrsNumber]->ResetLightController();
            isDRRunning = false;
            ui->updateBG->setEnabled(false);
            ui->closeBG->setEnabled(false);
            ui->backgroundLineEdit->setEnabled(false);
        }
    }
}

