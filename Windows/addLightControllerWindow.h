#ifndef ADDLIGHTCONTROLLERWINDOW_H
#define ADDLIGHTCONTROLLERWINDOW_H

#include <QDialog>
#include <QFileDialog>

#include "../COMDeviceList/ComDeviceList.h"


namespace Ui {
class addLightControllerWindow;
}

class addLightControllerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit addLightControllerWindow(ComDeviceList *cdList, QWidget *parent = nullptr);
    ~addLightControllerWindow();

private slots:
    void on_pushButton_clicked();

    void on_controllerComboBox_currentIndexChanged(int index);

    void on_addPushButton_clicked();

    void on_addClosePushButton_clicked();

    void on_closePushButton_clicked();

private:

    //Collects Ultimarc Device Data
    void CollectUltimarcData();

    //Displays Ultimarc Device Data
    void DisplayUltimarcData(quint8 index);

    void CheckSaveLightCntlrsWithNew();

    void RemoveUltimarcData(quint8 index);

    void RemoveComboBoxDisplay(quint8 index);


    Ui::addLightControllerWindow *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList           *p_comDeviceList;

    //Number of Saved Light Controillers
    quint8                  numberSaveLightCntlrs;

    //Number of Ultrimarc Devices
    qint8                   numberUltimarcDevices;

    //Ultimarc Device Data
    UltimarcData            dataUltimarc[ULTIMARCMAXDEVICES];

    bool                    noControllers;

};

#endif // ADDLIGHTCONTROLLERWINDOW_H
