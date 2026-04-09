#include "LightCommand.h"
#include "../Global.h"

//Constructor
//LightCommand::LightCommand(QString outputSig, QString cmd, QStringList cmdArgs, QMap<quint8,QList<quint8>> cntlrsGrps, QObject *parent)
//   : QObject{parent}


LightCommand::LightCommand()
{
    outputSignal = "";
    command = "";
    isCommandValid = false;
    numberCntlrs = 0;
    kindOfCommand = 0;

    isColor = false;
    isSideColor = false;
    isColorMap = false;

    isGeneralCommand = false;
    isBackground = false;

    isInvertData = false;

    isALEDStrip = false;
    isDisplayRange = false;
    enable2ndColor = false;

    errorTitle = "Light Game/Default File Error";
}


LightCommand::LightCommand(QString outputSig, QString cmd, QStringList cmdArgs, QMap<quint8,QList<quint8>> cntlrsGrps)
{
    outputSignal = outputSig;
    command = cmd;
    commandArg = cmdArgs;
    cntlrsGroup = cntlrsGrps;
    isCommandValid = false;
    isRGB = true;

    isColor = false;
    isSideColor = false;
    isColorMap = false;


    isGeneralCommand = false;
    isBackground = false;

    isInvertData = false;

    isALEDStrip = false;
    isDisplayRange = false;
    isStripFlash = false;
    isStripRndFlash = false;
    isStripSequential = false;
    enable2ndColor = false;


    errorTitle = "Light Game/Default File Error";

    //numberCntlrs = cntlrsGroup.count ();
    numberCntlrs = 0;

    QMapIterator<quint8, QList<quint8>> x(cntlrsGrps);
    while (x.hasNext())
    {
        x.next();
        //listCntlrs << x.key();
        listCntlrs.insert(numberCntlrs, x.key());
        numberCntlrs++;
    }

    //quint8 i;
    //for(i = 0; i < listCntlrs.count(); i++)
    //    qDebug() << "i" << i << "listCntlrs" << listCntlrs[i];

    //listCntlrs = cntlrsGroup.keys();

    kindOfCommand = 0;

    ProcessLightCommand();
}


/*
//Copy Constructor
LightCommand::LightCommand(LightCommand const &other, QObject *parent)
   : QObject{parent}
{
    outputSignal = other.outputSignal;
    command = other.command;
    commandArg = other.commandArg;
    cntlrsGroup = other.cntlrsGroup;
    isCommandValid = false;

    numberCntlrs = cntlrsGroup.count ();

    listCntlrs = cntlrsGroup.keys();

    ProcessLightCommand();
}
*/



void LightCommand::ProcessLightCommand()
{
    if(command == FLASHRGB)
    {
        kindOfCommand = FLASHCOMMAND;
        commandNumber = FLASHRGBCMD;
        isCommandValid = ProcessFlashRGB();
    }
    else if(command == RELOADFLASHRGB)
    {
        kindOfCommand = FLASHCOMMAND;
        commandNumber = RELOADFLASHRGBCMD;
        isCommandValid = ProcessReloadFlashRGB();
    }
    else if(command == DEATHFLASHRGB)
    {
        kindOfCommand = FLASHCOMMAND;
        commandNumber = DEATHFLASHRGBCMD;
        isCommandValid = ProcessReloadFlashRGB();
    }
    else if(command == RANDOMFLASHRGB)
    {
        kindOfCommand = FLASHCOMMAND;
        commandNumber = RANDOMFLASHRGBCMD;
        isCommandValid = ProcessRandomFlashRGB();
    }
    else if(command == RANDOMFLASHRGB2C)
    {
        kindOfCommand = FLASHCOMMAND;
        commandNumber = RANDOMFLASHRGB2CCMD;
        isCommandValid = ProcessRandomFlash2CRGB();
    }
    else if(command == SEQUENCERGB)
    {
        kindOfCommand = SEQUENCECOMMAND;
        commandNumber = SEQUENCERGBCMD;
        isCommandValid = ProcessSequenceRGB();
    }
    else if(command == RELOADSEQUENCERGB)
    {
        kindOfCommand = SEQUENCECOMMAND;
        commandNumber = RELOADSEQUENCERGBCMD;
        isCommandValid = ProcessReloadSequenceRGB();
    }
    else if(command == FOLLOWERRGB)
    {
        kindOfCommand = FOLLOWERCOMMAND;
        commandNumber = FOLLOWERRGBCMD;
        isCommandValid = ProcessFollowerRGB();
    }
    else if(command == FOLLOWERRANDOMRGB)
    {
        kindOfCommand = FOLLOWERCOMMAND;
        commandNumber = FOLLOWERRANDOMRGBCMD;
        color = "";
        isCommandValid = true;
    }
    else if(command == FLASHREG)
    {
        kindOfCommand = REGFLASHCOMMAND;
        commandNumber = FLASHREGCMD;
        isCommandValid = ProcessFlashRegular();
        isRGB = false;
    }
    else if(command == RELOADFLASHREG)
    {
        kindOfCommand = REGFLASHCOMMAND;
        commandNumber = RELOADFLASHREGCMD;
        isCommandValid = ProcessPlayerFlashRegular();
        isRGB = false;
    }
    else if(command == DEATHFLASHREG)
    {
        kindOfCommand = REGFLASHCOMMAND;
        commandNumber = DEATHFLASHREGCMD;
        isCommandValid = ProcessPlayerFlashRegular();
        isRGB = false;
    }
    else if(command == RANDOMFLASHREG)
    {
        kindOfCommand = REGFLASHCOMMAND;
        commandNumber = RANDOMFLASHREGCMD;
        isCommandValid = ProcessRandomFlashRegular();
        isRGB = false;
    }
    else if(command == SEQUENCEREG)
    {
        kindOfCommand = REGSEQUENCECOMMAND;
        commandNumber = SEQUENCEREGCMD;
        isCommandValid = ProcessSequenceRegular();
        isRGB = false;
    }
    else if(command == RELOADSEQUENCEREG)
    {
        kindOfCommand = REGSEQUENCECOMMAND;
        commandNumber = RELOADSEQUENCEREGCMD;
        isCommandValid = ProcessPlayerSequenceRegular();
        isRGB = false;
    }
    else if(command == FOLLOWERREG)
    {
        kindOfCommand = REGFOLLOWERCOMMAND;
        commandNumber = FOLLOWERREGCMD;
        isCommandValid = true;
        isRGB = false;
    }
    else if(command == TURNOFFLIGHTS)
    {
        kindOfCommand = GENERALCOMMAND;
        commandNumber = TURNOFFLIGHTSCMD;
        isCommandValid = true;
        isGeneralCommand = true;
    }
    else if(command == RANDOMFLASHRGBCM)
    {
        kindOfCommand = FLASHCMCOMMAND;
        commandNumber = RANDOMFLASHRGBCMCMD;
        isCommandValid = ProcessRandomFlashRGBCM();
    }
    else if(command == SEQUENCERGBCM)
    {
        kindOfCommand = SEQUENCECMCOMMAND;
        commandNumber = SEQUENCERGBCMCMD;
        isCommandValid = ProcessSequenceRGBCM();
    }
    else if(command == RELOADSEQUENCERGBCM)
    {
        kindOfCommand = SEQUENCECMCOMMAND;
        commandNumber = RELOADSEQUENCERGBCMCMD;
        isCommandValid = ProcessReloadSequenceRGBCM();
    }
    else if(command == BACKGROUNDRGB)
    {
        kindOfCommand = BACKGROUNDCOMMAND;
        commandNumber = BACKGROUNDRGBCMD;
        isBackground = true;
        isCommandValid = ProcessBackgroundRGB();
    }
    else if(command == BACKGROUNDREG)
    {
        kindOfCommand = BACKGROUNDCOMMAND;
        commandNumber = BACKGROUNDREGCMD;
        isBackground = true;
        isRGB = false;
        isCommandValid = ProcessBackgroundRegular();
    }
    else if(command == DISPLAYRANGEALEDS)
    {
        isRGB = false;
        isALEDStrip = true;
        isDisplayRange = true;
        kindOfCommand = ALEDSDISPLAYRANGE;
        commandNumber = DISPLAYRANGEALEDSCMD;
        isCommandValid = ProcessDisplayRange();
    }
    else if(command == FLASHALEDS)
    {
        isRGB = false;
        isALEDStrip = true;
        isStripFlash = true;
        kindOfCommand = ALEDSFLASHCMD;
        commandNumber = FLASHALEDSCMD;
        isCommandValid = ProcessStripFlash();
    }
    else if(command == RELOADFLASHALEDS)
    {
        isRGB = false;
        isALEDStrip = true;
        isStripFlash = true;
        kindOfCommand = ALEDSFLASHCMD;
        commandNumber = RELOADFLASHALEDSCMD;
        isCommandValid = ProcessStripPlayerFlash();
    }
    else if(command == DEATHFLASHALEDS)
    {
        isRGB = false;
        isALEDStrip = true;
        isStripFlash = true;
        kindOfCommand = ALEDSFLASHCMD;
        commandNumber = DEATHFLASHALEDSCMD;
        isCommandValid = ProcessStripPlayerFlash();
    }
    else if(command == RANDOMFLASHALEDS)
    {
        isRGB = false;
        isALEDStrip = true;
        isStripRndFlash = true;
        kindOfCommand = ALEDSRNDFLASH;
        commandNumber = RANDOMFLASHALEDSCMD;
        isCommandValid = ProcessStripRndFlash();
    }
    else if(command == SEQUENCEALEDS)
    {
        isRGB = false;
        isALEDStrip = true;
        isStripSequential = true;
        kindOfCommand = ALEDSSEQUENCECMD;
        commandNumber = SEQUENCEALEDSCMD;
        isCommandValid = ProcessStripSequential();
    }
    else if(command == RELOADSEQUENCEALEDS)
    {
        isRGB = false;
        isALEDStrip = true;
        isStripSequential = true;
        kindOfCommand = ALEDSSEQUENCECMD;
        commandNumber = RELOADSEQUENCEALEDSCMD;
        isCommandValid = ProcessStripPlayerSequential();
    }
}


bool LightCommand::CheckTimeOn(QString tOn)
{
    bool isNumber;

    timeOn = tOn.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Time on is not a number.\nFailing Number: "+tOn;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(isALEDStrip)
    {
        if(timeOn > ALEDSTRIPTIME || timeOn == 0)
            return false;
    }

    return true;
}

bool LightCommand::CheckTimeOff(QString tOff)
{
    bool isNumber;

    timeOff = tOff.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Time off is not a number.\nFailing Number: "+tOff;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(isALEDStrip)
    {
        if(timeOff > ALEDSTRIPTIME || timeOff == 0)
            return false;
    }

    return true;
}

bool LightCommand::CheckTimeDelay(QString tDelay)
{
    bool isNumber;

    timeDelay = tDelay.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Time off is not a number.\nFailing Number: "+tDelay;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(isALEDStrip)
    {
        if(timeDelay == 0 || timeDelay > ALEDSTRIPTIME)
            return false;
    }


    return true;
}

bool LightCommand::CheckNumberFlashes(QString numF)
{
    bool isNumber;

    numberFlashes = numF.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Number of flashes is not a number.\nFailing Number: "+numF;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(isALEDStrip)
    {
        if(numberFlashes > ALEDSTRIPMAXNUM)
            return false;
    }

    return true;
}

bool LightCommand::CheckPlayerNumber(QString pNum)
{
    bool isNumber;

    playerNumber = pNum.toUInt(&isNumber);

    if(!isNumber)
        return false;

    //Take one off of Player Number, P1 is 0
    playerNumber--;

    if(playerNumber >= MAXGAMEPLAYERS)
        return false;

    return true;
}

bool LightCommand::CheckHighCount(QString hCount)
{
    bool isNumber;

    highCount = hCount.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Number of flashes is not a number.\nFailing Number: "+numF;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    return true;
}


bool LightCommand::CheckBGTimeDelayReload(QString tDelay)
{
    bool isNumber;

    timeBGReload = tDelay.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Time off is not a number.\nFailing Number: "+tDelay;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    return true;
}

bool LightCommand::CheckMaxRange(QString maxRan)
{
    bool isNumber;

    maxRange = maxRan.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Number of flashes is not a number.\nFailing Number: "+numF;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    return true;
}

bool LightCommand::CheckNumberSteps(QString numSt)
{
    bool isNumber;

    numberSteps = numSt.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Number of flashes is not a number.\nFailing Number: "+numF;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(numberSteps > ALEDSTRIPMAXNUM)
        return false;

    return true;
}

bool LightCommand::CheckNumberLEDs(QString numLEDs)
{
    bool isNumber;

    numberLEDs = numLEDs.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Number of flashes is not a number.\nFailing Number: "+numF;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(numberLEDs > ALEDSTRIPMAXNUM+1)
        return false;

    return true;
}

bool LightCommand::CheckSeqReloadDR(QString seqR)
{
    bool isNumber;

    quint8 seqRNum = seqR.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Number of flashes is not a number.\nFailing Number: "+numF;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(seqRNum > 0)
        sequenceReload = true;
    else
        sequenceReload = false;

    return true;
}

bool LightCommand::CheckTimeDelayDR(QString tDelay)
{
    bool isNumber;

    timeDelay = tDelay.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Time off is not a number.\nFailing Number: "+tDelay;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(timeDelay == 0 || timeDelay > ALEDSTRIPTIME)
        return false;

    return true;
}

bool LightCommand::CheckStripFlashWait(QString sFW)
{
    bool isNumber;

    quint8 stripFW = sFW.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Number of flashes is not a number.\nFailing Number: "+numF;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(stripFW == 0)
        stripFlashWait = false;
    else
        stripFlashWait = true;

    return true;
}

bool LightCommand::CheckNumberLEDsSeq(QString numLED)
{
    bool isNumber;

    quint8 nLEDs = numLED.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Number of flashes is not a number.\nFailing Number: "+numF;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(nLEDs == 1 || nLEDs == 2 || nLEDs == 4 || nLEDs == 8)
    {
        sequenceNumLEDs = nLEDs;
        return true;
    }
    else
        return false;
}

bool LightCommand::CheckEnable2ndColor(QString en2ndC)
{
    bool isNumber;

    quint8 enable2nd = en2ndC.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Number of flashes is not a number.\nFailing Number: "+numF;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    if(enable2nd > 0)
        enable2ndColor = true;
    else
        enable2ndColor = false;

    return true;
}

bool LightCommand::CheckProbability2ndColor(QString prob2nd)
{
    bool isNumber;

    quint8 prob2ndNum = prob2nd.toUInt(&isNumber);

    if(!isNumber)
    {
        //QString failMeg = "Number of flashes is not a number.\nFailing Number: "+numF;
        //emit ShowErrorMessage(errorTitle, failMeg);
        return false;
    }

    // Check if Number is 2-12
    if(prob2ndNum > 1 && prob2ndNum < 13)
    {
        probability2nd = prob2ndNum;
        return true;
    }

    return false;
}


bool LightCommand::ProcessFlashRGB()
{
    bool check;

    color = commandArg[0];

    isColor = true;

    check = CheckTimeOn(commandArg[1]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[2]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[3]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}


bool LightCommand::ProcessReloadFlashRGB()
{
    bool check;

    color = commandArg[0];

    isColor = true;

    check = CheckTimeOn(commandArg[1]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[2]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[3]);

    if(!check)
        return false;

    check = CheckPlayerNumber(commandArg[4]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}

bool LightCommand::ProcessSequenceRGB()
{
    bool check;

    color = commandArg[0];

    isColor = true;

    check = CheckTimeDelay(commandArg[1]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}

bool LightCommand::ProcessReloadSequenceRGB()
{
    bool check;

    color = commandArg[0];

    isColor = true;

    check = CheckTimeDelay(commandArg[1]);

    if(!check)
        return false;

    check = CheckPlayerNumber(commandArg[2]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}

bool LightCommand::ProcessSequenceRGBCM()
{
    bool check;

    colorMapName = commandArg[0];

    isColorMap = true;

    check = CheckTimeDelay(commandArg[1]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}

bool LightCommand::ProcessReloadSequenceRGBCM()
{
    bool check;

    colorMapName = commandArg[0];

    isColorMap = true;

    check = CheckTimeDelay(commandArg[1]);

    if(!check)
        return false;

    check = CheckPlayerNumber(commandArg[2]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}


bool LightCommand::ProcessRandomFlashRGB()
{
    bool check;

    color = commandArg[0];

    isColor = true;

    check = CheckTimeOn(commandArg[1]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[2]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[3]);

    if(!check)
        return false;


    return true;
}

bool LightCommand::ProcessRandomFlash2CRGB()
{
    bool check;

    color = commandArg[0];

    isColor = true;

    sideColor = commandArg[1];

    isSideColor = true;

    check = CheckTimeOn(commandArg[2]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[3]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[4]);

    if(!check)
        return false;


    return true;
}

bool LightCommand::ProcessRandomFlashRGBCM()
{
    bool check;

    colorMapName = commandArg[0];

    isColorMap = true;

    check = CheckTimeOn(commandArg[1]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[2]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[3]);

    if(!check)
        return false;


    return true;
}





bool LightCommand::ProcessFollowerRGB()
{
    color = commandArg[0];

    isColor = true;

    return true;
}


bool LightCommand::ProcessFlashRegular()
{
    bool check;

    check = CheckTimeOn(commandArg[0]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[1]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[2]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}


bool LightCommand::ProcessPlayerFlashRegular()
{
    bool check;

    check = CheckTimeOn(commandArg[0]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[1]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[2]);

    if(!check)
        return false;

    check = CheckPlayerNumber(commandArg[3]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}


bool LightCommand::ProcessSequenceRegular()
{
    bool check;

    check = CheckTimeDelay(commandArg[0]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}

bool LightCommand::ProcessPlayerSequenceRegular()
{
    bool check;

    check = CheckTimeDelay(commandArg[0]);

    if(!check)
        return false;

    check = CheckPlayerNumber(commandArg[1]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}


bool LightCommand::ProcessRandomFlashRegular()
{
    bool check;

    check = CheckTimeOn(commandArg[0]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[1]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[2]);

    if(!check)
        return false;


    return true;
}

bool LightCommand::ProcessBackgroundRGB()
{
    bool check;
    quint8 i;

    colorMapName = commandArg[0];

    isColorMap = true;

    check = CheckPlayerNumber(commandArg[1]);

    if(!check)
        return false;

    check = CheckTimeDelay(commandArg[2]);

    if(!check)
        return false;

    check = CheckBGTimeDelayReload(commandArg[3]);

    if(!check)
        return false;

    check = CheckHighCount(commandArg[4]);

    if(!check)
        return false;


    if(commandArg.count() > 5)
    {
        bool isNumber;

        for(i = 5; i < commandArg.count(); i++)
        {
            quint8 tempGroupNum = commandArg[i].toUInt (&isNumber);

            if(!isNumber)
                return false;

            otherBGGroups << tempGroupNum;
        }
    }

    //Ran the Gauntlet
    return true;
}

bool LightCommand::ProcessBackgroundRegular()
{
    bool check;
    quint8 i;

    check = CheckPlayerNumber(commandArg[0]);

    if(!check)
        return false;

    check = CheckTimeDelay(commandArg[1]);

    if(!check)
        return false;

    check = CheckBGTimeDelayReload(commandArg[2]);

    if(!check)
        return false;

    check = CheckHighCount(commandArg[3]);

    if(!check)
        return false;


    if(commandArg.count() > 4)
    {
        bool isNumber;

        for(i = 4; i < commandArg.count(); i++)
        {
            quint8 tempGroupNum = commandArg[i].toUInt (&isNumber);

            if(!isNumber)
                return false;

            otherBGGroups << tempGroupNum;
        }
    }

    //Ran the Gauntlet
    return true;
}


bool LightCommand::ProcessDisplayRange()
{
    bool check;

    check = CheckMaxRange(commandArg[0]);

    if(!check)
        return false;

    check = CheckNumberSteps(commandArg[1]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[2]);

    if(!check)
        return false;

    colorMapName = commandArg[3];

    isColorMap = true;

    check = CheckSeqReloadDR(commandArg[4]);

    if(!check)
        return false;

    if(sequenceReload)
    {

        check = CheckTimeDelayDR(commandArg[5]);

        if(!check)
            return false;

        check = CheckNumberLEDsSeq(commandArg[6]);

        if(!check)
            return false;
    }
    else
    {
        timeDelay = 0;
        sequenceNumLEDs = 0;
    }

    //Ran the Gauntlet
    return true;
}


bool LightCommand::ProcessStripFlash()
{
    bool check;

    check = CheckTimeOn(commandArg[0]);

    //qDebug() << "Time On Check" << check;

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[1]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[2]);

    if(!check)
        return false;

    color = commandArg[3];

    isColor = true;

    check = CheckStripFlashWait(commandArg[4]);

    if(!check)
        return false;


    //Ran the Gauntlet
    return true;
}


bool LightCommand::ProcessStripPlayerFlash()
{
    bool check;

    check = CheckTimeOn(commandArg[0]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[1]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[2]);

    if(!check)
        return false;

    color = commandArg[3];

    isColor = true;

    check = CheckPlayerNumber(commandArg[4]);

    if(!check)
        return false;

    check = CheckStripFlashWait(commandArg[5]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}


bool LightCommand::ProcessStripRndFlash()
{
    bool check;

    check = CheckNumberLEDs(commandArg[0]);

    if(!check)
        return false;

    check = CheckTimeOn(commandArg[1]);

    if(!check)
        return false;

    check = CheckTimeOff(commandArg[2]);

    if(!check)
        return false;

    check = CheckNumberFlashes(commandArg[3]);

    if(!check)
        return false;

    color = commandArg[4];

    isColor = true;

    check = CheckEnable2ndColor(commandArg[5]);

    if(!check)
        return false;

    if(enable2ndColor)
    {

        check = CheckProbability2ndColor(commandArg[6]);

        if(!check)
            return false;

        sideColor = commandArg[7];
        isSideColor = true;
    }

    //Ran the Gauntlet
    return true;
}

bool LightCommand::ProcessStripSequential()
{
    bool check;

    check = CheckTimeDelay(commandArg[0]);

    if(!check)
        return false;


    check = CheckNumberLEDsSeq(commandArg[1]);

    if(!check)
        return false;

    color = commandArg[2];

    isColor = true;



    //Ran the Gauntlet
    return true;
}

bool LightCommand::ProcessStripPlayerSequential()
{
    bool check;

    check = CheckTimeDelay(commandArg[0]);

    if(!check)
        return false;

    check = CheckNumberLEDsSeq(commandArg[1]);

    if(!check)
        return false;

    color = commandArg[2];

    isColor = true;


    check = CheckPlayerNumber(commandArg[3]);

    if(!check)
        return false;

    //Ran the Gauntlet
    return true;
}
