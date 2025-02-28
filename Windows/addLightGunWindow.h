#ifndef ADDLIGHTGUNWINDOW_H
#define ADDLIGHTGUNWINDOW_H


#include <QDialog>


//#include <QStringList>
//#include <QString>
#include <QMessageBox>
#include <QSerialPortInfo>



#include "../COMDeviceList/ComDeviceList.h"


namespace Ui {
class addLightGunWindow;
}

class addLightGunWindow : public QDialog
{
    Q_OBJECT

public:
    explicit addLightGunWindow(ComDeviceList *cdList, QWidget *parent = nullptr);
    ~addLightGunWindow();


private slots:
    //When Default Light Gun is Selected in Combo Box, Fills out the Data & Turns Off Selected Combo Boxes
    void on_defaultLightGunComboBox_currentIndexChanged(int index);

    //Updates the Serial COM Port Info on Right Side of Window
    void on_comPortComboBox_currentIndexChanged(int index);

    //Adds a New LightGun to the ComDeviceList. Doesn't Close the Window
    void on_addPushButton_clicked();

    //Adds a New LightGun to the ComDeviceList and Closes the Window
    void on_okPushButton_clicked();

    //Closes the Window
    void on_cancelPushButton_clicked();

    void on_hubComComboBox_currentIndexChanged(int index);

private:

    //Checks if Data is Valid for a LightGun
    bool IsValidData();

    //Checks if Light gun is a Default Light Gun
    bool IsDefaultLightGun();

    //Adds a New LightGun to the ComDeviceList
    void AddLightGun();

    //Gets Baud Rate from Combo Box.
    void FindBaudRate();

    //Gets Data Bits from Combo Box.
    void FindDataBits();

    //Gets Parity from Combo Box.
    void FindParity();

    //Gets Stop Bits from Combo Box.
    void FindStopBits();

    //Gets Flow from Combo Box.
    void FindFlow();

    //Fill Out Serial Port Info on Right Side of Window
    void FillSerialPortInfo(quint8 index);

    //Enable or Disable Combo Boxes if a Default Light Gun is Selected
    void SetEnableComboBoxes(bool enableCB);

    ///////////////////////////////////////////////////////////////////////////

    //For Window
    Ui::addLightGunWindow   *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList           *p_comDeviceList;

    //Temp Pointers to Add a Light Gun and Serial Port Info. Delete if not Set to nullptr
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

    //Lists Used for the Combo Boxes
    bool                    unusedComPort[MAXCOMPORTS];

    //Dip Switch PLayers
    bool                    usedDipPlayers[DIPSWITCH_NUMBER];

};

#endif // ADDLIGHTGUNWINDOW_H
