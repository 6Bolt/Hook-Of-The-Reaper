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

            if(type >= PACDRIVE && type <= IPACULTIMATEIO)
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
                else if(type == NANOLED || type == PACLED64)
                    numberGroups.insert(i, OTHERGRPS);
                else
                    numberGroups.insert(i, SMALLGROUPS);

                QList<quint8> stateData;

                for(j = 0; j < numberGroups[i]; j++)
                {
                    if(j == 7 && type == NANOLED)
                        stateData.insert(j, 0x0F);
                    else
                        stateData.insert(j, 0xFF);
                }

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

        if(dataUltimarc[id].type >= NANOLED && dataUltimarc[id].type <= IPACULTIMATEIO)
        {
            PBYTE intensityData = new BYTE[pins];

            for(i = 0; i < pins; i++)
                intensityData[i] = 0;

            //Turn States On
            for(i = 0; i < numberGroups[id]; i++)
            {
                if(i == 7 && dataUltimarc[id].type == NANOLED)
                    Pac64SetLEDStates(id, i+1, 0x0F);
                else
                    Pac64SetLEDStates(id, i+1, 0xFF);
            }

            //Turn Intensity for all Pins Off
            Pac64SetLEDIntensities(id,intensityData);

            delete[] intensityData;
        }
        else if(dataUltimarc[id].type >= PACDRIVE && dataUltimarc[id].type <= BLUEHID)
        {
            //Turn Off the 16 States
            PacSetLEDStates(id, 0);

            groupStateData[id][0] = 0;
            groupStateData[id][1] = 0;
        }
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
                    QString msg = "Write to Ultimarc light controller with ID: "+QString::number(id)+" failed after 3 write attampts with Pac64SetLEDIntensity. Something is wrong with the hardware. Please check device and USB path.";
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
        bool writePass, stateChk;
        quint8 group = (pin >> 3);
        quint8 port = pin & 0x07;
        quint8 pinData = (1 << port);
        quint8 dataChk = groupStateData[id][group] & pinData;
        //INT groupW = group + 1;
        //INT portW = port;
        //INT idW = id;

        if(dataChk == 0)
            stateChk = false;
        else
            stateChk = true;

        //qDebug() << "stateChk" << stateChk << "State" << state << "dataChk" << dataChk;

        if(stateChk != state)
        {
            quint8 writeCount = 0;

            //qDebug() << "pin" << pin << "group" << group << "port" << port;

            do
            {
                writePass = Pac64SetLEDState(id, group+1, port, state);
                writeCount++;
            } while(!writePass && writeCount < WRITERETRYATTEMPTS+1);

            if(writePass)
            {
                if(state)
                    groupStateData[id][group] |= pinData;
                else
                    groupStateData[id][group] &= ~(pinData);

                //qDebug() << "groupStateData[id][group]" << groupStateData[id][group];
            }
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


void PacDriveControl::SetPinStates(quint8 id, quint8 group, quint8 groupData, bool all)
{
    if(dataUltimarc[id].valid)
    {
        INT useGroup;
        BYTE data = groupData;
        bool writePass;
        quint8 writeCount = 0;

        if(all)
            useGroup = 0;
        else
            useGroup = group + 1;

        if(groupStateData[id][group] != groupData || all)
        {
            //qDebug() << "useGroup" << useGroup << "data" << data << "groupStateData[id][group]" << groupStateData[id][group];

            do
            {
                writePass = Pac64SetLEDStates(id, useGroup, data);
                writeCount++;
            } while(!writePass && writeCount < WRITERETRYATTEMPTS+1);

            if(writePass)
            {
                if(all)
                {
                    quint8 i;
                    for(i = 0; i < numberGroups[id]; i++)
                        groupStateData[id][i] = groupData;
                }
                else
                    groupStateData[id][group] = groupData;
            }
            else
            {
                //qDebug() << "Failed writePass" << writePass << "writeCount" << writeCount;
                QString title = "Write to Ultimarc Light Controller Failed";
                QString msg = "Write to Ultimarc light controller with ID: "+QString::number(id)+" failed after 3 write attampts using Pac64SetLEDStates. Something is wrong with the hardware. Please check device and USB path.";
                emit ShowErrorMessage(title, msg);
            }
        }
    }
}


void PacDriveControl::SetPACLEDStates(quint8 id, quint16 data)
{
    if(dataUltimarc[id].valid)
    {
        USHORT dataState = data;
        bool writePass;
        quint8 writeCount = 0;
        quint16 tempHigh = data & 0xFF00;
        quint8 high = (tempHigh >> 8);
        quint8 low = data & 0x00FF;

        if(groupStateData[id][0] != low || groupStateData[id][1] != high)
        {
            do
            {
                writePass = PacSetLEDStates(id, dataState);
                writeCount++;
            } while(!writePass && writeCount < WRITERETRYATTEMPTS+1);

            if(writePass)
            {
                groupStateData[id][0] = low;
                groupStateData[id][1] = high;

                //qDebug() << "groupStateData[id][0]" << groupStateData[id][0] << "groupStateData[id][1]" << groupStateData[id][1];
            }
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


void PacDriveControl::SetPACLEDState(quint8 id, quint8 pin, bool state)
{
    if(dataUltimarc[id].valid)
    {
        bool writePass, stateChk;
        quint8 pinData;
        quint8 dataChk;
        quint8 group = 0;

        //Check State Data
        if(pin > 7)
        {
            group = 1;
            pinData = (1 << (pin-8));
        }
        else
            pinData = (1 << pin);

        dataChk = groupStateData[id][group] & pinData;

        if(dataChk == 0)
            stateChk = false;
        else
            stateChk = true;

        if(stateChk != state)
        {
            quint8 writeCount = 0;

            //qDebug() << "pin" << pin << "group" << group << "port" << port;

            do
            {
                writePass = PacSetLEDState(id, pin, state);
                writeCount++;
            } while(!writePass && writeCount < WRITERETRYATTEMPTS+1);

            if(writePass)
            {
                if(state)
                    groupStateData[id][group] |= pinData;
                else
                    groupStateData[id][group] &= ~(pinData);

                //qDebug() << "groupStateData[id][group]" << groupStateData[id][group];
            }
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













