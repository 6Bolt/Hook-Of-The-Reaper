#include "LightBackground.h"

LightBackground::LightBackground(quint8 player, quint8 grpNum, QMap<quint8,QList<quint8>> lightMap, bool rgb, bool rgbFast, QList<RGBColor> cMap, quint8 hCount, quint16 tDelay, quint16 rDelay, QObject *parent)
    : QObject{parent}
{
    //backgroundNumber = bgNum;
    playerNumber = player;

    highCount = hCount;
    highCountX2 = (highCount << 1);
    bigCount = false;
    groupLights = 0;

    isRGB = rgb;
    rgbFastMode = rgbFast;

    groupNumber = grpNum;
    groupList << grpNum;

    ledMap = lightMap;

    bgColorMap = cMap;
    bgColorMapCount = bgColorMap.count();

    bgDelay = tDelay;
    reloadDelay = rDelay;

    rgbOff.r = 0;
    rgbOff.g = 0;
    rgbOff.b = 0;

    useRandomPins = false;
    useSideColor = false;

    isFlash = false;
    isSequence = false;

    isCommandRunning = false;

    grpPins = ledMap[groupNumber];
    grpPinsCount = grpPins.count();

    //Timer Set-up
    p_timer = new QTimer(this);
    p_timer->setSingleShot (true);


    ammoCount = 0;
    ammoPerLight = 1;
    shotsFired = 0;

    //qDebug() << "highCount:" << highCount << "Highcount2x" << highCountX2;
}

LightBackground::~LightBackground()
{
    //TurnOffLightsEnd();
}


void LightBackground::TurnOnBackGround()
{
    //qDebug() << "TurnOn - groupLights" << groupLights;
    quint8 i;
    quint8 bgIndex = 0;
    quint8 colorMapCount = 0;
    quint8 offset = 1;

    if(!rgbFastMode && isRGB)
        offset = 3;

    for(i = 0; i < grpPinsCount; i+=offset)
    {
        QList<quint8> index;
        index << i;

        if(bgIndex < groupLights)
        {
            if(isRGB)
                emit ShowRGBColorOne(groupList, bgColorMap[colorMapCount], index, 0);
            else
                emit ShowRegularStateOne(groupList, true, index, 0);
        }
        else
        {
            if(isRGB)
                emit ShowRGBColorOne(groupList, rgbOff, index, 0);
            else
                emit ShowRegularStateOne(groupList, false, index, 0);

        }
        bgIndex++;

        if(isRGB)
        {
            colorMapCount++;

            if(colorMapCount >= bgColorMapCount)
                colorMapCount = 0;
        }
    }
}

void LightBackground::TurnOffBackGround()
{
    if(isRGB)
        emit ShowRGBColor(groupList, rgbOff);
    else
        emit ShowRegularState(groupList, false);
}


void LightBackground::ReloadBackground()
{

    isSequence = true;
    isCommandRunning = true;

    otherGroupList = groupList;
    sequenceDelay = reloadDelay;
    sequenceCount = oldGroupLights;
    sequenceMaxCount = groupLights;

    if(isRGB)
    {
        sequenceColorCount = oldGroupLights;
        sequenceColorListCount = bgColorMapCount;
        sequenceColorList = bgColorMap;

        if(sequenceColorCount >= sequenceColorListCount)
            sequenceColorCount = 0;

        rgbSequenceColor = bgColorMap[sequenceColorCount];
    }

    if(!rgbFastMode && isRGB)
    {
        //reloadColorCount = oldGroupLights/3;
        //Times sequenceCount and sequenceCountMax by 3
        sequenceCount = (sequenceCount << 1) + sequenceCount;
        sequenceMaxCount = (sequenceMaxCount << 1) + sequenceMaxCount;
    }


    if(isRGB)
    {
        emit ShowRGBColorOneSequence(groupList, rgbSequenceColor, sequenceCount);
        //Connect Timer to Turn On Next LEDs in the Sequence
        connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBSequenceDelayDoneCM()));
    }
    else
    {
        emit ShowRegularStateOneSequence(groupList, true, sequenceCount);
        //Connect Timer to Turn On Next LEDs in the Sequence
        connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularSequenceDelayDone()));
    }

    p_timer->setInterval (reloadDelay);
    p_timer->start();
}



void LightBackground::UpdateCount(quint16 ammo)
{
    //Check for Reload
    if(ammo > ammoCount)
    {
        reloadCount = ammo;

        if(ammo >= highCountX2)
            bigCount = true;
        else
            bigCount = false;

        shotsFired = 0;
        oldGroupLights = groupLights;

        if(ammo < highCount)
            groupLights = ammo;
        else
            groupLights = highCount;

        if(bigCount)
            ammoPerLight = ammo / highCount;
        else
            ammoPerLight = 1;

        //qDebug() << "bigCount" << bigCount << "ammoPerLight" << ammoPerLight;

        if(!isCommandRunning)
        {
            doingReload = true;
            //reloadColorCount = oldGroupLights;
            ReloadBackground();
        }
    }
    else if(ammo < ammoCount)
    {
        shotsFired++;
        bool redoLights = false;

        if(shotsFired >= ammoPerLight)
        {
            if(bigCount)
            {
                shotsFired = 0;
                if(groupLights != 0)
                    groupLights--;
                redoLights = true;
            }
            else
            {
                quint16 ammoDelta = reloadCount - shotsFired;
                if(ammoDelta < groupLights)
                {
                    if(groupLights != 0)
                        groupLights--;
                    redoLights = true;
                }
            }
        }

        if(ammo == 0)
        {
            groupLights = 0;
            redoLights = true;
        }

        if(!isCommandRunning && redoLights)
            TurnOnBackGround();
    }

    ammoCount = ammo;
}

//Light Commands


//Flash Commands

void LightBackground::FlashRegularLights(QList<quint8> grpList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes)
{
    if(!isCommandRunning)
    {
        //Turn Off Background
        TurnOffBackGround();

        otherGroupList = grpList;

        isFlash = true;
        isCommandRunning = true;

        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;

        //Connect Timer to Turn Off LEDs
        connect(p_timer, SIGNAL(timeout()), this, SLOT(FlashRegularLightsPost()));
        p_timer->setInterval (bgDelay);
        p_timer->start();

        //Now Wait until timer runs out
    }
}

void LightBackground::FlashRGBLights(QList<quint8> grpList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color)
{
    if(!isCommandRunning)
    {
        //Turn Off Background
        TurnOffBackGround();

        otherGroupList = grpList;

        isFlash = true;
        isCommandRunning = true;

        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;
        rgbFlashColor = color;

        //emit ShowRGBColor(otherGroupList, rgbFlashColor);

        //Connect Timer to Turn Off LEDs
        connect(p_timer, SIGNAL(timeout()), this, SLOT(FlashRGBLightsPost()));
        p_timer->setInterval (bgDelay);
        p_timer->start();

        //Now Wait until timer runs out
    }
}


//Random Flash Commands

void LightBackground::FlashRandomRegularLights(QList<quint8> grpList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes)
{
    if(!isCommandRunning)
    {
        quint8 i;

        //Turn Off Background
        TurnOffBackGround();

        otherGroupList = grpList;

        isFlash = true;
        isCommandRunning = true;

        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;

        useRandomPins = true;
        randomPins.clear();

        //Find Random Pin for Groups
        for(i = 0; i < otherGroupList.count(); i++)
        {
            quint8 count = ledMap[otherGroupList[i]].count();

            quint8 randomPin = QRandomGenerator::global()->bounded(0, count);

            //But Random Number into List
            randomPins << randomPin;
        }

        //emit ShowRegularStateOne(otherGroupList, true, randomPins, 0);

        //Connect Timer to Turn Off LEDs
        connect(p_timer, SIGNAL(timeout()), this, SLOT(FlashRandomRegularLightsPost()));
        p_timer->setInterval (bgDelay);
        p_timer->start();

        //Now Wait until timer runs out
    }
}

void LightBackground::FlashRandomRGBLights(QList<quint8> grpList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color)
{
    if(!isCommandRunning)
    {
        //Turn Off Background
        TurnOffBackGround();

        quint8 i;
        isFlash = true;
        isCommandRunning = true;

        otherGroupList = grpList;
        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;
        rgbFlashColor = color;
        randomPins.clear();

        useRandomPins = true;

        //Find Random Pin for Groups
        for(i = 0; i < otherGroupList.count(); i++)
        {
            quint8 count = ledMap[otherGroupList[i]].count();

            if(!rgbFastMode)
                count = count / 3;

            quint8 randomPin = QRandomGenerator::global()->bounded(0, count);

            if(!rgbFastMode)
                randomPin = randomPin * 3;

            randomPins << randomPin;

            //qDebug() << "randomPin" << randomPin << "randomPins" << randomPins;
        }

        connect(p_timer, SIGNAL(timeout()), this, SLOT(FlashRandomRGBLightsPost()));
        p_timer->setInterval (bgDelay);
        p_timer->start();

        //Now Wait until timer runs out
    }
}


//Random Flash 2 C Commands



void LightBackground::FlashRandomRGB2CLights(QList<quint8> grpList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color, RGBColor sColor)
{
    if(!isCommandRunning)
    {
        //Turn Off Background
        TurnOffBackGround();

        quint8 i;
        isFlash = true;
        isCommandRunning = true;

        otherGroupList = grpList;
        numberFlash = numFlashes;
        timesFlashed = 0;
        flashTimeOn = timeOnMs;
        flashTimeOff = timeOffMs;
        rgbFlashColor = color;
        sideColor = sColor;

        useRandomPins = true;
        useSideColor = true;
        randomPins.clear();

        //Find Random Pin for Groups
        for(i = 0; i < otherGroupList.count(); i++)
        {
            quint8 count = ledMap[otherGroupList[i]].count();
            quint8 randomPin;

            if(!rgbFastMode)
                count = count / 3;

            randomPin = QRandomGenerator::global()->bounded(1, count-1);

            if(!rgbFastMode)
                randomPin = randomPin * 3;

            randomPins << randomPin;

            //qDebug() << "Group:" << i << "Random Pin:" << randomPin << "Count" << count;
        }

        connect(p_timer, SIGNAL(timeout()), this, SLOT(FlashRandomRGB2CLightsPost()));
        p_timer->setInterval (bgDelay);
        p_timer->start();

        //Now Wait until timer runs out
    }
}



//Sequence Commands

void LightBackground::SequenceRegularLights(QList<quint8> grpList, quint16 delay)
{
    if(!isCommandRunning)
    {
        //Turn Off Background
        TurnOffBackGround();

        isSequence = true;
        isCommandRunning = true;

        otherGroupList = grpList;
        sequenceDelay = delay;
        sequenceCount = 0;
        sequenceMaxCount = 1;

        FindMaxSequence();

        //Connect Timer to Turn On Next LEDs in the Sequence
        connect(p_timer, SIGNAL(timeout()), this, SLOT(SequenceRegularLightsPost()));
        p_timer->setInterval (bgDelay);
        p_timer->start();
    }
}


void LightBackground::SequenceRGBLights(QList<quint8> grpList, quint16 delay, RGBColor color)
{
    if(!isCommandRunning)
    {
        //Turn Off Background
        TurnOffBackGround();

        isSequence = true;
        isCommandRunning = true;

        otherGroupList = grpList;
        sequenceDelay = delay;
        sequenceCount = 0;
        sequenceMaxCount = 1;
        rgbSequenceColor = color;

        FindMaxSequence();

        //Wait for Background Delay
        connect(p_timer, SIGNAL(timeout()), this, SLOT(SequenceRGBLightsPost()));
        p_timer->setInterval (bgDelay);
        p_timer->start();
    }
}

void LightBackground::SequenceRGBLightsCM(QList<quint8> grpList, quint16 delay, QList<RGBColor> colorsMap)
{
    if(!isCommandRunning)
    {
        isSequence = true;
        isCommandRunning = true;

        otherGroupList = grpList;
        sequenceDelay = delay;
        sequenceCount = 0;
        sequenceMaxCount = 1;

        sequenceColorList = colorsMap;
        sequenceColorListCount = sequenceColorList.count();
        sequenceColorCount = 0;
        rgbSequenceColor = sequenceColorList[sequenceColorCount];

        FindMaxSequence();

        //Wait for Background Delay
        connect(p_timer, SIGNAL(timeout()), this, SLOT(SequenceRGBLightsCMPost()));
        p_timer->setInterval (bgDelay);
        p_timer->start();
    }
}


void LightBackground::FindMaxSequence()
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


void LightBackground::TurnOffLightsEnd()
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
        }
        else if(isSequence)
        {
            //Stop Timer
            p_timer->stop();

            //Turn Off Lights
            emit ShowRGBColor(groupList, rgbOff);
        }
        else
            TurnOffBackGround();
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
        }
        else if(isSequence)
        {
            //Stop Timer
            p_timer->stop();

            //Turn Off Lights
            emit ShowRegularState(groupList, false);
        }
        else
            TurnOffBackGround();
    }
}






void LightBackground::FlashRGBLightsPost()
{
    //Diconnect Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(FlashRGBLightsPost()));

    emit ShowRGBColor(otherGroupList, rgbFlashColor);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();
}


void LightBackground::FlashRandomRGBLightsPost()
{
    //Diconnect Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(FlashRandomRGBLightsPost()));

    emit ShowRGBColorOne(otherGroupList, rgbFlashColor, randomPins, 0);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();
}


void LightBackground::FlashRandomRGB2CLightsPost()
{
    //Diconnect Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(FlashRandomRGB2CLightsPost()));

    emit ShowRGBColorOne(otherGroupList, rgbFlashColor, randomPins, 0);
    emit ShowRGBColorOne(otherGroupList, sideColor, randomPins, 1);
    emit ShowRGBColorOne(otherGroupList, sideColor, randomPins, -1);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();
}

void LightBackground::SequenceRGBLightsPost()
{
    //Diconnect Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(SequenceRGBLightsPost()));

    emit ShowRGBColorOneSequence(otherGroupList, rgbSequenceColor, sequenceCount);

    //Connect Timer to Turn On Next LEDs in the Sequence
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBSequenceDelayDone()));
    p_timer->setInterval (sequenceDelay);
    p_timer->start();
}

void LightBackground::SequenceRGBLightsCMPost()
{
    //Diconnect Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(SequenceRGBLightsCMPost()));

    emit ShowRGBColorOneSequence(otherGroupList, rgbSequenceColor, sequenceCount);

    //Connect Timer to Turn On Next LEDs in the Sequence, to Next Color in List
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBSequenceDelayDoneCM()));
    p_timer->setInterval (sequenceDelay);
    p_timer->start();
}


void LightBackground::FlashRegularLightsPost()
{
    //Diconnect Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(FlashRegularLightsPost()));

    emit ShowRegularState(otherGroupList, true);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();

    //Now Wait until timer runs out
}


void LightBackground::FlashRandomRegularLightsPost()
{
    //Diconnect Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(FlashRandomRegularLightsPost()));

    emit ShowRegularStateOne(otherGroupList, true, randomPins, 0);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
    p_timer->setInterval (flashTimeOn);
    p_timer->start();

    //Now Wait until timer runs out
}

void LightBackground::SequenceRegularLightsPost()
{
    //Diconnect Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(SequenceRegularLightsPost()));

    emit ShowRegularStateOneSequence(otherGroupList, true, sequenceCount);

    //Connect Timer to Turn On Next LEDs in the Sequence
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularSequenceDelayDone()));
    p_timer->setInterval (sequenceDelay);
    p_timer->start();
}






void LightBackground::RGBFlashOff()
{
    //Unconnect the Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));

    if(useRandomPins)
    {
        emit ShowRGBColorOne(otherGroupList, rgbOff, randomPins, 0);

        if(useSideColor)
        {
            emit ShowRGBColorOne(otherGroupList, rgbOff, randomPins, 1);
            emit ShowRGBColorOne(otherGroupList, rgbOff, randomPins, -1);
        }
    }
    else
        emit ShowRGBColor(otherGroupList, rgbOff);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOn()));
    p_timer->setInterval (flashTimeOff);
    p_timer->start();
}

void LightBackground::RGBFlashOn()
{
    //Increament Flash Time when Off Happens
    timesFlashed++;

    //Unconnect the Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOn()));

    if(timesFlashed < numberFlash)
    {
        if(useRandomPins)
        {
            emit ShowRGBColorOne(otherGroupList, rgbFlashColor, randomPins, 0);

            if(useSideColor)
            {
                emit ShowRGBColorOne(otherGroupList, sideColor, randomPins, 1);
                emit ShowRGBColorOne(otherGroupList, sideColor, randomPins, -1);
            }
        }
        else
            emit ShowRGBColor(otherGroupList, rgbFlashColor);

        //Connect Timer to Turn Off LEDs
        connect(p_timer, SIGNAL(timeout()), this, SLOT(RGBFlashOff()));
        p_timer->setInterval (flashTimeOn);
        p_timer->start();

        //Now Wait until timer runs out
    }
    else
    {
        useRandomPins = false;
        useSideColor = false;
        isFlash = false;
        isCommandRunning = false;
        TurnOnBackGround();
    }
}


void LightBackground::RGBSequenceDelayDone()
{
    if(rgbFastMode)
        sequenceCount++;
    else
        sequenceCount = sequenceCount + 3;

    if(sequenceCount < sequenceMaxCount)
    {
        //qDebug() << "Sequence Timer Ended: sequence" << sequenceCount << "sequenceMaxCount" << sequenceMaxCount;
        //Light Up the Next Sequence
        emit ShowRGBColorOneSequence(otherGroupList, rgbSequenceColor, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        p_timer->start();
    }
    else
    {
        //Turn Off Lights and Turn On Background
        //emit ShowRGBColor(otherGroupList, rgbOff);
        TurnOnBackGround();

        //Disconnect the Timer
        disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RGBSequenceDelayDone()));

        //Turn Off isCommandRunning
        isCommandRunning = false;
        isSequence = false;
    }
}

void LightBackground::RGBSequenceDelayDoneCM()
{
    if(rgbFastMode)
        sequenceCount++;
    else
        sequenceCount = sequenceCount + 3;

    if(sequenceCount < sequenceMaxCount)
    {
        //Get Color From List
        sequenceColorCount++;

        if(sequenceColorCount >= sequenceColorListCount)
            sequenceColorCount = 0;

        rgbSequenceColor = sequenceColorList[sequenceColorCount];

        //qDebug() << "Sequence Timer Ended: sequence" << sequenceCount << "sequenceMaxCount" << sequenceMaxCount;
        //Light Up the Next Sequence
        emit ShowRGBColorOneSequence(otherGroupList, rgbSequenceColor, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        p_timer->start();
    }
    else
    {
        if(doingReload)
            doingReload = false;
        else
        {
            //Turn Off Lights
            //emit ShowRGBColor(otherGroupList, rgbOff);
            TurnOnBackGround();
        }

        //Disconnect the Timer
        disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RGBSequenceDelayDoneCM()));

        //Turn Off isCommandRunning
        isCommandRunning = false;
        isSequence = false;
    }
}



void LightBackground::RegularFlashOff()
{
    //Unconnect the Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));

    if(useRandomPins)
        emit ShowRegularStateOne(otherGroupList, false, randomPins, 0);
    else
        emit ShowRegularState(otherGroupList, false);

    //Connect Timer to Turn Off LEDs
    connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOn()));
    p_timer->setInterval (flashTimeOff);
    p_timer->start();
}


void LightBackground::RegularFlashOn()
{
    //Increament Flash Time when Off Happens
    timesFlashed++;

    //Unconnect the Timer
    disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOn()));

    if(timesFlashed < numberFlash)
    {
        if(useRandomPins)
            emit ShowRegularStateOne(otherGroupList, true, randomPins, 0);
        else
            emit ShowRegularState(otherGroupList, true);

        //Connect Timer to Turn Off LEDs
        connect(p_timer, SIGNAL(timeout()), this, SLOT(RegularFlashOff()));
        p_timer->setInterval (flashTimeOn);
        p_timer->start();

        //Now Wait until timer runs out
    }
    else
    {
        isCommandRunning = false;
        isFlash = false;
        useRandomPins = false;
        TurnOnBackGround();
    }
}

void LightBackground::RegularSequenceDelayDone()
{
    sequenceCount++;

    if(sequenceCount < sequenceMaxCount)
    {
        //qDebug() << "Sequence Timer Ended: sequence" << sequenceCount << "sequenceMaxCount" << sequenceMaxCount;
        //Light Up the Next Sequence
        emit ShowRegularStateOneSequence(otherGroupList, true, sequenceCount);

        //Connect Timer to Turn On Next LEDs in the Sequence
        p_timer->start();
    }
    else
    {
        if(doingReload)
            doingReload = false;
        else
        {
            //Turn Background Back On
            //emit ShowRegularState(otherGroupList, false);
            TurnOnBackGround();
        }

        //Disconnect the Timer
        disconnect(p_timer, SIGNAL(timeout()), this, SLOT(RegularSequenceDelayDone()));

        isSequence = false;
        isCommandRunning = false;
    }
}

