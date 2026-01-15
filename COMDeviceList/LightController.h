#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTimer>
#include <QRandomGenerator>

#include "../Global.h"



class LightController : public QObject
{
    Q_OBJECT

public:

    explicit LightController(quint8 cntlrNum, LightController const &other, QObject *parent = nullptr);

    explicit LightController(quint8 cntlrNum, UltimarcData dataU, QObject *parent = nullptr);

    ~LightController();


    void CopyLightController(LightController const &lcMember);

    //Set and Get Light Controller List Number
    void SetLightCntlrNumber(quint8 lcNum) { lightCntlrNum = lcNum; }
    quint8 GetLightCntlrNumber() { return lightCntlrNum; }

    //Get Light Controller Maker
    quint8 GetLightCntlrMaker() { return lightCntlrMaker; }

    //Get Number of Regular LEDs
    quint8 GetNumberLEDs() { return numberLEDs;}

    //Get Number of RGBs
    quint8 GetNumberRGBLEDs() { return numberRGBLEDs;}


    //Get the Max Brightness
    quint8 GetMaxBrightness() { return maxBrightness; }

    //Get Ultimarc Data Struct
    UltimarcData GetUltimarcData() { return dataUltimarc; }

    //Get Group File and Path
    void SetGroupFile(QString filePath);
    QString GetGroupFile() { return groupFilePath; }

    bool DidGroupFileLoad() { return didGroupFileLoad; }

    //Loading Group File Functions

    //Load Group File
    void LoadGroupFile();

    //Load Group GRP_RGB_FAST
    bool LoadGroupRGBFast(quint16 lineNum, quint16 lineMax, QStringList fileData);

    //Load Group GRP_RGB
    bool LoadGroupRGB(quint16 lineNum, quint16 lineMax, QStringList fileData);

    //Load Group GRP Regular
    bool LoadGroupRegular(quint16 lineNum, quint16 lineMax, QStringList fileData);

    //Check Group, and Gets Group Number and Default Brightness
    LightControllerTop GetGroupNumber(quint8 groupMode, quint16 lineNum, QString lineData);

    //Get Group Pin Data
    LCPinData GetGroupPinData(quint8 lcKind, quint16 grpStart, quint16 grpEnd, quint16 lineMax, QStringList fileData);

    //Load RGB Color from Group File
    bool LoadRGBColor(QString line);



    //Light Displays Functions

    //Flash Regular Lights
    void FlashRegularLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes);

    //Flash RGB Lights
    void FlashRGBLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color);

    //Flash Random RGB Lights
    void FlashRandomRGBLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color);

    //Flash Random RGB Lights with 2 Colors
    void FlashRandomRGB2CLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color, QString sColor);

    //Sequence RGB Lights, Light Up Lights One by One
    void SequenceRGBLights(QList<quint8> grpNumList, quint16 delay, QString color);

    //Turn on RGB Groups with a RGB Color
    void ShowRGBColor(QList<quint8> grpNumList, RGBColor color);

    //Turn on 0ne RGB set, in each Group
    void ShowRGBColorOne(QList<quint8> grpNumList, RGBColor color, quint8 index);

    //Check Group Number for Regular
    bool CheckRegularGroupNumber(quint8 grpNum);

    //Check Group Number for RGB
    bool CheckRGBGroupNumber(quint8 grpNum);


signals:

    //Set 1 Light Intensity
    void SetLightIntensity(quint8 id, quint8 pin, quint8 intensity);

    //Set RGB Lights Intensity
    void SetRGBLightIntensity(const quint8 &id, const RGBPins &pins, const RGBColor &color);

    //Show Error Message Box in Main Thread
    void ShowErrorMessage(const QString &title, const QString &message);

private slots:

    void RGBFlashOff();
    void RGBFlashOn();

    void SequenceDelayDone();

    //Time out for Regular Timer
    //void RegularTimeOut();

    //Time out for RGB Timer
    //void RGBTimeOut();


private:

    //Private Varibles

    //Light Controller List Number
    quint8                          lightCntlrNum;

    //Maker of Light Controller
    quint8                          lightCntlrMaker;

    //Number of Regular LEDs
    quint8                          numberLEDs;

    //Number of RGB LEDs
    quint8                          numberRGBLEDs;

    //Max Brightness of LEDs
    quint8                          maxBrightness;

    //Ultimarc Data Struct
    UltimarcData                    dataUltimarc;

    //Ultimarc ID for Device
    int                             id;

    //Group File with Path
    QString                         groupFilePath;

    //Group File Loaded Correctly
    bool                            didGroupFileLoad;

    //Groups Regular LED Map
    QMap<quint8,QList<quint8>>      regularLEDMap;

    //Groups RGB LED Map
    QMap<quint8,QList<quint8>>      rgbLEDMap;

    //Number of RGB Pins in a Group
    QMap<quint8,quint8>             regularPinsCount;

    //Number of RGB Pins in a Group
    QMap<quint8,quint8>             rgbPinsCount;

    //Brightness for Regular LED Groups
    QMap<quint8,quint8>             regularBrightnessMap;

    //Brightness for RGB LED Groups
    QMap<quint8,quint8>             rgbBrightnessMap;

    //Regular LED Group List
    QList<quint8>                   regularLEDGroupList;

    //RGB LED Group List
    QList<quint8>                   rgbLEDGroupList;

    //RGB 8bit Color Map
    QMap<QString,RGBColor>          rgbColorMap;

    //Used Pins List
    QList<quint8>                   usedPinsList;

    //If RGB Fast Mode is Enabled
    bool                            rgbFastMode;

    //If No Normal LEDs Group
    bool                            noRegularGroups;

    //If No RGB LEDs Group
    bool                            noRGBGroups;


    //Timer for Regular Lights
    QTimer                          *p_regularTimer;

    //Timer for RGB Lights
    QTimer                          *p_rgbTimer;

    //If Command is is Running
    bool                            isCommandRunning;

    //Error Title
    QString                         title;

    //Flash RGB Group List
    //RGB LED Group List
    QList<quint8>                   rgbFlashGroupList;
    quint8                          rgbNumberFlash;
    quint8                          rgbTimesFlashed;
    quint16                         rgbFlashTimeOn;
    quint16                         rgbFlashTimeOff;
    RGBColor                        rgbFlashColor;
    RGBColor                        rgbOff;
    QList<quint8>                   randomPins;
    bool                            useRandomPins;
    bool                            useSideColor;
    RGBColor                        sideColor;


    //Sequence RGB
    quint16                         sequenceDelay;
    quint8                          sequenceCount;
    quint8                          sequenceMaxCount;
    bool                            sequenceMaxCountSet;
};

#endif // LIGHTCONTROLLER_H
