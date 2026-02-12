#ifndef LIGHTCOMMAND_H
#define LIGHTCOMMAND_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QMessageBox>



//class LightCommand : public QObject
class LightCommand
{
//    Q_OBJECT

public:

    LightCommand();
    LightCommand(QString outputSig, QString cmd, QStringList cmdArgs, QMap<quint8,QList<quint8>> cntlrsGrps);

    //explicit LightCommand(QObject *parent = nullptr);
    //explicit LightCommand(QString outputSig, QString cmd, QStringList cmdArgs, QMap<quint8,QList<quint8>> cntlrsGrps, QObject *parent = nullptr);

    //LightCommand(LightCommand const &other);

    //explicit LightCommand(QString outputSig, QString cmd, QStringList cmdArgs, QMap<quint8,QList<quint8>> cntlrsGrps, QObject *parent = nullptr);
    //explicit LightCommand(LightCommand const &other, QObject *parent = nullptr);



    bool IsCommandValid() { return isCommandValid; }
    bool IsRGB() { return isRGB; }
    QString GetCommand() { return command; }
    quint16 GetTimeOn() { return timeOn; }
    quint16 GetTimeOff() { return timeOff; }
    quint16 GetTimeDelay() { return timeDelay; }
    QString GetColor() { return color; }
    QString GetSideColor() { return sideColor; }
    quint8 GetNumberFlashes() { return numberFlashes; }
    quint8 GetKindOfCommand() { return kindOfCommand; }
    quint8 GetCommandNumber() { return commandNumber; }
    quint8 GetNumberControllers() { return numberCntlrs; }
    quint8 GetPlayerNumber() { return playerNumber; }
    QString GetColorMap() { return colorMapName; }
    bool IsGeneralCommand() { return isGeneralCommand; }
    bool IsBackgroundCommand() { return isBackground; }
    quint8 GetHighCount() { return highCount; }
    quint16 GetBGTimeDelayReload() { return timeBGReload; }

    bool IsColor() { return isColor; }
    bool IsSideColor() { return isSideColor; }
    bool IsColorMap() { return isColorMap; }


    quint8 GetControllerNumber(quint8 index) { return listCntlrs[index]; }
    QList<quint8> GetControllerGroups(quint8 index) { return cntlrsGroup[index]; }

    QList<quint8> GetOtherBGGroups() { return otherBGGroups; }


    void SetOutputSignal(QString outputSig) { outputSignal = outputSig; }

    void SetInvertData() { isInvertData = true; }
    bool IsInvertData() { return isInvertData; }

    void ProcessLightCommand();

    //Check Arguments
    bool CheckTimeOn(QString tOn);
    bool CheckTimeOff(QString tOff);
    bool CheckTimeDelay(QString tDelay);
    bool CheckNumberFlashes(QString numF);
    bool CheckPlayerNumber(QString pNum);
    bool CheckHighCount(QString hcount);
    bool CheckBGTimeDelayReload(QString tDelay);

    //Check Command's Arguments

    //Flash RGB
    bool ProcessFlashRGB();
    bool ProcessReloadFlashRGB();

    //Sequence RGB
    bool ProcessSequenceRGB();
    bool ProcessReloadSequenceRGB();
    bool ProcessSequenceRGBCM();
    bool ProcessReloadSequenceRGBCM();

    //Random Flash RGB
    bool ProcessRandomFlashRGB();
    bool ProcessRandomFlash2CRGB();
    bool ProcessRandomFlashRGBCM();

    //Follower RGB
    bool ProcessFollowerRGB();



    //Flash Regular
    bool ProcessFlashRegular();
    bool ProcessPlayerFlashRegular();

    //Sequence Regular
    bool ProcessSequenceRegular();
    bool ProcessPlayerSequenceRegular();

    //Random Flash Regular
    bool ProcessRandomFlashRegular();

    //Background RGB Command
    bool ProcessBackgroundRGB();







//signals:

    //Show Error Message Box in Main Thread
    //void ShowErrorMessage(const QString &title, const QString &message);

public:

    QString                             errorTitle;
    QString                             command;
    QStringList                         commandArg;
    QMap<quint8,QList<quint8>>          cntlrsGroup;
    QList<quint8>                       listCntlrs;
    quint8                              numberCntlrs;
    QString                             outputSignal;
    bool                                isCommandValid;
    bool                                isRGB;
    bool                                isGeneralCommand;
    bool                                isInvertData;
    bool                                isBackground;


    quint8                              kindOfCommand;
    quint8                              commandNumber;

    quint8                              numberFlashes;

    quint8                              playerNumber;

    quint16                             timeOn;
    quint16                             timeOff;
    quint16                             timeDelay;
    quint16                             timeBGReload;

    bool                                isColor;
    bool                                isSideColor;

    QString                             color;
    QString                             sideColor;

    bool                                isColorMap;
    QString                             colorMapName;

    quint8                              highCount;


    //Background
    QList<quint8>                       otherBGGroups;

};

#endif // LIGHTCOMMAND_H
