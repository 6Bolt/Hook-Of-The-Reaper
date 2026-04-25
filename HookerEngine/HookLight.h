#ifndef HOOKLIGHT_H
#define HOOKLIGHT_H

#include <QObject>
#include <QFile>

#include "../COMDeviceList/ComDeviceList.h"
#include "LightCommand.h"

class HookLight : public QObject
{
    Q_OBJECT


public:

    explicit HookLight(ComDeviceList *cdList, QString currentPath, QObject *parent = nullptr);

    ~HookLight();

    void UpdateSettings();

    bool LoadLightFile();

    bool CheckLightFile(QStringList file, quint16 lineCount);

    bool CheckLightFileSequence(quint8 lastLineType, QString lastLine, QString line);

    quint8 CheckCommandArgs(QString command);

    bool CheckExsitingCommands(QString outputSignal, LightCommand newCmd);

    void GameEnded();

public slots:

    void GameStarted(const QString &gameN);

    void GameStopped();

    void TCPSocketDisconnected();

    void ProcessSignal(const QString &signal, const QString &data);

    //Handle Error Message Box from LightCommand Class
    void ErrorMessage(const QString &title, const QString &message);

signals:

    //Gives the TCP Socket the Output Signals String List
    void GameStartSignals(const QStringList &outputSignals);

    //Show Error Message Box in Main Thread, from LightCommand
    void ShowErrorMessage(const QString &title, const QString &message);

public:

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList                       *p_comDeviceList;

    QString                             pathLightFiles;

    quint8                              numberLightCntlrs;

    QString                             gameName;

    QString                             defaultFilePath;

    QString                             lightFilePath;

    QMap<QString,QList<LightCommand>>   signalLightCommand;

    QStringList                         outputSignals;

    bool                                isGameFile;

    bool                                isFileLoaded;

    bool                                inGame;

    quint16                             ammoValue[MAXGAMEPLAYERS];
    quint16                             lifeValue[MAXGAMEPLAYERS];

    bool                                playerAlive[MAXGAMEPLAYERS];
    quint8                              maxLifeValue[MAXGAMEPLAYERS];
    quint8                              maxDamage[MAXGAMEPLAYERS];

    //Struct Counters for ALED Strips
    quint8                              flashCount[MAXLIGHTCONTROLLERS];
    quint8                              rndFlashCount[MAXLIGHTCONTROLLERS];
    quint8                              sequentialCount[MAXLIGHTCONTROLLERS];

};

#endif // HOOKLIGHT_H
