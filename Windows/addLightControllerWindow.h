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

    void RemoveUltimarcData(quint8 index);

    void RemoveComboBoxDisplay(quint8 index);

    bool AddLightController();

public slots:

    //Handle Error Message Box from a different Thread
    void ErrorMessage(const QString &title, const QString &message);

private:

    Ui::addLightControllerWindow *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList           *p_comDeviceList;

    //Number of Saved Light Controillers in List
    quint8                  numberSaveLightCntlrs;

    //Number of Ultrimarc Devices
    qint8                   numberUltimarcDevices;

    //Number of Ultimarc Devices in PacDrive
    quint8                  devicesInList;

    //Ultimarc Device Data
    UltimarcData            dataUltimarc[ULTIMARCMAXDEVICES];
    quint8                  ultimarcArrayNumber;

    bool                    noControllers;

};

#endif // ADDLIGHTCONTROLLERWINDOW_H
