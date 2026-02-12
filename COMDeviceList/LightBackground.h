#ifndef LIGHTBACKGROUND_H
#define LIGHTBACKGROUND_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>

//Global Stuff
#include "../Global.h"

class LightBackground : public QObject
{

    Q_OBJECT

public:
    explicit LightBackground(quint8 player, quint8 mode, quint8 grpNum, QMap<quint8,QList<quint8>> lightMap, bool rgb, bool rgbFast, QList<RGBColor> cMap, quint8 hCount, quint16 tDelay, quint16 rDelay, QObject *parent = nullptr);

    ~LightBackground();

    bool GetIsRGB() { return isRGB; }


    //Turn On Background
    void TurnOnBackGround();

    //Turn Off Background
    void TurnOffBackGround();

    //Reload Ammo or Health
    void ReloadBackground();

    void UpdateCount(quint16 ammo);

    //Flash Commands
    //Normal Flash

    //Flash Regular Lights
    void FlashRegularLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes);

    //Flash RGB Lights
    void FlashRGBLights(QList<quint8> grpList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color);

    //Random Flash

    //Flash Random Regular Light
    void FlashRandomRegularLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes);

    //Flash Random RGB Lights
    void FlashRandomRGBLights(QList<quint8> grpList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color);

    //Random Flash 2 C

    //Flash Random RGB Lights with 2 Colors
    void FlashRandomRGB2CLights(QList<quint8> grpList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color, RGBColor sColor);

    //Sequence Commands

    //Sequence Regular Lights, Light Up Lights One by One
    void SequenceRegularLights(quint16 delay);

    //Sequence RGB Lights, Light Up Lights One by One
    void SequenceRGBLights(QList<quint8> grpList, quint16 delay, RGBColor color);

    //Sequence RGB Lights, Light Up Lights One by One with Color Map
    void SequenceRGBLightsCM(QList<quint8> grpList, quint16 delay, QList<RGBColor> colorsMap);

    //Find Max Sequence Count for Regular & RGB Lights
    void FindMaxSequence();

    //Turn Off Lights and End
    void TurnOffLightsEnd();


private slots:

    //After Delay for RGB
    void FlashRGBLightsPost();
    void FlashRandomRGBLightsPost();
    void FlashRandomRGB2CLightsPost();
    void SequenceRGBLightsPost();
    void SequenceRGBLightsCMPost();

    //Slots for RGB Commands with Timer

    void RGBFlashOff();
    void RGBFlashOn();

    void RGBSequenceDelayDone();

    void RGBSequenceDelayDoneCM();

    //Slots for Regular Commands with Timer

    void RegularFlashOff();
    void RegularFlashOn();

    void RegularSequenceDelayDone();



signals:

    //Regular Lights Changing Intensity

    //Set State for Regular Light Groups
    void ShowRegularState(QList<quint8> grpNumList, bool state);

    //Set 1 Pin State
    void ShowRegularStateOne(QList<quint8> grpNumList, bool state, QList<quint8> indexList, qint8 offset);

    //Set 1 Pin State for Sequence
    void ShowRegularStateOneSequence(QList<quint8> grpNumList, bool state, quint8 index);



    //RGB Lights Changing Intensity

    //Turn on RGB Groups with a RGB Color
    void ShowRGBColor(QList<quint8> grpNumList, RGBColor color);

    //Turn on 0ne RGB set, in each Group
    void ShowRGBColorOne(QList<quint8> grpNumList, RGBColor color, QList<quint8> indexList, qint8 offset);

    //Turn on 0ne RGB set, in each Group, for Sequence
    void ShowRGBColorOneSequence(QList<quint8> grpNumList, RGBColor color, quint8 index);


private:

    //Player Number
    quint8                          playerNumber;

    //Different Background Modes
    //0 is normal background, 1 is Ammo, and 2 is Life
    bool                            ammoMode;
    bool                            lifeMode;

    //High Count, How much the count Goes Up
    quint16                         highCount;

    //High Count Times 2, for Big Count
    quint16                         highCountX2;

    //Big Count Mode, is when Reload or Life Start is 2x then High Count
    bool                            bigCount;

    //Group Lights that are On
    quint16                         groupLights;
    quint16                         oldGroupLights;

    //Time Delay, when Background Lights Go Off, then Delay, and then New Command
    quint16                         bgDelay;

    //List of the Group Pins
    QList<quint8>                   grpPins;
    quint8                          grpPinsCount;


    bool                            isCommandRunning;

    quint16                         ammoCount;
    quint16                         ammoPerLight;
    quint16                         shotsFired;

    bool                            doingReload;
    quint8                          reloadColorCount;
    quint16                         reloadDelay;

    //Timer

    //Timer for Commands
    QTimer                          *p_timer;
    RGBColor                        rgbOff;


    //Commands Varible

    //Generic
    bool                            isRGB;
    bool                            rgbFastMode;
    //Background Group List for Command
    quint8                          groupNumber;
    QList<quint8>                   groupList;
    //Groups LED Map
    QMap<quint8,QList<quint8>>      ledMap;
    //Is a Flash Command
    bool                            isFlash;
    bool                            isSequence;

    //Background RGB Color Map
    QList<RGBColor>                 bgColorMap;
    quint8                          bgColorMapCount;

    //RGB 8bit Color Map
    //QMap<QString,RGBColor>          colorMap;

    //RGB Color Map Map
    //QMap<QString,QStringList>       colorMapMap;

    QList<quint8>                   otherGroupList;

    //Flash Command
    //Generic
    quint8                          numberFlash;
    quint8                          timesFlashed;
    quint16                         flashTimeOn;
    quint16                         flashTimeOff;

    //Random Flash
    QList<quint8>                   randomPins;
    bool                            useRandomPins;
    bool                            useSideColor;

    //Color & Intensity
    RGBColor                        rgbFlashColor;
    RGBColor                        sideColor;
    quint8                          flashIntensity;
    quint8                          flashSideIntensity;

    //Sequence RGB
    //Generic
    quint16                         sequenceDelay;
    quint8                          sequenceCount;
    quint8                          sequenceMaxCount;

    QList<RGBColor>                 sequenceColorList;
    quint8                          sequenceColorListCount;
    quint8                          sequenceColorCount;

    //Color & Intensity
    RGBColor                        rgbSequenceColor;
    quint8                          sequenceIntensity;

};

#endif // LIGHTBACKGROUND_H
