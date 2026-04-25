#ifndef ALEDSTRIPLC_H
#define ALEDSTRIPLC_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QDebug>
//#include <QMessageBox>
//#include <QTimer>
#include <QRandomGenerator>
#include <QThread>


#include "LightController.h"
//#include "LightExecution.h"
//#include "LightBackground.h"

#include "../Global.h"

//Need Windows for the Serial Port COM
#include <Windows.h>

// Serial Port Commands

// End Termination
#define ENDCHAR             '|'

//Check if HOTR ALED Strip Controller
#define HOTRCHECK           '='
#define HOTRCHECKMSGSU      "69DE"
#define HOTRCHECKMSGASU     "DE69"

// Setting Up LED Strings
#define LEDSTRINGNUMBER     '!'
#define LEDSTRINGELEMENTS   '#'

// Go Back to LED String Set-up
#define BACKTOSETUP         '_'

// Setting Up the Display Range, for a LED String
#define DISPLAYRANGE        '$'
#define DISPLAYRANGEMAX     '?'
#define STEPNUMBERS         '%'
#define RELOADSEQ           ','
#define RELOADNUMLEDS       '.'

// Set Up the Color
#define REDCOLOR            '*'
#define GREENCOLOR          '('
#define BLUECOLOR           ')'

// Update A Display Range
#define UPDATEDISPLAYRANGE  ':'

// Setting Up the Flash, for a LED String
#define FLASHSTRING         ';'
#define TIMEON              '<'
#define TIMEOFF             '>'
#define FLASHTIMES          '^'

#define FLASH               'F'
#define FLASHWAIT           'W'

// Setting Up the Random Flash, for a LED String
#define RNDFLASHSTRING      '&'
#define RNDFLASHNUMLED      '['

#define RNDFLASH2NDCOLOR    '{'
#define RNDFLSH2NDNUMBER    '}'

#define RNDFLASH            'R'

// Setting Up the Sequential, for a LED String
#define SEQUENTIALSTRING    ']'

#define SEQUENTIAL          'S'


// When A Supported Game Has Started and All Display Data Has Been Updated
#define GAMESTARTSTRIP      '+'

// When A Supported Game Has Ended and Release All Data
#define GAMEENDED           '-'

// Select What Pattern to Display when Not In a Supported Game
#define SELECTPATTERN       '@'


#define NUMBEROFSTRUCTS     10


class ALEDStripLC : public LightController
{
    Q_OBJECT

public:

    explicit ALEDStripLC(quint8 cntlrNum, quint8 comNum, QString comNm, SerialPortInfo spInfo, quint8 stripCount, QList<quint16> elementsCounts, quint8 pat, QString grpFile, QObject *parent = nullptr);

    virtual ~ALEDStripLC();


    bool ConnectCOMPort();

    bool ReconnectCOMPort();

    void CloseCOMPort();

    bool COMWrite(QByteArray writeData);

    QString COMRead(bool *didRead);

    void SetUpALEDStrips();

    void RedoSetUpALEDStrips() override;

    bool SetGroupFile(QString filePath) override;

    void UpdateALEDPattern() override;

    void GameStarted() override;

    void GameEnded() override;

    void SetUpDisplayRange(QList<quint8> stps, quint16 mRange, quint8 numSteps, quint16 tOff, QString cMap, bool enSeqR, quint16 tDelay, quint8 numLEDs) override;

    void UpdateDisplayRange(QList<quint8> stps, quint16 value) override;

    void SetUpStripFlash(quint8 structN, quint8 stp, quint16 timeOn, quint16 timeOff, quint8 numFlash, QString color) override;

    void DoStripFlash(quint8 structN) override;

    void DoStripFlashWait(quint8 structN) override;

    void SetUpStripRndFlash(quint8 structN, quint8 stp, quint8 numLEDs, quint16 timeOn, quint16 timeOff, quint8 numFlash, QString color, bool enable2nd, quint8 prob, QString color2) override;

    void DoStripRndFlash(quint8 structN) override;

    void SetUpStripSequential(quint8 structN, quint8 stp, quint16 timeDelay, QString color, quint8 numLEDs) override;

    void DoStripSequential(quint8 structN) override;

signals:




private:

    //Pointer Array of Serial COM Ports
    HANDLE                          comPort;

    //COM Port Data
    DCB                             comPortDCB = {0};
    COMMTIMEOUTS                    comPortTO = {0};

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    LPCWSTR                         comPortLPC;
    LPCWSTR                         comPortJustLPC;
#else
    LPCSTR                          comPortLPC;
#endif

    bool                            isCOMPortOpen;

    quint8                          flashCount;
    quint8                          rndFlashCount;
    quint8                          sequentialCount;

    quint16                         displayRangeMax[MAXNUMALEDSTRIPS];
    quint16                         displayRangeValue[MAXNUMALEDSTRIPS];

};

#endif // ALEDSTRIPLC_H
