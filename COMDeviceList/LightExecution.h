#ifndef LIGHTEXECUTION_H
#define LIGHTEXECUTION_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QList>
#include <QRandomGenerator>
//#include <QDebug>

//Global Stuff
#include "../Global.h"

class LightExecution : public QObject
{

    Q_OBJECT


public:

    explicit LightExecution(quint8 exeNum, QList<quint8> grpNumList, QMap<quint8,QList<quint8>> lightMap, bool rgb, bool rgbFast, QObject *parent = nullptr);



    bool GetIsRGB() { return isRGB; }

    //Flash Commands
    //Normal Flash

    //Flash Regular Lights
    void FlashRegularLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity);

    //Flash RGB Lights
    void FlashRGBLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color);

    //Random Flash

    //Flash Random Regular Light
    void FlashRandomRegularLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity);

    //Flash Random RGB Lights
    void FlashRandomRGBLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color);

    //Random Flash 2 I/C

    //Flash Random Regular Lights with 2 Intensities
    void FlashRandomRegular2ILights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity, quint8 sIntensity);

    //Flash Random RGB Lights with 2 Colors
    void FlashRandomRGB2CLights(quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, RGBColor color, RGBColor sColor);

    //Sequence Commands

    //Sequence Regular Lights, Light Up Lights One by One
    void SequenceRegularLights(quint16 delay, quint8 intensity);

    //Sequence RGB Lights, Light Up Lights One by One
    void SequenceRGBLights(quint16 delay, RGBColor color);

    //Find Max Sequence Count for Regular & RGB Lights
    void FindMaxSequence();




private slots:

    //Slots for RGB Commands with Timer

    void RGBFlashOff();
    void RGBFlashOn();

    void RGBSequenceDelayDone();


    //Slots for Regular Commands with Timer

    void RegularFlashOff();
    void RegularFlashOn();

    void RegularSequenceDelayDone();



signals:

    //Regular Lights Changing Intensity

    //Set Intensity for Regular Light Groups
    void ShowRegularIntensity(QList<quint8> grpNumList, quint8 intensity);

    //Set 1 Pins in Group Intensity
    void ShowRegularIntensityOne(QList<quint8> grpNumList, quint8 intensity, QList<quint8> indexList, qint8 offset);

    //Set 1 Pins in Group Intensity for Sequence
    void ShowRegularIntensityOneSequence(QList<quint8> grpNumList, quint8 intensity, quint8 index);



    //RGB Lights Changing Intensity

    //Turn on RGB Groups with a RGB Color
    void ShowRGBColor(QList<quint8> grpNumList, RGBColor color);

    //Turn on 0ne RGB set, in each Group
    void ShowRGBColorOne(QList<quint8> grpNumList, RGBColor color, QList<quint8> indexList, qint8 offset);

    //Turn on 0ne RGB set, in each Group, for Sequence
    void ShowRGBColorOneSequence(QList<quint8> grpNumList, RGBColor color, quint8 index);


    //Command Has Been Executed and Finished
    void CommandExecuted(quint8 exeNum, QList<quint8> grpNumList);


private:


    quint8                          executionNumber;


    //Timer

    //Timer for Commands
    QTimer                          *p_timer;
    RGBColor                        rgbOff;


    //Commands Varible

    //Generic
    bool                            isRGB;
    bool                            rgbFastMode;
    //Group List for Command
    QList<quint8>                   groupList;
    //Groups LED Map
    QMap<quint8,QList<quint8>>      ledMap;

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
    bool                            sequenceMaxCountSet;

    //Color & Intensity
    RGBColor                        rgbSequenceColor;
    quint8                          sequenceIntensity;



};

#endif // LIGHTEXECUTION_H
