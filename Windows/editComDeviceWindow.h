#ifndef EDITCOMDEVICEWINDOW_H
#define EDITCOMDEVICEWINDOW_H

#include <QDialog>


#include <QString>
#include <QMessageBox>
#include <QSerialPortInfo>

#include "../COMDeviceList/ComDeviceList.h"

namespace Ui {
class editComDeviceWindow;
}

class editComDeviceWindow : public QDialog
{
    Q_OBJECT

public:
    explicit editComDeviceWindow(ComDeviceList *cdList, QWidget *parent = nullptr);
    ~editComDeviceWindow();

private slots:

    //When the Saved COM Device Changes, Update the Info
    void on_savedComDevicesComboBox_currentIndexChanged(int index);

    //When the COM Port Changes, Updates the Serial COM Port Info on right Side
    void on_comPortComboBox_currentIndexChanged(int index);

    //Deletes the COM Device for the List
    void on_deletePushButton_clicked();

    //Edits the Selected COM Device, and Updates the Info
    void on_editPushButton_clicked();

    //Edits the Selected COM Device, and Updates the Info. Also Closes Window
    void on_okPushButton_clicked();

    //Closes Window
    void on_cancelPushButton_clicked();


private:

    //Checks if Data is Valid for a COM Device
    bool IsValidData();

    //Editss LightGun on the ComDeviceList
    void EditComDevice();

    //Gets Baud Rate from Combo Box. If Found, return true, else return false.
    void FindBaudRate();

    //Gets Data Bits from Combo Box. If Found, return true, else return false.
    void FindDataBits();

    //Gets Parity from Combo Box. If Found, return true, else return false.
    void FindParity();

    //Gets Stop Bits from Combo Box. If Found, return true, else return false.
    void FindStopBits();

    //Gets Flow from Combo Box. If Found, return true, else return false.
    void FindFlow();

    //Fill Out Serial Port Info
    void FillSerialPortInfo(quint8 index);

    //Loads Up data on a Saved COM device
    void LoadSavedComDevice(quint8 index);

    ///////////////////////////////////////////////////////////////////////////

    //For Window
    Ui::editComDeviceWindow *ui;

    //ComDeviceList to Add the COM Devices Too. Do Not Delete!
    ComDeviceList *p_comDeviceList;

    //Temp Pointers to Add Data. Delete if not Set to nullptr
    QSerialPortInfo *p_comPortInfo = nullptr;

    //COM Device Data
    //bool defaultComDevice;
    //quint8 defaultComDeviceNum;
    QString comDeviceName;
    quint8 comDeviceNum;
    quint8 comPortNum;
    QString comPortName;
    qint32 comPortBaud;
    quint8 comPortDataBits;
    quint8 comPortParity;
    quint8 comPortStopBits;
    quint8 comPortFlow;

    //Number of COM Devices in the List
    quint8 numberComDevices;

    //Lists Used for the Combo Boxes
    bool unusedComPort[MAXCOMPORTS];

    //Combo Boxes Indexes
    quint8 baudIndex;
    quint8 dataBitsIndex;
    quint8 parityIndex;
    quint8 stopBitsIndex;
    quint8 flowIndex;

};

#endif // EDITCOMDEVICEWINDOW_H
