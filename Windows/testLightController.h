#ifndef TESTLIGHTCONTROLLER_H
#define TESTLIGHTCONTROLLER_H

#include <QDialog>


#include "../COMDeviceList/ComDeviceList.h"

namespace Ui {
class testLightController;
}

class testLightController : public QDialog
{
    Q_OBJECT

public:
    explicit testLightController(ComDeviceList *cdList, QWidget *parent = nullptr);
    ~testLightController();

    void LoadCommands();

    void SetUpArguments(quint8 index);

    void SetUpRegularFlash();

    void SetUpRegularSequence();

    void SetUpRegularBackground();

    void SetUpRGBFlash();

    void SetUpRGBFlash2C();

    void SetUpRGBFlashCM();

    void SetUpRGBSequence();

    void SetUpRGBSequenceCM();

    void SetUpRGBBackground();

    bool GetTimeOn();

    bool GetTimeOff();

    bool GetNumberFlashes();

    bool GetTimeDelay();

    bool CheckColor();

    bool CheckSideColor();

    bool CheckColorMap();

    bool GetGroups();

    bool GetTimeBG();

    bool GetTimeBGR();

    bool GetHighCount();

    bool GetOtherGroups();

    bool GetPlayerNumber();

private slots:
    void on_lightCntlrComboBox_currentIndexChanged(int index);

    void on_commandsComboBox_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_updateBG_clicked();

private:
    Ui::testLightController *ui;


    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList       *p_comDeviceList;

    //First Controller Loaded
    bool                isFirstController;

    //Number of Light Controllers
    quint8              numberLightCntlrs;

    //Controller Number
    quint8              cntlrsNumber;

    //Ultimarc Device Data
    UltimarcData        dataUltimarc[ULTIMARCMAXDEVICES];

    //Have Regular Lights
    bool                regularLights;

    //Have RGB Lights
    bool                rgbLights;

    //Number of Commands Loaded
    quint8              numberCommands;

    //Groups
    QList<quint8>       groups;

    //Time On
    quint16             timeOn;

    //Time Off
    quint16             timeOff;

    //Number of Flashes
    quint8              numberFlashes;

    //Time Delay
    quint16             timeDelay;

    //Color
    QString             color;

    //Side Color
    QString             sideColor;

    //Color Map
    QString             colorMap;

    //Command Index
    quint8              commandIndex;

    //Background Time Delay
    quint16             timeBG;

    //Background Time Delay for Reload
    quint16             timeBGR;

    //Background High Count
    quint8              highCount;
    quint8              bgGroupPinCount;

    //Background Other Groups
    QList<quint8>       otherGroups;

    //Background Player Number
    quint8              playerNumber;

    //Is Background Running
    bool                isBGRunning;
    bool                isBGRGB;

};

#endif // TESTLIGHTCONTROLLER_H
