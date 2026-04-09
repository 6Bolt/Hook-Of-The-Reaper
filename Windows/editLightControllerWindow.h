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

    void on_typeComboBox_currentIndexChanged(int index);

private:

    //Displays Ultimarc Device Data
    void DisplayUltimarcData(quint8 index);

    //Display ALED Strip Controller Data
    void DisplayALEDStripData(quint8 index);

    void RemoveUltimarcData(quint8 index);

    void RemoveComboBoxDisplay(quint8 index);

    bool CollectUltimarcData();

    void SetALED(bool enable);

    bool CollectALEDStripData();


    Ui::editLightControllerWindow *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList           *p_comDeviceList;

    //Temp Pointers to Serial Port Info, so can Scan Through Serial Ports
    QSerialPortInfo         *p_comPortInfo = nullptr;

    //Current Type Setting
    qint8                   typeSet;

    //Number of Saved Light Controillers
    quint8                  numberSaveUltimarcCntlrs;
    quint8                  numberALEDStripCntlrs;

    QList<quint8>           ultimarcPotitions;
    QList<quint8>           aledStripPotitions;

    //Ultimarc Device Data
    UltimarcData            dataUltimarc[ULTIMARCMAXDEVICES];

    //If there is No More Saved Light Controllers
    bool                    noControllers;

    //ALED Strip Controller
    QStringList             aledCntlrCOMNames;
    QList<quint8>           aledCntlrNumber;

    quint8                  aledNumberStrings;
    QList<quint16>          aledElements;
    bool                    aledStripDataChanged;

    quint8                  aledPattern;
    bool                    aledPatChanged;

    QString                 fileAndPath;
    bool                    fileChanged;
};

#endif // EDITLIGHTCONTROLLERWINDOW_H
