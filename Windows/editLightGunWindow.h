#ifndef EDITLIGHTGUNWINDOW_H
#define EDITLIGHTGUNWINDOW_H

#include <QDialog>


#include <QString>
#include <QMessageBox>
#include <QSerialPortInfo>

#include <QSoundEffect>
#include <QUrl>
#include <QThread>

#include "../COMDeviceList/ComDeviceList.h"

#include <Windows.h>
#include "hidapi_winapi.h"

namespace Ui {
class editLightGunWindow;
}

class editLightGunWindow : public QDialog
{
    Q_OBJECT

public:
    explicit editLightGunWindow(ComDeviceList *cdList, QWidget *parent = nullptr);
    ~editLightGunWindow();

private slots:
    //When the Saved Light Gun Changes, Update the Info
    void on_savedLightGunsComboBox_currentIndexChanged(int index);

    //When the Default Light Gun Changes, Updates the Inf0
    void on_defaultLightGunComboBox_currentIndexChanged(int index);

    //When the COM Port Changes, Updates the Serial COM Port Info on right Side
    void on_comPortComboBox_currentIndexChanged(int index);

    //Deletes the Light Gun for the List
    void on_deletePushButton_clicked();

    //Edits the Selected Light Gun, and Updates the Info
    void on_editPushButton_clicked();

    //Edits the Selected Light Gun, and Updates the Info. Also Closes Window
    void on_okPushButton_clicked();

    //Closes Window
    void on_cancelPushButton_clicked();





    void on_hubComComboBox_currentIndexChanged(int index);

    void on_usbDevicesComboBox_currentIndexChanged(int index);

    void on_allHIDDevicesCheckBox_checkStateChanged(const Qt::CheckState &arg1);

private:

    //Checks if Data is Valid for a LightGun
    bool IsValidData();

    //Checks if Light gun is a Default Light Gun
    bool IsDefaultLightGun();

    //Edits LightGun on the ComDeviceList
    void EditLightGun();

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

    //Enable or Disable Combo Boxes if a Default Light Gun is Selected
    void SetEnableComboBoxes(bool enableCB);

    //Loads Up data on a Saved Light Gun
    void LoadSavedLightGun(quint8 index);

    //Fills In the USB Devices Combo Box
    void FillUSBDevicesComboBox();

    //Process the HID Devices into the QList
    void ProcessHIDInfo();

    //Find The Light Gun Vendor ID, Product ID and Serial Number (optional) in the HID Devices
    qint16 FindLightGunHID(quint8 lgNum);

    ///////////////////////////////////////////////////////////////////////////

    //For Window
    Ui::editLightGunWindow  *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList           *p_comDeviceList;

    //Temp Pointers to Add Data. Delete if not Set to nullptr
    QSerialPortInfo         *p_comPortInfo = nullptr;

    //Light Gun Data
    bool                    defaultLightGun;
    quint8                  defaultLightGunNum;
    QString                 lightGunName;
    quint8                  lightGunNum;
    quint8                  comPortNum;
    QString                 comPortName;
    qint32                  comPortBaud;
    quint8                  comPortDataBits;
    quint8                  comPortParity;
    quint8                  comPortStopBits;
    quint8                  comPortFlow;
    quint8                  maxAmmoNum;
    quint8                  reloadValueNum;
    bool                    dipSwitchSet;
    quint8                  dipSwitchNumber;
    bool                    analogStrengthSet;
    quint8                  analogStrength;
    quint8                  hubComPortNumber;
    bool                    defaultLightGunNumChanged;
    quint8                  oldDefaultLightGunNum;
    bool                    oldDefaultLightGun;

    //Number of Light Gun in the List
    quint8                  numberLightGuns;

    //Lists Used for the Combo Boxes
    bool                    unusedComPort[MAXCOMPORTS];

    //Dip Switch PLayers
    bool                    usedDipPlayers[DIPSWITCH_NUMBER];

    //Records the Combo Boxes Index
    quint8                  baudIndex;
    quint8                  dataBitsIndex;
    quint8                  parityIndex;
    quint8                  stopBitsIndex;
    quint8                  flowIndex;

    //USB HID Info
    hid_device_info         *devs;
    quint16                 numberHIDDevices;
    QList<HIDInfo>          hidInfoList;
    QList<HIDInfo>          lgHIDInfoList;
    bool                    getAllHIDs;
    bool                    isHIDInit;

    //Sound Effects of Edit & Delete Light Gun
    QSoundEffect            editLightGunSound;
    QSoundEffect            deleteLightGunSound;


};

#endif // EDITLIGHTGUNWINDOW_H
