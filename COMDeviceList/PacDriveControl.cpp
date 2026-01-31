#include "PacDriveControl.h"
#include <qdebug.h>


PacDriveControl::PacDriveControl(QObject *parent)
    : QObject{parent}
{
    devicesInList = 0;

    //Init Ultimarc Light Controllers and Find How Many They Are
    numberUltimarcDevices = PacInitialize();

    //Collect Ultimarc Data
    CollectUltimarcData();

    //Turn Off All Lights
    //TurnOffAllLights();

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
        quint8 index = 0;
        PWCHAR buffer = new wchar_t[256];
        numberUltimarcDevicesValid = 0;

        for(i = 0; i < numberUltimarcDevices; i++)
        {
            bool invalid = false;

            quint8 type = PacGetDeviceType(i);

            if(type >= NANOLED && type <= IPACULTIMATEIO)
            {

                //2 IDs used, first is postition in list, Second is what is Programmed to
                //The position in the list ID is the important one
                dataUltimarc[i].id = i;

                dataUltimarc[i].type = type;

                dataUltimarc[i].typeName = ULTIMARCTYPENAME[dataUltimarc[i].type];

                dataUltimarc[i].vendorID = PacGetVendorId(i);
                dataUltimarc[i].productID = PacGetProductId(i);

                dataUltimarc[i].vendorIDS = QString("0x%1").arg( dataUltimarc[i].vendorID, 4, 16, QChar('0')).toUpper();
                dataUltimarc[i].productIDS = QString("0x%1").arg(dataUltimarc[i].productID, 4, 16, QChar('0')).toUpper();

                dataUltimarc[i].version = PacGetVersionNumber(i);
                dataUltimarc[i].versionS = QString::number(dataUltimarc[i].version);

                PacGetVendorName(i, buffer);
                dataUltimarc[i].vendorName = QString::fromWCharArray(buffer);

                if(dataUltimarc[i].vendorName.size() > 255)
                    invalid = true;

                PacGetProductName(i, buffer);
                dataUltimarc[i].productName = QString::fromWCharArray(buffer);

                if(dataUltimarc[i].productName.size() > 255)
                    invalid = true;

                PacGetSerialNumber(i, buffer);
                dataUltimarc[i].serialNumber = QString::fromWCharArray(buffer);

                if(dataUltimarc[i].serialNumber.size() > 255)
                    invalid = true;

                PacGetDevicePath(i, buffer);
                dataUltimarc[i].devicePath = QString::fromWCharArray(buffer);

                if(dataUltimarc[i].devicePath.size() > 255)
                    invalid = true;

                dataUltimarc[i].groupFile = "";

                //Get Light Controller ID
                if(dataUltimarc[i].type == PACDRIVE)
                    dataUltimarc[i].deviceID = dataUltimarc[i].version;
                else if(dataUltimarc[i].type == UHID)
                    dataUltimarc[i].deviceID = dataUltimarc[i].productID - UHID_LOW;
                else if(dataUltimarc[i].type == NANOLED)
                    dataUltimarc[i].deviceID = dataUltimarc[i].productID - NANOLED_LOW;
                else if(dataUltimarc[i].type == PACLED64)
                    dataUltimarc[i].deviceID = dataUltimarc[i].productID - PACLED64_LOW;
                else if(dataUltimarc[i].type == IPACULTIMATEIO)
                    dataUltimarc[i].deviceID = dataUltimarc[i].productID - IPACULTIMATEIO_LOW;
                else if(dataUltimarc[i].type == BLUEHID)
                    dataUltimarc[i].deviceID = 69;
                else
                    dataUltimarc[i].deviceID = 69;

                if(dataUltimarc[i].deviceID > 7)
                    invalid = true;

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

                if(type == IPACULTIMATEIO)
                    numberGroups.insert(i, ULTIMATEGRPS);
                else
                    numberGroups.insert(i, OTHERGRPS);

                QList<quint8> stateData;

                for(j = 0; j < numberGroups[i]; j++)
                    stateData.insert(j, 0xFF);

                groupStateData.insert(i, stateData);

                if(!invalid)
                {
                    dataUltimarc[i].valid = true;
                    numberUltimarcDevicesValid++;
                }

                index++;
            }
            else
                dataUltimarc[i].valid = false;
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
        quint8 i, j, pins;

        for(i = 0; i < numberUltimarcDevices; i++)
        {

            if(dataUltimarc[i].type >= NANOLED && dataUltimarc[i].type <= IPACULTIMATEIO && dataUltimarc[i].valid)
            {
                pins = numberPins[i];

                PBYTE intensityData = new BYTE[pins];

                for(j = 0; j < pins; j++)
                    intensityData[j] = 0;

                //Turn States On
                for(j = 0; j < numberGroups[i]; j++)
                {
                    if(j == 7 && dataUltimarc[i].type == NANOLED)
                        Pac64SetLEDStates(dataUltimarc[i].id, j, 0x0F);
                    else
                        Pac64SetLEDStates(dataUltimarc[i].id, j, 0xFF);
                }

                //qDebug() << "Turning Off the Lights";
                //Turn Intensity to 0
                Pac64SetLEDIntensities(dataUltimarc[i].id,intensityData);

                delete[] intensityData;
            }
            //else
            //{
            //    PacSetLEDStates(dataUltimarc[i].id,0);
            //}
        }


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


void PacDriveControl::TurnOffLights(quint8 id)
{
    if(dataUltimarc[id].valid)
    {
        quint8 i, pins;

        pins = numberPins[id];

        PBYTE intensityData = new BYTE[pins];

        for(i = 0; i < pins; i++)
            intensityData[i] = 0;

        //Turn States On
        for(i = 0; i < numberGroups[id]; i++)
        {
            if(i == 7 && dataUltimarc[id].type == NANOLED)
                Pac64SetLEDStates(id, i, 0x0F);
            else
                Pac64SetLEDStates(id, i, 0xFF);
        }

        //Turn Intensity for all Pins Off
        Pac64SetLEDIntensities(id,intensityData);

        delete[] intensityData;
    }
}


bool PacDriveControl::CheckLoadedUltimarcDevice(UltimarcData dataU)
{
    if(numberUltimarcDevices == 0)
        return false;
    else
    {
        quint8 i;

        for(i = 0; i < numberUltimarcDevices; i++)
        {
            if(dataUltimarc[i] == dataU && !dataUltimarc[i].inList && dataUltimarc[i].valid)
            {
                dataUltimarc[i].inList = true;
                devicesInList++;
                return true;
            }
        }
    }

    //Didn't Find Device
    return false;
}

void PacDriveControl::DeletedFromList(UltimarcData dataU)
{
    if(numberUltimarcDevices == 0)
        return;
    else
    {
        quint8 i;

        for(i = 0; i < numberUltimarcDevices; i++)
        {
            if(dataUltimarc[i] == dataU && dataUltimarc[i].inList)
            {
                dataUltimarc[i].inList = false;
                devicesInList--;
                return;
            }
        }
    }
}




// Public Slots


void PacDriveControl::SetLightIntensity(quint8 id, quint8 pin, quint8 intensity)
{
    if(dataUltimarc[id].valid)
    {
        if(lightIntensityMap[id][pin] != intensity)
        {
            bool writePass;
            quint8 writeCount = 0;

            do
            {
                writePass = Pac64SetLEDIntensity(id, pin, intensity);
                writeCount++;
            } while(!writePass && writeCount < WRITERETRYATTEMPTS+1);

            if(writePass)
            {
                lightIntensityMap[id][pin] = intensity;
            }
            else
            {
                QString title = "Write to Ultimarc Light Controller Failed";
                QString msg = "Write to Ultimarc light controller with ID: "+QString::number(id)+" failed after 3 write attampts. Something is wrong with the hardware. Please check device and USB path.";
                emit ShowErrorMessage(title, msg);
            }
        }
    }
    else
    {
        QString title = "Invalid Ultimarc Light Controller ID";
        QString msg = "Invalid Ultimarc ID: "+QString::number(id)+". The ID is not found in the valid Ultimarc light controllers.";
        emit ShowErrorMessage(title, msg);
    }
}

void PacDriveControl::SetLightIntensityGroup(quint8 id, QList<quint8> group, quint8 intensity)
{
    if(dataUltimarc[id].valid)
    {
        quint8 i, pin;

        for(i = 0; i < group.count(); i++)
        {
            pin = group[i]-1;

            if(lightIntensityMap[id][pin] != intensity)
            {
                bool writePass;
                quint8 writeCount = 0;

                do
                {
                    writePass = Pac64SetLEDIntensity(id, pin, intensity);
                    writeCount++;
                } while(!writePass && writeCount < WRITERETRYATTEMPTS+1);

                if(writePass)
                {
                    lightIntensityMap[id][pin] = intensity;
                }
                else
                {
                    QString title = "Write to Ultimarc Light Controller Failed";
                    QString msg = "Write to Ultimarc light controller with ID: "+QString::number(id)+" failed after 3 write attampts. Something is wrong with the hardware. Please check device and USB path.";
                    emit ShowErrorMessage(title, msg);
                }
            }
        }
    }
    else
    {
        QString title = "Invalid Ultimarc Light Controller ID";
        QString msg = "Invalid Ultimarc ID: "+QString::number(id)+". The ID is not found in the valid Ultimarc light controllers.";
        emit ShowErrorMessage(title, msg);
    }

}



void PacDriveControl::SetRGBLightIntensity(const quint8 &id, const RGBPins &pins, const RGBColor &color)
{
    if(dataUltimarc[id].valid)
    {
        bool writePass;
        quint8 writeCount = 0;

        //qDebug() << "Setting RGB Intensity for ID:" << id << "Red pin" << pins.r;

        if(lightIntensityMap[id][pins.r] != color.r)
        {
            do
            {
                writePass = Pac64SetLEDIntensity(id, pins.r, color.r);
                writeCount++;
            } while(!writePass && writeCount < WRITERETRYATTEMPTS+1);

            if(writePass)
            {
                lightIntensityMap[id][pins.r] = color.r;
            }
            else
            {
                QString title = "Write to Ultimarc Light Controller Failed";
                QString msg = "Write to Ultimarc light controller with ID: "+QString::number(id)+" failed after 3 write attampts. Something is wrong with the hardware. Please check device and USB path.";
                emit ShowErrorMessage(title, msg);
            }
        }

        if(lightIntensityMap[id][pins.g] != color.g)
        {
            writeCount = 0;

            do
            {
                writePass = Pac64SetLEDIntensity(id, pins.g, color.g);
                writeCount++;
            } while(!writePass && writeCount < WRITERETRYATTEMPTS+1);

            if(writePass)
            {
                lightIntensityMap[id][pins.g] = color.g;
            }
            else
            {
                QString title = "Write to Ultimarc Light Controller Failed";
                QString msg = "Write to Ultimarc light controller with ID: "+QString::number(id)+" failed after 3 write attampts. Something is wrong with the hardware. Please check device and USB path.";
                emit ShowErrorMessage(title, msg);
            }
        }

        if(lightIntensityMap[id][pins.b] != color.b)
        {
            writeCount = 0;

            do
            {
                writePass = Pac64SetLEDIntensity(id, pins.b, color.b);
                writeCount++;
            } while(!writePass && writeCount < WRITERETRYATTEMPTS+1);

            if(writePass)
            {
                lightIntensityMap[id][pins.b] = color.b;
            }
            else
            {
                QString title = "Write to Ultimarc Light Controller Failed";
                QString msg = "Write to Ultimarc light controller with ID: "+QString::number(id)+" failed after 3 write attampts. Something is wrong with the hardware. Please check device and USB path.";
                emit ShowErrorMessage(title, msg);
            }
        }
    }
    else
    {
        QString title = "Invalid Ultimarc Light Controller ID";
        QString msg = "Invalid Ultimarc ID: "+QString::number(id)+". The ID is not found in the valid Ultimarc light controllers.";
        emit ShowErrorMessage(title, msg);
    }
}



void PacDriveControl::SetPinState(quint8 id, quint8 pin, bool state)
{
    if(dataUltimarc[id].valid)
    {
        //qDebug() << "lightStateMap[id][pin]" << lightStateMap[id][pin];

        if(lightStateMap[id][pin] != state)
        {
            bool writePass;
            quint8 writeCount = 0;
            quint8 group = pin/8;
            quint8 port = pin - (group*8);
            group++;

            //qDebug() << "pin" << pin << "group" << group << "port" << port;

            do
            {
                writePass = Pac64SetLEDState(id, group, port, state);
                writeCount++;
            } while(!writePass && writeCount < WRITERETRYATTEMPTS+1);

            if(writePass)
                lightStateMap[id][pin] = state;
            else
            {
                //qDebug() << "Failed writePass" << writePass << "writeCount" << writeCount;
                QString title = "Write to Ultimarc Light Controller Failed";
                QString msg = "Write to Ultimarc light controller with ID: "+QString::number(id)+" failed after 3 write attampts. Something is wrong with the hardware. Please check device and USB path.";
                emit ShowErrorMessage(title, msg);
            }
        }
    }
}








