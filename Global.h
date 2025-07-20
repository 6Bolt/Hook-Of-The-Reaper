#ifndef GLOBAL_H
#define GLOBAL_H

#include <qobject.h>

#define VERSION                 "1.1.8"
#define VERSIONMAIN             1
#define VERSIONMID              1
#define VERSIONLAST             3

//Global Settings
#define MAXPLAYERLIGHTGUNS      8
#define MAXGAMEPLAYERS          4
#define UNASSIGN                69

#define NOGUIARG                "-nogui"

//Serial COM Port Data
//Max Number of COM Ports
#define MAXCOMPORTS             50
#define BEGINCOMPORTNAME        "COM"
#define COMPORTWAITFORWRITE     50
#define COMPORTPATHFRONT        "\\\\.\\"

//Number of Different Settings
#define BAUD_NUMBER             8
#define DATABITS_NUMBER         4
#define PARITY_NUMBER           5
#define STOPBITS_NUMBER         3
#define FLOW_NUMBER             3

#define WRITERETRYATTEMPTS      2

#define DATABITS_MAX            8
#define DATABITS_MIN            5

#define DIPSWITCH_NUMBER        4

#define DEFAULTANALOGSTRENGTH   63

//Arrays that Store the Different Settings
//Located in the Top of hookOfTheReaper.cpp
extern QString BAUDNAME_ARRAY[];
extern qint32  BAUDDATA_ARRAY[];
extern QString DATABITSNAME_ARRAY[];
extern quint8  DATABITSDATA_ARRAY[];
extern QString PARITYNAME_ARRAY[];
extern quint8  PARITYDATA_ARRAY[];
extern QString STOPBITSNAME_ARRAY[];
extern quint8  STOPBITSDATA_ARRAY[];
extern QString FLOWNAME_ARRAY[];
extern quint8  FLOWDATA_ARRAY[];
extern QString DEFAULTLGFILENAMES_ARRAY[];


//Default Light Gun Definition - There is No Zero, as that is blan and nonDefaultLG
#define NUM_DEFAULTLG           11

//First Default Light Gun
//Retro Shooter: RS3 Reaper
#define RS3_REAPER              1
#define REAPERNAME              "Retro Shooter: RS3 Reaper"
#define REAPERBAUD              0
#define REAPERDATA              0
#define REAPERPARITY            0
#define REAPERSTOP              0
#define REAPERFLOW              0
#define REAPERMAXAMMO           "5"
#define REAPERRELOAD            "6"
#define REAPERMAXAMMONUM        5
#define REAPERRELOADNUM         6

#define DISABLEREAPERLEDSOPEN   "ZS"
#define DISABLEREAPERLEDSCLOSE  "ZX"

#define MAXRELOADVALUE          14
#define REAPERMAXAMMOF          5.0f
#define REAPERRECOIL            "Z5"
#define REAPERINITLEDS          "ZS"
#define REAPERHOLDBACKSLIDE     "Z0"
#define REAPERRELOADCOMMAND     "Z6"
#define DEFAULTAMMO0DELAY       145
#define REAPERHOLDSLIDETIME     5000
#define REAPERHOLDSLIDETIMEF    5.0f
#define REAPERHOLDSLIDEMIN      1000
#define REAPERHOLDSLIDEMAX      9000

//Second Default Light Gun
//Retro Shooter: MX24
#define MX24                    2
#define MX24NAME                "Retro Shooter: MX24"
#define MX24BAUD                0
#define MX24DATA                0
#define MX24PARITY              0
#define MX24STOP                0
#define MX24FLOW                0
#define MX24MAXAMMO             "0"
#define MX24RELOAD              "0"
#define MX24MAXAMMONUM          0
#define MX24RELOADNUM           0


//Third Default Light Gun
//JB Gun4IR
#define JBGUN4IR                3
#define JBGUN4IRNAME            "JB Gun4IR"
#define JBGUN4IRBAUD            4
#define JBGUN4IRDATA            0
#define JBGUN4IRPARITY          0
#define JBGUN4IRSTOP            0
#define JBGUN4IRFLOW            0
#define JBGUN4IRMAXAMMO         "0"
#define JBGUN4IRRELOAD          "0"
#define JBGUN4IRMAXAMMONUM      0
#define JBGUN4IRRELOADNUM       0
#define JBGUN4IRSTRENGTH        63

//Fourth Default Light Gun
//Fusion Light Gun
#define FUSION                  4
#define FUSIONNAME              "Fusion"
#define FUSIONBAUD              0
#define FUSIONDATA              0
#define FUSIONPARITY            0
#define FUSIONSTOP              0
#define FUSIONFLOW              0
#define FUSIONMAXAMMO           "0"
#define FUSIONRELOAD            "0"
#define FUSIONMAXAMMONUM        0
#define FUSIONRELOADNUM         0


//Fifth Default Light Gun
//Blamcon Light Gun
#define BLAMCON                 5
#define BLAMCONNAME             "Blamcon"
#define BLAMCONBAUD             4
#define BLAMCONDATA             0
#define BLAMCONPARITY           0
#define BLAMCONSTOP             0
#define BLAMCONFLOW             0
#define BLAMCONMAXAMMO          "0"
#define BLAMCONRELOAD           "0"
#define BLAMCONMAXAMMONUM       0
#define BLAMCONRELOADNUM        0

//Sixth Default Light Gun
//OpenFire Light Gun
#define OPENFIRE                6
#define OPENFIRENAME            "OpenFire"
#define OPENFIREBAUD            4
#define OPENFIREDATA            0
#define OPENFIREPARITY          0
#define OPENFIRESTOP            0
#define OPENFIREFLOW            0
#define OPENFIREMAXAMMO         "0"
#define OPENFIRERELOAD          "0"
#define OPENFIREMAXAMMONUM      0
#define OPENFIRERELOADNUM       0


//Seventh Default Light Gun
//Alien USB Light Gun
#define ALIENUSB                7
#define ALIENUSBNAME            "Alien USB"
#define ALIENUSBBAUD            0
#define ALIENUSBDATA            0
#define ALIENUSBPARITY          0
#define ALIENUSBSTOP            0
#define ALIENUSBFLOW            0
#define ALIENUSBMAXAMMO         "0"
#define ALIENUSBRELOAD          "0"
#define ALIENUSBMAXAMMONUM      0
#define ALIENUSBRELOADNUM       0

#define ALIENUSBVENDORID        0x04B4
#define ALIENUSBPRODUCTID       0x6870
#define ALIENUSBFRONTPATHREM    26
#define ALIENUSBPATHFIRST       15
#define ALIENUSBMINDELAY        40
#define ALIENUSBDELAYDFLT       65
#define ALIENUSBDELAYDFLTS      "65"

//Eighth Default Light Gun
//X-Gunner
#define XGUNNER                 8
#define XGUNNERNAME             "X-Gunner"
#define XGUNNERBAUD             4
#define XGUNNERDATA             0
#define XGUNNERPARITY           0
#define XGUNNERSTOP             0
#define XGUNNERFLOW             0
#define XGUNNERMAXAMMO          "0"
#define XGUNNERRELOAD           "0"
#define XGUNNERMAXAMMONUM       0
#define XGUNNERRELOADNUM        0

//Nineth Default Light Gun
//Ultimarc AimTrak USB Light Gun
#define AIMTRAK                9
#define AIMTRAKNAME            "Ultimarc AimTrak"
#define AIMTRAKBAUD            0
#define AIMTRAKDATA            0
#define AIMTRAKPARITY          0
#define AIMTRAKSTOP            0
#define AIMTRAKFLOW            0
#define AIMTRAKMAXAMMO         "0"
#define AIMTRAKRELOAD          "0"
#define AIMTRAKMAXAMMONUM      0
#define AIMTRAKRELOADNUM       0

#define AIMTRAKVENDORID        0xD209
#define AIMTRAKPRODUCTID       0x1601
#define AIMTRAKPRODUCTIDS      "0x160"
#define AIMTRAKPATHFRONT       "MI_00"
#define AIMTRAKFRONTPATHREM    26
#define AIMTRAKPATHFIRST       15
#define AIMTRAKMINDELAY        250
#define AIMTRAKDELAYDFLT       250
#define AIMTRAKDELAYDFLTS      "250"


//Tenth Default Light Gun
//Xenas Light Gun - Serial Port
#define XENAS                   10
#define XENASNAME               "Xenas Gun"
#define XENASBAUD               0
#define XENASDATA               0
#define XENASPARITY             0
#define XENASSTOP               0
#define XENASFLOW               0
#define XENASMAXAMMO            "0"
#define XENASRELOAD             "0"
#define XENASMAXAMMONUM         0
#define XENASRELOADNUM          0


//TCP Socket
//Address Name & Port Number
#define TCPHOSTNAME             "localhost"
#define TCPHOSTPORT             8000
//Time to Wait for TCP Socket Connection (msec)
#define TIMETOWAIT              12000
//Time for the TCP Socket Timer (msec)
#define TCPTIMERTIME            12200


//File & Dir Data

#define ENDOFFILE               "END_OF_FILE"
#define DEFAULTFILE             "default"

//Saved Light Gun & COM Devices Files & Dir
#define DATAFILEDIR             "data"
#define LIGHTGUNSAVEFILE        "lightguns.hor"
#define COMDEVICESAVEFILE       "comdevices.hor"
#define STARTLIGHTGUNSAVEFILE   "Light Gun Data File"
#define PLAYERSASSIGNMENTS      "Player Assignments"
#define STARTCOMDEVICESAVEFILE  "COM Device Data File"
#define LIGHTGUNNUMBERFILE      "Light Gun #"
#define COMDEVICENUMBERFILE     "COM Device #"
#define PASAVEFILE              "playersAss.hor"

//INI Game & Default Files
#define INIFILEDIR              "ini"
#define INIMAMEFILEDIR          "MAME"
#define ENDOFINIFILE            ".ini"
#define INIDEFAULTFILE          "iniDefault.hor"
#define JUSTMAME                "Mame"
#define SIGNALSTATE             "Output"

//Default Light Gun Game & Default Files
#define DEFAULTLGDIR            "defaultLG"
#define ENDOFLGFILE             ".txt"
#define LGDEFAULTFILE           "lgDefault.hor"



//Settings
#define SETTINGSSAVEFILE        "settings.hor"
#define STARTSETTINGSFILE       "Settings"
#define DEFAULTREFRESHDISPLAY   400


/////////
/// MASKS

//Input Masks for Refresh Display Time in Settings
#define REFRESHDISPLAYMASK      "000000"
//Input Mask for Analog Strength 8bit 0-255
#define ANALOGSTRENGTHMASK      "000"
//Input Mask for Reaper Ammo 0 Delay 1-255
#define REAPERAMMO0DELAYMASK    "000"
//Input Mask for Reaper Hold Slide Back Timing 1-9 seconds
#define REAPERHOLDSLIDEMASK     "9.9"

#define USBDEVICEMASK           "000"
#define USBINPUTMASKHEX         "HHHH"
#define USBRECOILDELAYMASK      "9999"

#define TESTHIDMASK             "HHHHHHHHHHHHHHHHHHHHHHHH"
#define TESTCOMMASK             "NNNNNNNNNNNNNNNNNNNNNNNN"

//Display
#define GAMEINFO                "Game Info:"
#define GAMEINFODASHES          "------------"
#define ROMEQUALS               "ROM="
#define ROMCOLON                "ROM: "
#define ORIENTATIONEQUAL0       "Orientation=0"
#define ORIENTATIONCLNEQUAL0    "Orientation: 0"
#define PAUSEEQUALS0            "Pause=0"
#define PAUSECLNEQUALS0         "Pause: 0"
#define OUTPUTSIGNALS           "Output Signals:"
#define OUTPUTSIGNALSDASHES     "-----------------"
#define PAUSEEQUALS             "Pause = "
#define PAUSECOLON              "Pause: "
#define PAUSEINDEX              6
#define ORIENTATIONINDEX        5
#define MAMENOGAMEEMPTY         "___empty"
#define TCPSOCKET               "TCP Socket: "
#define CONNECTED               "Connected\n"
#define NOCONNECTION            "No Connection\n"
#define GAMEFILE                "Game File: "





//Process TCP Socket Data
#define MAMESTART               "mame_start = "
#define MAMESTARTNOGAME         "mame_start = ___empty"
#define MAMESTOPFRONT           "mame_stop"
//Used In TCPReadReady
#define MAMEENDLINE             "\r"
#define FLYCASTENDLINE          "\n"
#define STATECHANGE             "statechange"
#define PAUSE                   "pause"
#define ROTATE                  "rotate"
#define REFRESHTIME             "refreshtime"
#define ORIENTATION             "Orientation"
#define GAMESTART               "game = "




//Processing INI & Default Light Gun
#define MAMESTARTAFTER          "mame_start"
#define MAMESTOPAFTER           "mame_stop"
#define MAMESTAFTER             "mame_st"

//Process INI Commands
#define PORTCMDSTART1           "cm"
#define PORTCMDSTART2           "cs"
#define COMPORTOPEN             "cmo"
#define COMPORTCLOSE            "cmc"
#define COMPORTSETTINGS         "css"
#define COMPORTSETLINE          "csl"
#define COMPORTREAD             "cmr"
#define COMPORTWRITE            "cmw"
#define BAUDREMOVE               0,5
#define PARITYREMOVE             0,7
#define DATAREMOVE               0,5
#define STOPREMOVE               0,5
#define SIGNALDATAVARIBLE       "%s%"
#define ININULLCMD              "nll"
#define USBHIDCMD               "ghd"
#define DIGIT1                  "%d1%"
#define DIGIT0                  "%d0%"
#define PERCENTAGESYMBOL        "%"




//Process Default LG Game File
#define PLAYERSSTART            "Players"
#define SIGNALSTARTCHAR         ':'
#define PLAYERSTARTCHAR         '*'
#define COMMANDSTARTCHAR        '>'
#define RECOMMANDSTARTCHAR      '#'
#define PLAYER2CHHAR            'P'
#define ALL2CHAR                'A'
#define SLOWMODE                "Slow"

//Number of Supported Recoil Commands 4: Ammo_Value, Recoil, Recoil_R2S, and Recoil_Value
#define NUMBEROFRECOILS         4

#define RECOILRELOAD            "Recoil & Reload"
#define OPTIONRELOADCMD         "#Reload"
#define OPTIONRECOIL_R2SCMD     "#Recoil_R2S"


//Process Default LG Commands
#define CMDSIGNAL               '>'
#define OPENCOMPORT             ">Open_COM"
#define OPENCOMPORTLENGTH       9
#define OPENCOMPORT2CHAR        'O'
#define OPENCOMPORT2CHAR2       'p'
#define CLOSECOMPORT            ">Close_COM"
#define CLOSECOMPORTLENGTH      10
#define CLOSECOMPORT2CHAR       "C"
#define DAMAGECMD               ">Damage"
#define RECOILCMD               ">Recoil"
#define RELOADCMD               ">Reload"
#define AMMOCMD                 ">Ammo"
#define AMMOCMDCOUNT            5
#define AMMOVALUECMD            ">Ammo_Value"
#define SHAKECMD                ">Shake"
#define AUTOLEDCMD              ">Auto_LED"
#define AUTOLEDCMD3CHAR         'u'
#define ARATIO169CMD            ">AspectRatio_16:9"
#define ARATIO169CMD13CHAR      "1"
#define ARATIO43CMD             ">AspectRatio_4:3"
#define JOYMODECMD              ">Joystick_Mode"
#define KANDMMODECMD            ">Keyboard_Mouse_Mode"
#define ALLPLAYERS              "*All"
#define DLGNULLCMD              ">Null"
#define RECOIL_R2SCMD           ">Recoil_R2S"
#define RECOILCMDCNT            8
#define RECOIL_R2SCMDCNT        12
#define RECOIL_R2SMINPERCT      30
#define OPENCOMPORTNOINIT       ">Open_COM_NoInit"
#define CLOSECOMPORTNOINIT      ">Close_COM_NoInit"
#define CLOSECOMPORTINITCHK     11
#define CLOSECOMPORTNOINIT11    'N'
#define CLOSECOMPORTINITONLYCMD ">Close_COM_InitOnly"
#define DISPLAYAMMOCMD          ">Display_Ammo"
#define DISPLAYAMMOINITCMD      ">Display_Ammo_Init"
#define DISPLAYLIFECMD          ">Display_Life"
#define DISPLAYLIFEINITCMD      ">Display_Life_Init"
#define DISPLAYOTHERCMD         ">Display_Other"
#define DISPLAYOTHERINITCMD     ">Display_Other_Init"
#define DISPLAYAMMOCMD9CHAR     'A'
#define DISPLAYLIFECMD9CHAR     'L'
#define DISPLAYOTHERCMD9CHAR    'O'
#define DISPLAYCHECK            9
#define RECOILVALUECMD          ">Recoil_Value"
#define RECOIL_R2SCMD8CHAR      'R'
#define RECOILVALUECMD8CHAR     'V'
#define RELOADVALUECMD          ">Reload_Value"
#define OFFSCREENBUTTONCMD      ">Offscreen_Button"
#define OFFSCREENNORMALSHOTCMD  ">Offscreen_Normal_Shot"

#define OPENCOMPORTONLY         "Open_COM"
#define CLOSECOMPORTONLY        "Close_COM"
#define DAMAGECMDONLY           "Damage"
#define RECOILCMDONLY           "Recoil"
#define RELOADCMDONLY           "Reload"
#define RELOADNORMBLCMDONLY     "Reload_No_Rumble"
#define AMMOCMDONLY             "Ammo"
#define AMMOVALUECMDONLY        "Ammo_Value"
#define SHAKECMDONLY            "Shake"
#define AUTOLEDCMDONLY          "Auto_LED"
#define ARATIO169CMDONLY        "AspectRatio_16:9"
#define ARATIO43CMDONLY         "AspectRatio_4:3"
#define JOYMODECMDONLY          "Joystick_Mode"
#define KANDMMODECMDONLY        "Keyboard_Mouse_Mode"
#define DLGNULLCMDONLY          "Null"
#define RECOIL_R2SONLY          "Recoil_R2S"
#define OPENCOMPORTNOINITONLY   "Open_COM_NoInit"
#define CLOSECOMPORTNOINITONLY  "Close_COM_NoInit"
#define CLOSECOMPORTINITONLYON  "Close_COM_InitOnly"
#define DISPLAYAMMOONLY         "Display_Ammo"
#define DISPLAYAMMOINITONLY     "Display_Ammo_Init"
#define DISPLAYLIFEONLY         "Display_Life"
#define DISPLAYLIFEINITONLY     "Display_Life_Init"
#define DISPLAYOTHERONLY        "Display_Other"
#define DISPLAYOTHERINITONLY    "Display_Other_Init"
#define DISPLAYREFRESHONLY      "Display_Refresh"
#define RECOILVALUEONLY         "Recoil_Value"
#define RELOADVALUECMDONLY      "Reload_Value"
#define RELOADVALUECMDSIZE      8
#define OFFSCREENBUTTONCMDONLY  "Offscreen_Button"
#define OFFSCREENRMLSHOTCMDONLY "Offscreen_Normal_Shot"
#define OFFSCREENCHARAT         11
#define OFFSCREENCHARATBUTTON   'B'



//Open Solenoid Safety Timer Default Time
#define OPENSOLENOIDDEFAULTTIME 200

//Display Refresh Default
#define DISPLAYREFRESHDEFAULT   100

#define OPENFIRELIFEBAR         'B'

#define OPENFIREAMMOINIT        "MDx2"
#define OPENFIRELIFEINIT        "MDx1"
#define OPENFIREALGLYPHS        "MDx3"
#define OPENFIREALBAR           "MDx3B"
#define OPENFIRELIFENUMCMD      "FDAx%s%"



//Not Used Yet, But Needed for Future
//#define DEFAULTCDDIR            "defaultCD"

////////////////////////
/// Structs
////////////////////////

//Struct that Stores the Default Light gun Data
struct S_DEFAULTLG
{
    quint8  BAUD;
    quint8  DATA;
    quint8  PARITY;
    quint8  STOP;
    quint8  FLOW;
    QString MAXAMMO;
    QString RELOADVALUE;
    quint16 MAXAMMON;
    quint16 RELOADVALUEN;
};

extern S_DEFAULTLG DEFAULTLG_ARRAY[];

struct INIPortStruct
{
    qint32 baud;
    quint8 data;
    quint8 parity;
    quint8 stop;
};


struct HIDInfo
{
    QString     path;
    QString     displayPath;
    quint16     vendorID;
    QString     vendorIDString;
    quint16     productID;
    QString     productIDString;
    QString     serialNumber;
    quint16     releaseNumber;
    QString     releaseString;
    QString     manufacturer;
    QString     productDiscription;
    quint16     usagePage;
    quint16     usage;
    QString     usageString;
    qint8       interfaceNumber;

    bool operator==(const HIDInfo& other) const
    {
        return (path == other.path) && (displayPath == other.displayPath) && (vendorID == other.vendorID) && (vendorIDString == other.vendorIDString)
               && (productID == other.productID) && (productIDString == other.productIDString) && (serialNumber == other.serialNumber) && (releaseNumber == other.releaseNumber)
               && (releaseString == other.releaseString)  && (manufacturer == other.manufacturer) && (productDiscription == other.productDiscription)
               && (usagePage == other.usagePage) && (usage == other.usage) && (usageString == other.usageString) && (interfaceNumber == other.interfaceNumber);
    }
};

struct SerialPortInfo
{
    QString     path;
    QString     productDiscription;
    QString     manufacturer;
    QString     serialNumber;
    bool        hasVendorID;
    bool        hasProductID;
    quint16     vendorID;
    QString     vendorIDString;
    quint16     productID;
    QString     productIDString;
    QString     portName;
};

struct SupportedRecoils
{
    quint8 ammoValue;
    quint8 recoil;
    quint8 recoilR2S;
    quint8 recoilValue;
};





#endif // GLOBAL_H
