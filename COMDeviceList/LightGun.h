#ifndef LIGHTGUN_H
#define LIGHTGUN_H


#include <QSerialPortInfo>

#include <QObject>
//#include <QDebug>

class LightGun
{
public:
    //Constructors
    LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow, quint16 maNumber, quint16 rvNumber);
    LightGun(bool lgDefault, quint8 dlgNum, QString lgName, quint8 lgNumber, quint8 cpNumber, QString cpString, QSerialPortInfo cpInfo, qint32 cpBaud, quint8 cpDataBits, quint8 cpParity, quint8 cpStopBits, quint8 cpFlow);
    LightGun(LightGun const &lgMember);


    //Set Functions that Sets the Stated Variable
    void SetDefaultLightGun(bool lgDefault);
    void SetDefaultLightGunNumer(quint8 dlgNum);
    void SetLightGunName(QString lgnString);
    void SetLightGunNumber(quint8 lgNumber);
    void SetComPortNumber(quint8 cpNumber);
    void SetComPortString(QString cpString);
    void SetComPortInfo(QSerialPortInfo cpInfo);
    void SetComPortBaud(qint32 cpBaud);
    void SetComPortDataBits(quint8 cpDataBits);
    void SetComPortParity(quint8 cpParity);
    void SetComPortStopBits(quint8 cpStopBits);
    void SetComPortFlow(quint8 cpFlow);
    void SetMaxAmmo(quint8 maNumber);
    void SetReloadValue(quint8 rvNumber);

    //Get Functions that Gets the Stated Variable
    bool GetDefaultLightGun();
    quint8 GetDefaultLightGunNumber();
    QString GetLightGunName();
    quint8 GetLightGunNumber();
    quint8 GetComPortNumber();
    QString GetComPortString();
    QSerialPortInfo GetComPortInfo();
    qint32 GetComPortBaud();
    quint8 GetComPortDataBits();
    quint8 GetComPortParity();
    quint8 GetComPortStopBits();
    quint8 GetComPortFlow();
    quint8 GetMaxAmmo();
    quint8 GetReloadValue();

    //If a Default Light Gun, is Needed Varibles Set
    bool IsMaxAmmoSet();
    bool IsReloadValueSet();


    //Copies a Light Gun - Used when Deleting a Light Gun
    //To move higher up Light guns Down 1
    void CopyLightGun(LightGun const &lgMember);



    //Sends Out the Needed Commands for a Default Light Gun
    //Used In the Hooker Engine
    QStringList OpenComPortCommands();
    QStringList CloseComPortCommands();
    QStringList DamageCommands();
    QStringList RecoilCommands();
    QStringList ReloadCommands();
    QStringList AmmoCommands();
    QStringList AmmoValueCommands(quint16 ammoValue);
    QStringList ShakeCommands();
    QStringList AutoLEDCommands();
    QStringList AspectRatio16b9Commands();
    QStringList AspectRatio4b3Commands();
    QStringList JoystickModeCommands();
    QStringList MouseAndKeyboardModeCommands();


private:


    ///////////////////////////////////////////////////////////////////////////

    //Default Light Gun
    bool                defaultLightGun;
    quint8              defaultLightGunNum;
    quint16             maxAmmo;
    quint16             reloadValue;
    bool                maxAmmoSet;
    bool                reloadValueSet;

    //Light Gun Name & Number
    quint8              lightGunNum;
    QString             lightGunName;

    //COM Port Info
    quint8              comPortNum;
    QString             comPortString;
    QSerialPortInfo     comPortInfo;
    qint32              comPortBaud;
    quint8              comPortDataBits;
    quint8              comPortParity;
    quint8              comPortStopBits;
    quint8              comPortFlow;



};

#endif // LIGHTGUN_H
