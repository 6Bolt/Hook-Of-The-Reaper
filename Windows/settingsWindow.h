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
    bool                enableNewGameFileCreation;

    //For Check Boxes
    Qt::CheckState      defaultLG;
    Qt::CheckState      multiThreading;
    Qt::CheckState      closeComPort;
    Qt::CheckState      ignoreUDLGGF;
    Qt::CheckState      bypassSWC;
    Qt::CheckState      enableNGFC;

    //Refresh Time in Number and String
    quint32             refreshDisplayTime;
    QString             refreshDisplayTimeString;


};

#endif // SETTINGSWINDOW_H
