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


    //Timer Set-up
    p_timer = new QTimer(this);
    p_timer->setSingleShot (true);

}





//Light Commands


//Flash Commands

void LightExecution::FlashRegularLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity)
{
    numberFlash = numFlashes;
    timesFlashed = 0;
    flashTimeOn = timeOnMs;
    flashTimeOff = timeOffMs;
    flashIntensity = intensity;

    emit ShowRegularIntensity(groupList, flashIntensity);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();

    //Now Wait until timer runs out
}

void LightExecution::FlashRGBLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color)
{
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

void LightExecution::FlashRandomRegularLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity)
{
    quint8 i;
    numberFlash = numFlashes;
    timesFlashed = 0;
    flashTimeOn = timeOnMs;
    flashTimeOff = timeOffMs;
    flashIntensity = intensity;

    useRandomPins = true;

    //Find Random Pin for Groups
    for(i = 0; i < groupList.count(); i++)
    {
        quint8 count = ledMap[groupList[i]].count();

        quint8 randomPin = QRandomGenerator::global()->bounded(0, count);

        //But Random Number into List
        randomPins << randomPin;
    }

    emit ShowRegularIntensityOne(groupList, flashIntensity, randomPins, 0);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();

    //Now Wait until timer runs out
}

void LightExecution::FlashRandomRGBLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color)
{
    quint8 i;
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



//Random Flash 2 I/C Commands


void LightExecution::FlashRandomRegular2ILights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity, quint8 sIntensity)
{
    quint8 i;
    numberFlash = numFlashes;
    timesFlashed = 0;
    flashTimeOn = timeOnMs;
    flashTimeOff = timeOffMs;
    flashIntensity = intensity;
    flashSideIntensity = sIntensity;

    useRandomPins = true;
    useSideColor = true;

    //Find Random Pin for Groups
    for(i = 0; i < groupList.count(); i++)
    {
        quint8 count = ledMap[groupList[i]].count();
        quint8 randomPin;

        randomPin = QRandomGenerator::global()->bounded(1, count-1);

        randomPins << randomPin;

        //qDebug() << "Group:" << i << "Random Pin:" << randomPin << "Count" << count;
    }

    emit ShowRegularIntensityOne(groupList, flashIntensity, randomPins, 0);
    emit ShowRegularIntensityOne(groupList, flashSideIntensity, randomPins, 1);
    emit ShowRegularIntensityOne(groupList, flashSideIntensity, randomPins, -1);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();

    //Now Wait until timer runs out
}


void LightExecution::FlashRandomRGB2CLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color, RGBColor sColor)
{
    quint8 i;
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

void LightExecution::SequenceRegularLights(quint16 delay, quint8 intensity)
{
    sequenceDelay = delay;
    sequenceCount = 0;
    sequenceMaxCount = 1;
    sequenceMaxCountSet = false;
    sequenceIntensity = intensity;

    FindMaxSequence();

    emit ShowRegularIntensityOneSequence(groupList, sequenceIntensity, sequenceCount);

    //Connect Timer to Turn On Next LEDs in the Sequence
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularSequenceDelayDone()));
    p_timer->setInterval (delay);
    p_timer->start();
}


void LightExecution::SequenceRGBLights(quint16 delay, RGBColor color)
{
    sequenceDelay = delay;
    sequenceCount = 0;
    sequenceMaxCount = 1;
    sequenceMaxCountSet = false;
    rgbSequenceColor = color;

    FindMaxSequence();

    emit ShowRGBColorOneSequence(groupList, rgbSequenceColor, sequenceCount);

    //Connect Timer to Turn On Next LEDs in the Sequence
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

        if(!sequenceMaxCountSet)
        {
            if(count > sequenceMaxCount)
                sequenceMaxCount = count;
        }
    }

    sequenceMaxCountSet = true;
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

        //Command Executed
        emit CommandExecuted(executionNumber, groupList);
    }
}


void LightExecution::RegularFlashOff()
{
    //Unconnect the Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));

    if(useRandomPins)
    {
        emit ShowRegularIntensityOne(groupList, 0, randomPins, 0);

        if(useSideColor)
        {
            emit ShowRegularIntensityOne(groupList, 0, randomPins, 1);
            emit ShowRegularIntensityOne(groupList, 0, randomPins, -1);
        }
    }
    else
        emit ShowRegularIntensity(groupList, 0);

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
        {
            emit ShowRegularIntensityOne(groupList, flashIntensity, randomPins, 0);

            if(useSideColor)
            {
                emit ShowRegularIntensityOne(groupList, flashSideIntensity, randomPins, 1);
                emit ShowRegularIntensityOne(groupList, flashSideIntensity, randomPins, -1);
            }
        }
        else
            emit ShowRegularIntensity(groupList, flashIntensity);

        //Connect Timer to Turn Off LEDs
        connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
        p_timer->setInterval (flashTimeOn);
        p_timer->start();

        //Now Wait until timer runs out
    }
    else
    {
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
        emit ShowRegularIntensityOneSequence(groupList, sequenceIntensity, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        p_timer->start();
    }
    else
    {
        //Turn Off Lights
        emit ShowRegularIntensity(groupList, 0);

        //Disconnect the Timer
        disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RegularSequenceDelayDone()));

        //Command Executed
        emit CommandExecuted(executionNumber, groupList);
    }
}

