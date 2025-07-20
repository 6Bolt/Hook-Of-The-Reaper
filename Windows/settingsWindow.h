#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>


#include "../COMDeviceList/ComDeviceList.h"

namespace Ui {
class settingsWindow;
}

class settingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit settingsWindow(ComDeviceList *cdList, QWidget *parent = nullptr);
    ~settingsWindow();

private slots:

    //Update Settings to List
    void on_updatePushButton_clicked();

    //Update Settings to List, and Close Window
    void on_okPushButton_clicked();

    //Close Window
    void on_cancelPushButton_clicked();

    void on_bypassSerialWriteCheckBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_ammoRadioButton_clicked();

    void on_lifeRadioButton_clicked();

    void on_ammoLifeCheckBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_glyphsRadioButton_clicked();

    void on_lifeBarRadioButton_clicked();

    void on_lifeNumRadioButton_clicked();

private:

    //Check the Setting and then Save Them
    void CheckAndSaveSetting();

    ///////////////////////////////////////////////////////////////////////////

    //For window Stuff
    Ui::settingsWindow  *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList       *p_comDeviceList;

    bool                finishedInit = false;

    //Bool Settings
    bool                useDefaultLGFirst;
    bool                useMultiThreading;
    bool                closeComPortGameExit;
    bool                ignoreUselessDLGGF;
    bool                bypassSerialWriteChecks;
    bool                disbleReaperLEDs;
    bool                displayAmmoPriority;
    bool                displayLifePriority;
    bool                displayOtherPriority;
    bool                displayAmmoLife;
    bool                displayAmmoLifeGlyphs;
    bool                displayAmmoLifeBar;
    bool                displayAmmoLifeNumber;
    bool                enableNewGameFileCreation;
    bool                enableReaperAmmo0Delay;

    //For Check Boxes
    Qt::CheckState      defaultLG;
    Qt::CheckState      multiThreading;
    Qt::CheckState      closeComPort;
    Qt::CheckState      ignoreUDLGGF;
    Qt::CheckState      bypassSWC;
    Qt::CheckState      disableRLED;
    Qt::CheckState      otherDisplay;
    Qt::CheckState      displayAL;
    Qt::CheckState      enableNGFC;
    Qt::CheckState      enableRA0D;

    //Refresh Time in Number and String
    quint32             refreshDisplayTime;
    QString             refreshDisplayTimeString;

    //Reaper Ammo 0 Delay Time and String
    quint8              reaperAmmo0Delay;
    QString             reaperAmmo0DelayString;

    //Reaper Hold Slide Back Time
    quint16             reaperHoldSlideTime;
    float               reaperHoldSlideTimeSec;
    QString             reaperHoldSlideTimeSecString;

};

#endif // SETTINGSWINDOW_H
