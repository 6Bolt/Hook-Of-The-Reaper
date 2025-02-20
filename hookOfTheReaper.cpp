#include "hookOfTheReaper.h"
#include "ui_hookOfTheReaper.h"

//Default Light Gun Array/List
S_DEFAULTLG DEFAULTLG_ARRAY[NUM_DEFAULTLG];

//Com Port Options for Combo Boxes
QString BAUDNAME_ARRAY[BAUD_NUMBER] = {"115200", "57600", "38400", "19200", "9600", "4800", "2400", "1200"};
qint32 BAUDDATA_ARRAY[BAUD_NUMBER] = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200};

QString DATABITSNAME_ARRAY[DATABITS_NUMBER] = {"8", "7", "6", "5"};
quint8  DATABITSDATA_ARRAY[DATABITS_NUMBER] = {8, 7, 6, 5};

QString PARITYNAME_ARRAY[PARITY_NUMBER] = {"None", "Even", "Odd", "Space", "Mark"};
quint8  PARITYDATA_ARRAY[PARITY_NUMBER] = {0, 2, 3, 4, 5};

QString STOPBITSNAME_ARRAY[STOPBITS_NUMBER] = {"1", "2", "1.5"};
quint8  STOPBITSDATA_ARRAY[STOPBITS_NUMBER] = {1, 2, 3};

QString FLOWNAME_ARRAY[FLOW_NUMBER] = {"No Flow Control", "RTS/CTS - Hardware", "Xon/Xoff - Software"};
quint8  FLOWDATA_ARRAY[FLOW_NUMBER] = {0, 1, 2};

QString DEFAULTLGFILENAMES_ARRAY[NUM_DEFAULTLG] = {"nonDefaultLG.hor","rs3Reaper.hor","mx24.hor","jbgun4ir.hor","fusion.hor","blamcon.hor"};

//Constructor
HookOfTheReaper::HookOfTheReaper(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HookOfTheReaper)
{
    ui->setupUi(this);

    //Set Up Reaper Default Light Gun Settings
    DEFAULTLG_ARRAY[RS3_REAPER].BAUD = REAPERBAUD;
    DEFAULTLG_ARRAY[RS3_REAPER].DATA = REAPERDATA;
    DEFAULTLG_ARRAY[RS3_REAPER].PARITY = REAPERPARITY;
    DEFAULTLG_ARRAY[RS3_REAPER].STOP = REAPERSTOP;
    DEFAULTLG_ARRAY[RS3_REAPER].FLOW = REAPERFLOW;
    DEFAULTLG_ARRAY[RS3_REAPER].MAXAMMO = REAPERMAXAMMO;
    DEFAULTLG_ARRAY[RS3_REAPER].RELOADVALUE = REAPERRELOAD;
    DEFAULTLG_ARRAY[RS3_REAPER].MAXAMMON = REAPERMAXAMMONUM;
    DEFAULTLG_ARRAY[RS3_REAPER].RELOADVALUEN = REAPERRELOADNUM;

    //Set Up MX24 Default Light Gun Settings
    DEFAULTLG_ARRAY[MX24].BAUD = MX24BAUD;
    DEFAULTLG_ARRAY[MX24].DATA = MX24DATA;
    DEFAULTLG_ARRAY[MX24].PARITY = MX24PARITY;
    DEFAULTLG_ARRAY[MX24].STOP = MX24STOP;
    DEFAULTLG_ARRAY[MX24].FLOW = MX24FLOW;
    DEFAULTLG_ARRAY[MX24].MAXAMMO = MX24MAXAMMO;
    DEFAULTLG_ARRAY[MX24].RELOADVALUE = MX24RELOAD;
    DEFAULTLG_ARRAY[MX24].MAXAMMON = MX24MAXAMMONUM;
    DEFAULTLG_ARRAY[MX24].RELOADVALUEN = MX24RELOADNUM;

    //Set Up JB Gun4IR Default Light Gun Settings
    DEFAULTLG_ARRAY[JBGUN4IR].BAUD = JBGUN4IRBAUD;
    DEFAULTLG_ARRAY[JBGUN4IR].DATA = JBGUN4IRDATA;
    DEFAULTLG_ARRAY[JBGUN4IR].PARITY = JBGUN4IRPARITY;
    DEFAULTLG_ARRAY[JBGUN4IR].STOP = JBGUN4IRSTOP;
    DEFAULTLG_ARRAY[JBGUN4IR].FLOW = JBGUN4IRFLOW;
    DEFAULTLG_ARRAY[JBGUN4IR].MAXAMMO = JBGUN4IRMAXAMMO;
    DEFAULTLG_ARRAY[JBGUN4IR].RELOADVALUE = JBGUN4IRRELOAD;
    DEFAULTLG_ARRAY[JBGUN4IR].MAXAMMON = JBGUN4IRMAXAMMONUM;
    DEFAULTLG_ARRAY[JBGUN4IR].RELOADVALUEN = JBGUN4IRRELOADNUM;

    //Set Up Fusion Default Light Gun Settings
    DEFAULTLG_ARRAY[FUSION].BAUD = FUSIONBAUD;
    DEFAULTLG_ARRAY[FUSION].DATA = FUSIONDATA;
    DEFAULTLG_ARRAY[FUSION].PARITY = FUSIONPARITY;
    DEFAULTLG_ARRAY[FUSION].STOP = FUSIONSTOP;
    DEFAULTLG_ARRAY[FUSION].FLOW = FUSIONFLOW;
    DEFAULTLG_ARRAY[FUSION].MAXAMMO = FUSIONMAXAMMO;
    DEFAULTLG_ARRAY[FUSION].RELOADVALUE = FUSIONRELOAD;
    DEFAULTLG_ARRAY[FUSION].MAXAMMON = FUSIONMAXAMMONUM;
    DEFAULTLG_ARRAY[FUSION].RELOADVALUEN = FUSIONRELOADNUM;


    //Set Up Blamcon Default Light Gun Settings
    DEFAULTLG_ARRAY[BLAMCON].BAUD = BLAMCONBAUD;
    DEFAULTLG_ARRAY[BLAMCON].DATA = BLAMCONDATA;
    DEFAULTLG_ARRAY[BLAMCON].PARITY = BLAMCONPARITY;
    DEFAULTLG_ARRAY[BLAMCON].STOP = BLAMCONSTOP;
    DEFAULTLG_ARRAY[BLAMCON].FLOW = BLAMCONFLOW;
    DEFAULTLG_ARRAY[BLAMCON].MAXAMMO = BLAMCONMAXAMMO;
    DEFAULTLG_ARRAY[BLAMCON].RELOADVALUE = BLAMCONRELOAD;
    DEFAULTLG_ARRAY[BLAMCON].MAXAMMON = BLAMCONMAXAMMONUM;
    DEFAULTLG_ARRAY[BLAMCON].RELOADVALUEN = BLAMCONRELOADNUM;


    //TCP Socket is Not Connected Yet
    isTCPConnected = false;

    //Is Game INI or DefaultLG
    isGameINI = false;


    //Creates a New COM Port Device List, which Includes Light Guns and Other COM Devices
    //Loads Up Save Data Files and Settings File
    //This is what is passed to the Windows and Hooker Engine
    p_comDeviceList = new ComDeviceList();

    //Creates the Hooker Engine and Gives Pointer Address of COM Device List
    p_hookEngine = new HookerEngine(p_comDeviceList, true, this);

    //Signals and Slots. Used to Display Info On Main Windows.
    //Hooker Engine Updates, Based on the Refresh Time Display
    connect(p_hookEngine, &HookerEngine::MameConnectedNoGame, this, &HookOfTheReaper::DisplayMameNoGame);
    connect(p_hookEngine, &HookerEngine::MameConnectedGame, this, &HookOfTheReaper::DisplayMameGame);
    connect(p_hookEngine, &HookerEngine::AddSignalFromGame, this, &HookOfTheReaper::AddSignalDisplay);
    connect(p_hookEngine, &HookerEngine::UpdateSignalFromGame, this, &HookOfTheReaper::UpdateSignalDisplay);
    connect(p_hookEngine, &HookerEngine::UpdatePauseFromGame, this, &HookOfTheReaper::UpdatePauseDisplay);
    connect(p_hookEngine, &HookerEngine::UpdateOrientationFromGame, this, &HookOfTheReaper::UpdateOrientationDisplay);
    connect(p_hookEngine, &HookerEngine::TCPStatus, this, &HookOfTheReaper::UpdateTCPConnectionStatus);

    //Get the Number of Devices, after Loading Up the Save Data
    numberLightGuns = p_comDeviceList->GetNumberLightGuns();
    numberComDevices = p_comDeviceList->GetNumberComPortDevices();

    //Start the Hooker Engine. If no Light Guns Saved, then must use INI Files
    p_hookEngine->Start ();
    engineRunning = true;
    ui->statusbar->showMessage ("Running");

}

//Deconstructor
HookOfTheReaper::~HookOfTheReaper()
{
    delete ui;
    delete p_comDeviceList;
}


//Public Slots

//Displays No Game Data on Main Window
void HookOfTheReaper::DisplayMameNoGame()
{
    //Display Text QMap needs to be cleared
    signalNumberMap.clear ();

    gameName = MAMENOGAMEEMPTY;

    MakeTopDisplayText();

    DisplayText();
}

//Displays Top of Game Data on Main Window
void HookOfTheReaper::DisplayMameGame(QString gName, bool iniGame)
{
    //Display Text QMap needs to be cleared
    signalNumberMap.clear ();

    isGameINI = iniGame;

    gameName = gName;

    MakeTopDisplayText();

    displayText << " " << OUTPUTSIGNALS << OUTPUTSIGNALSDASHES;

    DisplayText();

}

//Adds Output Signal to the Main Window
void HookOfTheReaper::AddSignalDisplay(const QString &sig, const QString &dat)
{
    QString temp = sig+" = "+dat;

    quint16 signalCount = displayText.count ();

    displayText << temp;

    //qDebug() << "Adding: " << temp << " to line number: " << signalCount;

    signalNumberMap.insert (sig,signalCount);

    DisplayText();

}

//Updates Output Signals Data on the Main Window
void HookOfTheReaper::UpdateSignalDisplay(const QString &sig, const QString &dat)
{
    QString temp = sig+" = "+dat;

    quint16 displayIndex = signalNumberMap[sig];

    displayText[displayIndex] = temp;

    DisplayText();
}

//Updates Pause Data on Main Window
void HookOfTheReaper::UpdatePauseDisplay(QString dat)
{
    QString pTemp = PAUSECOLON+dat;
    displayText[PAUSEINDEX] = pTemp;

    DisplayText();
}

//Updates Orientation Data on the Main Window
void HookOfTheReaper::UpdateOrientationDisplay(QString sig, QString dat)
{
    QString oTemp = sig+": "+dat;
    displayText[ORIENTATIONINDEX] = oTemp;

    DisplayText();
}

void HookOfTheReaper::UpdateTCPConnectionStatus(bool tcpConStatus)
{
    isTCPConnected = tcpConStatus;

    MakeTopDisplayText();

    DisplayText();
}

//Private Slots

//Open the Add Light Window
void HookOfTheReaper::on_actionAdd_Light_Gun_triggered()
{
    //Checks if Window is Already Open with a QPointer
    if (!p_aLGW)
    {
        numberLightGuns = p_comDeviceList->GetNumberLightGuns();

        if(engineRunning)
        {
            //Stop the Hooker Engine
            p_hookEngine->Stop ();
            engineRunning = false;
            ui->statusbar->showMessage ("Stopped");
        }

        //Create New Window, and Set it to Delete when Closed
        p_aLGW = new addLightGunWindow(p_comDeviceList, this);
        p_aLGW->setAttribute(Qt::WA_DeleteOnClose);
        //Set the Dialog Window Signals to the Window Close Member function
        connect(p_aLGW, SIGNAL(accepted()), this, SLOT(Add_Light_Gun_Window_Closed()));
        connect(p_aLGW, SIGNAL(rejected()), this, SLOT(Add_Light_Gun_Window_Closed()));
    }
    //Execute and Show Window
    p_aLGW->exec ();
}

//When the Add Light Gun Window is Closed
void HookOfTheReaper::Add_Light_Gun_Window_Closed()
{
    quint8 newNumberLightGuns = p_comDeviceList->GetNumberLightGuns();

    //If a Light Gun was Added, then Save Light Gun data
    if(newNumberLightGuns > numberLightGuns)
    {
        p_comDeviceList->SaveLightGunList();
        numberLightGuns = newNumberLightGuns;
    }

    numberComDevices = p_comDeviceList->GetNumberComPortDevices();

    p_hookEngine->Start ();
    engineRunning = true;
    ui->statusbar->showMessage ("Running");

}


void HookOfTheReaper::on_actionAdd_Device_triggered()
{
    if (!p_aCDW)
    {
        numberComDevices = p_comDeviceList->GetNumberComPortDevices();


        if(engineRunning)
        {
            //Stop the Hooker Engine
            p_hookEngine->Stop ();
            engineRunning = false;
            ui->statusbar->showMessage ("Stopped");
        }

        p_aCDW = new addComDeviceWindow(p_comDeviceList, this);
        p_aCDW->setAttribute(Qt::WA_DeleteOnClose);
        connect(p_aCDW, SIGNAL(accepted()), this, SLOT(Add_Com_Device_Window_Closed()));
        connect(p_aCDW, SIGNAL(rejected()), this, SLOT(Add_Com_Device_Window_Closed()));
    }
    p_aCDW->exec ();
}

void HookOfTheReaper::Add_Com_Device_Window_Closed()
{
    quint8 newNumberComDevices = p_comDeviceList->GetNumberComPortDevices();

    if(newNumberComDevices > numberComDevices)
    {
        p_comDeviceList->SaveComDeviceList();
        numberComDevices = newNumberComDevices;
    }

    numberLightGuns = p_comDeviceList->GetNumberLightGuns();

    p_hookEngine->Start ();
    engineRunning = true;
    ui->statusbar->showMessage ("Running");
}

void HookOfTheReaper::on_actionExit_triggered()
{
    //Closed Down the hookOfTheReaper Close, Which Closes the Program
    close();
}


void HookOfTheReaper::on_actionEdit_Light_Gun_triggered()
{
    quint8 numLightGuns = p_comDeviceList->GetNumberLightGuns();

    //If No Light Guns, then Cannot Edit Them. No Need to Open New window. Display Warning Message
    if(numLightGuns == 0)
    {
        QMessageBox::warning (this, "No Saved Light Guns", "There are no saved Light Guns to edit. Please add a Light Gun first.");
    }
    else
    {
        if (!p_eLGW)
        {
            if(engineRunning)
            {
                //Stop the Hooker Engine
                p_hookEngine->Stop ();
                engineRunning = false;
                ui->statusbar->showMessage ("Stopped");
            }

            p_eLGW = new editLightGunWindow(p_comDeviceList, this);
            p_eLGW->setAttribute(Qt::WA_DeleteOnClose);
            connect(p_eLGW, SIGNAL(accepted()), this, SLOT(Edit_Light_Gun_Window_Closed()));
            connect(p_eLGW, SIGNAL(rejected()), this, SLOT(Edit_Light_Gun_Window_Closed()));
        }
        p_eLGW->exec ();
    }
}

void HookOfTheReaper::Edit_Light_Gun_Window_Closed()
{
    //Always save light gun data after the edit window closes
    p_comDeviceList->SaveLightGunList();

    numberComDevices = p_comDeviceList->GetNumberComPortDevices();
    numberLightGuns = p_comDeviceList->GetNumberLightGuns();

    p_hookEngine->Start ();
    engineRunning = true;
    ui->statusbar->showMessage ("Running");
}

void HookOfTheReaper::on_actionEdit_Device_triggered()
{
    quint8 numComDevices = p_comDeviceList->GetNumberComPortDevices();

    if(numComDevices == 0)
    {
        QMessageBox::warning (this, "No Saved COM Devices", "There are no saved COM Port Devices to edit. Please add a COM Device first.");
    }
    else
    {
        if (!p_eCDW)
        {
            if(engineRunning)
            {
                //Stop the Hooker Engine
                p_hookEngine->Stop ();
                engineRunning = false;
                ui->statusbar->showMessage ("Stopped");
            }

            p_eCDW = new editComDeviceWindow(p_comDeviceList, this);
            p_eCDW->setAttribute(Qt::WA_DeleteOnClose);
            connect(p_eCDW, SIGNAL(accepted()), this, SLOT(Edit_Com_Device_Window_Closed()));
            connect(p_eCDW, SIGNAL(rejected()), this, SLOT(Edit_Com_Device_Window_Closed()));
        }
        p_eCDW->exec ();
    }
}

void HookOfTheReaper::Edit_Com_Device_Window_Closed()
{
    //Always save COM device data after the edit window closes
    p_comDeviceList->SaveComDeviceList();

    numberComDevices = p_comDeviceList->GetNumberComPortDevices();
    numberLightGuns = p_comDeviceList->GetNumberLightGuns();

    p_hookEngine->Start ();
    engineRunning = true;
    ui->statusbar->showMessage ("Running");
}

void HookOfTheReaper::on_actionPlayer_Assignment_triggered()
{
    quint8 numLightGuns = p_comDeviceList->GetNumberLightGuns();

    if(numLightGuns == 0)
    {
        QMessageBox::warning (this, "No Saved Light Guns", "There are no saved Light Guns to assign. Please add a Light Gun first.");
    }
    else
    {
        if (!p_pAW)
        {
            if(engineRunning)
            {
                //Stop the Hooker Engine
                p_hookEngine->Stop ();
                engineRunning = false;
                ui->statusbar->showMessage ("Stopped");
            }

            p_pAW = new playerAssignWindow(p_comDeviceList, this);
            p_pAW->setAttribute(Qt::WA_DeleteOnClose);
            connect(p_pAW, SIGNAL(accepted()), this, SLOT(Player_Assign_Window_Closed()));
            connect(p_pAW, SIGNAL(rejected()), this, SLOT(Player_Assign_Window_Closed()));
        }
        p_pAW->exec ();
    }
}

void HookOfTheReaper::Player_Assign_Window_Closed()
{
    //Always save light gun data after the player assignment window closes
    p_comDeviceList->SaveLightGunList();

    numberComDevices = p_comDeviceList->GetNumberComPortDevices();
    numberLightGuns = p_comDeviceList->GetNumberLightGuns();

    p_hookEngine->Start ();
    engineRunning = true;
    ui->statusbar->showMessage ("Running");
}


void HookOfTheReaper::on_actionSettings_triggered()
{
    if (!p_setW)
    {


        if(engineRunning)
        {
            //Stop the Hooker Engine
            p_hookEngine->Stop ();
            engineRunning = false;
            ui->statusbar->showMessage ("Stopped");
        }

        p_setW = new settingsWindow(p_comDeviceList, this);
        p_setW->setAttribute(Qt::WA_DeleteOnClose);
        connect(p_setW, SIGNAL(accepted()), this, SLOT(SettingsWindowClosed()));
        connect(p_setW, SIGNAL(rejected()), this, SLOT(SettingsWindowClosed()));
    }
    p_setW->exec ();
}

void HookOfTheReaper::SettingsWindowClosed()
{
    numberComDevices = p_comDeviceList->GetNumberComPortDevices();
    numberLightGuns = p_comDeviceList->GetNumberLightGuns();

    p_hookEngine->Start ();
    engineRunning = true;
    ui->statusbar->showMessage ("Running");
}

void HookOfTheReaper::on_actionHook_of_the_Reaper_triggered()
{
    if (!p_aboutW)
    {


        if(engineRunning)
        {
            //Stop the Hooker Engine
            p_hookEngine->Stop ();
            engineRunning = false;
            ui->statusbar->showMessage ("Stopped");
        }

        p_aboutW = new aboutWindow(this);
        p_aboutW->setAttribute(Qt::WA_DeleteOnClose);
        connect(p_aboutW, SIGNAL(accepted()), this, SLOT(AboutWindowClosed()));
        connect(p_aboutW, SIGNAL(rejected()), this, SLOT(AboutWindowClosed()));
    }
    p_aboutW->exec ();
}

void HookOfTheReaper::AboutWindowClosed()
{
    p_hookEngine->Start ();
    engineRunning = true;
    ui->statusbar->showMessage ("Running");
}


void HookOfTheReaper::on_actionTest_COM_Port_triggered()
{
    quint8 numLightGuns = p_comDeviceList->GetNumberLightGuns();

    if(numLightGuns == 0)
    {
        QMessageBox::warning (this, "No Saved Light Guns", "There are no saved Light Guns to test. Please add a Light Gun first.");
    }
    else
    {

        if (!p_tcpW)
        {


            if(engineRunning)
            {
                //Stop the Hooker Engine
                p_hookEngine->Stop ();
                engineRunning = false;
                ui->statusbar->showMessage ("Stopped");
            }

            p_tcpW = new testComPortWindow(p_comDeviceList, this);
            p_tcpW->setAttribute(Qt::WA_DeleteOnClose);
            connect(p_tcpW, SIGNAL(accepted()), this, SLOT(TestComPortWindowClosed()));
            connect(p_tcpW, SIGNAL(rejected()), this, SLOT(TestComPortWindowClosed()));
        }
        p_tcpW->exec ();

    }
}

void HookOfTheReaper::TestComPortWindowClosed()
{
    p_hookEngine->Start ();
    engineRunning = true;
    ui->statusbar->showMessage ("Running");
}


void HookOfTheReaper::on_actionTest_defaultLG_Game_File_triggered()
{
    //Open File Dialog
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this, "Test DefaultLG Game File", "defaultLG", "DefaultLG Game Files (*.txt)");

    if(!fileNames.isEmpty ())
    {
        bool isGoodFile;
        quint8 fileCount = fileNames.length ();

        for(quint8 i = 0; i < fileCount; i++)
        {
            //qDebug() << "DefaultLG Game File: " << fileNames[i];

            isGoodFile = p_hookEngine->LoadLGFileTest(fileNames[i]);

            qint16 lastIndex = fileNames[i].lastIndexOf ('/');
            QString fName = fileNames[i].sliced (lastIndex+1);

            if(isGoodFile)
                QMessageBox::information (this, "DefaultLG Game File: "+fName, "The DefaultLG game file, "+fName+" passed the checks.\nFile: "+fileNames[i]);
        }
    }
    else
    {
        QMessageBox::information(this, "No File Selected", "No file(s) was selected.");
    }


}


void HookOfTheReaper::on_actionTest_INI_Game_File_triggered()
{
    //Open File Dialog
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this, "Test INI Game File", "ini/MAME", "INI Game Files (*.ini)");

    if(!fileNames.isEmpty ())
    {
        bool isGoodFile;
        quint8 fileCount = fileNames.length ();

        for(quint8 i = 0; i < fileCount; i++)
        {
            //qDebug() << "INI Game File: " << fileNames[i];

            isGoodFile = p_hookEngine->LoadINIFileTest(fileNames[i]);

            qint16 lastIndex = fileNames[i].lastIndexOf ('/');
            QString fName = fileNames[i].sliced (lastIndex+1);

            if(isGoodFile)
                QMessageBox::information (this, "INI Game File: "+fName, "The INI game file, "+fName+" passed the checks.\nFile: "+fileNames[i]);
        }
    }
    else
    {
        QMessageBox::information(this, "No File Selected", "No file(s) was selected.");
    }
}


//Private Functions

void HookOfTheReaper::MakeTopDisplayText()
{
    displayText.clear ();
    //QString temp = MAMESTARTAFTER;
    //temp.append ("\n");
    //displayText << temp;
    QString tcpSock = TCPSOCKET;

    if(isTCPConnected)
        tcpSock.append (CONNECTED);
    else
        tcpSock.append (NOCONNECTION);

    displayText << tcpSock;
    displayText << GAMEINFO << GAMEINFODASHES;
    QString temp = ROMCOLON;
    temp.append (gameName);
    displayText << temp;

    if(gameName != MAMENOGAMEEMPTY)
    {
        temp = GAMEFILE;
        temp.append (gameName);

        if(isGameINI)
            temp.append (ENDOFINIFILE);
        else
            temp.append (ENDOFLGFILE);

    }
    else
    {
        temp = GAMEFILE;
    }

    displayText << temp << ORIENTATIONCLNEQUAL0 << PAUSECLNEQUALS0;
}

void HookOfTheReaper::DisplayText()
{
    quint16 lineCount = displayText.count ();

    ui->textBrowser->clear ();

    ui->textBrowser->setPlainText (displayText[0]);

    for(quint8 i = 1; i < lineCount; i++)
        ui->textBrowser->append (displayText[i]);
}










