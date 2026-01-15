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

    errorTitle = "Light Game/Default File Error";

    numberCntlrs = cntlrsGroup.count ();

    listCntlrs = cntlrsGroup.keys();

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



bool LightCommand::ProcessFlashRGB()
{
    bool check;

    color = commandArg[0];

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


bool LightCommand::ProcessReloadFlashRGB()
{
    bool check;

    color = commandArg[0];

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

    sideColor = commandArg[1];

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
