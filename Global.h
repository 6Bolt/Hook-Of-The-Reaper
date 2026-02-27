#ifndef GLOBAL_H
#define GLOBAL_H

#include <qobject.h>

#define VERSION                 "1.2.9"
#define VERSIONMAIN             1
#define VERSIONMID              2
#define VERSIONLAST             8

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

//Light Guns Output Connections
#define SERIALPORT              0
#define USBHID                  1
#define BTLE                    2
#define TCP                     3
#define UNKNOWNCONNECT          -1

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
#define NUM_DEFAULTLG           14

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

#define MAXRELOADVALUE          15
#define LARGEAMMOVALUEDEFAULT   15
#define LARGEAMMOVALUEDEFAULTS  "15"
#define REAPERMAXAMMOF          5.0f
#define REAPERRECOIL            "Z5"
#define REAPERINITLEDS          "ZS"
#define REAPERHOLDBACKSLIDE     "Z0"
#define REAPERRELOADCOMMAND     "Z6"
#define REPEARDISABLESLIDECMD   "Z1"
#define DEFAULTAMMO0DELAY       145
#define REAPERHOLDSLIDETIME     5000
#define REAPERHOLDSLIDETIMEF    5.0f
#define REAPERHOLDSLIDEMIN      1000
#define REAPERHOLDSLIDEMAX      9000
#define DEFAULTAMMODELAYS       "145"
#define DEFAULTHOLDTIMES        "5.0"

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

#define OPENFIREENDCOM          "E"


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
#define AIMTRAKDELAYDFLTS      "550"


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

//Eleventh Default Light Gun
//Xenas Light Gun - BlueToothLE
#define XENASBTLE               11
#define XENASBTLENAME           "Xenas Gun BlueToothLE"
#define XENASBTLEBAUD           0
#define XENASBTLEDATA           0
#define XENASBTLEPARITY         0
#define XENASBTLESTOP           0
#define XENASBTLEFLOW           0
#define XENASBTLEMAXAMMO        "0"
#define XENASBTLERELOAD         "0"
#define XENASBTLEMAXAMMONUM     0
#define XENASBTLERELOADNUM      0

//Twelth Default Light Gun
//Sinden Light Gun - TCP Server on localhost
#define SINDEN                  12
#define SINDENNAME              "Sinden"
#define SINDENBAUD              0
#define SINDENDATA              0
#define SINDENPARITY            0
#define SINDENSTOP              0
#define SINDENFLOW              0
#define SINDENMAXAMMO           "0"
#define SINDENRELOAD            "0"
#define SINDENMAXAMMONUM        0
#define SINDENRELOADNUM         0

#define DISABLETRIGGERP1        "1K0"
#define DISABLETRIGGERP2        "2K0"
#define ENABLETRIGGERP1         "1K1"
#define ENABLETRIGGERP2         "2K1"
#define RECOILVOLTAGESETNUM     11
#define RECOILVOLTAGEMAX        10
#define RECOILVOLTDEFAULT       8
#define RECOILVOLTP1CMD         "1N"
#define RECOILVOLTP2CMD         "2N"
#define SINDENSINGLESHOT        0
#define SINDENSINGLESHOTCMD     'D'
#define SINDENAUTODEFAULT       1
#define SINDENAUTODEFAULTCMD    'G'
#define SINDENAUTOFAST          2
#define SINDENAUTOFASTCMD       'H'
#define SINDENAUTOSTRONGE       3
#define SINDENAUTOSTRONGECMD    'I'


//Thirtenth Default Light Gun
//RKADE Light Guns - Modified Alien USB Light Guns
#define RKADE                   13
#define RKADENAME               "RKADE Fixed Gun"
#define RKADEBAUD               0
#define RKADEDATA               0
#define RKADEPARITY             0
#define RKADESTOP               0
#define RKADEFLOW               0
#define RKADEMAXAMMO            "0"
#define RKADERELOAD             "0"
#define RKADEMAXAMMONUM         0
#define RKADERELOADNUM          0



//TCP Socket
//Address Name & Port Number
#define TCPHOSTNAME             "localhost"
#define TCPHOSTPORT             8000
//Time to Wait for TCP Socket Connection (msec)
#define TIMETOWAIT              3000
//Time for the TCP Socket Timer (msec)
#define TCPTIMERTIME            3050
#define TCPSLEEPTIME            500    // In ms
#define TIMETOWAITTCPSERVER     3000


//Light Gun Connection Wait Time
#define LGWAITTIME              50  //In ms

//File & Dir Data

#define ENDOFFILE               "END_OF_FILE"
#define DEFAULTFILE             "default"
#define FILELOCKTIME            300


//Saved Light Gun & COM Devices Files & Dir
#define DATAFILEDIR             "data"
#define LIGHTGUNSAVEFILE        "lightguns.hor"
#define COMDEVICESAVEFILE       "comdevices.hor"
#define STARTLIGHTGUNSAVEFILEV2 "Light Gun Data File V2"
#define STARTLIGHTGUNSAVEFILEV3 "Light Gun Data File V3"
#define PLAYERSASSIGNMENTS      "Player Assignments"
#define STARTCOMDEVICESAVEFILE  "COM Device Data File"
#define LIGHTGUNNUMBERFILE      "Light Gun #"
#define COMDEVICENUMBERFILE     "COM Device #"
#define PASAVEFILE              "playersAss.hor"
#define ENDGENERALSETTINGS      "END_GENERAL_SETTINGS"

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

#define DISPLAYCMDFRONT         ">Display_"
#define RELOADCMDFRONT          ">Reload"
#define DAMAGECMDFRONT          ">Damage"
#define DEATHCMDFRONT           ">Death_"
#define LIFECMDFRONT            ">Life_"
#define SHAKECMDFRONT           ">Shake"



//Settings
#define SETTINGSSAVEFILE        "settings.hor"
#define STARTSETTINGSFILE       "Settings"
#define STARTSETTINGSFILEV2     "Settings V2"
#define DEFAULTREFRESHDISPLAY   400


/////////
/// MASKS

//Input Masks for Refresh Display Time in Settings
#define REFRESHDISPLAYMASK      "000000"
//Input Mask for Analog Strength 8bit 0-255
#define ANALOGSTRENGTHMASK      "000"
//Input Mask for Reaper Ammo 0 Delay 1-255
#define REAPERAMMO0DELAYMASK    "000"
//Input Mask for Reaper Hold Slide Back Timing 1.0-9.0 seconds
#define REAPERHOLDSLIDEMASK     "9.9"

//Input Mask for Light Controller Time Delay
#define LIGHTCNTLRTIMEMASK      "00000"

#define USBDEVICEMASK           "000"
#define USBINPUTMASKHEX         "HHHH"
#define USBRECOILDELAYMASK      "9999"

#define TESTHIDMASK             "HHHHHHHHHHHHHHHHHHHHHHHH"
#define TESTCOMMASK             "NNNNNNNNNNNNNNNNNNNNNNNN"

#define TCPSERVERMASK           "00000"
#define TCPSERVERDEFAULT        "13000"
#define TCPSERVERDEFAULTNUM     13000
#define MAXTCPPORT              65535
#define TCPPLAYER1              1
#define TCPPLAYER2              2
#define TCPPLAYER1NAME          "Player 1"
#define TCPPLAYER2NAME          "Player 2"
#define MAXTCPSERVERS           2

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
//#define MAMESTART               "mame_start = "
#define MAMESTART               "mame_start"
#define MAMESTARTNOGAME         "mame_start = ___empty"
#define MAMESTOPFRONT           "mame_stop"
#define MAMEEMPTY               "___empty"
//Used In TCPReadReady
#define MAMEENDLINE             "\r"
#define FLYCASTENDLINE          "\n"
#define STATECHANGE             "statechange"
#define PAUSE                   "pause"
#define MAMEPAUSE               "MamePause"
#define ROTATE                  "rotate"
#define REFRESHTIME             "refreshtime"
#define ORIENTATION             "Orientation"
#define MAMEORIENTATION         "MameOrientation"
//#define GAMESTART               "game = "
#define GAMESTART               "game"
#define GAMESTOP                "game_stop"




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
#define STATESSECTION           "[States]"
#define OPTIONSSECTION          "[Options]"
#define ENDOPTIONS              "End Options"
#define BLOCKSHAKEOPTION        "Block Shake"
#define BLOCKRECOIL_R2SOPTION   "Block Recoil_R2S"
#define BLOCKSHAKELENGTH        4
#define BLOCKRECOIL_R2SLENGTH   4
#define BLOCKSHAKESTARTLENGTH   5
#define BLOCKRECOIL_R2STRLENGTH 5
#define BLOCKSTART              "Start"
#define OVERRIDERECOILVOLT      "Override_Recoil_Voltage"
#define OVERRIDERECOILLENGTH    2
#define OVERRIDERECOILVOLTNUM   1
#define SINDENTRIGGERRECOIL     "Sinden_Trigger_Recoil"
#define SINDENTRIGGERRECOILLEN  2
#define SINDENTRIGGERRECOILNUM  1
#define SINDENTRIGGERRECOILMAX  3
#define AMMOCHECKOPTION         "Ammo_Check"


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
#define OFFSCREENLEFTCORNERCMD  ">Offscreen_Left_Corner"
#define OFFSCREENDISABLECMD     ">Offscreen_Disable"
#define BLOCKSIGNALCOMMAND      ">Block_Signal"
#define RELOADVALUECMDSIZE      8
#define OFFSCREENCHARAT         11
#define OFFSCREENCHARATBUTTON   'B'
#define OFFSCREENCHARATNORMAL   'N'
#define OFFSCREENCHARATLEFT     'L'
#define LIFEVALUECMD            ">Life_Value"
#define DEATHVALUECMD           ">Death_Value"

#define OPENCOMPORTONLY         "Open_COM"
#define CLOSECOMPORTONLY        "Close_COM"
#define DAMAGECMDONLY           "Damage"
#define DAMAGELEDCMDONLY        "Damage_LED"
#define RECOILCMDONLY           "Recoil"
#define RELOADCMDONLY           "Reload"
#define RELOADLEDCMDONLY        "Reload_LED"
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
#define OUTOFAMMOONLY           "Out_Of_Ammo"
#define RELOADAMMOONLY          "Reload_Ammo"
#define OFFSCREENBUTTONCMDONLY  "Offscreen_Button"
#define OFFSCREENRMLSHOTCMDONLY "Offscreen_Normal_Shot"
#define OFFSCREENLEFTCORNERONLY "Offscreen_Left_Corner"
#define OFFSCREENDISABLECMDONLY "Offscreen_Disable"
#define LIFEVALUECMDONLY        "Life_Value"
#define LIFEVALUELEDCMDONLY     "Life_Value_LED"
#define DEATHVALUECMDONLY       "Death_Value"
#define DEATHVALUELEDCMDONLY    "Death_Value_LED"

//Max Damage Percentage
#define MAXDAMAGEPERCENTAGE     0.35f


//Delay
#define DELAYCMD0               'D'
#define DELAYCMD1               ':'


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



////////////////////////
/// Light Controllers
////////////////////////

//Light Controllers Save File
#define LIGHTCNTLRSSAVEFILE     "lightControllers.hor"
#define STARTLIGHTCNTLRSSAVE    "Light Controllers Data File V0"
#define LIGHTCNTLRNUMBERFILE    "Light Controller #"

//Light Controller Game Files
#define LIGHTGAMEFILESDIR       "lightFiles"
#define ENDOFLIGHTFILE          ".txt"
#define DEFAULTLIGHTFILE        "default.txt"

#define INTERNALRECOILR2S       "_CtmRecoil"



// Light Controller Group File Stuff
#define REGULARGROUP            "GRP"
#define RGBGROUP                "GRP_RGB"
#define RGBFASTGROUP            "GRP_RGB_FAST"
#define RGBCOLOR                "Color"
#define ENDGROUP                "END_GRP"
#define COLORSIZE               5
#define COLORMAP                "Color_Map"
#define COLORMAPSIZE            4
#define DEFAULTBRIGHTNESS       "Default_Brightness"
#define DEFAULTBRIGHTNESSSIZE   2
#define INVERTDATASYMBOL        '~'


//Max Light Controllers Supported
#define MAXLIGHTCONTROLLERS     32


//Light Controller Makers
#define ULTIMARC                0


//PacDrive, U-HID, and Blue-HID: 16 LED Channels with No Brightness (0 - Off and 1 - On)
//Nano-LED: 60 LED Channels with 256 Brightness Levels (20 RGB LEDs)
//PacLED64: 64 LED Channels with 256 Brightness Levels (21 RGB LEDs)
//I-Pac Ultimate I/O: 96 LED Channels with 256 Brightness Levels (32 RGB LEDs)

//Ultimarc
#define PACDRIVE                1
#define UHID                    2
#define BLUEHID                 3
#define NANOLED                 4
#define PACLED64                5
#define IPACULTIMATEIO          6

#define ULTIMARCTYPES           7

#define UHID_LOW                5377
#define NANOLED_LOW             5249
#define PACLED64_LOW            5121
#define IPACULTIMATEIO_LOW      1040

#define SIMPLEPACHID            16
#define NANOLEDPINS             60
#define PACLED64PINS            64
#define PACULTIMATEPINS         96

#define ULTIMATEGRPS            12
#define OTHERGRPS               8
#define SMALLGROUPS             2

#define PACDRIVENAME            "PacDrive"
#define UHIDNAME                "U-HID"
#define BLUEHIDNAME             "Blue-HID"
#define NANOLEDNAME             "Nano-LED"
#define PACLED64NAME            "PacLED64"
#define IPACULTIMATEIONAME      "I-Pac Ultimate I/O"

extern QString ULTIMARCTYPENAME[];
extern quint8 ULTIMARCTYPELEDCOUNT[];
extern quint8 ULTIMARCTYPERGBLEDCOUNT[];
extern quint8 ULTIMARCTYPEBRIGHTNESS[];


#define ULTIMARCMAXDEVICES      24
//#define DEFAULTBRIGHTNESS       160

//Light Controller Commands

//Kinds of Commands
#define UNKNOWNCOMMAND          0

//RGB
#define FLASHCOMMAND            1
#define SEQUENCECOMMAND         2
#define FOLLOWERCOMMAND         3

#define FLASHCMCOMMAND          4
#define SEQUENCECMCOMMAND       5

//Regular
#define REGCOMMANDS             20
#define REGFLASHCOMMAND         21
#define REGSEQUENCECOMMAND      22
#define REGFOLLOWERCOMMAND      23

//Background Command
#define BACKGROUNDCOMMAND       40

//General Commands
#define GENERALCOMMAND          50



//Flashes

//RGB
#define FLASHRGB                "Flash_RGB"
#define FLASHRGBARGS            4
#define FLASHRGBCMD             0
#define RELOADFLASHRGB          "Reload_Flash_RGB"
#define RELOADFLASHRGBARGS      5
#define RELOADFLASHRGBCMD       1
#define DEATHFLASHRGB           "Death_Flash_RGB"
#define DEATHFLASHRGBARGS       5
#define DEATHFLASHRGBCMD        2
#define RANDOMFLASHRGB          "Random_Flash_RGB"
#define RANDOMFLASHRGBARGS      4
#define RANDOMFLASHRGBCMD       3
#define RANDOMFLASHRGB2C        "Random_Flash_2C_RGB"
#define RANDOMFLASHRGB2CARGS    5
#define RANDOMFLASHRGB2CCMD     4

//RGB Color Map
#define RANDOMFLASHRGBCM        "Random_Flash_RGB_CM"
#define RANDOMFLASHRGBCMARGS    4
#define RANDOMFLASHRGBCMCMD     0



//Regular
#define FLASHREG                "Flash_Regular"
#define FLASHREGARGS            3
#define FLASHREGCMD             0
#define RELOADFLASHREG          "Reload_Flash_Regular"
#define RELOADFLASHREGARGS      4
#define RELOADFLASHREGCMD       1
#define DEATHFLASHREG           "Death_Flash_Regular"
#define DEATHFLASHREGARGS       4
#define DEATHFLASHREGCMD        2
#define RANDOMFLASHREG          "Random_Flash_Regular"
#define RANDOMFLASHREGARGS      3
#define RANDOMFLASHREGCMD       3



//Sequence

//RGB
#define SEQUENCERGB             "Sequence_RGB"
#define SEQUENCERGBARGS         2
#define SEQUENCERGBCMD          0
#define RELOADSEQUENCERGB       "Reload_Sequence_RGB"
#define RELOADSEQUENCERGBARGS   3
#define RELOADSEQUENCERGBCMD    1

//RGB Color Map
#define SEQUENCERGBCM           "Sequence_RGB_CM"
#define SEQUENCERGBCMARGS       2
#define SEQUENCERGBCMCMD        0
#define RELOADSEQUENCERGBCM     "Reload_Sequence_RGB_CM"
#define RELOADSEQUENCERGBCMARGS 3
#define RELOADSEQUENCERGBCMCMD  1


//Regular
#define SEQUENCEREG             "Sequence_Regular"
#define SEQUENCEREGARGS         1
#define SEQUENCEREGCMD          0
#define RELOADSEQUENCEREG       "Reload_Sequence_Regular"
#define RELOADSEQUENCEREGARGS   2
#define RELOADSEQUENCEREGCMD    1

//Follower Commands

//RGB
#define FOLLOWERRGB             "Follower_RGB"
#define FOLLOWERRGBARGS         1
#define FOLLOWERRGBCMD          0
#define FOLLOWERRANDOMRGB       "Follower_Random_RGB"
#define FOLLOWERRANDOMRGBARGS   0
#define FOLLOWERRANDOMRGBCMD    1

//Regular
#define FOLLOWERREG             "Follower_Regular"
#define FOLLOWERREGARGS         0
#define FOLLOWERREGCMD          0


//Background Commands

//Background RGB
#define BACKGROUNDRGB           "Background_RGB"
#define BACKGROUNDRGBARGS       5
#define BACKGROUNDRGBCMD        0

//Background Regular
#define BACKGROUNDREG           "Background_Regular"
#define BACKGROUNDREGARGS       4
#define BACKGROUNDREGCMD        1


//General Commands

#define TURNOFFLIGHTS           "TurnOffLights"
#define TURNOFFLIGHTSARGS       0
#define TURNOFFLIGHTSCMD        0






//Execution List
#define NUMBEREXECUTIONS        256


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


struct LightGunSettings
{
    quint8 reload;
    quint8 damage;
    quint8 death;
    bool shake;
};

struct ReaperSlideData
{
    bool disableHoldBack;
    bool enableHoldDelay;
    quint8 holdDelay;
    quint16 slideHoldTime;
};

struct DisplayPriority
{
    bool ammo;
    bool life;
    bool other;
};

struct DisplayOpenFire
{
    bool ammoAndLifeDisplay;
    bool lifeGlyphs;
    bool lifeBar;
    bool lifeNumber;
};

struct UltimarcData
{
    qint8 type;
    QString typeName;
    quint16 vendorID;
    QString vendorIDS;
    quint16 productID;
    QString productIDS;
    quint8 id;
    quint16 version;
    QString versionS;
    QString vendorName;
    QString productName;
    QString serialNumber;
    QString devicePath;
    QString groupFile = "";
    bool valid = false;
    bool inList = false;
    quint8 deviceID;

    bool operator==(const UltimarcData& other) const
    {
        return (type == other.type) && (typeName == other.typeName) && (vendorID == other.vendorID) && (vendorIDS == other.vendorIDS) && (productID == other.productID)
        && (productIDS == other.productIDS) && (version == other.version) && (vendorName == other.vendorName) && (productName == other.productName)
               && (serialNumber == other.serialNumber)  && (devicePath == other.devicePath) && (versionS == other.versionS)  && (id == other.id) && (deviceID == other.deviceID);
    }
};


struct LightControllerTop
{
    quint8 groupNumber;
    //quint8 defaultBrightness;
    bool isDataSet;
};

struct LCPinData
{
    QList<quint8> pinData;
    quint8 numberPins;
    bool isDataSet;
};

struct RGBColor
{
    quint8 r;
    quint8 g;
    quint8 b;
};

struct RGBPins
{
    quint8 r;
    quint8 g;
    quint8 b;
};


struct SerialPortData
{
    quint8 portNum;
    QString portName;
    qint32 portBaud;
    quint8 portData;
    quint8 portParity;
    quint8 portStop;
    quint8 portFlow;
    QString portPath;
    bool isWriteOnly;
};


#endif // GLOBAL_H
