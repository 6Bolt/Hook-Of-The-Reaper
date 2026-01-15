#include "PacDriveControl.h"
#include <qdebug.h>


PacDriveControl::PacDriveControl(QObject *parent)
    : QObject{parent}
{
    //Init Ultimarc Light Controllers and Find How Many They Are
    numberUltimarcDevices = PacInitialize();

    //Collect Ultimarc Data
    CollectUltimarcData();

    //Turn Off All Lights
    TurnOffAllLights();

}



PacDriveControl::~PacDriveControl()
{
    //Shut Down Ultimarc Light Controllers
    PacShutdown();
}




void PacDriveControl::CollectUltimarcData()
{
    if(numberUltimarcDevices > 0)
    {
        //Varibles
        quint8 i, j;
        PWCHAR buffer = new wchar_t[256];

        for(i = 0; i < numberUltimarcDevices; i++)
        {
            dataUltimarc[i].type = PacGetDeviceType(i);
            dataUltimarc[i].typeName = ULTIMARCTYPENAME[dataUltimarc[i].type];

            dataUltimarc[i].vendorID = PacGetVendorId(i);
            dataUltimarc[i].productID = PacGetProductId(i);

            dataUltimarc[i].vendorIDS = QString("0x%1").arg( dataUltimarc[i].vendorID, 4, 16, QChar('0')).toUpper();
            dataUltimarc[i].productIDS = QString("0x%1").arg(dataUltimarc[i].productID, 4, 16, QChar('0')).toUpper();

            dataUltimarc[i].version = PacGetVersionNumber(i);
            dataUltimarc[i].versionS = QString::number(dataUltimarc[i].version);

            PacGetVendorName(i, buffer);
            dataUltimarc[i].vendorName = QString::fromWCharArray(buffer);

            PacGetProductName(i, buffer);
            dataUltimarc[i].productName = QString::fromWCharArray(buffer);

            PacGetSerialNumber(i, buffer);
            dataUltimarc[i].serialNumber = QString::fromWCharArray(buffer);

            PacGetDevicePath(i, buffer);
            dataUltimarc[i].devicePath = QString::fromWCharArray(buffer);

            dataUltimarc[i].groupFile = "";

            //Get Light Controller ID
            if(dataUltimarc[i].type == PACDRIVE)
                dataUltimarc[i].id = dataUltimarc[i].version;
            else if(dataUltimarc[i].type == UHID)
                dataUltimarc[i].id = dataUltimarc[i].productID - UHID_LOW;
            else if(dataUltimarc[i].type == NANOLED)
                dataUltimarc[i].id = dataUltimarc[i].productID - NANOLED_LOW;
            else if(dataUltimarc[i].type == PACLED64)
                dataUltimarc[i].id = dataUltimarc[i].productID - PACLED64_LOW;
            else if(dataUltimarc[i].type == IPACULTIMATEIO)
                dataUltimarc[i].id = dataUltimarc[i].productID - IPACULTIMATEIO_LOW;
            else if(dataUltimarc[i].type == BLUEHID)
                dataUltimarc[i].id = 0;
            else
                dataUltimarc[i].id = 69;

            //Get Number of Pins
            numberPins.insert (i,ULTIMARCTYPELEDCOUNT[dataUltimarc[i].type]);

            //Set Pins State and Intensity to 0, for a Known State and Intensity
            QList<bool> states;
            QList<quint8> intensity;

            for(j = 0; j < numberPins[i]; j++)
            {
                //states << false;
                states << true;
                intensity << 0;
            }

            lightStateMap.insert(i,states);
            lightIntensityMap.insert(i,intensity);
        }

        delete[] buffer;
    }
}


UltimarcData PacDriveControl::GetUltimarcData(quint8 index)
{
    if(index < numberUltimarcDevices)
        return dataUltimarc[index];
    else
    {
        UltimarcData temp;
        return temp;
    }
}


void PacDriveControl::TurnOffAllLights()
{
    //Turn Off Lights and Set Intensity to 0
    if(numberUltimarcDevices > 0)
    {
        quint8 i;
        PBYTE intensityData = new BYTE[64];

        for(i = 0; i < 63; i++)
            intensityData[i] = 0;

        for(i = 0; i < numberUltimarcDevices; i++)
        {
            if(dataUltimarc[i].type >= NANOLED && dataUltimarc[i].type <= IPACULTIMATEIO)
            {
                //qDebug() << "Turning Off the Lights";
                //Turn States on but Intensity to 0
                Pac64SetLEDStates(i, 0, 255);
                Pac64SetLEDIntensities(i,intensityData);
            }
            else
            {
                PacSetLEDStates(i,0);
            }
        }

        delete[] intensityData;
    }
}

quint8 PacDriveControl::GetIDFromDevicePath(QString devicePath)
{
    if(numberUltimarcDevices > 0)
    {
        quint8 i;

        for(i = 0; i < numberUltimarcDevices; i++)
        {
            if(dataUltimarc[i].devicePath == devicePath)
                return dataUltimarc[i].id;
        }
    }

    //Cound't Find Device
    return 255;
}

// Public Slots


void PacDriveControl::SetLightIntensity(quint8 id, quint8 pin, quint8 intensity)
{
    if(lightIntensityMap[id][pin] != intensity)
    {
        Pac64SetLEDIntensity(id, pin, intensity);
        lightIntensityMap[id][pin] = intensity;

        if(intensity > 0)
            lightStateMap[id][pin] = true;
        else
            lightStateMap[id][pin] = false;
    }
}

void PacDriveControl::SetRGBLightIntensity(const quint8 &id, const RGBPins &pins, const RGBColor &color)
{
    //qDebug() << "Setting RGB Intensity for ID:" << id;

    if(lightIntensityMap[id][pins.r] != color.r)
    {
        Pac64SetLEDIntensity(id, pins.r, color.r);
        lightIntensityMap[id][pins.r] = color.r;

        if(color.r > 0)
            lightStateMap[id][pins.r] = true;
        else
            lightStateMap[id][pins.r] = false;
    }

    if(lightIntensityMap[id][pins.g] != color.g)
    {
        Pac64SetLEDIntensity(id, pins.g, color.g);
        lightIntensityMap[id][pins.g] = color.g;

        if(color.g > 0)
            lightStateMap[id][pins.g] = true;
        else
            lightStateMap[id][pins.g] = false;
    }

    if(lightIntensityMap[id][pins.b] != color.b)
    {
        Pac64SetLEDIntensity(id, pins.b, color.b);
        lightIntensityMap[id][pins.b] = color.b;

        if(color.b > 0)
            lightStateMap[id][pins.b] = true;
        else
            lightStateMap[id][pins.b] = false;
    }
}
