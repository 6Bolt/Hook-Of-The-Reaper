#include "HookCOMPortWin.h"

HookCOMPortWin::HookCOMPortWin(QObject *parent)
    : QObject{parent}
{
    numPortOpen = 0;
    isPortOpen = false;

    for(quint8 i = 0; i < MAXCOMPORTS; i++)
    {
        comPortOpen[i] = false;
        comPortArray[i] = nullptr;
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
}


//public slots

 void HookCOMPortWin::Connect(const quint8 &comPortNum, const QString &comPortName, const qint32 &comPortBaud, const quint8 &comPortData, const quint8 &comPortParity, const quint8 &comPortStop, const quint8 &comPortFlow, const bool &isWriteOnly)
{
    //Check if it is Already Open, if so, do nothing
    if(comPortOpen[comPortNum] == false)
    {

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

        if (comPortArray[comPortNum] == INVALID_HANDLE_VALUE)
        {
            COMSTAT status;
            DWORD errors;

            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                          GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPWSTR)&messageBuffer, 1020, NULL);

            if (GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                // serial port not found. Handle error here.
                QString critMessage = "Can not open the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". COM port not found. Make sure a light gun or a COM device is on that port number.";
                emit ErrorMessage("Serial COM Port Error",critMessage);
                return;
            }
            // any other error. Handle error here.
            QString critMessage = "Can not open the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". Unknown error. Make sure a light gun or a COM device is on that port number. "+QString::fromWCharArray(messageBuffer);
            emit ErrorMessage("Serial COM Port Error",critMessage);
            return;
        }

        //Set COM Port Params

        DCB dcbSerialParam = {0};
        dcbSerialParam.DCBlength = sizeof(dcbSerialParam);

        if (!GetCommState(comPortArray[comPortNum], &dcbSerialParam))
        {
            COMSTAT status;
            DWORD errors;

            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                          GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPWSTR)&messageBuffer, 1020, NULL);

            ClearCommError(comPortArray[comPortNum], &errors, &status);

            QString critMessage = "Can not get the CommState for the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". This is the default settings for the serial port. "+QString::fromWCharArray(messageBuffer);
            emit ErrorMessage("Serial COM Port Error",critMessage);
            return;
        }

        if(comPortBaud == 115200)
            dcbSerialParam.BaudRate = CBR_115200;
        else if(comPortBaud == 9600)
            dcbSerialParam.BaudRate = CBR_9600;

        dcbSerialParam.ByteSize = comPortData;

        if(comPortStop == 1)
            dcbSerialParam.StopBits = ONESTOPBIT;
        else if(comPortStop == 2)
            dcbSerialParam.StopBits = TWOSTOPBITS;
        else
            dcbSerialParam.StopBits = ONE5STOPBITS;

        if(comPortParity == 0)
            dcbSerialParam.Parity = NOPARITY;
        else if(comPortParity == 2)
            dcbSerialParam.Parity = EVENPARITY;
        else if(comPortParity == 3)
            dcbSerialParam.Parity = ODDPARITY;


        // setup flowcontrol
        if (comPortFlow == 0)  //None
        {
            dcbSerialParam.fOutxCtsFlow = false;
            dcbSerialParam.fRtsControl = RTS_CONTROL_DISABLE;
            dcbSerialParam.fOutX = false;
            dcbSerialParam.fInX = false;
        }
        else if (comPortFlow == 2)  //Software
        {
            dcbSerialParam.fOutxCtsFlow = false;
            dcbSerialParam.fRtsControl = RTS_CONTROL_DISABLE;
            dcbSerialParam.fOutX = true;
            dcbSerialParam.fInX = true;
        }
        else if (comPortFlow == 1)  //Hardware
        {
            dcbSerialParam.fOutxCtsFlow = true;
            dcbSerialParam.fRtsControl = RTS_CONTROL_HANDSHAKE;
            dcbSerialParam.fOutX = false;
            dcbSerialParam.fInX = false;
        }



        if (!SetCommState(comPortArray[comPortNum], &dcbSerialParam))
        {
            COMSTAT status;
            DWORD errors;

            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                          GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPWSTR)&messageBuffer, 1020, NULL);

            ClearCommError(comPortArray[comPortNum], &errors, &status);

            QString critMessage = "Can not set the CommState for the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". This is the settings for the serial port. "+QString::fromWCharArray(messageBuffer);
            emit ErrorMessage("Serial COM Port Error",critMessage);
            return;
        }


        //Set the Times Out for the COM Port

        COMMTIMEOUTS timeout = {0};
        timeout.ReadIntervalTimeout = 60;
        timeout.ReadTotalTimeoutConstant = 60;
        timeout.ReadTotalTimeoutMultiplier = 15;
        timeout.WriteTotalTimeoutConstant = 60;
        timeout.WriteTotalTimeoutMultiplier = 8;
        if (!SetCommTimeouts(comPortArray[comPortNum], &timeout))
        {
            COMSTAT status;
            DWORD errors;

            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                          GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPWSTR)&messageBuffer, 1020, NULL);

            ClearCommError(comPortArray[comPortNum], &errors, &status);

            QString critMessage = "Can not set the CommTimeouts for the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". This is the settings for the serial port. "+QString::fromWCharArray(messageBuffer);
            emit ErrorMessage("Serial COM Port Error",critMessage);
            return;
        }


        //Done, COM Port is Set-Up

        comPortOpen[comPortNum] = true;
        numPortOpen++;
        isPortOpen = true;

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

        DWORD dwWrite = 0;
        if (!WriteFile(comPortArray[comPortNum], charArray, size, &dwWrite, NULL))
        {
            COMSTAT status;
            DWORD errors;

            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                          GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPWSTR)&messageBuffer, 1020, NULL);

            ClearCommError(comPortArray[comPortNum], &errors, &status);

            QString critMessage = "Serial COM Port write failed on Port: "+QString::number(comPortNum)+". Please check you Serial COM Port connections. Error: "+QString::number(errors)+" "+QString::fromWCharArray(messageBuffer);
            emit ErrorMessage("Serial COM Port Error",critMessage);
            return;
        }

        if(size != dwWrite)
        {
            COMSTAT status;
            DWORD errors;

            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                          GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPWSTR)&messageBuffer, 1020, NULL);

            ClearCommError(comPortArray[comPortNum], &errors, &status);

            QString critMessage = "Serial COM Port write still has data to write on Port: "+QString::number(comPortNum)+". Size is "+QString::number(size)+" Bytes Written is "+QString::number(dwWrite)+". Error: "+QString::number(errors)+" "+QString::fromWCharArray(messageBuffer);
            emit ErrorMessage("Serial COM Port Error",critMessage);
        }

        delete [] charArray;

    }

}


void HookCOMPortWin::DisconnectAll()
{
    if(isPortOpen)
    {
        //Close All Connections
        for(quint8 i = 0; i < MAXCOMPORTS; i++)
        {
            if(comPortOpen[i])
            {
                this->Disconnect(i);
            }
        }
        isPortOpen = false;
    }
}
