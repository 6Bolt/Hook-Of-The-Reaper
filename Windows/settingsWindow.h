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

private:

    //Check the Setting and then Save Them
    void CheckAndSaveSetting();

    ///////////////////////////////////////////////////////////////////////////

    //For window Stuff
    Ui::settingsWindow  *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList       *p_comDeviceList;

    //Two Settings
    bool                useDefaultLGFirst;
    bool                useMultiThreading;
    bool                closeComPortGameExit;

    //For Check Boxes
    Qt::CheckState      defaultLG;
    Qt::CheckState      multiThreading;
    Qt::CheckState      closeComPort;

    //Refresh Time in Number and String
    quint32             refreshDisplayTime;
    QString             refreshDisplayTimeString;



};

#endif // SETTINGSWINDOW_H
