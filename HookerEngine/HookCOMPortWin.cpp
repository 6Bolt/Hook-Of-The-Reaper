#include "HookCOMPortWin.h"

HookCOMPortWin::HookCOMPortWin(QObject *parent)
    : QObject{parent}
{
    numPortOpen = 0;
    isPortOpen = false;
    bypassCOMPortConnectFailWarning = false;

    for(quint8 i = 0; i < MAXCOMPORTS; i++)
    {
        comPortOpen[i] = false;
        comPortArray[i] = nullptr;
        noLightGunWarning[i] = false;
    }

    //Init the USB HID
    if (hid_init())
    {
        QString critMessage = "The USB HID Init failed. Something is really wrong, or got fucked up";
        emit ErrorMessage("USB HID Init Failed",critMessage);
    }

    for(quint8 i = 0; i < MAXGAMEPLAYERS; i++)
    {
        //p_hidConnection[i] = nullptr;
        hidOpen[i] = false;
    }
}


//Deconstructor
HookCOMPortWin::~HookCOMPortWin()
{
    //Close and Delete Serial COM Ports
    for(quint8 i = 0; i < MAXCOMPORTS; i++)
    {
        if(comPortOpen[i])
           this->Disconnect(i);
    }

    //Close All Open USB HID Connections
    for(quint8 i = 0; i < MAXGAMEPLAYERS; i++)
    {
        if(hidOpen[i])
        {
            hid_close(p_hidConnection[i]);
            hidOpen[i] = false;
        }
    }

    //Exit the USB HID
    hid_exit ();

}


//public slots

 void HookCOMPortWin::Connect(const quint8 &comPortNum, const QString &comPortName, const qint32 &comPortBaud, const quint8 &comPortData, const quint8 &comPortParity, const quint8 &comPortStop, const quint8 &comPortFlow, const QString &comPortPath, const bool &isWriteOnly)
{
    //Check if it is Already Open, if so, do nothing
    if(comPortOpen[comPortNum] == false)
    {
        //qDebug() << "Connecting to Serial Port" << comPortNum;
        //Create COM Port

        //Check if comPortNum matches the comPortName ending Number
        if(!comPortName.endsWith (QString::number (comPortNum)))
        {
            QString critMessage = "Serial COM Port Name Number: "+comPortName+" doesn't match COM Port Number: "+QString::number(comPortNum)+". Something got fucked up.";
            emit ErrorMessage("Serial COM Port Error",critMessage);
            return;
        }

        std::wstring portName = comPortName.toStdWString ();
        LPCWSTR portNameLPC = portName.c_str ();

        if(isWriteOnly)
            comPortArray[comPortNum] = CreateFile(portNameLPC, GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        else
            comPortArray[comPortNum] = CreateFile(portNameLPC, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


        //Use Old COM Port Naming, for people who cannot update their drivers. Not naming anyone
        if (comPortArray[comPortNum] == INVALID_HANDLE_VALUE)
        {
            portName = comPortPath.toStdWString ();
            portNameLPC = portName.c_str ();

            if(isWriteOnly)
                comPortArray[comPortNum] = CreateFile(portNameLPC, GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
            else
                comPortArray[comPortNum] = CreateFile(portNameLPC, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        }

        //If Serial COM Port Connection Failed
        if (comPortArray[comPortNum] == INVALID_HANDLE_VALUE)
        {
            COMSTAT status;
            DWORD errors;
            quint16 lastError = GetLastError();

            //qDebug() << "lastError: " << lastError;
            //qDebug() << "comPortArray[comPortNum]: " << comPortArray[comPortNum] << "GetLastError(): " << lastError;

            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                          GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPWSTR)&messageBuffer, 1020, NULL);

            if(!bypassCOMPortConnectFailWarning)
            {

                if ((lastError == ERROR_FILE_NOT_FOUND || lastError == ERROR_PATH_NOT_FOUND) && !noLightGunWarning[comPortNum])
                {
                    // serial port not found. Handle error here.
                    QString critMessage = "Can not open the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". COM port not found. Make sure a light gun is on that port number.\n Error MSG: "+QString::number(lastError);
                    critMessage.append ("\nThis message will only happen once, but will try to connect to light gun for every game.");
                    emit ErrorMessage("Serial COM Port Error",critMessage);
                    noLightGunWarning[comPortNum] = true;
                    return;
                }
                else if((lastError == ERROR_FILE_NOT_FOUND || lastError == ERROR_PATH_NOT_FOUND) && noLightGunWarning[comPortNum])
                {
                    //Do nothing, as already gave warning. Light Gun may not be plugged in.
                    return;
                }
                else
                {
                    QString critMessage = "Can not open the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". Uknown error. Error Number: "+QString::number(lastError);
                    emit ErrorMessage("Serial COM Port Error",critMessage);
                    return;
                }
            }
        }
        else
        {
            //Set COM Port Params, if Connection Didn't Fail

            DCB dcbSerialParam = {0};
            dcbSerialParam.DCBlength = sizeof(dcbSerialParam);

            //Get Serial Port Params, and fail handling
            if (!GetCommState(comPortArray[comPortNum], &dcbSerialParam))
            {
                COMSTAT status;
                DWORD errors;

                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                              GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              (LPWSTR)&messageBuffer, 1020, NULL);

                ClearCommError(comPortArray[comPortNum], &errors, &status);

                QString critMessage;

                if(messageBuffer == nullptr)
                    critMessage = "Can not get the CommState for the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". This is the default settings for the serial port.";
                else
                    critMessage = "Can not get the CommState for the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". This is the default settings for the serial port. "+QString::fromWCharArray(messageBuffer);

                emit ErrorMessage("Serial COM Port Error",critMessage);
                return;
            }

            //Setting Params Based on Old Qt Values

            //Baud Rates
            //115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200

            if(comPortBaud == 115200)
                dcbSerialParam.BaudRate = CBR_115200;
            else if(comPortBaud == 9600)
                dcbSerialParam.BaudRate = CBR_9600;
            else if(comPortBaud == 57600)
                dcbSerialParam.BaudRate = CBR_57600;
            else if(comPortBaud == 38400)
                dcbSerialParam.BaudRate = CBR_38400;
            else if(comPortBaud == 19200)
                dcbSerialParam.BaudRate = CBR_19200;
            else if(comPortBaud == 4800)
                dcbSerialParam.BaudRate = CBR_4800;
            else if(comPortBaud == 2400)
                dcbSerialParam.BaudRate = CBR_2400;
            else if(comPortBaud == 1200)
                dcbSerialParam.BaudRate = CBR_1200;
            else
                dcbSerialParam.BaudRate = CBR_115200;  //Default to 115200 as it is most common

            if(comPortData >= DATABITS_MIN && comPortData <= DATABITS_MAX)
                dcbSerialParam.ByteSize = comPortData;
            else
                dcbSerialParam.ByteSize = DATABITS_MAX; //Default to 8, as it is most common

            if(comPortStop == 1)
                dcbSerialParam.StopBits = ONESTOPBIT;
            else if(comPortStop == 2)
                dcbSerialParam.StopBits = TWOSTOPBITS;
            else if(comPortStop == 3)
                dcbSerialParam.StopBits = ONE5STOPBITS;
            else
                dcbSerialParam.StopBits = ONESTOPBIT;  //Default to 1, as it is most common

            if(comPortParity == 0)
                dcbSerialParam.Parity = NOPARITY;
            else if(comPortParity == 2)
                dcbSerialParam.Parity = EVENPARITY;
            else if(comPortParity == 3)
                dcbSerialParam.Parity = ODDPARITY;
            else if(comPortParity == 4)
                dcbSerialParam.Parity = SPACEPARITY;
            else if(comPortParity == 5)
                dcbSerialParam.Parity = MARKPARITY;
            else
                dcbSerialParam.Parity = NOPARITY;  //Default to No Parity, as it is most common


            // setup flowcontrol
            if (comPortFlow == 0)  //None - based on Qt Numbering
            {
                dcbSerialParam.fOutxCtsFlow = false;
                dcbSerialParam.fRtsControl = RTS_CONTROL_DISABLE;
                dcbSerialParam.fOutX = false;
                dcbSerialParam.fInX = false;
            }
            else if (comPortFlow == 2)  //Software - based on Qt Numbering
            {
                dcbSerialParam.fOutxCtsFlow = false;
                dcbSerialParam.fRtsControl = RTS_CONTROL_DISABLE;
                dcbSerialParam.fOutX = true;
                dcbSerialParam.fInX = true;
            }
            else if (comPortFlow == 1)  //Hardware - based on Qt Numbering
            {
                dcbSerialParam.fOutxCtsFlow = true;
                dcbSerialParam.fRtsControl = RTS_CONTROL_HANDSHAKE;
                dcbSerialParam.fOutX = false;
                dcbSerialParam.fInX = false;
            }


            //Set the Params to the Serial Port, and fail handling
            if (!SetCommState(comPortArray[comPortNum], &dcbSerialParam))
            {
                COMSTAT status;
                DWORD errors;

                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                              GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              (LPWSTR)&messageBuffer, 1020, NULL);

                ClearCommError(comPortArray[comPortNum], &errors, &status);

                QString critMessage;

                if(messageBuffer == nullptr)
                    critMessage = "Can not set the CommState for the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". This is the settings for the serial port. ";
                else
                    critMessage = "Can not set the CommState for the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". This is the settings for the serial port. "+QString::fromWCharArray(messageBuffer);
                emit ErrorMessage("Serial COM Port Error",critMessage);
                return;
            }


            //Set the Times Out for the Serial COM Port

            COMMTIMEOUTS timeout = {0};
            //Safe
            //timeout.ReadIntervalTimeout = 60;
            //timeout.ReadTotalTimeoutConstant = 60;
            //timeout.ReadTotalTimeoutMultiplier = 15;
            //timeout.WriteTotalTimeoutConstant = 60;
            //timeout.WriteTotalTimeoutMultiplier = 8;

            //Works Good So Far
            //timeout.ReadIntervalTimeout = 15;
            //timeout.ReadTotalTimeoutConstant = 15;
            //timeout.ReadTotalTimeoutMultiplier = 5;
            //timeout.WriteTotalTimeoutConstant = 15;
            //timeout.WriteTotalTimeoutMultiplier = 2;

            //Max Out
            timeout.ReadIntervalTimeout = MAXDWORD;
            timeout.ReadTotalTimeoutConstant = 0;
            timeout.ReadTotalTimeoutMultiplier = 0;
            timeout.WriteTotalTimeoutConstant = 0;
            timeout.WriteTotalTimeoutMultiplier = 0;

            //Set the Timeouts to the Serial Port, and fail handling
            if (!SetCommTimeouts(comPortArray[comPortNum], &timeout))
            {
                COMSTAT status;
                DWORD errors;

                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                              GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              (LPWSTR)&messageBuffer, 1020, NULL);

                ClearCommError(comPortArray[comPortNum], &errors, &status);

                QString critMessage;

                if(messageBuffer == nullptr)
                    critMessage = "Serial COM Port failed to set TimeOuts, on COM Port: "+QString::number(comPortNum)+". Please check you Serial COM Port connections. Error: "+QString::number(errors);
                else
                    critMessage = "Serial COM Port failed to set TimeOuts, on COM Port: : "+QString::number(comPortNum)+". Please check you Serial COM Port connections. Error: "+QString::number(errors)+" "+QString::fromWCharArray(messageBuffer);
                emit ErrorMessage("Serial COM Port Error",critMessage);
                return;
            }


            //Done, COM Port is Set-Up, Set Connection bools

            comPortOpen[comPortNum] = true;
            numPortOpen++;
            isPortOpen = true;


        }
    }

}



void HookCOMPortWin::Disconnect(const quint8 &comPortNum)
{
    //Check if Port is Open
    if(comPortOpen[comPortNum] == true)
    {
        CloseHandle(comPortArray[comPortNum]);
        comPortOpen[comPortNum] = false;

        if(numPortOpen > 0)
            numPortOpen--;

        if(numPortOpen == 0)
            isPortOpen = false;
    }
}


void HookCOMPortWin::WriteData(const quint8 &comPortNum, const QByteArray &writeData)
{
    //Check if Port is Open
    if(comPortOpen[comPortNum] == true)
    {
        quint32 size = writeData.size ();
        char* charArray = new char[size + 1];
        //strcpy_s(charArray, size+1, writeData.constData ());
        std::memcpy(charArray, writeData.constData (), size);
        charArray[size] = '\0';
        quint8 retry = 0;
        DWORD dwWrite = 0;
        qint16 writeOutput;
        COMSTAT status;
        DWORD errors;

        //qDebug() << "COM Port: " << comPortNum << " Data: " << writeData.toStdString ();

        writeOutput = WriteFile(comPortArray[comPortNum], charArray, size, &dwWrite, NULL);

        //If Failed Write, then Retry
        while(writeOutput == 0 && retry != WRITERETRYATTEMPTS)
        {
            //Clean Out Error & Set dwWrite back to 0
            ClearCommError(comPortArray[comPortNum], &errors, &status);
            dwWrite = 0;

            //Retry Write Again
            writeOutput = WriteFile(comPortArray[comPortNum], charArray, size, &dwWrite, NULL);

            retry++;
        }

        //Bypass Errors if bypassSerialWriteChecks is True
        if(!bypassSerialWriteChecks)
        {
            //If Write Failed
            if (writeOutput == 0)
            {
                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                              GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              (LPWSTR)&messageBuffer, 1020, NULL);

                ClearCommError(comPortArray[comPortNum], &errors, &status);

                QString critMessage;

                if(messageBuffer == nullptr)
                    critMessage = "Serial COM Port write failed on Port: "+QString::number(comPortNum)+". Please check you Serial COM Port connections. Error: "+QString::number(errors);
                else
                    critMessage = "Serial COM Port write failed on Port: "+QString::number(comPortNum)+". Please check you Serial COM Port connections. Error: "+QString::number(errors)+" "+QString::fromWCharArray(messageBuffer);
                emit ErrorMessage("Serial COM Port Error",critMessage);
                delete [] charArray;
                return;
            }

            //If Size Doesn't Match Byte Written
            if(size != dwWrite)
            {
                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                              GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              (LPWSTR)&messageBuffer, 1020, NULL);

                ClearCommError(comPortArray[comPortNum], &errors, &status);

                QString critMessage;

                if(messageBuffer == nullptr)
                    critMessage = "Serial COM Port write failed on number of bytes written on Port: "+QString::number(comPortNum)+". Please check you Serial COM Port connections. Error: "+QString::number(errors);
                else
                    critMessage = "Serial COM Port write failed on number of bytes written on Port: "+QString::number(comPortNum)+". Please check you Serial COM Port connections. Error: "+QString::number(errors)+" "+QString::fromWCharArray(messageBuffer);
                emit ErrorMessage("Serial COM Port Error",critMessage);
            }
        }

        delete [] charArray;
    }
}


void HookCOMPortWin::DisconnectAll()
{
    //Close Down Serial Port Connections
    if(isPortOpen)
    {
        //Close All Connections
        for(quint8 i = 0; i < MAXCOMPORTS; i++)
        {
            if(comPortOpen[i])
            {
                CloseHandle(comPortArray[i]);
                comPortOpen[i] = false;
                //this->Disconnect(i);
            }
        }
        isPortOpen = false;
        numPortOpen = 0;
    }

    //Close All Open USB HID Connections
    for(quint8 i = 0; i < MAXGAMEPLAYERS; i++)
    {
        if(hidOpen[i])
        {
            hid_close(p_hidConnection[i]);
            hidOpen[i] = false;
        }
    }
}


void HookCOMPortWin::ConnectHID(const quint8 &playerNum, const HIDInfo &lgHIDInfo)
{
    //Check if Connection is Already Made for USB HID
    if(!hidOpen[playerNum])
    {
        //qDebug() << "PlayerNumber: " << playerNum;
        //qDebug() << "Path: " << lgHIDInfo.path;

        hidInfoArray[playerNum] = lgHIDInfo;

        QByteArray pathBA = lgHIDInfo.path.toUtf8();

        char* pathPtr = pathBA.data();

        p_hidConnection[playerNum] = hid_open_path(pathPtr);

        if(!p_hidConnection[playerNum])
        {
            if(!bypassCOMPortConnectFailWarning)
            {
                //Failed to Open USB HID
                const wchar_t* errorWChar = hid_error(p_hidConnection[playerNum]);
                QString errorMSG = QString::fromWCharArray(errorWChar);
                QString critMessage = "The USB HID failed to connect for player: "+QString::number(playerNum+1)+"\nError Message: "+errorMSG;
                emit ErrorMessage("USB HID Failed to Open",critMessage);
            }
        }
        else
        {
            //Connection Made, Ready to Go
            hidOpen[playerNum] = true;
        }
    }
}


void HookCOMPortWin::DisconnectHID(const quint8 &playerNum)
{
    //Check if it is Open
    if(hidOpen[playerNum])
    {
        hid_close(p_hidConnection[playerNum]);
        hidOpen[playerNum] = false;
    }
}


void HookCOMPortWin::WriteDataHID(const quint8 &playerNum, const QByteArray &writeData)
{
    //Check if it is Open
    if(hidOpen[playerNum])
    {
        //qDebug() << "USB HID Write - playerNum: " << QString::number(playerNum) << " writeData: " << writeData.toHex ();

        const unsigned char *constDataPtr = reinterpret_cast<const unsigned char*>(writeData.constData());
        std::size_t size = writeData.size();
        quint8 retry = 0;
        qint16 bytesWritten = hid_write(p_hidConnection[playerNum], constDataPtr, size);

        while(bytesWritten == -1 && retry != WRITERETRYATTEMPTS)
        {
            bytesWritten = hid_write(p_hidConnection[playerNum], constDataPtr, size);
            retry++;
        }


        if(bytesWritten == -1)
        {
            //Write Failed
            const wchar_t* errorWChar = hid_error(p_hidConnection[playerNum]);
            QString errorMSG = QString::fromWCharArray(errorWChar);
            QString critMessage = "The USB HID failed to write data.\nError Message: "+errorMSG;
            emit ErrorMessage("USB HID Write Failed",critMessage);
        }

    }
}


void HookCOMPortWin::SetBypassSerialWriteChecks(const bool &bypassSPWC)
{
    bypassSerialWriteChecks = bypassSPWC;
}

void HookCOMPortWin::SetBypassCOMPortConnectFailWarning(const bool &bypassCPCFW)
{
    bypassCOMPortConnectFailWarning = bypassCPCFW;
}
