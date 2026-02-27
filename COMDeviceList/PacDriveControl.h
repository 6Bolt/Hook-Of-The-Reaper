#ifndef PACDRIVECONTROL_H
#define PACDRIVECONTROL_H

#include <QObject>
#include <QList>
#include <QMap>



#include "../Global.h"

//Ultimarc SDK
#include "StdAfx.h"
#include "Windows.h"
#include "PacDrive.h"


class PacDriveControl : public QObject
{
        Q_OBJECT

public:

    explicit PacDriveControl(QObject *parent = nullptr);

    ~PacDriveControl();

    //Collect Ultimarc Data
    void CollectUltimarcData();

    //Get Ultimarc Data
    UltimarcData GetUltimarcData(quint8 index);

    //Get Number of Ultimarc Devices
    qint8 GetNumberUltimarcDevices() { return numberUltimarcDevices; }

    //Get Number of Ultimarc Devices in List
    quint8 GetNumberDevicesInList() { return devicesInList; }

    //Turns Off All Lights
    void TurnOffAllLights();

    quint8 GetIDFromDevicePath(QString devicePath);

    void TurnOffLights(quint8 id);

    bool CheckLoadedUltimarcDevice(UltimarcData dataU);

    void DeletedFromList(UltimarcData dataU);

public slots:

    void SetLightIntensity(quint8 id, quint8 pin, quint8 intensity);

    void SetLightIntensityGroup(quint8 id, QList<quint8> group, quint8 intensity);

    void SetRGBLightIntensity(const quint8 &id, const RGBPins &pins, const RGBColor &color);

    //Set Pin State for Pac64
    void SetPinState(quint8 id, quint8 pin, bool state);

    //Set Group States for Pac64
    void SetPinStates(quint8 id, quint8 group, quint8 groupData, bool all);

    //Set Pin States for PacDrive
    void SetPACLEDStates(quint8 id, quint16 data);

    //Set Pin State for PacDrive
    void SetPACLEDState(quint8 id, quint8 pin, bool state);



signals:

    //Show Error Message Box in Main Thread
    void ShowErrorMessage(const QString &title, const QString &message);



public:

    //Number of Ultimac Devices that are Valid
    qint8                       numberUltimarcDevicesValid;

    //Number of Ultrimarc Devices Filtered
    qint8                       numberUltimarcDevices;

    quint8                      devicesInList;

    //Ultimarc Device Data
    UltimarcData                dataUltimarc[ULTIMARCMAXDEVICES];

    bool                        noControllers;

    QList<quint8>               numberPins;

    QMap<quint8,QList<bool>>    lightStateMap;

    QMap<quint8,QList<quint8>>  lightIntensityMap;

    QList<quint8>               numberGroups;

    QMap<quint8,QList<quint8>>  groupStateData;



};

#endif // PACDRIVECONTROL_H
