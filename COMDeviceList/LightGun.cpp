#include "LightGun.h"
#include "../Global.h"

#include "../../saeicmrterta.h"

//Constructors

//For RS3 Reaper Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, quint16 maNumber, quint16 rvNumber, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    defaultLightGun = lgDefault;
    if(defaultLightGun)
        defaultLightGunNum = dlgNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgName;
    lightGunNum = lgNumber;

    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

    maxAmmo = maNumber;
    reloadValue = rvNumber;

    maxAmmoSet = true;
    reloadValueSet = true;

    disableReaperLEDs = false;

    lastAmmoValue = 0;

    isDipSwitchPlayerNumberSet = false;

    analogStrength = DEFAULTANALOGSTRENGTH;
    isAnalogStrengthSet = false;

    hubComPortNumber = UNASSIGN;

    isUSBLightGun = false;
    isRecoilDelaySet = false;


    //Display Init
    hasDisplayAmmoInited = false;
    hasDisplayLifeInited = false;
    hasDisplayOtherInited = false;
    hasDisplayAmmoAndLifeInited = false;
    ammoDisplayValue = -1;
    lifeDisplayValue = -1;
    otherDisplayValue = -1;
    displayAmmoPriority = true;
    displayLifePriority = false;
    displayOtherPriority = false;
    displayRefresh = DISPLAYREFRESHDEFAULT;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    reloadNoRumble = reloadNR;
    reloadDisable = reloadDis;

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = 1;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;

    FillSerialPortInfo();

    LoadDefaultLGCommands();

}

//For Normal Serial Port Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    defaultLightGun = lgDefault;
    if(defaultLightGun)
        defaultLightGunNum = dlgNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgName;
    lightGunNum = lgNumber;

    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

    if(defaultLightGun)
    {
        if(defaultLightGunNum == RS3_REAPER)
        {
            maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
            reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
            maxAmmoSet = true;
            reloadValueSet = true;
        }
        else
        {
            maxAmmoSet = false;
            reloadValueSet = false;
        }
    }
    else
    {
        maxAmmoSet = false;
        reloadValueSet = false;
    }

    disableReaperLEDs = false;

    isDipSwitchPlayerNumberSet = false;
    hubComPortNumber = UNASSIGN;

    analogStrength = DEFAULTANALOGSTRENGTH;
    isAnalogStrengthSet = false;

    lastAmmoValue = 0;

    isUSBLightGun = false;
    isRecoilDelaySet = false;


    //Display Init
    hasDisplayAmmoInited = false;
    hasDisplayLifeInited = false;
    hasDisplayOtherInited = false;
    hasDisplayAmmoAndLifeInited = false;
    ammoDisplayValue = -1;
    lifeDisplayValue = -1;
    otherDisplayValue = -1;
    displayAmmoPriority = true;
    displayLifePriority = false;
    displayOtherPriority = false;
    displayRefresh = DISPLAYREFRESHDEFAULT;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    reloadNoRumble = reloadNR;
    reloadDisable = reloadDis;

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = 1;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;

    FillSerialPortInfo();

    LoadDefaultLGCommands();
}

//For Copy Light Gun
LightGun::LightGun(LightGun const &lgMember)
{
    //*this = lgMember;

    defaultLightGun = lgMember.defaultLightGun;
    if(defaultLightGun)
        defaultLightGunNum = lgMember.defaultLightGunNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgMember.lightGunName;
    lightGunNum = lgMember.lightGunNum;

    comPortNum = lgMember.comPortNum;
    comPortString = lgMember.comPortString;
    comPortInfo = lgMember.comPortInfo;
    comPortBaud = lgMember.comPortBaud;
    comPortDataBits = lgMember.comPortDataBits;
    comPortParity = lgMember.comPortParity;
    comPortStopBits = lgMember.comPortStopBits;
    comPortFlow = lgMember.comPortFlow;

    if(lgMember.maxAmmoSet)
    {
        maxAmmo = lgMember.maxAmmo;
        maxAmmoSet = true;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
        maxAmmoSet = true;
    }
    else
    {
        maxAmmoSet = false;
    }

    if(lgMember.reloadValueSet)
    {
        reloadValue = lgMember.reloadValue;
        reloadValueSet = true;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
        reloadValueSet = true;
    }
    else
    {
        reloadValueSet = false;
    }

    disableReaperLEDs = lgMember.disableReaperLEDs;

    if(lgMember.isDipSwitchPlayerNumberSet)
    {
        dipSwitchPlayerNumber = lgMember.dipSwitchPlayerNumber;
        isDipSwitchPlayerNumberSet = true;
    }
    else
        isDipSwitchPlayerNumberSet = false;

    hubComPortNumber = lgMember.hubComPortNumber;

    if(lgMember.isAnalogStrengthSet)
    {
        analogStrength = lgMember.analogStrength;
        isAnalogStrengthSet = true;
    }
    else
    {
        analogStrength = DEFAULTANALOGSTRENGTH;
        isAnalogStrengthSet = false;
    }

    isUSBLightGun = lgMember.isUSBLightGun;
    isRecoilDelaySet = lgMember.isRecoilDelaySet;

    if(isUSBLightGun)
        usbHIDInfo = lgMember.usbHIDInfo;

    if(isRecoilDelaySet)
        recoilDelay = lgMember.recoilDelay;
    else
        recoilDelay = 0;


    serialPortInfo = lgMember.serialPortInfo;

    if(!isUSBLightGun)
        FillSerialPortInfo();

    currentPath = lgMember.currentPath;
    dataPath = lgMember.dataPath;
    defaultLGFilePath = lgMember.defaultLGFilePath;

    lastAmmoValue = 0;

    //Display Init
    hasDisplayAmmoInited = lgMember.hasDisplayAmmoInited;
    hasDisplayLifeInited = lgMember.hasDisplayLifeInited;
    hasDisplayOtherInited = lgMember.hasDisplayOtherInited;
    hasDisplayAmmoAndLifeInited = lgMember.hasDisplayAmmoAndLifeInited;
    ammoDisplayValue = lgMember.ammoDisplayValue;
    lifeDisplayValue = lgMember.lifeDisplayValue;
    otherDisplayValue = lgMember.otherDisplayValue;
    displayAmmoPriority = lgMember.displayAmmoPriority;
    displayLifePriority = lgMember.displayLifePriority;
    displayOtherPriority = lgMember.displayOtherPriority;
    displayRefresh = lgMember.displayRefresh;

    displayAmmoLife = lgMember.displayAmmoLife;
    displayAmmoLifeGlyphs = lgMember.displayAmmoLifeGlyphs;
    displayAmmoLifeBar = lgMember.displayAmmoLifeBar;
    displayAmmoLifeNumber = lgMember.displayAmmoLifeNumber;
    lifeBarMaxLife = lgMember.lifeBarMaxLife;

    lgRecoilPriority[0] = lgMember.lgRecoilPriority[0];
    lgRecoilPriority[1] = lgMember.lgRecoilPriority[1];
    lgRecoilPriority[2] = lgMember.lgRecoilPriority[2];
    lgRecoilPriority[3] = lgMember.lgRecoilPriority[3];

    reloadNoRumble = lgMember.reloadNoRumble;
    reloadDisable = lgMember.reloadDisable;

    reaperLargeAmmo = lgMember.reaperLargeAmmo;
    isReaper5LEDsInited = lgMember.isReaper5LEDsInited;
    enableReaperAmmo0Delay = lgMember.enableReaperAmmo0Delay;
    repearAmmo0Delay = lgMember.repearAmmo0Delay;
    isReaperAmmo0TimerInited = false;

    if(lgMember.isReaperAmmo0TimerInited)
        InitReaperAmmo0Timer();


    reaperHoldSlideTime = lgMember.reaperHoldSlideTime;
    isReaperTimerRunning = lgMember.isReaperTimerRunning;
    isReaperSlideHeldBack = lgMember.isReaperSlideHeldBack;

    LoadDefaultLGCommands();

}

//For MX24 Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, bool dipSwitchSet, quint8 dipSwitchNumber, quint8 hubcpNumber, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    defaultLightGun = lgDefault;
    if(defaultLightGun)
        defaultLightGunNum = dlgNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgName;
    lightGunNum = lgNumber;

    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

    isDipSwitchPlayerNumberSet = dipSwitchSet;
    dipSwitchPlayerNumber = dipSwitchNumber;

    hubComPortNumber = hubcpNumber;

    if(defaultLightGun)
    {
        if(defaultLightGunNum == RS3_REAPER)
        {
            maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
            reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
            maxAmmoSet = true;
            reloadValueSet = true;
        }
        else
        {
            maxAmmoSet = false;
            reloadValueSet = false;
        }
    }
    else
    {
        maxAmmoSet = false;
        reloadValueSet = false;
    }

    disableReaperLEDs = false;

    analogStrength = DEFAULTANALOGSTRENGTH;
    isAnalogStrengthSet = false;

    lastAmmoValue = 0;

    isUSBLightGun = false;
    isRecoilDelaySet = false;

    //Display Init
    hasDisplayAmmoInited = false;
    hasDisplayLifeInited = false;
    hasDisplayOtherInited = false;
    hasDisplayAmmoAndLifeInited = false;
    ammoDisplayValue = -1;
    lifeDisplayValue = -1;
    otherDisplayValue = -1;
    displayAmmoPriority = true;
    displayLifePriority = false;
    displayOtherPriority = false;
    displayRefresh = DISPLAYREFRESHDEFAULT;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    reloadNoRumble = reloadNR;
    reloadDisable = reloadDis;

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = 1;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;

    FillSerialPortInfo();

    LoadDefaultLGCommands();
}

//For JB Gun4IR
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, quint8 analStrength, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    defaultLightGun = lgDefault;
    if(defaultLightGun)
        defaultLightGunNum = dlgNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgName;
    lightGunNum = lgNumber;

    comPortNum = cpNumber;
    comPortString = cpString;
    comPortInfo = cpInfo;
    comPortBaud = cpBaud;
    comPortDataBits = cpDataBits;
    comPortParity = cpParity;
    comPortStopBits = cpStopBits;
    comPortFlow = cpFlow;

    //For MX24
    isDipSwitchPlayerNumberSet = false;
    hubComPortNumber = UNASSIGN;


    //For RS3 Reaper
    if(defaultLightGun)
    {
        if(defaultLightGunNum == RS3_REAPER)
        {
            maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
            reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
            maxAmmoSet = true;
            reloadValueSet = true;
        }
        else
        {
            maxAmmoSet = false;
            reloadValueSet = false;
        }
    }
    else
    {
        maxAmmoSet = false;
        reloadValueSet = false;
    }

    disableReaperLEDs = false;

    analogStrength = analStrength;
    isAnalogStrengthSet = true;

    lastAmmoValue = 0;

    isUSBLightGun = false;
    isRecoilDelaySet = false;

    //Display Init
    hasDisplayAmmoInited = false;
    hasDisplayLifeInited = false;
    hasDisplayOtherInited = false;
    hasDisplayAmmoAndLifeInited = false;
    ammoDisplayValue = -1;
    lifeDisplayValue = -1;
    otherDisplayValue = -1;
    displayAmmoPriority = true;
    displayLifePriority = false;
    displayOtherPriority = false;
    displayRefresh = DISPLAYREFRESHDEFAULT;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    reloadNoRumble = reloadNR;
    reloadDisable = reloadDis;

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = 1;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;

    FillSerialPortInfo();

    LoadDefaultLGCommands();
}

//Alien USB Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    defaultLightGun = lgDefault;
    defaultLightGunNum = dlgNum;
    lightGunName = lgName;
    lightGunNum = lgNumber;

    usbHIDInfo = hidInfoStruct;

    comPortNum = UNASSIGN;
    comPortBaud = UNASSIGN;
    comPortDataBits = UNASSIGN;
    comPortParity = UNASSIGN;
    comPortStopBits = UNASSIGN;
    comPortFlow = UNASSIGN;

    isDipSwitchPlayerNumberSet = false;
    hubComPortNumber = UNASSIGN;
    maxAmmoSet = false;
    reloadValueSet = false;
    disableReaperLEDs = false;
    isAnalogStrengthSet = false;

    lastAmmoValue = 0;

    isUSBLightGun = true;
    isRecoilDelaySet = false;
    recoilDelay = 0;

    //Display Init
    hasDisplayAmmoInited = false;
    hasDisplayLifeInited = false;
    hasDisplayOtherInited = false;
    hasDisplayAmmoAndLifeInited = false;
    ammoDisplayValue = -1;
    lifeDisplayValue = -1;
    otherDisplayValue = -1;
    displayAmmoPriority = true;
    displayLifePriority = false;
    displayOtherPriority = false;
    displayRefresh = DISPLAYREFRESHDEFAULT;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    reloadNoRumble = reloadNR;
    reloadDisable = reloadDis;

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = 1;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;

    LoadDefaultLGCommands();
}


LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, quint16 rcDelay, SupportedRecoils lgRecoils, bool reloadNR, bool reloadDis)
{
    defaultLightGun = lgDefault;
    defaultLightGunNum = dlgNum;
    lightGunName = lgName;
    lightGunNum = lgNumber;

    usbHIDInfo = hidInfoStruct;

    comPortNum = UNASSIGN;
    comPortBaud = UNASSIGN;
    comPortDataBits = UNASSIGN;
    comPortParity = UNASSIGN;
    comPortStopBits = UNASSIGN;
    comPortFlow = UNASSIGN;

    isDipSwitchPlayerNumberSet = false;
    hubComPortNumber = UNASSIGN;
    maxAmmoSet = false;
    reloadValueSet = false;
    disableReaperLEDs = false;
    isAnalogStrengthSet = false;

    lastAmmoValue = 0;

    isUSBLightGun = true;
    isRecoilDelaySet = true;

    if(rcDelay == AIMTRAKDELAYDFLT)
        recoilDelay = rcDelay;
    else
        recoilDelay = AIMTRAKDELAYDFLT;

    //Display Init
    hasDisplayAmmoInited = false;
    hasDisplayLifeInited = false;
    hasDisplayOtherInited = false;
    hasDisplayAmmoAndLifeInited = false;
    ammoDisplayValue = -1;
    lifeDisplayValue = -1;
    otherDisplayValue = -1;
    displayAmmoPriority = true;
    displayLifePriority = false;
    displayOtherPriority = false;
    displayRefresh = DISPLAYREFRESHDEFAULT;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    reloadNoRumble = reloadNR;
    reloadDisable = reloadDis;

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = 1;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;

    LoadDefaultLGCommands();
}

//Deconstructor

LightGun::~LightGun()
{

    if(isReaperTimerRunning && isReaperAmmo0TimerInited)
    {
        //Stop Timer
        p_reaperAmmo0Timer->stop ();
        isReaperTimerRunning = false;

        //Check if the Slide is Held Back
        if(isReaperSlideHeldBack)
        {
            //Send Reload command (Z6) to the Reaper Light Gun tp bring slide back
            emit WriteCOMPort(comPortNum, REAPERRELOADCOMMAND);
            isReaperSlideHeldBack = false;
        }
    }

    if(isReaperAmmo0TimerInited)
        delete p_reaperAmmo0Timer;
}

//public member functions


//Set Member Functions

void LightGun::SetDefaultLightGun(bool lgDefault)
{
    defaultLightGun = lgDefault;
}

void LightGun::SetDefaultLightGunNumer(quint8 dlgNum)
{
    defaultLightGunNum = dlgNum;
}

void LightGun::SetLightGunName(QString lgnString)
{
    lightGunName = lgnString;
}

void LightGun::SetLightGunNumber(quint8 lgNumber)
{
    lightGunNum = lgNumber;
}

void LightGun::SetComPortNumber(quint8 cpNumber)
{
    comPortNum = cpNumber;
}

void LightGun::SetComPortString(QString cpString)
{
    comPortString = cpString;
}

void LightGun::SetComPortInfo(QSerialPortInfo cpInfo)
{
    comPortInfo = cpInfo;
}

void LightGun::SetComPortBaud(qint32 cpBaud)
{
    comPortBaud = cpBaud;
}

void LightGun::SetComPortDataBits(quint8 cpDataBits)
{
    comPortDataBits = cpDataBits;
}

void LightGun::SetComPortParity(quint8 cpParity)
{
    comPortParity = cpParity;
}

void LightGun::SetComPortStopBits(quint8 cpStopBits)
{
    comPortStopBits = cpStopBits;
}

void LightGun::SetComPortFlow(quint8 cpFlow)
{
    comPortFlow = cpFlow;
}

void LightGun::SetMaxAmmo(quint16 maNumber)
{
    maxAmmo = maNumber;
    maxAmmoSet = true;
}

void LightGun::SetReloadValue(quint16 rvNumber)
{
    reloadValue = rvNumber;
    reloadValueSet = true;
}

void LightGun::SetDipSwitchPlayerNumber(quint8 dsNumber)
{
    dipSwitchPlayerNumber = dsNumber;
    isDipSwitchPlayerNumberSet = true;
}

void LightGun::SetAnalogStrength(quint8 analStrength)
{
    analogStrength = analStrength;
    isAnalogStrengthSet = true;
}

void LightGun::SetHubComPortNumber(quint8 hcpNumber)
{
    hubComPortNumber = hcpNumber;
}

void LightGun::SetHIDInfo(HIDInfo hidInfoStruct)
{
    usbHIDInfo = hidInfoStruct;
}

void LightGun::SetRecoilDelay(quint16 rcDelay)
{
    recoilDelay = rcDelay;
    isRecoilDelaySet = true;
}

void LightGun::SetDisableReaperLEDs(bool disableRLED)
{
    disableReaperLEDs = disableRLED;
}

void LightGun::SetDisplayPriority(bool ammo, bool life)
{
    if((ammo && life) || (!ammo && !life))
    {
        displayAmmoPriority = true;
        displayLifePriority = false;
    }
    else
    {
        displayAmmoPriority = ammo;
        displayLifePriority = life;
    }
}

void LightGun::SetDisplayOtherPriority(bool other)
{
    displayOtherPriority = other;
}

void LightGun::SetDisplayAmmoAndLife(bool displayAAL, bool displayLG, bool displayLB, bool displayLN)
{
    displayAmmoLife = displayAAL;
    displayAmmoLifeGlyphs = displayLG;
    displayAmmoLifeBar = displayLB;
    displayAmmoLifeNumber = displayLN;

}

void LightGun::SetRecoilPriority(SupportedRecoils lgRecoils)
{
    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;
}

void LightGun::SetReloadOptions(bool reloadNR, bool reloadDis)
{
    reloadNoRumble = reloadNR;
    reloadDisable = reloadDis;

    LoadDefaultLGCommands();
}

void LightGun::SetReaperAmmo0Delay(bool isAmmo0DelayEnabled, quint8 delayTime, quint16 reaperHST)
{
    enableReaperAmmo0Delay = isAmmo0DelayEnabled;
    repearAmmo0Delay = delayTime;
    reaperHoldSlideTime = reaperHST;

    if(!isReaperAmmo0TimerInited)
        InitReaperAmmo0Timer();
    else
        p_reaperAmmo0Timer->setInterval(repearAmmo0Delay);

}

//Get Member Functions

bool LightGun::GetDefaultLightGun()
{
    return defaultLightGun;
}

quint8 LightGun::GetDefaultLightGunNumber()
{
    return defaultLightGunNum;
}

QString LightGun::GetLightGunName()
{
    return lightGunName;
}

quint8 LightGun::GetLightGunNumber()
{
    return lightGunNum;
}

quint8 LightGun::GetComPortNumber()
{
    if(defaultLightGun && defaultLightGunNum == MX24)
        return hubComPortNumber;

    return comPortNum;
}

quint8 LightGun::GetComPortNumberBypass()
{
    return comPortNum;
}

QString LightGun::GetComPortString()
{
    return comPortString;
}

QSerialPortInfo LightGun::GetComPortInfo()
{
    return comPortInfo;
}

SerialPortInfo LightGun::GetSerialProtInfo()
{
    return serialPortInfo;
}

qint32 LightGun::GetComPortBaud()
{
    return comPortBaud;
}

quint8 LightGun::GetComPortDataBits()
{
    return comPortDataBits;
}

quint8 LightGun::GetComPortParity()
{
    return comPortParity;
}

quint8 LightGun::GetComPortStopBits()
{
    return comPortStopBits;
}

quint8 LightGun::GetComPortFlow()
{
    return comPortFlow;
}

quint16 LightGun::GetMaxAmmo()
{
    return maxAmmo;
}

quint16 LightGun::GetReloadValue()
{
    return reloadValue;
}

bool LightGun::IsMaxAmmoSet()
{
    return maxAmmoSet;
}

bool LightGun::IsReloadValueSet()
{
    return reloadValueSet;
}

quint8 LightGun::GetDipSwitchPlayerNumber(bool *isSet)
{
    if(isDipSwitchPlayerNumberSet)
    {
        *isSet = true;
        return dipSwitchPlayerNumber;
    }
    else
    {
        *isSet = false;
        return UNASSIGN;
    }
}

quint8 LightGun::GetDipSwitchPlayerNumber()
{
    if(isDipSwitchPlayerNumberSet)
        return dipSwitchPlayerNumber;
    else
        return UNASSIGN;
}

bool LightGun::GetIsDipSwitchPlayerNumberSet()
{
    return isDipSwitchPlayerNumberSet;
}

quint8 LightGun::GetAnalogStrength(bool *isSet)
{
    if(isAnalogStrengthSet)
    {
        *isSet = true;
        return analogStrength;
    }
    else
    {
        *isSet = false;
        return UNASSIGN;
    }
}

quint8 LightGun::GetAnalogStrength()
{
    if(isAnalogStrengthSet)
        return analogStrength;
    else
        return UNASSIGN;
}

bool LightGun::GetIsAnalogStrengthSet()
{
    return isAnalogStrengthSet;
}

quint8 LightGun::GetHubComPortNumber()
{
    return hubComPortNumber;
}

HIDInfo LightGun::GetUSBHIDInfo()
{
    return usbHIDInfo;
}

bool LightGun::IsLightGunUSB()
{
    return isUSBLightGun;
}

quint16 LightGun::GetRecoilDelay()
{
    if(isRecoilDelaySet)
        return recoilDelay;
    else
        return 0;
}

bool LightGun::IsRecoilDelay()
{
    return isRecoilDelaySet;
}

QString LightGun::GetComPortPath()
{
    if(!isUSBLightGun)
    {
        if(serialPortInfo.path.isEmpty ())
            return comPortInfo.systemLocation();
        else
            return serialPortInfo.path;
    }
    else
        return "";
}

bool LightGun::GetDisableReaperLEDs()
{
    return disableReaperLEDs;
}

void LightGun::GetDisplayPriority(bool *ammo, bool *life)
{
    *ammo = displayAmmoPriority;
    *life = displayLifePriority;
}

bool LightGun::GetDisplayOtherPriority()
{
    return displayOtherPriority;
}


bool LightGun::GetDisplayAmmoAndLife(bool *displayLG, bool *displayLB, bool *displayLN)
{
    *displayLG = displayAmmoLifeGlyphs;
    *displayLB = displayAmmoLifeBar;
    *displayLN = displayAmmoLifeNumber;

    return displayAmmoLife;
}

quint16 LightGun::GetDisplayRefresh(bool *isRDS)
{
    *isRDS = displayRefreshSet;

    if(displayAmmoLife && displayAmmoLifeBar)
        return displayRefresh+5;

    return displayRefresh;
}

bool LightGun::GetReloadNoRumble()
{
    return reloadNoRumble;
}

bool LightGun::GetReloadDisabled()
{
    return reloadDisable;
}

quint8 LightGun::GetReaperAmmo0Delay(bool *isAmmo0DelayEnabled, quint16 *reaperHST)
{
    *isAmmo0DelayEnabled = enableReaperAmmo0Delay;
    *reaperHST = reaperHoldSlideTime;
    return repearAmmo0Delay;
}


void LightGun::CopyLightGun(LightGun const &lgMember)
{

    //*this = lgMember;

    defaultLightGun = lgMember.defaultLightGun;
    if(defaultLightGun)
        defaultLightGunNum = lgMember.defaultLightGunNum;
    else
        defaultLightGunNum = 0;

    lightGunName = lgMember.lightGunName;
    lightGunNum = lgMember.lightGunNum;

    comPortNum = lgMember.comPortNum;
    comPortString = lgMember.comPortString;
    comPortInfo = lgMember.comPortInfo;
    comPortBaud = lgMember.comPortBaud;
    comPortDataBits = lgMember.comPortDataBits;
    comPortParity = lgMember.comPortParity;
    comPortStopBits = lgMember.comPortStopBits;
    comPortFlow = lgMember.comPortFlow;

    if(lgMember.maxAmmoSet)
    {
        maxAmmo = lgMember.maxAmmo;
        maxAmmoSet = true;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        maxAmmo = DEFAULTLG_ARRAY[defaultLightGunNum].MAXAMMON;
        maxAmmoSet = true;
    }
    else
    {
        maxAmmoSet = false;
    }

    if(lgMember.reloadValueSet)
    {
        reloadValue = lgMember.reloadValue;
        reloadValueSet = true;
    }
    else if(defaultLightGun && defaultLightGunNum == RS3_REAPER)
    {
        reloadValue = DEFAULTLG_ARRAY[defaultLightGunNum].RELOADVALUEN;
        reloadValueSet = true;
    }
    else
    {
        reloadValueSet = false;
    }

    disableReaperLEDs = lgMember.disableReaperLEDs;

    if(lgMember.isDipSwitchPlayerNumberSet)
    {
        dipSwitchPlayerNumber = lgMember.dipSwitchPlayerNumber;
        isDipSwitchPlayerNumberSet = true;
    }
    else
        isDipSwitchPlayerNumberSet = false;

    hubComPortNumber = lgMember.hubComPortNumber;

    if(lgMember.isAnalogStrengthSet)
    {
        analogStrength = lgMember.analogStrength;
        isAnalogStrengthSet = true;
    }
    else
    {
        analogStrength = DEFAULTANALOGSTRENGTH;
        isAnalogStrengthSet = false;
    }

    isUSBLightGun = lgMember.isUSBLightGun;
    isRecoilDelaySet = lgMember.isRecoilDelaySet;

    if(isUSBLightGun)
        usbHIDInfo = lgMember.usbHIDInfo;

    if(isRecoilDelaySet)
        recoilDelay = lgMember.recoilDelay;


    serialPortInfo = lgMember.serialPortInfo;

    if(!isUSBLightGun)
        FillSerialPortInfo();

    currentPath = lgMember.currentPath;
    dataPath = lgMember.dataPath;
    defaultLGFilePath = lgMember.defaultLGFilePath;

    lastAmmoValue = 0;

    //Display Init
    hasDisplayAmmoInited = lgMember.hasDisplayAmmoInited;
    hasDisplayLifeInited = lgMember.hasDisplayLifeInited;
    hasDisplayOtherInited = lgMember.hasDisplayOtherInited;
    hasDisplayAmmoAndLifeInited = lgMember.hasDisplayAmmoAndLifeInited;
    ammoDisplayValue = lgMember.ammoDisplayValue;
    lifeDisplayValue = lgMember.lifeDisplayValue;
    otherDisplayValue = lgMember.otherDisplayValue;
    displayAmmoPriority = lgMember.displayAmmoPriority;
    displayLifePriority = lgMember.displayLifePriority;
    displayOtherPriority = lgMember.displayOtherPriority;
    displayRefresh = lgMember.displayRefresh;

    displayAmmoLife = lgMember.displayAmmoLife;
    displayAmmoLifeGlyphs = lgMember.displayAmmoLifeGlyphs;
    displayAmmoLifeBar = lgMember.displayAmmoLifeBar;
    displayAmmoLifeNumber = lgMember.displayAmmoLifeNumber;
    lifeBarMaxLife = lgMember.lifeBarMaxLife;

    lgRecoilPriority[0] = lgMember.lgRecoilPriority[0];
    lgRecoilPriority[1] = lgMember.lgRecoilPriority[1];
    lgRecoilPriority[2] = lgMember.lgRecoilPriority[2];
    lgRecoilPriority[3] = lgMember.lgRecoilPriority[3];

    reloadNoRumble = lgMember.reloadNoRumble;
    reloadDisable = lgMember.reloadDisable;

    reaperLargeAmmo = lgMember.reaperLargeAmmo;
    isReaper5LEDsInited = lgMember.isReaper5LEDsInited;
    enableReaperAmmo0Delay = lgMember.enableReaperAmmo0Delay;
    repearAmmo0Delay = lgMember.repearAmmo0Delay;
    isReaperAmmo0TimerInited = false;

    if(lgMember.isReaperAmmo0TimerInited)
        InitReaperAmmo0Timer();

    reaperHoldSlideTime = lgMember.reaperHoldSlideTime;
    isReaperTimerRunning = lgMember.isReaperTimerRunning;
    isReaperSlideHeldBack = lgMember.isReaperSlideHeldBack;

    LoadDefaultLGCommands();

}


void LightGun::LoadDefaultLGCommands()
{
    QString line;
    QStringList splitLines, commands;
    quint8 numberCommands, i, equalIndex;
    quint16 lineLength;
    bool noCommands;

    openComPortCmdsSet = false;
    closeComPortCmdsSet = false;
    damageCmdsSet = false;
    recoilCmdsSet = false;
    recoilR2SCmdsSet = false;
    reloadCmdsSet = false;
    ammoValueCmdsSet = false;
    shakeCmdsSet = false;
    autoLedCmdsSet = false;
    aspect16x9CmdsSet = false;
    aspect4x3CmdsSet = false;
    joystickCmdsSet = false;
    keyMouseCmdsSet = false;
    displayAmmoCmdsSet = false;
    displayAmmoInitCmdsSet = false;
    displayLifeCmdsSet = false;
    displayLifeInitCmdsSet = false;
    displayOtherCmdsSet = false;
    displayOtherInitCmdsSet = false;
    displayRefreshSet = false;
    recoilValueCmdsSet = false;
    offscreenButtonCmdsSet = false;
    offscreenNormalShotCmdsSet = false;


    openComPortCmds.clear();
    closeComPortCmds.clear();
    damageCmds.clear();
    recoilCmds.clear();
    recoilR2SCmds.clear();
    recoilValueCmds.clear();
    reloadCmds.clear();
    ammoValueCmds.clear();
    shakeCmds.clear();
    autoLedCmds.clear();
    aspect16x9Cmds.clear();
    aspect4x3Cmds.clear();
    joystickCmds.clear();
    keyMouseCmds.clear();
    displayAmmoCmds.clear();
    displayAmmoInitCmds.clear();
    displayLifeCmds.clear();
    displayLifeInitCmds.clear();
    displayOtherCmds.clear();
    displayOtherInitCmds.clear();
    offscreenButtonCmds.clear();
    offscreenNormalShotCmds.clear();

    //If Not a Default Light Gun, then make sure defaultLightGunNum is 0
    //To load in nonDefaultLG.hor file
    if(!defaultLightGun)
        defaultLightGunNum = 0;


    if(defaultLightGunNum != AIMTRAK)
    {

        //Get Current Path
        //currentPath = QDir::currentPath();
        currentPath = QApplication::applicationDirPath();

        dataPath = currentPath + "/" + DATAFILEDIR;

        defaultLGFilePath = dataPath + "/" + DEFAULTLGFILENAMES_ARRAY[defaultLightGunNum];

        QFile defaultLGCmdFile(defaultLGFilePath);

        bool openFile = defaultLGCmdFile.open (QIODeviceBase::ReadOnly | QIODevice::Text);

        if(!openFile)
        {
            QString tempCrit = "Can not open Default Light Gun command file to read. Please close program and solve file problem. Might be permissions problem.\nFile: "+defaultLGFilePath;
            QMessageBox::critical (nullptr, "File Error", tempCrit, QMessageBox::Ok);
            return;
        }

        //Create a Text Stream, to Stream in the Data Easier
        QTextStream in(&defaultLGCmdFile);

        while(!in.atEnd ())
        {
            line = in.readLine();

            if(line.contains ('='))
            {
                equalIndex = line.indexOf ('=',0);
                lineLength = line.length ();

                if(lineLength == equalIndex+1)
                    noCommands = true;
                else
                    noCommands = false;
            }
            else
                noCommands = true;


            if(line != ENDOFFILE && !noCommands)
            {
                splitLines = line.split ('=', Qt::SkipEmptyParts);

                if(splitLines[1].contains ('|'))
                    commands = SplitLoadedCommands(splitLines[1]);
                else
                    commands = splitLines[1].split (' ', Qt::SkipEmptyParts);


                numberCommands = commands.length();

                //See if Commands
                if(numberCommands > 0)
                {
                    //Get rid of before and after open spaces
                    splitLines[0] = splitLines[0].trimmed ();

                    //Now Find the
                    if(splitLines[0] == OPENCOMPORTONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            openComPortCmds << commands[i];
                        }

                        //if(disableReaperLEDs)
                        //    openComPortCmds << REAPERINITLEDS;

                        openComPortCmdsSet = true;
                    }
                    else if(splitLines[0] == CLOSECOMPORTONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            closeComPortCmds << commands[i];
                        }
                        closeComPortCmdsSet = true;
                    }
                    else if(splitLines[0] == DAMAGECMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            damageCmds << commands[i];
                        }
                        damageCmdsSet = true;
                    }
                    else if(splitLines[0] == RECOILCMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            recoilCmds << commands[i];
                        }
                        recoilCmdsSet = true;
                    }
                    else if(splitLines[0] == RECOIL_R2SONLY)
                    {
                        //qDebug() << "Recoiol_R2S Commands: " << commands;
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            recoilR2SCmds << commands[i];
                        }
                        recoilR2SCmdsSet = true;
                    }
                    else if(splitLines[0] == RECOILVALUEONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            recoilValueCmds << commands[i];
                        }
                        recoilValueCmdsSet = true;
                    }
                    else if(splitLines[0] == RELOADCMDONLY && !reloadNoRumble && !reloadDisable)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            reloadCmds << commands[i];
                        }
                        reloadCmdsSet = true;
                    }
                    else if(splitLines[0] == RELOADNORMBLCMDONLY && reloadNoRumble && !reloadDisable)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            reloadCmds << commands[i];
                        }
                        reloadCmdsSet = true;
                    }
                    else if(splitLines[0] == RELOADNORMBLCMDONLY && !reloadNoRumble && reloadDisable && defaultLightGunNum == RS3_REAPER)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            reloadCmds << commands[i];
                        }
                        reloadCmdsSet = true;
                    }
                    else if(splitLines[0] == AMMOVALUECMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            ammoValueCmds << commands[i];
                        }
                        ammoValueCmdsSet = true;
                    }
                    else if(splitLines[0] == SHAKECMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            shakeCmds << commands[i];
                        }
                        shakeCmdsSet = true;
                    }
                    else if(splitLines[0] == AUTOLEDCMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            autoLedCmds << commands[i];
                        }
                        autoLedCmdsSet = true;
                    }
                    else if(splitLines[0] == ARATIO169CMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            aspect16x9Cmds << commands[i];
                        }
                        aspect16x9CmdsSet = true;
                    }
                    else if(splitLines[0] == ARATIO43CMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            aspect4x3Cmds << commands[i];
                        }
                        aspect4x3CmdsSet = true;
                    }
                    else if(splitLines[0] == JOYMODECMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            joystickCmds << commands[i];
                        }
                        joystickCmdsSet = true;
                    }
                    else if(splitLines[0] == KANDMMODECMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            keyMouseCmds << commands[i];
                        }
                        keyMouseCmdsSet = true;
                    }
                    else if(splitLines[0] == DISPLAYAMMOONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            displayAmmoCmds << commands[i];
                        }
                        displayAmmoCmdsSet = true;
                    }
                    else if(splitLines[0] == DISPLAYAMMOINITONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();

                            if(displayAmmoLife && !displayOtherPriority)
                            {
                                if(commands[i].startsWith (OPENFIREAMMOINIT))
                                {
                                    if(displayAmmoLifeGlyphs)
                                        commands[i] = OPENFIREALGLYPHS;
                                    else if(displayAmmoLifeBar)
                                        commands[i] = OPENFIREALBAR;
                                }
                            }

                            displayAmmoInitCmds << commands[i];
                        }
                        displayAmmoInitCmdsSet = true;
                    }
                    else if(splitLines[0] == DISPLAYLIFEONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();

                            if(defaultLightGun && defaultLightGunNum == OPENFIRE && displayAmmoLifeNumber)
                                commands[i] = OPENFIRELIFENUMCMD;

                            displayLifeCmds << commands[i];
                        }
                        displayLifeCmdsSet = true;
                    }
                    else if(splitLines[0] == DISPLAYLIFEINITONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();

                            if(displayAmmoLife && !displayOtherPriority)
                            {
                                if(commands[i].startsWith (OPENFIRELIFEINIT))
                                {
                                    if(displayAmmoLifeGlyphs)
                                        commands[i] = OPENFIREALGLYPHS;
                                    else if(displayAmmoLifeBar)
                                        commands[i] = OPENFIREALBAR;
                                }
                            }
                            else if(defaultLightGun && defaultLightGunNum == OPENFIRE)
                            {
                                if(displayAmmoLifeBar)
                                    commands[i].append(OPENFIRELIFEBAR);
                                else if(displayAmmoLifeNumber)
                                    commands[i] = OPENFIREAMMOINIT;
                            }
                            //qDebug() << "Display Life Init CMD: " << commands[i] << " displayAmmoLifeNumber: " << displayAmmoLifeNumber;

                            displayLifeInitCmds << commands[i];
                        }
                        displayLifeInitCmdsSet = true;
                    }
                    else if(splitLines[0] == DISPLAYOTHERONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            displayOtherCmds << commands[i];
                        }
                        displayOtherCmdsSet = true;
                    }
                    else if(splitLines[0] == DISPLAYOTHERINITONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            displayOtherInitCmds << commands[i];
                        }
                        displayOtherInitCmdsSet = true;
                    }
                    else if(splitLines[0] == DISPLAYREFRESHONLY)
                    {
                        commands[0] = commands[0].trimmed ();
                        bool isNumber;
                        displayRefresh = commands[0].toShort (&isNumber);

                        if(!isNumber)
                        {
                            displayRefresh = DISPLAYREFRESHDEFAULT;
                        }
                        else
                            displayRefreshSet = true;
                    }
                    else if(splitLines[0] == OFFSCREENBUTTONCMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            offscreenButtonCmds << commands[i];
                        }
                        offscreenButtonCmdsSet = true;
                    }
                    else if(splitLines[0] == OFFSCREENRMLSHOTCMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            offscreenNormalShotCmds << commands[i];
                        }
                        offscreenNormalShotCmdsSet = true;
                    }


                }//if(numberCommands > 1)

            }//if(line != ENDOFFILE)

        }//while(!in.atEnd ())

        //Close the File
        defaultLGCmdFile.close ();

    } //if(defaultLightGunNum != AIMTRAK)
    else
    {

        QString prodID = usbHIDInfo.productIDString;
        prodID.replace ("0x160", "");
        prodID.prepend ('0');
        quint8 prodIDNum = prodID.toUInt ();
        prodIDNum--;
        QString front = QString::number (prodIDNum);
        front.prepend ('0');

        //Ultimarc AimTrak
        recoilCmdsSet = true;
        recoilR2SCmdsSet = true;
        ammoValueCmdsSet = true;

        QString recoilCMD = ARIEMCTORIRLK;
        recoilCMD.prepend (prodID);

        //qDebug() << "AimTrak Recoil CMD: " << recoilCMD;

        recoilCmds << recoilCMD;
        recoilR2SCmds << "100";
        ammoValueCmds << recoilCMD;

        //ReadConfigData();
    }
/*
    qDebug() << openComPortCmdsSet;
    qDebug() << closeComPortCmdsSet;
    qDebug() << damageCmdsSet;
    qDebug() << recoilCmdsSet;
    qDebug() << recoilR2SCmdsSet;
    qDebug() << recoilValueCmdsSet
    qDebug() << reloadCmdsSet;
    qDebug() << ammoCmdsSet;
    qDebug() << ammoValueCmdsSet;
    qDebug() << shakeCmdsSet;
    qDebug() << autoLedCmdsSet;
    qDebug() << aspect16x9CmdsSet;
    qDebug() << aspect4x3CmdsSet;
    qDebug() << joystickCmdsSet;
    qDebug() << keyMouseCmdsSet;
    qDebug() << displayAmmoCmdsSet;
    qDebug() << displayAmmoInitCmdsSet;
    qDebug() << displayLifeCmdsSet;
    qDebug() << displayLifeInitCmdsSet;
    qDebug() << displayOtherCmdsSet;
    qDebug() << displayOtherInitCmdsSet;
    qDebug() << displayRefreshSet

    qDebug() << openComPortCmds;
    qDebug() << closeComPortCmds;
    qDebug() << damageCmds;
    qDebug() << recoilCmds;
    qDebug() << recoilR2SCmds;
    qDebug() << recoilR2SCmds;
    qDebug() << reloadCmds;
    qDebug() << ammoCmds;
    qDebug() << ammoValueCmds;
    qDebug() << shakeCmds;
    qDebug() << autoLedCmds;
    qDebug() << aspect16x9Cmds;
    qDebug() << aspect4x3Cmds;
    qDebug() << joystickCmds;
    qDebug() << keyMouseCmds;
    qDebug() << displayAmmoCmds;
    qDebug() << displayAmmoInitCmds;
    qDebug() << displayLifeCmds;
    qDebug() << displayLifeInitCmds;
    qDebug() << displayOtherCmds;
    qDebug() << displayOtherInitCmds;
    qDebug() << displayRefresh;

*/
}





QStringList LightGun::SplitLoadedCommands(QString commandList)
{
    QStringList cmdSplit, returnList;
    quint8 cmdCount;
    QString goodCMD;
    QString frontPart;
    bool needFront = false;

    quint16 indexOfP = commandList.indexOf('P');

    if(indexOfP != 0)
    {
        frontPart = commandList.first (indexOfP);
        frontPart = frontPart.trimmed ();
        commandList = commandList.remove(0,indexOfP);
        needFront = true;
    }

    cmdSplit = commandList.split ('|', Qt::SkipEmptyParts);

    if(defaultLightGunNum == MX24 && isDipSwitchPlayerNumberSet)
    {
        if(dipSwitchPlayerNumber == 0)
        {
            if(cmdSplit[0][1] == '1')
                goodCMD = cmdSplit[0].remove (0,3);
            else if (cmdSplit[1][1] == '1')
                goodCMD = cmdSplit[1].remove (0,3);
        }
        else if(dipSwitchPlayerNumber == 1)
        {
            if(cmdSplit[0][1] == '2')
                goodCMD = cmdSplit[0].remove (0,3);
            else if (cmdSplit[1][1] == '2')
                goodCMD = cmdSplit[1].remove (0,3);
        }

        returnList = goodCMD.split (' ', Qt::SkipEmptyParts);
    }

    if(needFront)
        returnList.prepend(frontPart);

    //qDebug() << "returnList: " << returnList;

    return returnList;
}














//Default Light Gun Commands for Certain Signals
QStringList LightGun::OpenComPortCommands(bool *isSet)
{
    *isSet = openComPortCmdsSet;
    return openComPortCmds;
}

QStringList LightGun::CloseComPortCommands(bool *isSet)
{
    *isSet = closeComPortCmdsSet;
    return closeComPortCmds;
}

QStringList LightGun::DamageCommands(bool *isSet)
{
    *isSet = damageCmdsSet;
    return damageCmds;
}

QStringList LightGun::RecoilCommands(bool *isSet)
{
    *isSet = recoilCmdsSet;
    return recoilCmds;
}

QStringList LightGun::RecoilValueCommands(bool *isSet, quint16 recoilValue)
{
    QString cmdString;
    QStringList tempSL;
    quint16 recoilNum = recoilValue;

    *isSet = recoilValueCmdsSet;

    if(recoilValueCmdsSet)
    {
        if(recoilNum > 1)
            recoilNum = 1;

        for(quint8 i = 0; i < recoilValueCmds.length(); i++)
        {
            cmdString = recoilValueCmds[i];
            if(cmdString.contains (SIGNALDATAVARIBLE))
                cmdString.replace (SIGNALDATAVARIBLE,QString::number(recoilNum));
            tempSL << cmdString;
        }
    }

    return tempSL;
}




QStringList LightGun::ReloadCommands(bool *isSet)
{
    *isSet = reloadCmdsSet;
    return reloadCmds;
}

QStringList LightGun::ReloadValueCommands(bool *isSet, quint16 ammoValue)
{
    if(ammoValue <= lastAmmoValue)
    {
        lastAmmoValue = ammoValue;
        *isSet = false;
        QStringList tempSL; 
        return tempSL;
    }

    *isSet = reloadCmdsSet;
    lastAmmoValue = ammoValue;   
    return reloadCmds;
}


QStringList LightGun::AmmoValueCommands(bool *isSet, quint16 ammoValue)
{
    QString tempAVS;
    QString cmdString;
    QStringList tempSL;
    quint8 cmdCount = ammoValueCmds.length ();
    bool doReloadCMD = false;

    //Check if Reload Happened
    if(ammoValue > lastAmmoValue)
    {
        lastAmmoValue = ammoValue;
        *isSet = reloadCmdsSet;
        reloadAmmoValue = ammoValue;

        if(defaultLightGunNum == RS3_REAPER)
        {
            //If Reaper Ammo 0 Timer Running, cancel it, as reload happen (Z6 command)
            if(isReaperTimerRunning)
            {
                p_reaperAmmo0Timer->stop();
                isReaperTimerRunning = false;
                isReaperSlideHeldBack = false;
                p_reaperAmmo0Timer->setInterval(repearAmmo0Delay);
            }

            if(!disableReaperLEDs)
            {
                if(reloadAmmoValue > MAXRELOADVALUE)
                {
                    reaperLargeAmmo = true;
                    reaper5LEDNumber = REAPERMAXAMMONUM;
                    reaperBulletCount = 0;
                    reaperBullets1LED = static_cast<quint8>(qRound(static_cast<float>(reloadAmmoValue)/REAPERMAXAMMOF));
                    //qDebug() << "reaperBullets1LED:" <<  reaperBullets1LED;
                }
                else
                    reaperLargeAmmo = false;

                if(!isReaper5LEDsInited)
                {
                    isReaper5LEDsInited = true;
                    tempSL = reloadCmds;
                    tempSL.prepend(REAPERINITLEDS);
                    return tempSL;
                }
            }
        }

        return reloadCmds;
    }
    else if((lastAmmoValue == 0 && ammoValue == 0) || (lastAmmoValue > 1 && ammoValue == 0)) //Boot Up or Closing Game, Do Nothing
    {
        lastAmmoValue = ammoValue;
        *isSet = false;
        return tempSL;
    }

    //If Command Set Loaded
    *isSet = ammoValueCmdsSet;

    if(ammoValueCmdsSet) //If There is Ammo_Value CMDs
    {
        if(defaultLightGunNum == RS3_REAPER) //RS3 Reaper
        {
            quint16 tempAV;

            if(ammoValue == 0 && enableReaperAmmo0Delay)
            {
                //Start Timer
                p_reaperAmmo0Timer->start();
                isReaperTimerRunning = true;
                //Do Nothing, as after timeout, it will then send Z0 Command, unless reload happens
                *isSet = false;
                return tempSL;
            }


            if(reaperLargeAmmo)
            {
                if(ammoValue == 0)
                    tempAV = 0;
                else
                {
                    reaperBulletCount++;

                    if(reaperBulletCount == reaperBullets1LED && reaper5LEDNumber > 1)
                    {
                        reaperBulletCount = 0;
                        reaper5LEDNumber--;
                        //qDebug() << "reaper5LEDNumber:" << reaper5LEDNumber << "ammoValue:" << ammoValue;
                    }

                    tempAV = reaper5LEDNumber;
                }
            }
            else
            {
                if(ammoValue > maxAmmo) //If ammoValue is higher than maxAmmo, then = to maxValue
                    tempAV = maxAmmo;
                else
                    tempAV = ammoValue;
            }

            tempAVS = QString::number (tempAV);

        }
        else
            tempAVS = QString::number (ammoValue); //Non-Reaper Light Guns, Including USB HID

        //Replace the %s% with the ammo value (tempAVS)
        for(quint8 i = 0; i < cmdCount; i++)
        {
            cmdString = ammoValueCmds[i];
            if(cmdString.contains (SIGNALDATAVARIBLE))
                cmdString.replace (SIGNALDATAVARIBLE,tempAVS);
            tempSL << cmdString;
        }
    }

    lastAmmoValue = ammoValue;

    //Return the Commands
    return tempSL;
}

//QString tempAVS = QString::number(ammoValue, 16).rightJustified(2, '0');


QStringList LightGun::ShakeCommands(bool *isSet)
{
    *isSet = shakeCmdsSet;
    return shakeCmds;
}

QStringList LightGun::AutoLEDCommands(bool *isSet)
{
    *isSet = autoLedCmdsSet;
    return autoLedCmds;
}

QStringList LightGun::AspectRatio16b9Commands(bool *isSet)
{
    *isSet = aspect16x9CmdsSet;
    return aspect16x9Cmds;
}

QStringList LightGun::AspectRatio4b3Commands(bool *isSet)
{
    *isSet = aspect4x3CmdsSet;
    return aspect4x3Cmds;
}

QStringList LightGun::JoystickModeCommands(bool *isSet)
{
    *isSet = joystickCmdsSet;
    return joystickCmds;
}


QStringList LightGun::MouseAndKeyboardModeCommands(bool *isSet)
{
    *isSet = keyMouseCmdsSet;
    return keyMouseCmds;
}

QStringList LightGun::RecoilR2SCommands(bool *isSet)
{
    *isSet = recoilR2SCmdsSet;
    return recoilR2SCmds;
}


QStringList LightGun::DisplayAmmoCommands(bool *isSet, quint16 ammoValue)
{
    QString tempAVS, ammoValueD1, ammoValueD0;
    QString cmdString;
    QStringList tempSL;
    quint8 cmdCount = displayAmmoCmds.length ();
    quint16 newAmmoValue;

    //Set Light Guns Ammo Display Value
    ammoDisplayValue = ammoValue;

    if((displayLifePriority && hasDisplayLifeInited && !displayAmmoLife) || (displayOtherPriority && hasDisplayOtherInited))
    {
        //Send False on CMDs, and Send No CMDs, when other displays have priority & have been Display INIted
        *isSet = false;
        return tempSL;
    }

    //If Command Set Loaded
    *isSet = displayAmmoCmdsSet;

    //Init Ammo Display
    if(!hasDisplayAmmoInited)
    {
        if(displayAmmoInitCmdsSet && displayAmmoCmdsSet && !hasDisplayAmmoAndLifeInited)
        {
            for(quint8 i = 0; i < displayAmmoInitCmds.length(); i++)
                tempSL << displayAmmoInitCmds[i];
        }

        if(displayAmmoLife)
            hasDisplayAmmoAndLifeInited = true;

        hasDisplayAmmoInited = true;
    }


    if(displayAmmoCmdsSet)
    {
        //If Ammo is bigger than 99, then make it 99
        if(ammoValue > 99)
            newAmmoValue = 99;
        else
            newAmmoValue = ammoValue;

        if(isUSBLightGun)
        {   
            //For Alien USB Light Gun Ammo Counter which is 2 Digits, so 0-99
            tempAVS = QString::number(newAmmoValue, 10).rightJustified(2, '0');

            ammoValueD1 = '0'+tempAVS[0];
            ammoValueD0 = '0'+tempAVS[1];

            //qDebug() << "ammoValueD1: " << ammoValueD1 << " ammoValueD0: " << ammoValueD0 << " tempAVS: " << tempAVS;

            //Replace the %d1% & %d0% with the ammoValueD[1:0]. Also for USB HID %dX% is 2 Hex Values which is 1 Byte
            for(quint8 i = 0; i < cmdCount; i++)
            {
                cmdString = displayAmmoCmds[i];
                cmdString.replace (DIGIT1,ammoValueD1);
                cmdString.replace (DIGIT0,ammoValueD0);
                tempSL << cmdString;
                //qDebug() << "cmdString: " << cmdString;
            }
        }
        else
        {
            //Replace the %s% with the ammo value (ammoValue)
            for(quint8 i = 0; i < cmdCount; i++)
            {
                cmdString = displayAmmoCmds[i];
                cmdString.replace (SIGNALDATAVARIBLE,QString::number(newAmmoValue));
                tempSL << cmdString;
            }
        }
    }
    //Return the Commands
    return tempSL;
}


QStringList LightGun::DisplayLifeCommands(bool *isSet, quint16 lifeValue)
{
    QString tempAVS, lifeValueD1, lifeValueD0;
    QString cmdString;
    QStringList tempSL;
    quint8 cmdCount = displayLifeCmds.length ();
    quint16 newLifeValue;

    //Set Light Guns Ammo Display Value
    lifeDisplayValue = lifeValue;

    if(lifeValue > lifeBarMaxLife)
        lifeBarMaxLife = lifeValue;


    if((displayAmmoPriority && hasDisplayAmmoInited && !displayAmmoLife) || (displayOtherPriority && hasDisplayOtherInited))
    {
        //Send False on CMDs, and Send No CMDs, when other displays have priority & have been Display INIted
        *isSet = false;
        return tempSL;
    }

    //If Command Set Loaded
    *isSet = displayLifeCmdsSet;

    //Init Ammo Display
    if(!hasDisplayLifeInited)
    {
        if(displayLifeInitCmdsSet && displayLifeCmdsSet && !hasDisplayAmmoAndLifeInited)
        {
            for(quint8 i = 0; i < displayLifeInitCmds.length(); i++)
                tempSL << displayLifeInitCmds[i];
        }

        if(displayAmmoLife)
            hasDisplayAmmoAndLifeInited = true;

        hasDisplayLifeInited = true;
    }


    if(displayLifeCmdsSet)
    {
        if(isUSBLightGun)
        {
            //If Life is bigger than 99, then make it 99
            if(lifeValue > 99)
                newLifeValue = 99;
            else
                newLifeValue = lifeValue;

            //For Alien USB Light Gun Ammo Counter which is 2 Digits, so 0-99
            tempAVS = QString::number(newLifeValue, 10).rightJustified(2, '0');

            lifeValueD1 = '0'+tempAVS[0];
            lifeValueD0 = '0'+tempAVS[1];

            //qDebug() << "lifeValueD1: " << lifeValueD1 << " lifeValueD0: " << lifeValueD0 << " tempAVS: " << tempAVS;

            //Replace the %d1% & %d0% with the ammoValueD[1:0]. Also for USB HID %dX% is 2 Hex Values which is 1 Byte
            for(quint8 i = 0; i < cmdCount; i++)
            {
                cmdString = displayLifeCmds[i];
                cmdString.replace (DIGIT1,lifeValueD1);
                cmdString.replace (DIGIT0,lifeValueD0);
                tempSL << cmdString;
                //qDebug() << "cmdString: " << cmdString;
            }
        }
        else
        {
            if(defaultLightGun && defaultLightGunNum == OPENFIRE)
            {
                if(displayAmmoLifeBar && lifeBarMaxLife > 0)
                    newLifeValue = (lifeValue*100)/lifeBarMaxLife;
                else if(displayAmmoLifeGlyphs && lifeBarMaxLife > 19)
                {
                    quint16 lifeDivider = lifeBarMaxLife/10;
                    newLifeValue = lifeValue/lifeDivider;
                }
                else if(displayAmmoLifeNumber && lifeValue > 99)
                    newLifeValue = 99;
                else
                    newLifeValue = lifeValue;
            }
            else
                newLifeValue = lifeValue;

            //Replace the %s% with the ammo value (ammoValue)
            for(quint8 i = 0; i < cmdCount; i++)
            {
                cmdString = displayLifeCmds[i];
                cmdString.replace (SIGNALDATAVARIBLE,QString::number(newLifeValue));
                tempSL << cmdString;
            }
        }
    }
    //Return the Commands
    return tempSL;
}

QStringList LightGun::DisplayOtherCommands(bool *isSet, quint16 otherValue)
{
    QString tempAVS, otherValueD1, otherValueD0;
    QString cmdString;
    QStringList tempSL;
    quint8 cmdCount = displayOtherCmds.length ();
    quint16 newOtherValue;

    //Set Light Guns Ammo Display Value
    otherDisplayValue = otherValue;

    if(!displayOtherPriority)
    {
        //If Display_Other is not Enabled, then do nothing
        *isSet = false;
        return tempSL;
    }

    //If Command Set Loaded
    *isSet = displayOtherCmdsSet;

    //Init Ammo Display
    if(!hasDisplayOtherInited)
    {
        if(displayOtherInitCmdsSet && displayOtherCmdsSet)
        {
            for(quint8 i = 0; i < displayOtherInitCmds.length(); i++)
                tempSL << displayOtherInitCmds[i];
        }

        hasDisplayOtherInited = true;
    }


    if(displayOtherCmdsSet)
    {
        //If Ammo is bigger than 99, then make it 99
        if(otherValue > 99)
            newOtherValue = 99;
        else
            newOtherValue = otherValue;


        if(isUSBLightGun)
        {
            //For Alien USB Light Gun Ammo Counter which is 2 Digits, so 0-99
            tempAVS = QString::number(newOtherValue, 10).rightJustified(2, '0');

            otherValueD1 = '0'+tempAVS[0];
            otherValueD0 = '0'+tempAVS[1];

            //qDebug() << "otherValueD1: " << otherValueD1 << " otherValueD0: " << otherValueD0 << " tempAVS: " << tempAVS;

            //Replace the %d1% & %d0% with the ammoValueD[1:0]. Also for USB HID %dX% is 2 Hex Values which is 1 Byte
            for(quint8 i = 0; i < cmdCount; i++)
            {
                cmdString = displayOtherCmds[i];
                cmdString.replace (DIGIT1,otherValueD1);
                cmdString.replace (DIGIT0,otherValueD0);
                tempSL << cmdString;
                //qDebug() << "cmdString: " << cmdString;
            }
        }
        else
        {
            //Replace the %s% with the ammo value (ammoValue)
            for(quint8 i = 0; i < cmdCount; i++)
            {
                cmdString = displayOtherCmds[i];
                cmdString.replace (SIGNALDATAVARIBLE,QString::number(newOtherValue));
                tempSL << cmdString;
            }
        }
    }
    //Return the Commands
    return tempSL;
}


QStringList LightGun::OffscreenButtonCommands(bool *isSet)
{
    *isSet = offscreenButtonCmdsSet;
    return offscreenButtonCmds;
}

QStringList LightGun::OffscreenNormalShotCommands(bool *isSet)
{
    *isSet = offscreenNormalShotCmdsSet;
    return offscreenNormalShotCmds;
}





void LightGun::ResetLightGun()
{
    lastAmmoValue = 0;
    hasDisplayAmmoInited = false;
    hasDisplayLifeInited = false;
    hasDisplayOtherInited = false;
    hasDisplayAmmoAndLifeInited = false;
    ammoDisplayValue = -1;
    lifeDisplayValue = -1;
    otherDisplayValue = -1;
    lifeBarMaxLife = 0;
    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    if(isReaperTimerRunning)
    {
        p_reaperAmmo0Timer->stop();
        if(isReaperSlideHeldBack)
            emit WriteCOMPort(comPortNum, REAPERRELOADCOMMAND);
        p_reaperAmmo0Timer->setInterval(repearAmmo0Delay);
        isReaperSlideHeldBack = false;
        isReaperTimerRunning = false;
    }
}


bool LightGun::CheckUSBPath(QString lgPath)
{
    if(!isUSBLightGun)
        return false;
    else
    {
        if(lgPath == usbHIDInfo.displayPath)
            return true;
        else
            return false;
    }
}

bool LightGun::IsRecoilValueSupported()
{
    return recoilValueCmdsSet;
}

bool LightGun::GetSupportedReload()
{
    return reloadCmdsSet;
}

quint8* LightGun::GetRecoilPriority()
{
    return lgRecoilPriority;
}

quint8* LightGun::GetRecoilPriorityHE()
{
    recoilPriorityHE[lgRecoilPriority[0]] = 0;
    recoilPriorityHE[lgRecoilPriority[1]] = 1;
    recoilPriorityHE[lgRecoilPriority[2]] = 2;
    recoilPriorityHE[lgRecoilPriority[3]] = 3;

    return recoilPriorityHE;
}

/////////////////////////////////////////////
/// private slots


/////////////////////////////////////////////
/// private


void LightGun::FillSerialPortInfo()
{
    serialPortInfo.path = comPortInfo.systemLocation();
    serialPortInfo.productDiscription = comPortInfo.description();
    serialPortInfo.manufacturer = comPortInfo.manufacturer();
    serialPortInfo.serialNumber = comPortInfo.serialNumber();
    serialPortInfo.hasVendorID = comPortInfo.hasVendorIdentifier();

    if(serialPortInfo.hasVendorID)
    {
        serialPortInfo.vendorID = comPortInfo.vendorIdentifier();
        serialPortInfo.vendorIDString = QString::number(serialPortInfo.vendorID, 16).rightJustified(4, '0');
        serialPortInfo.vendorIDString = serialPortInfo.vendorIDString.toUpper ();
        serialPortInfo.vendorIDString.prepend ("0x");
    }

    serialPortInfo.hasProductID = comPortInfo.hasProductIdentifier();

    if(serialPortInfo.hasProductID)
    {
        serialPortInfo.productID = comPortInfo.productIdentifier();
        serialPortInfo.productIDString = QString::number(serialPortInfo.productID, 16).rightJustified(4, '0');
        serialPortInfo.productIDString = serialPortInfo.productIDString.toUpper ();
        serialPortInfo.productIDString.prepend ("0x");
    }

    serialPortInfo.portName = comPortInfo.portName ();
    //qDebug() << "Port Name: " << serialPortInfo.portName << " Path: " << serialPortInfo.path << " Port Num: " << comPortNum;
}


void LightGun::InitReaperAmmo0Timer()
{
    if(!isReaperAmmo0TimerInited && enableReaperAmmo0Delay)
    {
        p_reaperAmmo0Timer = new QTimer();
        p_reaperAmmo0Timer->setInterval(repearAmmo0Delay);
        p_reaperAmmo0Timer->setSingleShot(true);
        isReaperAmmo0TimerInited = true;

        //Connect Time Out Signal to Class Memeber Function Slot
        connect(p_reaperAmmo0Timer, SIGNAL(timeout()), this, SLOT(ReaperAmmo0TimeOut()));
    }
}



void LightGun::ReaperAmmo0TimeOut()
{
    //qDebug() << "Reaper Ammo 0 Time Out";

    if(isReaperSlideHeldBack)
    {
        isReaperTimerRunning = false;
        //Send Reload command (Z6) to the Reaper Light Gun
        emit WriteCOMPort(comPortNum, REAPERRELOADCOMMAND);
        isReaperSlideHeldBack = false;
        //Set timer to delay Z0 write
        p_reaperAmmo0Timer->setInterval(repearAmmo0Delay);
    }
    else
    {
        //Send Z0 to the Reaper Light Gun
        emit WriteCOMPort(comPortNum, REAPERHOLDBACKSLIDE);
        isReaperSlideHeldBack = true;
        //Set Timer to Safety Hold Slide Time
        p_reaperAmmo0Timer->setInterval(reaperHoldSlideTime);
        //Start Timer
        p_reaperAmmo0Timer->start();
    }
}

