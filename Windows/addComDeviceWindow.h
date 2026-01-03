#ifndef ADDCOMDEVICEWINDOW_H
#define ADDCOMDEVICEWINDOW_H

#include <QDialog>


#include <QString>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QThread>

#include "../COMDeviceList/ComPortDevice.h"
#include "../COMDeviceList/ComDeviceList.h"

//#include "StdAfx.h"
//#include "Windows.h"
//#include "PacDrive.h"


namespace Ui {
class addComDeviceWindow;
}

class addComDeviceWindow : public QDialog
{
    Q_OBJECT

public:
    explicit addComDeviceWindow(ComDeviceList *cdList, QWidget *parent = nullptr);
    ~addComDeviceWindow();

private slots:

    //Adds a New ComPortDevice to the ComDeviceList. Doesn't Close the Window
    void on_addPushButton_clicked();

    //Adds a New ComPortDevice to the ComDeviceList and Closes the Window
    void on_okPushButton_clicked();

    //Closes the Window
    void on_cancelPushButton_clicked();

    //Know When to Update Serial Port Info On the Right Side of Window
    void on_comPortComboBox_currentIndexChanged(int index);

private:

    //Checks if Data is Valid for a LightGun
    bool IsValidData();

    //Adds a New LightGun to the ComDeviceList
    void AddComPortDevice();

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

    ///////////////////////////////////////////////////////////////////////////

    //For Window
    Ui::addComDeviceWindow  *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList           *p_comDeviceList;

    //Temp Pointers to Add Data. Delete if not Set to nullptr
    ComPortDevice           *p_cpdTemp = nullptr;
    QSerialPortInfo         *p_comPortInfo = nullptr;

    //Light Gun Data
    QString                 comPortDeviceName;
    quint8                  comPortDeviceNum;
    quint8                  comPortNum;
    QString                 comPortName;
    qint32                  comPortBaud;
    quint8                  comPortDataBits;
    quint8                  comPortParity;
    quint8                  comPortStopBits;
    quint8                  comPortFlow;

    //Lists Used for the Combo Boxes
    bool                    unusedComPort[MAXCOMPORTS];

};

#endif // ADDCOMDEVICEWINDOW_H
