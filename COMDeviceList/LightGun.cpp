#include "LightGun.h"
#include "../Global.h"

#include "../../saeicmrterta.h"

//Constructors

//For RS3 Reaper Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, SupportedRecoils lgRecoils, LightGunSettings lgSet, bool disableLEDs, quint8 largeAmmo, ReaperSlideData slideData)
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

    maxAmmo = REAPERMAXAMMONUM;
    reloadValue = REAPERRELOADNUM;

    maxAmmoSet = true;
    reloadValueSet = true;

    disableReaperLEDs = disableLEDs;
    reapearLargeAmmoValue = largeAmmo;

    lastAmmoValue = 0;
    ammoCheckValue = 1;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;

    tcpPort = 0;
    tcpPlayer = UNASSIGN;
    recoilVoltage = UNASSIGN;
    recoilVoltageOverride = false;
    sindenRecoilOverride = false;

    isDipSwitchPlayerNumberSet = false;
    dipSwitchPlayerNumber = UNASSIGN;

    analogStrength = UNASSIGN;
    isAnalogStrengthSet = false;

    hubComPortNumber = UNASSIGN;

    outputConnection = SERIALPORT;

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
    noDisplay = true;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    reloadSetting = lgSet.reload;
    damageSetting = lgSet.damage;
    deathSetting = lgSet.death;
    shakeEnalbe = lgSet.shake;

    if(reloadSetting == 1)
        loadReloadShake = true;
    else
        loadReloadShake = false;

    if(damageSetting == 1)
        loadDamageShake = true;
    else
        loadDamageShake = false;

    if(deathSetting == 1)
        loadDeathShake = true;
    else
        loadDeathShake = false;

    loadReloadLED = false;
    loadDamageLED = false;
    loadDeathLED = false;

    disableReaperHoldBack = slideData.disableHoldBack;

    if(slideData.disableHoldBack || !slideData.enableHoldDelay)
    {
        enableReaperAmmo0Delay = false;
        repearAmmo0Delay = DEFAULTAMMO0DELAY;
        reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    }
    else if(slideData.enableHoldDelay)
    {
        enableReaperAmmo0Delay = true;
        repearAmmo0Delay = slideData.holdDelay;
        reaperHoldSlideTime = slideData.slideHoldTime;
    }

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    isReaperAmmo0TimerInited = false;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;
    reaperLoadedReload = false;

    if(disableReaperHoldBack || enableReaperAmmo0Delay)
        reaperAmmo0Check = true;
    else
        reaperAmmo0Check = false;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;


    FillSerialPortInfo();

    LoadDefaultLGCommands();

    //Connect Commands up
    connect(this, &LightGun::DoAmmoValueCommands, this, &LightGun::AmmoValueReaper);

    connect(this, &LightGun::DoDisplayAmmoCommands, this, &LightGun::DisplayAmmoNormal);
    connect(this, &LightGun::DoDisplayLifeCommands, this, &LightGun::DisplayLifeNormal);
    connect(this, &LightGun::DoDisplayOtherCommands, this, &LightGun::DisplayOtherNormal);

    if(enableReaperAmmo0Delay)
        InitReaperAmmo0Timer();

}

//For Normal Serial Port Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, SupportedRecoils lgRecoils, LightGunSettings lgSet)
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
    reapearLargeAmmoValue = LARGEAMMOVALUEDEFAULT;

    isDipSwitchPlayerNumberSet = false;
    dipSwitchPlayerNumber = UNASSIGN;

    hubComPortNumber = UNASSIGN;

    if(defaultLightGunNum == JBGUN4IR)
    {
        analogStrength = DEFAULTANALOGSTRENGTH;
        isAnalogStrengthSet = true;
    }
    else
    {
        analogStrength = UNASSIGN;
        isAnalogStrengthSet = false;
    }

    lastAmmoValue = 0;
    ammoCheckValue = 1;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;

    tcpPort = 0;
    tcpPlayer = UNASSIGN;
    recoilVoltage = UNASSIGN;
    recoilVoltageOverride = false;
    sindenRecoilOverride = false;

    outputConnection = SERIALPORT;

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
    noDisplay = false;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    //Settings for Reload, Damage, Death & Shake Feature
    reloadSetting = lgSet.reload;
    damageSetting = lgSet.damage;
    deathSetting = lgSet.death;
    shakeEnalbe = lgSet.shake;

    if(defaultLightGunNum == XGUNNER)
    {
        if(reloadSetting == 1)
            loadReloadShake = true;
        else
            loadReloadShake = false;

        if(damageSetting == 1)
            loadDamageShake = true;
        else
            loadDamageShake = false;

        if(deathSetting == 1)
            loadDeathShake = true;
        else
            loadDeathShake = false;

        loadReloadLED = false;
        loadDamageLED = false;
        loadDeathLED = false;
    }
    else    // For Fusion and Xena
    {
        if(reloadSetting == 1)
        {
            loadReloadShake = true;
            loadReloadLED = true;
        }
        else if(reloadSetting == 2)
        {
            loadReloadShake = true;
            loadReloadLED = false;
        }
        else if(reloadSetting == 3)
        {
            loadReloadShake = false;
            loadReloadLED = true;
        }
        else
        {
            loadReloadShake = false;
            loadReloadLED = false;
        }

        if(damageSetting == 1)
        {
            loadDamageShake = true;
            loadDamageLED = true;
        }
        else if(damageSetting == 2)
        {
            loadDamageShake = true;
            loadDamageLED = false;
        }
        else if(damageSetting == 3)
        {
            loadDamageShake = false;
            loadDamageLED = true;
        }
        else
        {
            loadDamageShake = false;
            loadDamageLED = false;
        }

        if(deathSetting == 1)
        {
            loadDeathShake = true;
            loadDeathLED = true;
        }
        else if(deathSetting == 2)
        {
            loadDeathShake = true;
            loadDeathLED = false;
        }
        else if(deathSetting == 3)
        {
            loadDeathShake = false;
            loadDeathLED = true;
        }
        else
        {
            loadDeathShake = false;
            loadDeathLED = false;
        }
    }


    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = DEFAULTAMMO0DELAY;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;
    reaperLoadedReload = false;
    disableReaperHoldBack = false;
    reaperAmmo0Check = false;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;

    FillSerialPortInfo();

    LoadDefaultLGCommands();

    //Connect Commands up
    connect(this, &LightGun::DoAmmoValueCommands, this, &LightGun::AmmoValueNormal);
    connect(this, &LightGun::DoDisplayAmmoCommands, this, &LightGun::DisplayAmmoNormal);
    connect(this, &LightGun::DoDisplayLifeCommands, this, &LightGun::DisplayLifeNormal);
    connect(this, &LightGun::DoDisplayOtherCommands, this, &LightGun::DisplayOtherNormal);
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
    reapearLargeAmmoValue = lgMember.reapearLargeAmmoValue;

    if(lgMember.isDipSwitchPlayerNumberSet)
    {
        dipSwitchPlayerNumber = lgMember.dipSwitchPlayerNumber;
        isDipSwitchPlayerNumberSet = true;
    }
    else
    {
        isDipSwitchPlayerNumberSet = false;
        dipSwitchPlayerNumber = UNASSIGN;
    }

    hubComPortNumber = lgMember.hubComPortNumber;

    if(lgMember.isAnalogStrengthSet)
    {
        analogStrength = lgMember.analogStrength;
        isAnalogStrengthSet = true;
    }
    else
    {
        analogStrength = UNASSIGN;
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
    ammoCheckValue = lgMember.ammoCheckValue;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;

    tcpPort = lgMember.tcpPort;
    tcpPlayer = lgMember.tcpPlayer;
    recoilVoltage = lgMember.recoilVoltage;
    recoilVoltageOverride = lgMember.recoilVoltageOverride;
    sindenRecoilOverride = lgMember.sindenRecoilOverride;

    outputConnection = lgMember.outputConnection;

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
    noDisplay = lgMember.noDisplay;

    displayAmmoLife = lgMember.displayAmmoLife;
    displayAmmoLifeGlyphs = lgMember.displayAmmoLifeGlyphs;
    displayAmmoLifeBar = lgMember.displayAmmoLifeBar;
    displayAmmoLifeNumber = lgMember.displayAmmoLifeNumber;
    lifeBarMaxLife = lgMember.lifeBarMaxLife;

    lgRecoilPriority[0] = lgMember.lgRecoilPriority[0];
    lgRecoilPriority[1] = lgMember.lgRecoilPriority[1];
    lgRecoilPriority[2] = lgMember.lgRecoilPriority[2];
    lgRecoilPriority[3] = lgMember.lgRecoilPriority[3];

    reloadSetting = lgMember.reloadSetting;
    damageSetting = lgMember.damageSetting;
    deathSetting = lgMember.deathSetting;
    shakeEnalbe = lgMember.shakeEnalbe;

    loadReloadShake = lgMember.loadReloadShake;
    loadReloadLED = lgMember.loadReloadLED;
    loadDamageShake = lgMember.loadDamageShake;
    loadDamageLED = lgMember.loadDamageLED;
    loadDeathShake = lgMember.loadDeathShake;
    loadDeathLED = lgMember.loadDeathLED;

    reaperLargeAmmo = lgMember.reaperLargeAmmo;
    isReaper5LEDsInited = lgMember.isReaper5LEDsInited;
    enableReaperAmmo0Delay = lgMember.enableReaperAmmo0Delay;
    repearAmmo0Delay = lgMember.repearAmmo0Delay;
    isReaperAmmo0TimerInited = false;
    reaperLoadedReload = false;
    disableReaperHoldBack = lgMember.disableReaperHoldBack;
    reaperAmmo0Check = lgMember.reaperAmmo0Check;

    if(!lgMember.isReaperAmmo0TimerInited && enableReaperAmmo0Delay)
        InitReaperAmmo0Timer();


    reaperHoldSlideTime = lgMember.reaperHoldSlideTime;
    isReaperTimerRunning = lgMember.isReaperTimerRunning;
    isReaperSlideHeldBack = lgMember.isReaperSlideHeldBack;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;

    LoadDefaultLGCommands();

}

//For MX24 Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, bool dipSwitchSet, quint8 dipSwitchNumber, quint8 hubcpNumber, SupportedRecoils lgRecoils)
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
    reapearLargeAmmoValue = LARGEAMMOVALUEDEFAULT;

    analogStrength = UNASSIGN;
    isAnalogStrengthSet = false;

    lastAmmoValue = 0;
    ammoCheckValue = 1;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;

    tcpPort = 0;
    tcpPlayer = UNASSIGN;
    recoilVoltage = UNASSIGN;
    recoilVoltageOverride = false;
    sindenRecoilOverride = false;

    outputConnection = SERIALPORT;

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
    noDisplay = true;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    reloadSetting = 0;
    damageSetting = 0;
    deathSetting = 0;
    shakeEnalbe = false;

    loadReloadShake = false;
    loadReloadLED = false;
    loadDamageShake = false;
    loadDamageLED = false;
    loadDeathShake = false;
    loadDeathLED = false;

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = DEFAULTAMMO0DELAY;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;
    reaperLoadedReload = false;
    disableReaperHoldBack = false;
    reaperAmmo0Check = false;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;

    FillSerialPortInfo();

    LoadDefaultLGCommands();

    //Connect Commands up
    connect(this, &LightGun::DoAmmoValueCommands, this, &LightGun::AmmoValueNormal);
    connect(this, &LightGun::DoDisplayAmmoCommands, this, &LightGun::DisplayAmmoNormal);
    connect(this, &LightGun::DoDisplayLifeCommands, this, &LightGun::DisplayLifeNormal);
    connect(this, &LightGun::DoDisplayOtherCommands, this, &LightGun::DisplayOtherNormal);
}


//For OpenFire
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, SupportedRecoils lgRecoils, LightGunSettings lgSet, bool noDis, DisplayPriority displayP, DisplayOpenFire displayOF)
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
    dipSwitchPlayerNumber = UNASSIGN;
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
    reapearLargeAmmoValue = LARGEAMMOVALUEDEFAULT;

    analogStrength = DEFAULTANALOGSTRENGTH;
    isAnalogStrengthSet = true;

    lastAmmoValue = 0;
    ammoCheckValue = 1;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;

    tcpPort = 0;
    tcpPlayer = UNASSIGN;
    recoilVoltage = UNASSIGN;
    recoilVoltageOverride = false;
    sindenRecoilOverride = false;

    outputConnection = SERIALPORT;

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
    displayAmmoPriority = displayP.ammo;
    displayLifePriority = displayP.life;
    displayOtherPriority = displayP.other;
    displayRefresh = DISPLAYREFRESHDEFAULT;
    noDisplay = noDis;

    displayAmmoLife = displayOF.ammoAndLifeDisplay;
    displayAmmoLifeGlyphs = displayOF.lifeGlyphs;
    displayAmmoLifeBar = displayOF.lifeBar;
    displayAmmoLifeNumber = displayOF.lifeNumber;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    reloadSetting = lgSet.reload;
    damageSetting = lgSet.damage;
    deathSetting = lgSet.death;
    shakeEnalbe = lgSet.shake;

    if(reloadSetting == 1)
    {
        loadReloadShake = true;
        loadReloadLED = true;
    }
    else if(reloadSetting == 2)
    {
        loadReloadShake = true;
        loadReloadLED = false;
    }
    else if(reloadSetting == 3)
    {
        loadReloadShake = false;
        loadReloadLED = true;
    }
    else
    {
        loadReloadShake = false;
        loadReloadLED = false;
    }

    if(damageSetting == 1)
    {
        loadDamageShake = true;
        loadDamageLED = true;
    }
    else if(damageSetting == 2)
    {
        loadDamageShake = true;
        loadDamageLED = false;
    }
    else if(damageSetting == 3)
    {
        loadDamageShake = false;
        loadDamageLED = true;
    }
    else
    {
        loadDamageShake = false;
        loadDamageLED = false;
    }

    if(deathSetting == 1)
    {
        loadDeathShake = true;
        loadDeathLED = true;
    }
    else if(deathSetting == 2)
    {
        loadDeathShake = true;
        loadDeathLED = false;
    }
    else if(deathSetting == 3)
    {
        loadDeathShake = false;
        loadDeathLED = true;
    }
    else
    {
        loadDeathShake = false;
        loadDeathLED = false;
    }

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = DEFAULTAMMO0DELAY;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;
    reaperLoadedReload = false;
    disableReaperHoldBack = false;
    reaperAmmo0Check = false;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;

    FillSerialPortInfo();

    LoadDefaultLGCommands();

    //Connect Commands up
    connect(this, &LightGun::DoAmmoValueCommands, this, &LightGun::AmmoValueNormal);
    connect(this, &LightGun::DoDisplayAmmoCommands, this, &LightGun::DisplayAmmoNormal);
    connect(this, &LightGun::DoDisplayLifeCommands, this, &LightGun::DisplayLifeOpenFire);
    connect(this, &LightGun::DoDisplayOtherCommands, this, &LightGun::DisplayOtherNormal);
}


//Alien USB Light Gun
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, SupportedRecoils lgRecoils, bool n2DDisplay, DisplayPriority displayP)
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
    dipSwitchPlayerNumber = UNASSIGN;
    hubComPortNumber = UNASSIGN;
    maxAmmoSet = false;
    reloadValueSet = false;
    disableReaperLEDs = false;
    reapearLargeAmmoValue = LARGEAMMOVALUEDEFAULT;
    isAnalogStrengthSet = false;
    analogStrength = UNASSIGN;

    lastAmmoValue = 0;
    ammoCheckValue = 1;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;

    tcpPort = 0;
    tcpPlayer = UNASSIGN;
    recoilVoltage = UNASSIGN;
    recoilVoltageOverride = false;
    sindenRecoilOverride = false;

    outputConnection = USBHID;

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
    displayAmmoPriority = displayP.ammo;
    displayLifePriority = displayP.life;
    displayOtherPriority = displayP.other;
    displayRefresh = DISPLAYREFRESHDEFAULT;
    noDisplay = n2DDisplay;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    // No Shake Motor, so no reload, damage, death, or shake feature
    reloadSetting = 0;
    damageSetting = 0;
    deathSetting = 0;
    shakeEnalbe = false;

    loadReloadShake = false;
    loadReloadLED = false;
    loadDamageShake = false;
    loadDamageLED = false;
    loadDeathShake = false;
    loadDeathLED = false;

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = DEFAULTAMMO0DELAY;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;
    reaperLoadedReload = false;
    disableReaperHoldBack = false;
    reaperAmmo0Check = false;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;

    LoadDefaultLGCommands();

    //Connect Commands up
    connect(this, &LightGun::DoAmmoValueCommands, this, &LightGun::AmmoValueNormal);
    connect(this, &LightGun::DoDisplayAmmoCommands, this, &LightGun::DisplayAmmo2Digit);
    connect(this, &LightGun::DoDisplayLifeCommands, this, &LightGun::DisplayLife2Digit);
    connect(this, &LightGun::DoDisplayOtherCommands, this, &LightGun::DisplayOther2Digit);
}

//AimTrak
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, HIDInfo hidInfoStruct, quint16 rcDelay, SupportedRecoils lgRecoils)
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
    dipSwitchPlayerNumber = UNASSIGN;
    hubComPortNumber = UNASSIGN;
    maxAmmoSet = false;
    reloadValueSet = false;
    disableReaperLEDs = false;
    reapearLargeAmmoValue = LARGEAMMOVALUEDEFAULT;
    isAnalogStrengthSet = false;
    analogStrength = UNASSIGN;

    lastAmmoValue = 0;
    ammoCheckValue = 1;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;

    tcpPort = 0;
    tcpPlayer = UNASSIGN;
    recoilVoltage = UNASSIGN;
    recoilVoltageOverride = false;
    sindenRecoilOverride = false;

    outputConnection = USBHID;

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
    noDisplay = true;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    // No Shake Motor, so no reload, damage, death, or shake feature
    reloadSetting = 0;
    damageSetting = 0;
    deathSetting = 0;
    shakeEnalbe = false;

    loadReloadShake = false;
    loadReloadLED = false;
    loadDamageShake = false;
    loadDamageLED = false;
    loadDeathShake = false;
    loadDeathLED = false;

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = DEFAULTAMMO0DELAY;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;
    reaperLoadedReload = false;
    disableReaperHoldBack = false;
    reaperAmmo0Check = false;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;

    LoadDefaultLGCommands();

    //Connect Commands up
    connect(this, &LightGun::DoAmmoValueCommands, this, &LightGun::AmmoValueNormal);
    connect(this, &LightGun::DoDisplayAmmoCommands, this, &LightGun::DisplayAmmoNormal);
    connect(this, &LightGun::DoDisplayLifeCommands, this, &LightGun::DisplayLifeNormal);
    connect(this, &LightGun::DoDisplayOtherCommands, this, &LightGun::DisplayOtherNormal);
}

//Sinden
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint16 port, quint8 player, quint8 recVolt, SupportedRecoils lgRecoils, LightGunSettings lgSet)
{
    defaultLightGun = lgDefault;
    defaultLightGunNum = dlgNum;
    lightGunName = lgName;
    lightGunNum = lgNumber;

    tcpPort = port;
    tcpPlayer = player;

    recoilVoltage = recVolt;
    recoilVoltageOverride = false;
    sindenRecoilOverride = false;

    if(tcpPlayer == 0)
        recoilVoltageCMD = RECOILVOLTP1CMD;
    else
        recoilVoltageCMD = RECOILVOLTP2CMD;

    recoilVoltageCMD.append (QString::number(recoilVoltage));

    comPortNum = UNASSIGN;
    comPortBaud = UNASSIGN;
    comPortDataBits = UNASSIGN;
    comPortParity = UNASSIGN;
    comPortStopBits = UNASSIGN;
    comPortFlow = UNASSIGN;

    isDipSwitchPlayerNumberSet = false;
    dipSwitchPlayerNumber = UNASSIGN;
    hubComPortNumber = UNASSIGN;
    maxAmmoSet = false;
    reloadValueSet = false;
    disableReaperLEDs = false;
    reapearLargeAmmoValue = LARGEAMMOVALUEDEFAULT;
    isAnalogStrengthSet = false;
    analogStrength = UNASSIGN;

    lastAmmoValue = 0;
    ammoCheckValue = 1;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;

    outputConnection = TCP;

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
    noDisplay = true;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    reloadSetting = lgSet.reload;
    damageSetting = lgSet.damage;
    deathSetting = lgSet.death;
    shakeEnalbe = lgSet.shake;

    if(reloadSetting == 1)
        loadReloadShake = true;
    else
        loadReloadShake = false;

    if(damageSetting == 1)
        loadDamageShake = true;
    else
        loadDamageShake = false;

    if(deathSetting == 1)
        loadDeathShake = true;
    else
        loadDeathShake = false;

    loadReloadLED = false;
    loadDamageLED = false;
    loadDeathLED = false;

    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = DEFAULTAMMO0DELAY;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;
    reaperLoadedReload = false;
    disableReaperHoldBack = false;
    reaperAmmo0Check = false;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;

    LoadDefaultLGCommands();

    //Connect Commands up
    connect(this, &LightGun::DoAmmoValueCommands, this, &LightGun::AmmoValueSinden);
    connect(this, &LightGun::DoDisplayAmmoCommands, this, &LightGun::DisplayAmmoNormal);
    connect(this, &LightGun::DoDisplayLifeCommands, this, &LightGun::DisplayLifeNormal);
    connect(this, &LightGun::DoDisplayOtherCommands, this, &LightGun::DisplayOtherNormal);
}

//For Blamcon
LightGun::LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, SupportedRecoils lgRecoils, LightGunSettings lgSet, bool has2DigitDiplay, DisplayPriority displayP)
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
    reapearLargeAmmoValue = LARGEAMMOVALUEDEFAULT;

    isDipSwitchPlayerNumberSet = false;
    dipSwitchPlayerNumber = UNASSIGN;

    hubComPortNumber = UNASSIGN;

    analogStrength = UNASSIGN;
    isAnalogStrengthSet = false;

    lastAmmoValue = 0;
    ammoCheckValue = 1;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;

    tcpPort = 0;
    tcpPlayer = UNASSIGN;
    recoilVoltage = UNASSIGN;
    recoilVoltageOverride = false;
    sindenRecoilOverride = false;

    outputConnection = SERIALPORT;

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
    displayAmmoPriority = displayP.ammo;
    displayLifePriority = displayP.life;
    displayOtherPriority = displayP.other;
    displayRefresh = DISPLAYREFRESHDEFAULT;

    if(has2DigitDiplay)
        noDisplay = false;
    else
        noDisplay = true;

    displayAmmoLife = false;
    displayAmmoLifeGlyphs = true;
    displayAmmoLifeBar = false;
    displayAmmoLifeNumber = false;
    lifeBarMaxLife = 0;

    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;

    //Settings for Reload, Damage, Death & Shake Feature
    reloadSetting = lgSet.reload;
    damageSetting = lgSet.damage;
    deathSetting = lgSet.death;
    shakeEnalbe = lgSet.shake;

    if(defaultLightGunNum == XGUNNER)
    {
        if(reloadSetting == 1)
            loadReloadShake = true;
        else
            loadReloadShake = false;

        if(damageSetting == 1)
            loadDamageShake = true;
        else
            loadDamageShake = false;

        if(deathSetting == 1)
            loadDeathShake = true;
        else
            loadDeathShake = false;

        loadReloadLED = false;
        loadDamageLED = false;
        loadDeathLED = false;
    }
    else    // For Fusion and Xena
    {
        if(reloadSetting == 1)
        {
            loadReloadShake = true;
            loadReloadLED = true;
        }
        else if(reloadSetting == 2)
        {
            loadReloadShake = true;
            loadReloadLED = false;
        }
        else if(reloadSetting == 3)
        {
            loadReloadShake = false;
            loadReloadLED = true;
        }
        else
        {
            loadReloadShake = false;
            loadReloadLED = false;
        }

        if(damageSetting == 1)
        {
            loadDamageShake = true;
            loadDamageLED = true;
        }
        else if(damageSetting == 2)
        {
            loadDamageShake = true;
            loadDamageLED = false;
        }
        else if(damageSetting == 3)
        {
            loadDamageShake = false;
            loadDamageLED = true;
        }
        else
        {
            loadDamageShake = false;
            loadDamageLED = false;
        }

        if(deathSetting == 1)
        {
            loadDeathShake = true;
            loadDeathLED = true;
        }
        else if(deathSetting == 2)
        {
            loadDeathShake = true;
            loadDeathLED = false;
        }
        else if(deathSetting == 3)
        {
            loadDeathShake = false;
            loadDeathLED = true;
        }
        else
        {
            loadDeathShake = false;
            loadDeathLED = false;
        }
    }


    reaperLargeAmmo = false;
    isReaper5LEDsInited = false;
    enableReaperAmmo0Delay = false;
    repearAmmo0Delay = DEFAULTAMMO0DELAY;
    isReaperAmmo0TimerInited = false;
    reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    isReaperTimerRunning = false;
    isReaperSlideHeldBack = false;
    reaperLoadedReload = false;
    disableReaperHoldBack = false;
    reaperAmmo0Check = false;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;

    FillSerialPortInfo();

    LoadDefaultLGCommands();

    //Connect Commands up
    connect(this, &LightGun::DoAmmoValueCommands, this, &LightGun::AmmoValueNormal);
    connect(this, &LightGun::DoDisplayAmmoCommands, this, &LightGun::DisplayAmmo2Digit);
    connect(this, &LightGun::DoDisplayLifeCommands, this, &LightGun::DisplayLife2Digit);
    connect(this, &LightGun::DoDisplayOtherCommands, this, &LightGun::DisplayOther2Digit);
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
void LightGun::SetDisplayPriority(DisplayPriority displayP)
{
    displayAmmoPriority = displayP.ammo;
    displayLifePriority = displayP.life;
    displayOtherPriority = displayP.other;
}

/*
void LightGun::SetDisplayAmmoAndLife(bool displayAAL, bool displayLG, bool displayLB, bool displayLN)
{
    displayAmmoLife = displayAAL;
    displayAmmoLifeGlyphs = displayLG;
    displayAmmoLifeBar = displayLB;
    displayAmmoLifeNumber = displayLN;

}
*/

void LightGun::SetRecoilPriority(SupportedRecoils lgRecoils)
{
    lgRecoilPriority[0] = lgRecoils.ammoValue;
    lgRecoilPriority[1] = lgRecoils.recoil;
    lgRecoilPriority[2] = lgRecoils.recoilR2S;
    lgRecoilPriority[3] = lgRecoils.recoilValue;
}

void LightGun::SetLightGunSettings(LightGunSettings lgSettings)
{
    reloadSetting = lgSettings.reload;
    damageSetting = lgSettings.damage;
    deathSetting = lgSettings.death;
    shakeEnalbe = lgSettings.shake;

    //Reload Loadings
    if(reloadSetting == 1)
    {
        loadReloadShake = true;
        loadReloadLED = true;
    }
    else if(reloadSetting == 2)
    {
        loadReloadShake = true;
        loadReloadLED = false;
    }
    else if(reloadSetting == 3)
    {
        loadReloadShake = false;
        loadReloadLED = true;
    }
    else
    {
        loadReloadShake = false;
        loadReloadLED = false;
    }

    //Damage Loadings
    if(damageSetting == 1)
    {
        loadDamageShake = true;
        loadDamageLED = true;
    }
    else if(damageSetting == 2)
    {
        loadDamageShake = true;
        loadDamageLED = false;
    }
    else if(damageSetting == 3)
    {
        loadDamageShake = false;
        loadDamageLED = true;
    }
    else
    {
        loadDamageShake = false;
        loadDamageLED = false;
    }

    //Death Loadings
    if(deathSetting == 1)
    {
        loadDeathShake = true;
        loadDeathLED = true;
    }
    else if(deathSetting == 2)
    {
        loadDeathShake = true;
        loadDeathLED = false;
    }
    else if(deathSetting == 3)
    {
        loadDeathShake = false;
        loadDeathLED = true;
    }
    else
    {
        loadDeathShake = false;
        loadDeathLED = false;
    }


    LoadDefaultLGCommands();
}

/*
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
*/

void LightGun::SetRecoilVoltage(quint8 recVolt)
{
    recoilVoltage = recVolt;

    if(tcpPlayer == 0)
        recoilVoltageCMD = RECOILVOLTP1CMD;
    else
        recoilVoltageCMD = RECOILVOLTP2CMD;

    recoilVoltageCMD.append (QString::number(recoilVoltage));
}

void LightGun::SetRecoilVoltageOverride(quint8 recVoltOvr)
{
    recoilVoltageOverride = true;
    recoilVoltageOriginal = recoilVoltage;

    if(recVoltOvr > RECOILVOLTAGEMAX)
        recoilVoltage = RECOILVOLTAGEMAX;
    else
        recoilVoltage = recVoltOvr;

    if(tcpPlayer == 0)
        recoilVoltageCMD = RECOILVOLTP1CMD;
    else
        recoilVoltageCMD = RECOILVOLTP2CMD;

    recoilVoltageCMD.append (QString::number(recoilVoltage));
}

void LightGun::SetSindenRecoilOverride(quint8 recOVRDE)
{
    if(defaultLightGunNum == SINDEN)
    {
        sindenRecoilOverride = true;
        sindenRecoilOverrideValue = recOVRDE;

        if(tcpPlayer == 0)
            sindenRecoilOverrideCMD = '1';
        else
            sindenRecoilOverrideCMD = '2';

        if(sindenRecoilOverrideValue == SINDENSINGLESHOT)
            sindenRecoilOverrideCMD.append (SINDENSINGLESHOTCMD);
        else if(sindenRecoilOverrideValue == SINDENAUTODEFAULT)
            sindenRecoilOverrideCMD.append (SINDENAUTODEFAULTCMD);
        else if(sindenRecoilOverrideValue == SINDENAUTOFAST)
            sindenRecoilOverrideCMD.append (SINDENAUTOFASTCMD);
        else if(sindenRecoilOverrideValue == SINDENAUTOSTRONGE)
            sindenRecoilOverrideCMD.append (SINDENAUTOSTRONGECMD);

        //qDebug() << "sindenRecoilOverrideValue" << sindenRecoilOverrideValue << "sindenRecoilOverrideCMD" << sindenRecoilOverrideCMD;
    }
}

void LightGun::SetAmmoCheck()
{
    if(defaultLightGunNum != AIMTRAK)
        doAmmoCheck = true;
}

void LightGun::SetReaperSlideData(ReaperSlideData slideData)
{
    disableReaperHoldBack = slideData.disableHoldBack;

    if(disableReaperHoldBack || !slideData.enableHoldDelay)
    {
        enableReaperAmmo0Delay = false;
        repearAmmo0Delay = DEFAULTAMMO0DELAY;
        reaperHoldSlideTime = REAPERHOLDSLIDETIME;
    }
    else if(slideData.enableHoldDelay)
    {
        enableReaperAmmo0Delay = true;
        repearAmmo0Delay = slideData.holdDelay;
        reaperHoldSlideTime = slideData.slideHoldTime;
    }

    if(disableReaperHoldBack || enableReaperAmmo0Delay)
        reaperAmmo0Check = true;
    else
        reaperAmmo0Check = false;

    if(!isReaperAmmo0TimerInited)
        InitReaperAmmo0Timer();
    else
        p_reaperAmmo0Timer->setInterval(repearAmmo0Delay);
}

void LightGun::SetDisplayOpenFire(DisplayOpenFire displayOF)
{
    if(defaultLightGunNum == OPENFIRE)
    {
        displayAmmoLife = displayOF.ammoAndLifeDisplay;
        displayAmmoLifeGlyphs = displayOF.lifeGlyphs;
        displayAmmoLifeBar = displayOF.lifeBar;
        displayAmmoLifeNumber = displayOF.lifeNumber;
    }
}




//Get Member Functions

quint8 LightGun::GetComPortNumber()
{
    if(defaultLightGun && defaultLightGunNum == MX24)
        return hubComPortNumber;

    return comPortNum;
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

/*
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
*/

quint16 LightGun::GetRecoilDelay()
{
    if(isRecoilDelaySet)
        return recoilDelay;
    else
        return 0;
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

DisplayPriority LightGun::GetDisplayPriority()
{
    DisplayPriority displayP;
    displayP.ammo = displayAmmoPriority;
    displayP.life = displayLifePriority;
    displayP.other = displayOtherPriority;
    return displayP;
}

/*
bool LightGun::GetDisplayAmmoAndLife(bool *displayLG, bool *displayLB, bool *displayLN)
{
    *displayLG = displayAmmoLifeGlyphs;
    *displayLB = displayAmmoLifeBar;
    *displayLN = displayAmmoLifeNumber;

    return displayAmmoLife;
}
*/

quint16 LightGun::GetDisplayRefresh(bool *isRDS)
{
    *isRDS = displayRefreshSet;

    if(displayAmmoLife)
        return displayRefresh+15;

    return displayRefresh;
}

LightGunSettings LightGun::GetLightGunSettings()
{
    LightGunSettings lgSet;

    lgSet.reload = reloadSetting;
    lgSet.damage = damageSetting;
    lgSet.death = deathSetting;
    lgSet.shake = shakeEnalbe;

    return lgSet;
}

/*
quint8 LightGun::GetReaperAmmo0Delay(bool *isAmmo0DelayEnabled, quint16 *reaperHST)
{
    *isAmmo0DelayEnabled = enableReaperAmmo0Delay;
    *reaperHST = reaperHoldSlideTime;
    return repearAmmo0Delay;
}
*/

ReaperSlideData LightGun::GetReaperSlideData()
{
    ReaperSlideData slideData;
    slideData.disableHoldBack = disableReaperHoldBack;
    slideData.enableHoldDelay = enableReaperAmmo0Delay;
    slideData.holdDelay = repearAmmo0Delay;
    slideData.slideHoldTime = reaperHoldSlideTime;
    return slideData;
}

DisplayOpenFire LightGun::GetDisplayOpenFire()
{
    DisplayOpenFire displayOF;

    displayOF.ammoAndLifeDisplay = displayAmmoLife;
    displayOF.lifeGlyphs = displayAmmoLifeGlyphs;
    displayOF.lifeBar = displayAmmoLifeBar;
    displayOF.lifeNumber = displayAmmoLifeNumber;

    return displayOF;
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
    reapearLargeAmmoValue = lgMember.reapearLargeAmmoValue;

    if(lgMember.isDipSwitchPlayerNumberSet)
    {
        dipSwitchPlayerNumber = lgMember.dipSwitchPlayerNumber;
        isDipSwitchPlayerNumberSet = true;
    }
    else
    {
        isDipSwitchPlayerNumberSet = false;
        dipSwitchPlayerNumber = UNASSIGN;
    }

    hubComPortNumber = lgMember.hubComPortNumber;

    if(lgMember.isAnalogStrengthSet)
    {
        analogStrength = lgMember.analogStrength;
        isAnalogStrengthSet = true;
    }
    else
    {
        analogStrength = UNASSIGN;
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
    ammoCheckValue = lgMember.ammoCheckValue;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;

    tcpPort = lgMember.tcpPort;
    tcpPlayer = lgMember.tcpPlayer;
    recoilVoltage = lgMember.recoilVoltage;
    recoilVoltageOverride = lgMember.recoilVoltageOverride;
    sindenRecoilOverride = lgMember.sindenRecoilOverride;

    outputConnection = lgMember.outputConnection;

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

    reloadSetting = lgMember.reloadSetting;
    damageSetting = lgMember.damageSetting;
    deathSetting = lgMember.deathSetting;
    shakeEnalbe = lgMember.shakeEnalbe;

    loadReloadShake = lgMember.loadReloadShake;
    loadReloadLED = lgMember.loadReloadLED;
    loadDamageShake = lgMember.loadDamageShake;
    loadDamageLED = lgMember.loadDamageLED;
    loadDeathShake = lgMember.loadDeathShake;
    loadDeathLED = lgMember.loadDeathLED;

    reaperLargeAmmo = lgMember.reaperLargeAmmo;
    isReaper5LEDsInited = lgMember.isReaper5LEDsInited;
    enableReaperAmmo0Delay = lgMember.enableReaperAmmo0Delay;
    repearAmmo0Delay = lgMember.repearAmmo0Delay;
    isReaperAmmo0TimerInited = false;
    reaperLoadedReload = false;
    disableReaperHoldBack = lgMember.disableReaperHoldBack;
    reaperAmmo0Check = lgMember.reaperAmmo0Check;

    if(!lgMember.isReaperAmmo0TimerInited && enableReaperAmmo0Delay)
        InitReaperAmmo0Timer();

    reaperHoldSlideTime = lgMember.reaperHoldSlideTime;
    isReaperTimerRunning = lgMember.isReaperTimerRunning;
    isReaperSlideHeldBack = lgMember.isReaperSlideHeldBack;

    noDisplay = lgMember.noDisplay;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;

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
    offscreenLeftCornerCmdsSet = false;
    offscreenDisableCmdsSet = false;
    outOfAmmoCmdSet = false;
    lifeValueCmdSet = false;
    deathValueCmdSet = false;

    reaperLoadedReload = false;

    hasDisplay = false;
    hasReload = false;
    hasDamage = false;
    hasDeath = false;
    hasShake = false;

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
    offscreenLeftCornerCmds.clear();
    offscreenDisableCmds.clear();
    outOfAmmoCmds.clear();
    lifeValueCmds.clear();
    deathValueCmds.clear();


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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        bool openFile = defaultLGCmdFile.open (QIODeviceBase::ReadOnly | QIODevice::Text);
#else
        bool openFile = defaultLGCmdFile.open (QIODevice::ReadOnly | QIODevice::Text);
#endif

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
                //Get rid of any blank spaces at the end and front
                line = line.trimmed ();

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
                    else if(splitLines[0] == DAMAGECMDONLY && loadDamageShake)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            damageCmds << commands[i];
                        }
                        damageCmdsSet = true;
                        hasDamage = true;
                    }
                    else if(splitLines[0] == DAMAGELEDCMDONLY && loadDamageLED)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            damageCmds << commands[i];
                        }
                        damageCmdsSet = true;
                        hasDamage = true;
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
                    else if(splitLines[0] == RELOADCMDONLY && loadReloadShake)
                    {
                        if(defaultLightGunNum == RS3_REAPER && !reaperLoadedReload)
                        {
                            reloadCmds << REAPERRELOADCOMMAND;
                            reaperLoadedReload = true;
                        }

                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            reloadCmds << commands[i];
                        }
                        reloadCmdsSet = true;
                        hasReload = true;
                    }
                    else if(splitLines[0] == RELOADLEDCMDONLY && loadReloadLED)
                    {
                        //Reaper doesn't have LED Command
                        //if(defaultLightGunNum == RS3_REAPER && !reaperLoadedReload)
                        //{
                        //    reloadCmds << REAPERRELOADCOMMAND;
                        //    reaperLoadedReload = true;
                        //}

                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            reloadCmds << commands[i];
                        }
                        reloadCmdsSet = true;
                        hasReload = true;
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
                    else if(splitLines[0] == SHAKECMDONLY &&  shakeEnalbe)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            shakeCmds << commands[i];
                        }
                        shakeCmdsSet = true;
                        hasShake = true;
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
                    else if(splitLines[0] == DISPLAYAMMOONLY && !noDisplay)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            displayAmmoCmds << commands[i];
                        }
                        displayAmmoCmdsSet = true;
                        hasDisplay = true;
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
                    else if(splitLines[0] == DISPLAYLIFEONLY && !noDisplay)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();

                            if(defaultLightGun && defaultLightGunNum == OPENFIRE && displayAmmoLifeNumber)
                                commands[i] = OPENFIRELIFENUMCMD;

                            displayLifeCmds << commands[i];
                        }
                        displayLifeCmdsSet = true;
                        hasDisplay = true;
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
                    else if(splitLines[0] == DISPLAYOTHERONLY && !noDisplay)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            displayOtherCmds << commands[i];
                        }
                        displayOtherCmdsSet = true;
                        hasDisplay = true;
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
                        displayRefresh = commands[0].toUInt (&isNumber);

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

                    else if(splitLines[0] == OFFSCREENLEFTCORNERONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            offscreenLeftCornerCmds << commands[i];
                        }
                        offscreenLeftCornerCmdsSet = true;
                    }
                    else if(splitLines[0] == OFFSCREENDISABLECMDONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            offscreenDisableCmds << commands[i];
                        }
                        offscreenDisableCmdsSet = true;
                    }
                    else if(splitLines[0] == OUTOFAMMOONLY)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            outOfAmmoCmds << commands[i];
                        }
                        outOfAmmoCmdSet = true;
                    }
                    else if(splitLines[0] == LIFEVALUECMDONLY && loadDeathShake)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            lifeValueCmds << commands[i];
                        }
                        lifeValueCmdSet = true;
                        hasDeath = true;
                    }
                    else if(splitLines[0] == LIFEVALUELEDCMDONLY && loadDeathLED)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            lifeValueCmds << commands[i];
                        }
                        lifeValueCmdSet = true;
                        hasDeath = true;
                    }
                    else if(splitLines[0] == DEATHVALUECMDONLY && loadDeathShake)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            deathValueCmds << commands[i];
                        }
                        deathValueCmdSet = true;
                        hasDeath = true;
                    }
                    else if(splitLines[0] == DEATHVALUELEDCMDONLY && loadDeathLED)
                    {
                        for(i = 0; i < numberCommands; i++)
                        {
                            commands[i] = commands[i].trimmed ();
                            deathValueCmds << commands[i];
                        }
                        deathValueCmdSet = true;
                        hasDeath = true;
                    }


                }//if(numberCommands > 1)

            }//if(line != ENDOFFILE)

        }//while(!in.atEnd ())

        //Close the File
        defaultLGCmdFile.close ();

        //Make Sure Reload Command Z6 gets loaded into Reaper LG
        if(defaultLightGunNum == RS3_REAPER && !reaperLoadedReload)
        {
            reloadCmds << REAPERRELOADCOMMAND;
            reaperLoadedReload = true;
            reloadCmdsSet = true;
            hasReload = true;
        }


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
    qDebug() << "No 2 Digit Display:" << noDisplay;

    qDebug() << openComPortCmdsSet;
    qDebug() << closeComPortCmdsSet;
    qDebug() << damageCmdsSet;
    qDebug() << recoilCmdsSet;
    qDebug() << recoilR2SCmdsSet;
    qDebug() << recoilValueCmdsSet;
    qDebug() << reloadCmdsSet;
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
    qDebug() << displayRefreshSet;
    qDebug() << outOfAmmoCmdSet;
    qDebug() << lifeValueCmdSet;
    qDebug() << deathValueCmdSet;

    qDebug() << openComPortCmds;
    qDebug() << closeComPortCmds;
    qDebug() << damageCmds;
    qDebug() << recoilCmds;
    qDebug() << recoilR2SCmds;
    qDebug() << reloadCmds;
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
    qDebug() << outOfAmmoCmds;
    qDebug() << lifeValueCmds;
    qDebug() << deathValueCmds;
*/

}





QStringList LightGun::SplitLoadedCommands(QString commandList)
{
    QStringList cmdSplit, returnList;
    quint8 cmdCount;
    QString goodCMD;
    QString frontPart;
    bool isError = false;

    commandList = commandList.trimmed ();

    quint16 indexOfP = commandList.indexOf('P');

    //qDebug() << "String after trimmed:" << commandList << "Index of P" << indexOfP;

    //Make sure P1 is at the begining of the string
    if(indexOfP != 0)
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        frontPart = commandList.first (indexOfP);
#else
        quint16 chopLength = commandList.size() - indexOfP;
        frontPart = commandList.chopped(chopLength);
#endif
        frontPart = frontPart.trimmed ();
        commandList = commandList.remove(0,indexOfP);
    }

    cmdSplit = commandList.split ('|', Qt::SkipEmptyParts);

    //Remove any empty spaces at the begining or the end
    for(quint8 i = 0; i < cmdSplit.length(); i++)
        cmdSplit[i] = cmdSplit[i].trimmed();

    //qDebug() << "cmdSplit[0]:" << cmdSplit[0] << "cmdSplit[1]:" << cmdSplit[1];

    if((defaultLightGunNum == MX24 && isDipSwitchPlayerNumberSet) || defaultLightGunNum == SINDEN)
    {
        if((defaultLightGunNum == MX24 && dipSwitchPlayerNumber == 0) || (tcpPlayer == 0 && defaultLightGunNum == SINDEN))
        {
            if(cmdSplit[0][1] == '1')
                goodCMD = cmdSplit[0].remove (0,3);
            else if (cmdSplit[1][1] == '1')
                goodCMD = cmdSplit[1].remove (0,3);
            else
            {
                isError = true;
                QString tempCrit = "Cannot find P1 in the split commands. Command String: \""+commandList+"\" Please close HOTR and fix file immediately.";
                QMessageBox::critical (nullptr, "Loading Light Gun HOR Commands File", tempCrit, QMessageBox::Ok);
            }
        }
        else if((defaultLightGunNum == MX24 && dipSwitchPlayerNumber == 1) || (tcpPlayer == 1 && defaultLightGunNum == SINDEN))
        {
            if(cmdSplit[0][1] == '2')
                goodCMD = cmdSplit[0].remove (0,3);
            else if (cmdSplit[1][1] == '2')
                goodCMD = cmdSplit[1].remove (0,3);
            else
            {
                isError = true;
                QString tempCrit = "Cannot find P2 in the split commands. Command String: \""+commandList+"\" Please close HOTR and fix file immediately.";
                QMessageBox::critical (nullptr, "Loading Light Gun HOR Commands File", tempCrit, QMessageBox::Ok);
            }
        }
        else if(defaultLightGunNum == MX24 && dipSwitchPlayerNumber == 2)
        {
            if(cmdSplit[0][1] == '3')
                goodCMD = cmdSplit[0].remove (0,3);
            else if (cmdSplit[1][1] == '3')
                goodCMD = cmdSplit[1].remove (0,3);
            else if (cmdSplit[2][1] == '3')
                goodCMD = cmdSplit[2].remove (0,3);
            else if (cmdSplit[3][1] == '3')
                goodCMD = cmdSplit[3].remove (0,3);
            else
            {
                isError = true;
                QString tempCrit = "Cannot find P3 in the split commands. Command String: \""+commandList+"\" Please close HOTR and fix file immediately.";
                QMessageBox::critical (nullptr, "Loading Light Gun HOR Commands File", tempCrit, QMessageBox::Ok);
            }
        }
        else if(defaultLightGunNum == MX24 && dipSwitchPlayerNumber == 3)
        {
            if(cmdSplit[0][1] == '4')
                goodCMD = cmdSplit[0].remove (0,3);
            else if (cmdSplit[1][1] == '4')
                goodCMD = cmdSplit[1].remove (0,3);
            else if (cmdSplit[2][1] == '4')
                goodCMD = cmdSplit[2].remove (0,3);
            else if (cmdSplit[3][1] == '4')
                goodCMD = cmdSplit[3].remove (0,3);
            else
            {
                isError = true;
                QString tempCrit = "Cannot find P4 in the split commands. Command String: \""+commandList+"\" Please close HOTR and fix file immediately.";
                QMessageBox::critical (nullptr, "Loading Light Gun HOR Commands File", tempCrit, QMessageBox::Ok);
            }
        }

        if(!isError)
        {
            goodCMD = goodCMD.trimmed ();
            //qDebug() << "goodCMD" << goodCMD;
            returnList = goodCMD.split (' ', Qt::SkipEmptyParts);
        }
    }

    //qDebug() << "returnList: " << returnList;

    return returnList;
}














//Default Light Gun Commands for Certain Signals
QStringList LightGun::OpenComPortCommands(bool *isSet)
{
    *isSet = openComPortCmdsSet;

    if(defaultLightGunNum == SINDEN)
    {
        QStringList tempCMDs = openComPortCmds;
        tempCMDs.append (recoilVoltageCMD);

        if(sindenRecoilOverride)
            tempCMDs.append (sindenRecoilOverrideCMD);

        return tempCMDs;
    }

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
    tempCommands.clear();

    emit DoAmmoValueCommands(ammoValue);

    *isSet = tempEnableCommands;

    return tempCommands;
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
    tempCommands.clear();

    emit DoDisplayAmmoCommands(ammoValue);

    *isSet = tempEnableCommands;

    return tempCommands;
}


QStringList LightGun::DisplayLifeCommands(bool *isSet, quint16 lifeValue)
{
    tempCommands.clear();

    emit DoDisplayLifeCommands(lifeValue);

    *isSet = tempEnableCommands;

    return tempCommands;
}

QStringList LightGun::DisplayOtherCommands(bool *isSet, quint16 otherValue)
{
    tempCommands.clear();

    emit DoDisplayOtherCommands(otherValue);

    *isSet = tempEnableCommands;

    return tempCommands;
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

QStringList LightGun::OffscreenLeftCornerCommands(bool *isSet)
{
    *isSet = offscreenLeftCornerCmdsSet;
    return offscreenLeftCornerCmds;
}

QStringList LightGun::OffscreenDisableCommands(bool *isSet)
{
    *isSet = offscreenDisableCmdsSet;
    return offscreenDisableCmds;
}

QStringList LightGun::LifeValueCommands(bool *isSet, quint8 lifeValue)
{
    QStringList tempCMDs;

    //If Player is not in game yet or died
    if(!playerAlive)
    {
        //If player starts game, make playerAlive = true
        //And Do Nothing
        if(lifeValue > lastLifeValue)
        {
            playerAlive = true;
            maxLifeValue = lifeValue;

            if(maxDamage == 0)
                maxDamage = maxLifeValue * MAXDAMAGEPERCENTAGE;
        }
    }
    else  //Player is in the game now and Alive
    {
        if(lifeValue < lastLifeValue)
        {
            //Try to catch skip cut scene stuff
            quint8 damage = lastLifeValue - lifeValue;

            if(damage <= maxDamage)
            {
                //Check Damage, as it will happen more than Death
                if(lifeValue != 0)
                {
                    lastLifeValue = lifeValue;
                    *isSet = damageCmdsSet;
                    return damageCmds;
                }
                else  //Death Happended
                {
                    playerAlive = false;
                    lastLifeValue = lifeValue;
                    *isSet = lifeValueCmdSet;
                    return lifeValueCmds;
                }
            }
        }
    }

    //Everything else, do nothing
    lastLifeValue = lifeValue;
    *isSet = false;
    return tempCMDs;
}

QStringList LightGun::DeathValueCommands(bool *isSet, quint8 deathValue)
{
    QStringList tempCMDs;

    //If Player is not in game yet or died
    if(!playerAlive)
    {
        //If player starts game, make playerAlive = true
        //And Do Nothing
        if(deathValue > lastDeathValue)
        {
            playerAlive = true;
            maxLifeValue = deathValue;

            if(maxDamage == 0)
                maxDamage = maxLifeValue * MAXDAMAGEPERCENTAGE;
        }
    }
    else  //Player is in the game now and Alive
    {
        //Check if Damage & Death Happened
        if(deathValue < lastDeathValue)
        {
            //Try to catch skip cut scene stuff
            quint8 damage = lastDeathValue - deathValue;
            if(damage <= maxDamage)
            {
                //Check Death, as Death Value only Check for Death, and not Damage
                if(deathValue == 0)
                {
                    playerAlive = false;
                    lastDeathValue = deathValue;
                    *isSet = deathValueCmdSet;
                    return deathValueCmds;
                }
            }
        }
    }

    //Everything else, do nothing
    lastDeathValue = deathValue;
    *isSet = false;
    return tempCMDs;
}


void LightGun::ResetLightGun()
{
    lastAmmoValue = 0;
    ammoCheckValue = 1;
    lastLifeValue = 0;
    lastDeathValue = 0;
    playerAlive = false;
    maxLifeValue = 0;
    maxDamage = 0;
    doAmmoCheck = false;
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

    if(recoilVoltageOverride)
    {
        recoilVoltageOverride = false;
        recoilVoltage = recoilVoltageOriginal;

        if(tcpPlayer == 0)
            recoilVoltageCMD = RECOILVOLTP1CMD;
        else
            recoilVoltageCMD = RECOILVOLTP2CMD;

        recoilVoltageCMD.append (QString::number(recoilVoltage));
    }

    sindenRecoilOverride = false;


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


void LightGun::AmmoValueNormal(quint16 ammoValue)
{
    QString tempAVS;
    QString cmdString;

    tempCommands.clear();

    //If Ammo is the same, do nothing
    if(ammoValue == lastAmmoValue)
    {
        tempEnableCommands = false;
        return;
    }
    //Check if Reload Happened
    else if(ammoValue > lastAmmoValue)
    {
        lastAmmoValue = ammoValue;
        tempEnableCommands = reloadCmdsSet;
        tempCommands = reloadCmds;

        return;
    }
    else if((lastAmmoValue == 0 && ammoValue == 0) || (lastAmmoValue > ammoCheckValue && ammoValue == 0)) //Boot Up or Closing Game, Do Nothing
    {
        lastAmmoValue = ammoValue;
        tempEnableCommands = false;
        return;
    }
    else if(doAmmoCheck && (lastAmmoValue - ammoValue) != ammoCheckValue)
    {
        //When doAmmoCheck is set, it checks the ammo delta against the ammoCheckValue
        //If not equal, then do reload. In Slow Mode, ammoCheckValue is 2, all other times it is 1
        lastAmmoValue = ammoValue;
        tempEnableCommands = reloadCmdsSet;
        tempCommands = reloadCmds;
        return;
    }

    //If Command Set Loaded
    tempEnableCommands = ammoValueCmdsSet;

    if(ammoValueCmdsSet) //If There is Ammo_Value CMDs
    {
        tempAVS = QString::number (ammoValue); //Non-Reaper Light Guns, Including USB HID

        //Replace the %s% with the ammo value (tempAVS)
        for(quint8 i = 0; i < ammoValueCmds.length(); i++)
        {
            cmdString = ammoValueCmds[i];
            if(cmdString.contains (SIGNALDATAVARIBLE))
                cmdString.replace (SIGNALDATAVARIBLE,tempAVS);
            tempCommands << cmdString;
        }
    }

    lastAmmoValue = ammoValue;

    //Return the Commands
    return;
}


void LightGun::AmmoValueReaper(quint16 ammoValue)
{
    QString tempAVS;
    QString cmdString;

    //If Ammo is the same, do nothing
    if(ammoValue == lastAmmoValue)
    {
        tempEnableCommands = false;
        return;
    }
    //Check if Reload Happened
    else if(ammoValue > lastAmmoValue)
    {
        lastAmmoValue = ammoValue;
        tempEnableCommands = reloadCmdsSet;
        reloadAmmoValue = ammoValue;


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
            if(reloadAmmoValue >= reapearLargeAmmoValue)
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
                tempCommands = reloadCmds;
                tempCommands.prepend(REAPERINITLEDS);
                return;
            }
        }

        tempCommands = reloadCmds;
        return;
    }
    else if((lastAmmoValue == 0 && ammoValue == 0) || (lastAmmoValue > ammoCheckValue && ammoValue == 0)) //Boot Up or Closing Game, Do Nothing
    {
        lastAmmoValue = ammoValue;
        tempEnableCommands = false;
        return;
    }
    else if(doAmmoCheck && (lastAmmoValue - ammoValue) != ammoCheckValue)
    {
        //When doAmmoCheck is set, it checks the ammo delta against the ammoCheckValue
        //If not equal, then do reload. In Slow Mode, ammoCheckValue is 2, all other times it is 1
        lastAmmoValue = ammoValue;
        tempEnableCommands = reloadCmdsSet;
        tempCommands = reloadCmds;
        return;
    }

    //If Command Set Loaded
    tempEnableCommands = ammoValueCmdsSet;

    if(ammoValueCmdsSet) //If There is Ammo_Value CMDs
    {
        quint16 tempAV;

        if(ammoValue == 0 && reaperAmmo0Check)
        {
            lastAmmoValue = ammoValue;

            if(enableReaperAmmo0Delay)
            {
                //Start Timer
                p_reaperAmmo0Timer->start();
                isReaperTimerRunning = true;
                //Do Nothing, as after timeout, it will then send Z0 Command, unless reload happens
                tempEnableCommands = false;
                return;
            }
            else if(disableReaperHoldBack)
            {
                //Don't Send Z0 Command to Hold Slide Back, as it is Disabled. Send Z1 Command instead
                tempCommands << REPEARDISABLESLIDECMD;
                tempEnableCommands = true;
                return;
            }
        }


        if(reaperLargeAmmo)
        {
            if(ammoValue == 0)
                tempAV = 0;
            else
            {
                //Use ammoCheckValue as it is 2 in Slow Mode, or 1 normally
                reaperBulletCount = reaperBulletCount + ammoCheckValue;

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


        //Replace the %s% with the ammo value (tempAVS)
        for(quint8 i = 0; i < ammoValueCmds.length(); i++)
        {
            cmdString = ammoValueCmds[i];
            if(cmdString.contains (SIGNALDATAVARIBLE))
                cmdString.replace (SIGNALDATAVARIBLE,tempAVS);
            tempCommands << cmdString;
        }
    }

    lastAmmoValue = ammoValue;

    return;
}


void LightGun::AmmoValueSinden(quint16 ammoValue)
{
    //If Ammo is the same, do nothing
    if(ammoValue == lastAmmoValue)
    {
        tempEnableCommands = false;
        return;
    }
    //Check if Reload Happened
    else if(ammoValue > lastAmmoValue)
    {
        lastAmmoValue = ammoValue;
        tempEnableCommands = reloadCmdsSet;
        reloadAmmoValue = ammoValue;

        if(reloadCmdsSet)
            tempCommands = reloadCmds;

        if(sindenRecoilOverride)
        {
            tempEnableCommands = true;

            if(tcpPlayer == 0)
                tempCommands.prepend(ENABLETRIGGERP1);
            else
                tempCommands.prepend(ENABLETRIGGERP2);
        }

        //qDebug() << "Reload Commands:" << tempSL;

        return;
    }
    else if((lastAmmoValue == 0 && ammoValue == 0) || (lastAmmoValue > ammoCheckValue && ammoValue == 0)) //Boot Up or Closing Game, Do Nothing
    {
        lastAmmoValue = ammoValue;
        tempEnableCommands = false;
        return;
    }
    else if(doAmmoCheck && (lastAmmoValue - ammoValue) != ammoCheckValue)
    {
        //When doAmmoCheck is set, it checks the ammo delta against the ammoCheckValue
        //If not equal, then do reload. In Slow Mode, ammoCheckValue is 2, all other times it is 1
        lastAmmoValue = ammoValue;
        tempEnableCommands = reloadCmdsSet;
        tempCommands = reloadCmds;
        return;
    }

    //If Command Set Loaded
    tempEnableCommands = ammoValueCmdsSet;

    if(ammoValueCmdsSet) //If There is Ammo_Value CMDs
    {
        if(!sindenRecoilOverride)
        {
            if(ammoValue == 0)
                tempCommands = ammoValueCmds + outOfAmmoCmds;
            else
                tempCommands = ammoValueCmds;

            lastAmmoValue = ammoValue;
            return;
        }
        else
        {
            if(ammoValue == 0)
            {
                //Turn Off Trigger Recoil
                tempEnableCommands = true;
                if(tcpPlayer == 0)
                    tempCommands << DISABLETRIGGERP1;
                else
                    tempCommands << DISABLETRIGGERP2;
            }
            else
                tempEnableCommands = false;

            //qDebug() << "Ammo is" << ammoValue << "Cmds is" << tempSL;

            lastAmmoValue = ammoValue;
            return;
        }
    }

    lastAmmoValue = ammoValue;

    //Return the Commands
    return;
}

//Normal Display like OpenFire
void LightGun::DisplayAmmoNormal(quint16 ammoValue)
{
    QString cmdString;
    QStringList tempSL;
    quint16 newAmmoValue;

    //Set Light Guns Ammo Display Value
    ammoDisplayValue = ammoValue;

    if((displayLifePriority && hasDisplayLifeInited && !displayAmmoLife) || (displayOtherPriority && hasDisplayOtherInited))
    {
        //Send False on CMDs, and Send No CMDs, when other displays have priority & have been Display INIted
        tempEnableCommands = false;
        return;
    }

    //If Command Set Loaded
    tempEnableCommands = displayAmmoCmdsSet;

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
        //If OpenFire Ammo is bigger than 99, then make it 99, as it can only Display 2 Digits
        if(ammoValue > 99)
            newAmmoValue = 99;
        else
            newAmmoValue = ammoValue;

        //Replace the %s% with the ammo value (ammoValue)
        for(quint8 i = 0; i < displayAmmoCmds.length(); i++)
        {
            cmdString = displayAmmoCmds[i];
            cmdString.replace (SIGNALDATAVARIBLE,QString::number(newAmmoValue));
            tempSL << cmdString;
        }

    }
    //Return the Commands
    tempCommands = tempSL;
}


//2 Digit Display on Alien and Blamcon Alien
void LightGun::DisplayAmmo2Digit(quint16 ammoValue)
{
    QString tempAVS, ammoValueD1, ammoValueD0;
    QString cmdString;
    QStringList tempSL;
    quint16 newAmmoValue;

    //Set Light Guns Ammo Display Value
    ammoDisplayValue = ammoValue;

    if((displayLifePriority && hasDisplayLifeInited && !displayAmmoLife) || (displayOtherPriority && hasDisplayOtherInited))
    {
        //Send False on CMDs, and Send No CMDs, when other displays have priority & have been Display INIted
        tempEnableCommands = false;
        return;
    }

    //If Command Set Loaded
    tempEnableCommands = displayAmmoCmdsSet;

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
        //If Ammo is bigger than 99, then make it 99, as only 2 Digits
        if(ammoValue > 99)
            newAmmoValue = 99;
        else
            newAmmoValue = ammoValue;

        //For Alien USB Light Gun Ammo Counter which is 2 Digits, so 0-99
        tempAVS = QString::number(newAmmoValue, 10).rightJustified(2, '0');

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        ammoValueD1 = '0'+tempAVS[0];
        ammoValueD0 = '0'+tempAVS[1];
#else
        ammoValueD1 = tempAVS[0];
        ammoValueD0 = tempAVS[1];
        ammoValueD1.prepend ('0');
        ammoValueD0.prepend ('0');
#endif
        //qDebug() << "ammoValueD1: " << ammoValueD1 << " ammoValueD0: " << ammoValueD0 << " tempAVS: " << tempAVS;

        //Replace the %d1% & %d0% with the ammoValueD[1:0]. Also for USB HID %dX% is 2 Hex Values which is 1 Byte
        for(quint8 i = 0; i < displayAmmoCmds.length(); i++)
        {
            cmdString = displayAmmoCmds[i];
            cmdString.replace (DIGIT1,ammoValueD1);
            cmdString.replace (DIGIT0,ammoValueD0);
            tempSL << cmdString;
            //qDebug() << "cmdString: " << cmdString;
        }

    }
    //Return the Commands
    tempCommands = tempSL;
}

//Display Life Normal
void LightGun::DisplayLifeNormal(quint16 lifeValue)
{
    QString cmdString;
    QStringList tempSL;
    quint16 newLifeValue;

    //Set Light Guns Ammo Display Value
    lifeDisplayValue = lifeValue;

    if(lifeValue > lifeBarMaxLife)
        lifeBarMaxLife = lifeValue;


    if((displayAmmoPriority && hasDisplayAmmoInited && !displayAmmoLife) || (displayOtherPriority && hasDisplayOtherInited))
    {
        //Send False on CMDs, and Send No CMDs, when other displays have priority & have been Display INIted
        tempEnableCommands = false;
        return;
    }

    //If Command Set Loaded
    tempEnableCommands = displayLifeCmdsSet;

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
        newLifeValue = lifeValue;

        //Replace the %s% with the ammo value (ammoValue)
        for(quint8 i = 0; i < displayLifeCmds.length(); i++)
        {
            cmdString = displayLifeCmds[i];
            cmdString.replace (SIGNALDATAVARIBLE,QString::number(newLifeValue));
            tempSL << cmdString;
        }
    }
    //Return the Commands
    tempCommands = tempSL;
}

//Display Life for OpenFire
void LightGun::DisplayLifeOpenFire(quint16 lifeValue)
{
    QString cmdString;
    QStringList tempSL;
    quint16 newLifeValue;

    //Set Light Guns Ammo Display Value
    lifeDisplayValue = lifeValue;

    if(lifeValue > lifeBarMaxLife)
        lifeBarMaxLife = lifeValue;


    if((displayAmmoPriority && hasDisplayAmmoInited && !displayAmmoLife) || (displayOtherPriority && hasDisplayOtherInited))
    {
        //Send False on CMDs, and Send No CMDs, when other displays have priority & have been Display INIted
        tempEnableCommands = false;
        return;
    }

    //If Command Set Loaded
    tempEnableCommands = displayLifeCmdsSet;

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
        //For Life Glyphs, Bar, and Number
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

        //Replace the %s% with the ammo value (ammoValue)
        for(quint8 i = 0; i < displayLifeCmds.length(); i++)
        {
            cmdString = displayLifeCmds[i];
            cmdString.replace (SIGNALDATAVARIBLE,QString::number(newLifeValue));
            tempSL << cmdString;
        }
    }
    //Return the Commands
    tempCommands = tempSL;
}

//Display Life for 2 Digit
void LightGun::DisplayLife2Digit(quint16 lifeValue)
{
    QString tempAVS, lifeValueD1, lifeValueD0;
    QString cmdString;
    QStringList tempSL;
    quint16 newLifeValue;

    //Set Light Guns Ammo Display Value
    lifeDisplayValue = lifeValue;

    if(lifeValue > lifeBarMaxLife)
        lifeBarMaxLife = lifeValue;


    if((displayAmmoPriority && hasDisplayAmmoInited && !displayAmmoLife) || (displayOtherPriority && hasDisplayOtherInited))
    {
        //Send False on CMDs, and Send No CMDs, when other displays have priority & have been Display INIted
        tempEnableCommands = false;
        return;
    }

    //If Command Set Loaded
    tempEnableCommands = displayLifeCmdsSet;

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
        //If Life is bigger than 99, then make it 99
        if(lifeValue > 99)
            newLifeValue = 99;
        else
            newLifeValue = lifeValue;

        //For Alien USB Light Gun Ammo Counter which is 2 Digits, so 0-99
        tempAVS = QString::number(newLifeValue, 10).rightJustified(2, '0');

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        lifeValueD1 = '0'+tempAVS[0];
        lifeValueD0 = '0'+tempAVS[1];
#else
        lifeValueD1 = tempAVS[0];
        lifeValueD0 = tempAVS[1];
        lifeValueD1.prepend ('0');
        lifeValueD0.prepend ('0');
#endif

        //qDebug() << "lifeValueD1: " << lifeValueD1 << " lifeValueD0: " << lifeValueD0 << " tempAVS: " << tempAVS;

        //Replace the %d1% & %d0% with the ammoValueD[1:0]. Also for USB HID %dX% is 2 Hex Values which is 1 Byte
        for(quint8 i = 0; i < displayLifeCmds.length(); i++)
        {
            cmdString = displayLifeCmds[i];
            cmdString.replace (DIGIT1,lifeValueD1);
            cmdString.replace (DIGIT0,lifeValueD0);
            tempSL << cmdString;
            //qDebug() << "cmdString: " << cmdString;
        }
    }
    //Return the Commands
    tempCommands = tempSL;
}

//Display Other Normal
void LightGun::DisplayOtherNormal(quint16 otherValue)
{
    QString cmdString;
    QStringList tempSL;
    quint16 newOtherValue;

    //Set Light Guns Ammo Display Value
    otherDisplayValue = otherValue;

    if(!displayOtherPriority)
    {
        //If Display_Other is not Enabled, then do nothing
        tempEnableCommands = false;
        return;
    }

    //If Command Set Loaded
    tempEnableCommands = displayOtherCmdsSet;

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

        //Replace the %s% with the ammo value (ammoValue)
        for(quint8 i = 0; i < displayOtherCmds.length(); i++)
        {
            cmdString = displayOtherCmds[i];
            cmdString.replace (SIGNALDATAVARIBLE,QString::number(newOtherValue));
            tempSL << cmdString;
        }
    }
    //Return the Commands
    tempCommands = tempSL;
}

//Display Other Normal
void LightGun::DisplayOther2Digit(quint16 otherValue)
{
    QString tempAVS, otherValueD1, otherValueD0;
    QString cmdString;
    QStringList tempSL;
    quint16 newOtherValue;

    //Set Light Guns Ammo Display Value
    otherDisplayValue = otherValue;

    if(!displayOtherPriority)
    {
        //If Display_Other is not Enabled, then do nothing
        tempEnableCommands = false;
        return;
    }

    //If Command Set Loaded
    tempEnableCommands = displayOtherCmdsSet;

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

        //For Alien USB Light Gun Ammo Counter which is 2 Digits, so 0-99
        tempAVS = QString::number(newOtherValue, 10).rightJustified(2, '0');

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        otherValueD1 = '0'+tempAVS[0];
        otherValueD0 = '0'+tempAVS[1];
#else
        otherValueD1 = tempAVS[0];
        otherValueD0 = tempAVS[1];
        otherValueD1.prepend ('0');
        otherValueD0.prepend ('0');
#endif
        //qDebug() << "otherValueD1: " << otherValueD1 << " otherValueD0: " << otherValueD0 << " tempAVS: " << tempAVS;

        //Replace the %d1% & %d0% with the ammoValueD[1:0]. Also for USB HID %dX% is 2 Hex Values which is 1 Byte
        for(quint8 i = 0; i < displayOtherCmds.length(); i++)
        {
            cmdString = displayOtherCmds[i];
            cmdString.replace (DIGIT1,otherValueD1);
            cmdString.replace (DIGIT0,otherValueD0);
            tempSL << cmdString;
            //qDebug() << "cmdString: " << cmdString;
        }
    }
    //Return the Commands
    tempCommands = tempSL;
}
