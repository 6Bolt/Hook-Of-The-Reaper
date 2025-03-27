#ifndef HOOKOFTHEREAPER_H
#define HOOKOFTHEREAPER_H

#include <QMainWindow>
#include <QPointer>
#include <QFileDialog>

#include <QDialog>
#include <QMessageBox>

#include <QDebug>

#include <QEvent>

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QRect>

#include "Windows/addLightGunWindow.h"
#include "Windows/addComDeviceWindow.h"
#include "Windows/editLightGunWindow.h"
#include "Windows/editComDeviceWindow.h"
#include "Windows/playerAssignWindow.h"
#include "Windows/settingsWindow.h"
#include "Windows/aboutWindow.h"
#include "Windows/testComPortWindow.h"

#include "COMDeviceList/ComDeviceList.h"

#include "HookerEngine/HookerEngine.h"






QT_BEGIN_NAMESPACE
namespace Ui {
class HookOfTheReaper;
}
QT_END_NAMESPACE

class HookOfTheReaper : public QMainWindow
{
    Q_OBJECT

public:
    HookOfTheReaper(QWidget *parent = nullptr);
    ~HookOfTheReaper();

public slots:

    //Display Data in the Text Browser when No Game
    void DisplayMameNoGame();

    //Display Data in the Text Browser when Game is Connected to TCP Socket
    void DisplayMameGame(QString gName, bool iniGame);

    //Add Output Signal to the Display Data in Text Browser
    void AddSignalDisplay(const QString &sig, const QString &dat);

    //Updates Output Signal Value in Display Data
    void UpdateSignalDisplay(const QString &sig, const QString &dat);

    //Update the Pause Vaule in the Display Data
    void UpdatePauseDisplay(QString dat);

    //Update the Orientation Vaule in the Display Data
    void UpdateOrientationDisplay(QString sig, QString dat);

    //Update TCP Connection
    void UpdateTCPConnectionStatus(bool tcpConStatus);


private slots:

    //Open Add Light Gun Windows
    void on_actionAdd_Light_Gun_triggered();

    //On Close of Add Light Gun Window
    void Add_Light_Gun_Window_Closed();

    //Open Add COM Device Window
    void on_actionAdd_Device_triggered();

    //On Close of Add Light Gun Window
    void Add_Com_Device_Window_Closed();

    //Close Main Window
    void on_actionExit_triggered();

    //Open Edit Light Gun Window
    void on_actionEdit_Light_Gun_triggered();

    //On Close of Edit Light Gun Window
    void Edit_Light_Gun_Window_Closed();

    //Open Edit COM Device Window
    void on_actionEdit_Device_triggered();

    //On Close of Add Light Gun Window
    void Edit_Com_Device_Window_Closed();

    //Assign Light Guns to a Player
    void on_actionPlayer_Assignment_triggered();

    //On Close of Player Assignment Window
    void Player_Assign_Window_Closed();

    //Can Change the Settings
    void on_actionSettings_triggered();

    //On Close of the Settings Window
    void SettingsWindowClosed();

    //Brings Up The About the Hook of the Reaper
    void on_actionHook_of_the_Reaper_triggered();

    //On Close of the Settings Window
    void AboutWindowClosed();

    //Brings Up Test COM Port Window
    void on_actionTest_COM_Port_triggered();

    //On Close of test COM Port Window
    void TestComPortWindowClosed();

    void on_actionTest_defaultLG_Game_File_triggered();

    void on_actionTest_INI_Game_File_triggered();

signals:

    //Starts the Hooker Engine
    void StartHookerEngine();

    //Stops the Hooker Engine
    void StopHookerEngine();

#ifdef Q_OS_WIN
    //Hide From Taskbar when Minimized
protected:
    void changeEvent(QEvent *event) override {
        if (event->type() == QEvent::WindowStateChange) {
            if (windowState() & Qt::WindowMinimized) {
                hide();
                trayIcon->show ();
            }

        }
        //QMainWindow::changeEvent(event);
    }
#endif

private:

    //Makes the Default Display Data that is Shown in the Text Browser
    void MakeTopDisplayText();

    //Display the Display Data to Text Browser
    void DisplayText();


    ///////////////////////////////////////////////////////////////////////////


    //Main Window & Window Stuff
    Ui::HookOfTheReaper             *ui;

    //Add Light Gun Window QPointer
    QPointer<addLightGunWindow>     p_aLGW;

    //Add COM Port Device Window QPointer
    QPointer<addComDeviceWindow>    p_aCDW;

    //Edit Light Gun Window QPointer
    QPointer<editLightGunWindow>    p_eLGW;

    //Edit COM Port Device Window QPointer
    QPointer<editComDeviceWindow>   p_eCDW;

    //Edit COM Port Device Window QPointer
    QPointer<playerAssignWindow>    p_pAW;

    //Edit COM Port Device Window QPointer
    QPointer<settingsWindow>        p_setW;

    //Edit COM Port Device Window QPointer
    QPointer<aboutWindow>           p_aboutW;

    //Test COM Port Window QPointer
    QPointer<testComPortWindow>     p_tcpW;

    //Serial Port Devices List of Light Guns & Other COM Port Devices (Not Implamented yet)
    ComDeviceList                   *p_comDeviceList;

    //When a Child Window is Open, stop the Engine. Then when Closed, Update and Restart Hooker Engine
    bool                            engineRunning;


    //Number of Saved Light Guns & Other COM Devices Max is 255 (2^8)
    quint8                          numberLightGuns;
    quint8                          numberComDevices;

    //Hooker Engine Pointer
    HookerEngine                    *p_hookEngine;

    //Used to Display Text on the Text Browser
    QStringList                     displayText;

    //Maps Where the Output Data is in the Display Data
    QMap<QString,quint8>            signalNumberMap;

    //If the TCP Socket is Connected or Not
    bool                            isTCPConnected;

    //Name of the ROM or Game
    QString                         gameName;

    //Is Game File DefaultLG or INI
    bool                            isGameINI;

#ifdef Q_OS_WIN

    //Tray Icon
    QSystemTrayIcon                 *trayIcon;

    //Tray Icon Menu
    QMenu                           *trayMenu;

    //Tray Icon Meun Quit Action
    QAction                         *quitAction;

    //Tray Icon Meun Add Light Gun Action
    QAction                         *addLGAction;

    //Tray Icon Meun Edit Light Gun Action
    QAction                         *editLGAction;

    //Tray Icon Meun Player's Assignment Action
    QAction                         *playAsignAction;

    //Tray Icon Meun Test COM Port Action
    QAction                         *testComAction;

#endif
};
#endif // HOOKOFTHEREAPER_H
