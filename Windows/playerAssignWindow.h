#ifndef PLAYERASSIGNWINDOW_H
#define PLAYERASSIGNWINDOW_H

#include <QDialog>


#include <QMessageBox>


#include "../COMDeviceList/ComDeviceList.h"


namespace Ui {
class playerAssignWindow;
}

class playerAssignWindow : public QDialog
{
    Q_OBJECT

public:
    explicit playerAssignWindow(ComDeviceList *cdList, QWidget *parent = nullptr);
    ~playerAssignWindow();

private slots:
    //Assignment of Light Guns to Players
    void on_assignPushButton_clicked();

    //Assignment of Light Guns to Players, and Close Window
    void on_okPushButton_clicked();

    //Close Window
    void on_cancelPushButton_clicked();

private:

    //Get the Combo Boxes Index
    void GetComboBoxIndexes();

    //Assign Save Light Guns to Players
    void AssignPlayers();


    ///////////////////////////////////////////////////////////////////////////

    //For Window Stuff
    Ui::playerAssignWindow  *ui;

    //ComDeviceList to Add the Light Gun Too. Do Not Delete!
    ComDeviceList           *p_comDeviceList;


    //Number of Light Gun in the List
    quint8                  numberLightGuns;

    quint8                  playersAssignment[MAXPLAYERLIGHTGUNS];
    qint16                  playersIndex[MAXPLAYERLIGHTGUNS];

};

#endif // PLAYERASSIGNWINDOW_H
