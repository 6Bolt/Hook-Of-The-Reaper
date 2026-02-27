#include "LightExecution.h"

LightExecution::LightExecution(quint8 exeNum, QList<quint8> grpNumList, QMap<quint8,QList<quint8>> lightMap, bool rgb, bool rgbFast, QObject *parent)
    : QObject{parent}
{

    executionNumber = exeNum;
    groupList = grpNumList;
    ledMap = lightMap;
    isRGB = rgb;
    rgbFastMode = rgbFast;

    rgbOff.r = 0;
    rgbOff.g = 0;
    rgbOff.b = 0;

    useRandomPins = false;
    useSideColor = false;

    isColorMap = false;

    isFlash = false;
    isSequence = false;


    //Timer Set-up
    p_timer = new QTimer(this);
    p_timer->setSingleShot (true);

}


LightExecution::~LightExecution()
{

}


//Light Commands


//Flash Commands

void LightExecution::FlashRegularLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes)
{
    isFlash = true;

    numberFlash = numFlashes;
    timesFlashed = 0;
    flashTimeOn = timeOnMs;
    flashTimeOff = timeOffMs;

    emit ShowRegularState(groupList, true);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();

    //Now Wait until timer runs out
}

void LightExecution::FlashRGBLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color)
{
    isFlash = true;

    numberFlash = numFlashes;
    timesFlashed = 0;
    flashTimeOn = timeOnMs;
    flashTimeOff = timeOffMs;
    rgbFlashColor = color;

    emit ShowRGBColor(groupList, rgbFlashColor);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();

    //Now Wait until timer runs out
}


//Random Flash Commands

void LightExecution::FlashRandomRegularLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes)
{
    quint8 i;
    isFlash = true;
    numberFlash = numFlashes;
    timesFlashed = 0;
    flashTimeOn = timeOnMs;
    flashTimeOff = timeOffMs;

    useRandomPins = true;


    //Find Random Pin for Groups
    for(i = 0; i < groupList.count(); i++)
    {
        quint8 count = ledMap[groupList[i]].count();

        quint8 randomPin = QRandomGenerator::global()->bounded(0, count);

        //But Random Number into List
        randomPins << randomPin;
    }

    emit ShowRegularStateOne(groupList, true, randomPins, 0);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();

    //Now Wait until timer runs out
}

void LightExecution::FlashRandomRGBLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color)
{
    quint8 i;
    isFlash = true;
    numberFlash = numFlashes;
    timesFlashed = 0;
    flashTimeOn = timeOnMs;
    flashTimeOff = timeOffMs;
    rgbFlashColor = color;

    useRandomPins = true;

    //Find Random Pin for Groups
    for(i = 0; i < groupList.count(); i++)
    {
        quint8 count = ledMap[groupList[i]].count();

        if(!rgbFastMode)
            count = count / 3;

        quint8 randomPin = QRandomGenerator::global()->bounded(0, count);

        if(!rgbFastMode)
            randomPin = randomPin * 3;

        randomPins << randomPin;
    }

    emit ShowRGBColorOne(groupList, rgbFlashColor, randomPins, 0);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();

    //Now Wait until timer runs out
}



//Random Flash 2 C Commands



void LightExecution::FlashRandomRGB2CLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color, RGBColor sColor)
{
    quint8 i;
    isFlash = true;
    numberFlash = numFlashes;
    timesFlashed = 0;
    flashTimeOn = timeOnMs;
    flashTimeOff = timeOffMs;
    rgbFlashColor = color;
    sideColor = sColor;

    useRandomPins = true;
    useSideColor = true;

    //Find Random Pin for Groups
    for(i = 0; i < groupList.count(); i++)
    {
        quint8 count = ledMap[groupList[i]].count();
        quint8 randomPin;

        if(!rgbFastMode)
            count = count / 3;

        randomPin = QRandomGenerator::global()->bounded(1, count-1);

        if(!rgbFastMode)
            randomPin = randomPin * 3;

        randomPins << randomPin;

        //qDebug() << "Group:" << i << "Random Pin:" << randomPin << "Count" << count;
    }

    emit ShowRGBColorOne(groupList, rgbFlashColor, randomPins, 0);
    emit ShowRGBColorOne(groupList, sideColor, randomPins, 1);
    emit ShowRGBColorOne(groupList, sideColor, randomPins, -1);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();

    //Now Wait until timer runs out
}



//Sequence Commands

void LightExecution::SequenceRegularLights(quint16 delay)
{
    isSequence = true;
    sequenceDelay = delay;
    sequenceCount = 0;
    sequenceMaxCount = 1;

    FindMaxSequence();

    emit ShowRegularStateOneSequence(groupList, true, sequenceCount);

    //Connect Timer to Turn On Next LEDs in the Sequence
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularSequenceDelayDone()));
    p_timer->setInterval (delay);
    p_timer->start();
}


void LightExecution::SequenceRGBLights(quint16 delay, RGBColor color)
{
    isSequence = true;
    sequenceDelay = delay;
    sequenceCount = 0;
    sequenceMaxCount = 1;
    rgbSequenceColor = color;

    FindMaxSequence();

    emit ShowRGBColorOneSequence(groupList, rgbSequenceColor, sequenceCount);

    //Connect Timer to Turn On Next LEDs in the Sequence
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBSequenceDelayDone()));
    p_timer->setInterval (delay);
    p_timer->start();
}

void LightExecution::SequenceRGBLightsCM(quint16 delay, QList<RGBColor> colorsMap)
{
    sequenceColorList = colorsMap;
    sequenceColorListCount = sequenceColorList.count();
    sequenceColorCount = 0;
    isSequence = true;
    sequenceDelay = delay;
    sequenceCount = 0;
    sequenceMaxCount = 1;
    isColorMap = true;

    rgbSequenceColor = sequenceColorList[sequenceColorCount];
    sequenceColorCount++;

    FindMaxSequence();

    emit ShowRGBColorOneSequence(groupList, rgbSequenceColor, sequenceCount);

    //Connect Timer to Turn On Next LEDs in the Sequence, to Next Color in List
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBSequenceDelayDone()));
    p_timer->setInterval (delay);
    p_timer->start();
}


void LightExecution::FindMaxSequence()
{
    quint8 i;

    //For Groups
    for(i = 0; i < groupList.count(); i++)
    {
        quint8 count = ledMap[groupList[i]].count();

        //qDebug() << "Group" << groupList[i] << "Count" << count;

        if(count > sequenceMaxCount)
            sequenceMaxCount = count;
    }
}


void LightExecution::TurnOffLightsEnd()
{
    if(isRGB)
    {
        if(isFlash)
        {
            //Stop Timer
            p_timer->stop();

            //Turn Off Lights
            if(useRandomPins)
            {
                emit ShowRGBColorOne(groupList, rgbOff, randomPins, 0);

                if(useSideColor)
                {
                    emit ShowRGBColorOne(groupList, rgbOff, randomPins, 1);
                    emit ShowRGBColorOne(groupList, rgbOff, randomPins, -1);
                }
            }
            else
                emit ShowRGBColor(groupList, rgbOff);

            //Command Executed
            emit CommandExecuted(executionNumber, groupList);
        }

        if(isSequence)
        {
            //Stop Timer
            p_timer->stop();

            //Turn Off Lights
            emit ShowRGBColor(groupList, rgbOff);

            //Command Executed
            emit CommandExecuted(executionNumber, groupList);
        }
    }
    else
    {
        if(isFlash)
        {
            //Stop Timer
            p_timer->stop();

            //Turn Off Lights
            if(useRandomPins)
                emit ShowRegularStateOne(groupList, false, randomPins, 0);
            else
                emit ShowRegularState(groupList, false);

            //Command Executed
            emit CommandExecuted(executionNumber, groupList);
        }

        if(isSequence)
        {
            //Stop Timer
            p_timer->stop();

            //Turn Off Lights
            emit ShowRegularState(groupList, false);

            //Command Executed
            emit CommandExecuted(executionNumber, groupList);
        }
    }
}





//Private Slots

void LightExecution::RGBFlashOff()
{
    //Unconnect the Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));

    if(useRandomPins)
    {
        emit ShowRGBColorOne(groupList, rgbOff, randomPins, 0);

        if(useSideColor)
        {
            emit ShowRGBColorOne(groupList, rgbOff, randomPins, 1);
            emit ShowRGBColorOne(groupList, rgbOff, randomPins, -1);
        }
    }
    else
        emit ShowRGBColor(groupList, rgbOff);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOn()));
    p_timer->setInterval (flashTimeOff);
    p_timer->start();
}

void LightExecution::RGBFlashOn()
{
    //Increament Flash Time when Off Happens
    timesFlashed++;

    //Unconnect the Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOn()));

    if(timesFlashed < numberFlash)
    {
        if(useRandomPins)
        {
            emit ShowRGBColorOne(groupList, rgbFlashColor, randomPins, 0);

            if(useSideColor)
            {
                emit ShowRGBColorOne(groupList, sideColor, randomPins, 1);
                emit ShowRGBColorOne(groupList, sideColor, randomPins, -1);
            }
        }
        else
            emit ShowRGBColor(groupList, rgbFlashColor);

        //Connect Timer to Turn Off LEDs
        connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
        p_timer->setInterval (flashTimeOn);
        p_timer->start();

        //Now Wait until timer runs out
    }
    else
    {
        isFlash = false;
        useRandomPins = false;
        useSideColor = false;
        emit CommandExecuted(executionNumber, groupList);
    }
}

void LightExecution::RGBSequenceDelayDone()
{
    if(rgbFastMode)
        sequenceCount++;
    else
        sequenceCount = sequenceCount + 3;

    if(sequenceCount < sequenceMaxCount)
    {
        if(isColorMap)
        {
            //Get Color From List
            rgbSequenceColor = sequenceColorList[sequenceColorCount];
            sequenceColorCount++;

            if(sequenceColorCount >= sequenceColorListCount)
                sequenceColorCount = 0;
        }

        //qDebug() << "Sequence Timer Ended: sequence" << sequenceCount << "sequenceMaxCount" << sequenceMaxCount;
        //Light Up the Next Sequence
        emit ShowRGBColorOneSequence(groupList, rgbSequenceColor, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        p_timer->start();
    }
    else
    {
        //Turn Off Lights
        emit ShowRGBColor(groupList, rgbOff);

        //Disconnect the Timer
        disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RGBSequenceDelayDone()));

        isSequence = false;
        isColorMap = false;

        //Command Executed
        emit CommandExecuted(executionNumber, groupList);
    }
}



void LightExecution::RegularFlashOff()
{
    //Unconnect the Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));

    if(useRandomPins)
        emit ShowRegularStateOne(groupList, false, randomPins, 0);
    else
        emit ShowRegularState(groupList, false);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOn()));
    p_timer->setInterval (flashTimeOff);
    p_timer->start();
}


void LightExecution::RegularFlashOn()
{
    //Increament Flash Time when Off Happens
    timesFlashed++;

    //Unconnect the Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOn()));

    if(timesFlashed < numberFlash)
    {
        if(useRandomPins)
            emit ShowRegularStateOne(groupList, true, randomPins, 0);
        else
            emit ShowRegularState(groupList, true);

        //Connect Timer to Turn Off LEDs
        connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
        p_timer->setInterval (flashTimeOn);
        p_timer->start();

        //Now Wait until timer runs out
    }
    else
    {
        isFlash = false;
        useRandomPins = false;

        //Command Executed
        emit CommandExecuted(executionNumber, groupList);
    }
}

void LightExecution::RegularSequenceDelayDone()
{
    sequenceCount++;

    if(sequenceCount < sequenceMaxCount)
    {
        //qDebug() << "Sequence Timer Ended: sequence" << sequenceCount << "sequenceMaxCount" << sequenceMaxCount;
        //Light Up the Next Sequence
        emit ShowRegularStateOneSequence(groupList, true, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        p_timer->start();
    }
    else
    {
        //Turn Off Lights
        emit ShowRegularState(groupList, false);

        //Disconnect the Timer
        disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RegularSequenceDelayDone()));

        isSequence = false;

        //Command Executed
        emit CommandExecuted(executionNumber, groupList);
    }
}

