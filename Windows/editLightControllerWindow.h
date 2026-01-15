#ifndef EDITLIGHTCONTROLLERWINDOW_H
#define EDITLIGHTCONTROLLERWINDOW_H

#include <QDialog>
#include <QFileDialog>
#include <QFile>

#include "../COMDeviceList/ComDeviceList.h"

namespace Ui {
class editLightControllerWindow;
}

class editLightControllerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit editLightControllerWindow(ComDeviceList *cdList, QWidget *parent = nullptr);
    ~editLightControllerWindow();

private slots:
    void on_grpFilePushButton_clicked();

    void on_controllerComboBox_currentIndexChanged(int index);

    void on_deletePushButton_clicked();

    void on_editPushButton_clicked();

    void on_editClosePushButton_clicked();

    void on_closePushButton_clicked();

private:

    //Displays Ultimarc Device Data
    void DisplayUltimarcData(quint8 index);

    void RemoveUltimarcData(quint8 index);

    void RemoveComboBoxDisplay(quint8 index);

    bool CollectUltimarcData();


    Ui::editLightControllerWindow *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList           *p_comDeviceList;

    //Number of Saved Light Controillers
    quint8                  numberSaveLightCntlrs;

    //Ultimarc Device Data
    UltimarcData            dataUltimarc[ULTIMARCMAXDEVICES];

    //If there is No More Saved Light Controllers
    bool                    noControllers;
};

#endif // EDITLIGHTCONTROLLERWINDOW_H
