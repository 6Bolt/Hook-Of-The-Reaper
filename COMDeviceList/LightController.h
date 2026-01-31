#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QDebug>
//#include <QMessageBox>
//#include <QTimer>
//#include <QRandomGenerator>



#include "LightExecution.h"

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

    quint8 GetID() { return id; }




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


    //Builds The Groups Array Positions and Array Byte Data

    //Build Regular Lights Group Byte Data if Bit Banging
    void BuildRegularGroupData();

    //Build RGB Lights Group Byte Data if Bit Banging
    void BuildRGBGroupData();

    //Prints out the Groups
    void PrintGroupData();


    //Regular Group Checking and Setting/Unsetting

    //Check Regular Groups Against Used Pins
    bool CheckRegularGroups(QList<quint8> groupNumber);

    //Set Regular Groups Against Used Pins
    void SetRegularGroups(QList<quint8> groupNumber);

    //Unset Regular Groups Against Used Pins
    void UnsetRegularGroups(QList<quint8> groupNumber);


    //RGB Group Checking and Setting/Unsetting

    //Check RGB Groups Against Used Pins
    bool CheckRGBGroups(QList<quint8> groupNumber);

    //Set RGB Groups Against Used Pins
    void SetRGBGroups(QList<quint8> groupNumber);

    //Unset RGB Groups Against Used Pins
    void UnsetRGBGroups(QList<quint8> groupNumber);



    //Light Displays Functions

    //Flash Commands

    //Flash Regular Lights
    void FlashRegularLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity);

    //Flash RGB Lights
    void FlashRGBLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color);

    //Flash Random Regular Light
    void FlashRandomRegularLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity);

    //Flash Random RGB Lights
    void FlashRandomRGBLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color);

    //Flash Random Regular Lights with 2 Intensities
    void FlashRandomRegular2ILights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, quint8 intensity, quint8 sIntensity);

    //Flash Random RGB Lights with 2 Colors
    void FlashRandomRGB2CLights(QList<quint8> grpNumList, quint16 timeOnMs, quint16 timeOffMs, quint8 numFlashes, QString color, QString sColor);


    //Sequence Commands

    //Sequence Regular Lights, Light Up Lights One by One
    void SequenceRegularLights(QList<quint8> grpNumList, quint16 delay, quint8 intensity);

    //Sequence RGB Lights, Light Up Lights One by One
    void SequenceRGBLights(QList<quint8> grpNumList, quint16 delay, QString color);


    //Follower Commands

    //Follower Regular Lights
    void FollowerRegularLights(QList<quint8> grpNumList, quint8 intensity, quint16 data);

    //Follower RGB Lights, Light up if data is > 0
    void FollowerRGBLights(QList<quint8> grpNumList, QString color, quint16 data);

    //Follower Random RGB Lights, Light up if data is > 0
    void FollowerRandomRGBLights(QList<quint8> grpNumList, quint16 data);


    //Connect Execution Signals and Slots

    //Connect for Regular Lights
    void ConnectRegular(quint8 index);

    //Connect for RGB Lights
    void ConnectRGB(quint8 index);


    //Disconnect Execution Signals and Slots

    //Disconnect for Regular Lights
    void DisconnectRegular(quint8 index);

    //Disconnect for RGB Lights
    void DisconnectRGB(quint8 index);



public slots:


    //Regular Lights Changing Intensity

    //Set Intensity for Regular Light Groups
    void ShowRegularIntensity(QList<quint8> grpNumList, quint8 intensity);

    //Set 1 Pins in Group Intensity
    void ShowRegularIntensityOne(QList<quint8> grpNumList, quint8 intensity, QList<quint8> indexList, qint8 offset);

    //Set 1 Pins in Group Intensity
    void ShowRegularIntensityOneSequence(QList<quint8> grpNumList, quint8 intensity, quint8 index);


    //RGB Lights Changing Intensity

    //Turn on RGB Groups with a RGB Color
    void ShowRGBColor(QList<quint8> grpNumList, RGBColor color);

    //Turn on 0ne RGB set, in each Group
    void ShowRGBColorOne(QList<quint8> grpNumList, RGBColor color, QList<quint8> indexList, qint8 offset);

    //Turn on 0ne RGB set, in each Group, for Sequence
    void ShowRGBColorOneSequence(QList<quint8> grpNumList, RGBColor color, quint8 index);


    //Change States for RGB and Regular

    //Turn State for a Group of Regular
    void TurnRegularState(QList<quint8> grpNumList, bool state);

    //Turn State for a Group of RGB
    void TurnRGBState(QList<quint8> grpNumList, bool state);


    //When an Execution has Finished

    //Regular Lights Execution has Finished
    void RegularExecutionFinished(quint8 exeNum, QList<quint8> grpNumList);

    //RGB Lights Execution has Finished
    void RGBExecutionFinished(quint8 exeNum, QList<quint8> grpNumList);



public:

    //Check Regular and RGB Groups Exists or Not

    //Check Group Number for Regular
    bool CheckRegularGroupNumber(quint8 grpNum);

    //Check Group Number for RGB
    bool CheckRGBGroupNumber(quint8 grpNum);



    //Reset Light Controller when Game Ends

    //Reset Light Controller when Game Ends
    void ResetLightController();

    //Set the Do Reset Flag to Reset after Command
    void DoReset() { doReset = true; }



signals:

    //Signals to PacDriveControl to Change Intensity or State

    //Set 1 Light Intensity
    void SetLightIntensity(quint8 id, quint8 pin, quint8 intensity);

    //Set a Group of Pins Light Intensity
    void SetLightIntensityGroup(quint8 id, QList<quint8> group, quint8 intensity);

    //Set RGB Lights Intensity
    void SetRGBLightIntensity(const quint8 &id, const RGBPins &pins, const RGBColor &color);

    //Set Pin State
    void SetPinState(quint8 id, quint8 pin, bool state);


    //Signal to Show Error Message

    //Show Error Message Box in Main Thread
    void ShowErrorMessage(const QString &title, const QString &message);


private slots:






private:

    //Private Varibles

    //Light Controller Info

    //Light Controller List Number
    quint8                          lightCntlrNum;

    //Maker of Light Controller
    quint8                          lightCntlrMaker;

    //Ultimarc Data Struct
    UltimarcData                    dataUltimarc;

    //Ultimarc ID for Device
    int                             id;

    //Group File with Path
    QString                         groupFilePath;

    //Number of Pins on Device
    quint8                          numberPins;

    //Number of Group Byte in an Array
    quint8                          numberGroups;

    //Number of Regular LEDs
    quint8                          numberLEDs;

    //Number of RGB LEDs
    quint8                          numberRGBLEDs;

    //Max Brightness of LEDs
    quint8                          maxBrightness;

    //Tells Light Controller to Reset when Command Finishes
    bool                            doReset;

    //Off Color
    RGBColor                        rgbOff;




    //Execution List

    //Execution Array
    LightExecution*                 p_execution[NUMBEREXECUTIONS];

    //Where the Next Execution Goes
    quint8                          executionCount;





    //Group File Data

    //Group File Loaded Correctly
    bool                            didGroupFileLoad;

    //If RGB Fast Mode is Enabled
    bool                            rgbFastMode;

    //If No Normal LEDs Group
    bool                            noRegularGroups;

    //If No RGB LEDs Group
    bool                            noRGBGroups;

    //Groups Regular LED Map
    QMap<quint8,QList<quint8>>      regularLEDMap;

    //Groups RGB LED Map
    QMap<quint8,QList<quint8>>      rgbLEDMap;

    //Regular LED Group List
    QList<quint8>                   regularLEDGroupList;

    //RGB LED Group List
    QList<quint8>                   rgbLEDGroupList;

    //RGB 8bit Color Map
    QMap<QString,RGBColor>          rgbColorMap;

    //String List of Colors
    QStringList                     colorNameList;

    //Number of Colors Stored
    quint16                         numberColors;

    //Used Pins List
    QList<quint8>                   usedPinsList;

    //Not Used

    //Brightness for Regular LED Groups
    //QMap<quint8,quint8>             regularBrightnessMap;

    //Brightness for RGB LED Groups
    //QMap<quint8,quint8>             rgbBrightnessMap;

    //Number of Regular Pins in a Group
    //QMap<quint8,quint8>             regularPinsCount;

    //Number of RGB Pins in a Group
    //QMap<quint8,quint8>             rgbPinsCount;





    //Check if Pins are Being Used

    //Used Pins, Being Used in a Command
    quint8                           *p_usedPins;


    //Group Array Position and Byte Data

    //Regular Lights Group - Array Position Map
    QMap<quint8,QList<quint8>>      regularArrayPosition;

    //Regular Lights Group - Array Data
    QMap<quint8,QList<quint8>>      regularArrayData;

    //RGB Lights Group - Array Position Map
    QMap<quint8,QList<quint8>>      rgbArrayPosition;

    //RGB Lights Group - Array Data
    QMap<quint8,QList<quint8>>      rgbArrayData;




    //Commands Varible

    //Sequence RGB
    quint8                          sequenceCount;
    quint8                          sequenceMaxCount;
    bool                            sequenceMaxCountSet;

    //Follower RGB
    RGBColor                        rgbFollowerColor;


    //Misc

    //Error Title
    QString                         title;

};

#endif // LIGHTCONTROLLER_H
