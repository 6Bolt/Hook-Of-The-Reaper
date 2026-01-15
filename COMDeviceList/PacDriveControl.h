#ifndef PACDRIVECONTROL_H
#define PACDRIVECONTROL_H

#include <QObject>
#include <QList>
#include <QMap>



#include "../Global.h"

//Ultimarc SDK
//#include "StdAfx.h"
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
    quint8 GetNumberUltimarcDevices() { return numberUltimarcDevices; }

    //Turns Off All Lights
    void TurnOffAllLights();

    quint8 GetIDFromDevicePath(QString devicePath);

public slots:

    void SetLightIntensity(quint8 id, quint8 pin, quint8 intensity);

    void SetRGBLightIntensity(const quint8 &id, const RGBPins &pins, const RGBColor &color);



public:

    //Number of Ultrimarc Devices
    qint8                       numberUltimarcDevices;

    //Ultimarc Device Data
    UltimarcData                dataUltimarc[ULTIMARCMAXDEVICES];

    bool                        noControllers;

    QList<quint8>               numberPins;

    QMap<quint8,QList<bool>>    lightStateMap;

    QMap<quint8,QList<quint8>>  lightIntensityMap;


};

#endif // PACDRIVECONTROL_H
