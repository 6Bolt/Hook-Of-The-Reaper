#include "ALEDStripLC.h"

ALEDStripLC::ALEDStripLC(quint8 cntlrNum, quint8 comNum, QString comNm, SerialPortInfo spInfo, quint8 stripCount, QList<quint16> elementsCounts, quint8 pat, QString grpFile, QObject *parent)
    : LightController{cntlrNum, spInfo, parent}
{

    comNumber = comNum;
    comName = comNm;

    comPath = spInfo.path;

    numberStrips = stripCount;
    elementsStripList = elementsCounts;

    aledPattern = pat;

    groupFilePath = grpFile;

    flashCount = 0;
    rndFlashCount = 0;
    sequentialCount = 0;

    //Open COM Port
    isCOMPortOpen = ConnectCOMPort();

    //Set Up ALED Strips
    if(isCOMPortOpen)
        SetUpALEDStrips();

    for(quint8 i = 0; i < MAXNUMALEDSTRIPS; i++)
        displayRangeValue[i] = 0;

    LoadGroupFile();
}

ALEDStripLC::~ALEDStripLC()
{
    //Send Game Ended to be Safe
    GameEnded();

    //Shut off the Pattern
    if(aledPattern != 0)
    {
        QString pattern = SELECTPATTERN + QString::number(0) + ENDCHAR;
        COMWrite(pattern.toUtf8());
    }

    CloseCOMPort();
}

bool ALEDStripLC::ConnectCOMPort()
{

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    std::wstring portName = comPath.toStdWString ();
    comPortLPC = portName.c_str ();
#else
    std::string portName = comPath.toStdString ();
    comPortLPC = portName.c_str ();
#endif

    comPort = CreateFile(comPortLPC, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (comPort == INVALID_HANDLE_VALUE)
    {
        qDebug() << "Failed to Create COM Port File on: " << comPath;
        return false;
    }

    comPortDCB.DCBlength = sizeof(comPortDCB);

    //Get Serial Port Params, and fail handling
    if (!GetCommState(comPort, &comPortDCB))
    {
        COMSTAT status;
        DWORD errors;

        ClearCommError(comPort, &errors, &status);

        QString critMessage = "Can not get the CommState for the Serial COM Port: "+comName+". This is the default settings for the serial port.";

        emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
        return false;
    }

    //Set Baud to 115200
    comPortDCB.BaudRate = CBR_115200;

    //Set Data Bits to 8
    comPortDCB.ByteSize = DATABITS_MAX;

    comPortDCB.StopBits = ONESTOPBIT;
    comPortDCB.Parity = NOPARITY;

    comPortDCB.fOutxCtsFlow = false;
    comPortDCB.fRtsControl = RTS_CONTROL_DISABLE;
    comPortDCB.fOutX = false;
    comPortDCB.fInX = false;

    //Set the Params to the Serial Port, and fail handling
    if (!SetCommState(comPort, &comPortDCB))
    {
        COMSTAT status;
        DWORD errors;

        ClearCommError(comPort, &errors, &status);

        QString critMessage = "Can not set the CommState for the Serial COM Port: "+comName+". This is the settings for the serial port.";
        emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
        return false;
    }

    //Set Time Outs
    //comPortTO.ReadIntervalTimeout = 15;
    //comPortTO.ReadTotalTimeoutConstant = 15;
    //comPortTO.ReadTotalTimeoutMultiplier = 5;
    //comPortTO.WriteTotalTimeoutConstant = 15;
    //comPortTO.WriteTotalTimeoutMultiplier = 2;

    comPortTO.ReadIntervalTimeout = MAXDWORD;
    comPortTO.ReadTotalTimeoutConstant = 0;
    comPortTO.ReadTotalTimeoutMultiplier = 0;
    comPortTO.WriteTotalTimeoutConstant = 0;
    comPortTO.WriteTotalTimeoutMultiplier = 0;

    //Set the Timeouts to the Serial Port, and fail handling
    if (!SetCommTimeouts(comPort, &comPortTO))
    {
        COMSTAT status;
        DWORD errors;

        ClearCommError(comPort, &errors, &status);

        QString critMessage = "Serial COM Port failed to set TimeOuts, on COM Port: "+comName+".";

        emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
        return false;
    }

    return true;
}

bool ALEDStripLC::ReconnectCOMPort()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    std::wstring portName = comPath.toStdWString ();
    comPortLPC = portName.c_str ();
#else
    std::string portName = comPath.toStdString ();
    comPortLPC = portName.c_str ();
#endif

    comPort = CreateFile(comPortLPC, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (comPort == INVALID_HANDLE_VALUE)
    {
        DWORD lastError = GetLastError();
        qDebug() << "Failed on COM Port Handle, error" << lastError;
        return false;
    }

    DCB dcbSerialParam = comPortDCB;
    dcbSerialParam.DCBlength = sizeof(dcbSerialParam);

    //Set the Params to the Serial Port, and fail handling
    if (!SetCommState(comPort, &dcbSerialParam))
    {
        COMSTAT status;
        DWORD errors;
        DWORD lastError = GetLastError();

        ClearCommError(comPort, &errors, &status);

        QString critMessage;
        critMessage = "Can not set the CommState for the Serial COM Port: "+comName+".\nLast error: "+QString::number(lastError)+"\nErrors: "+QString::number(errors);
        emit ShowErrorMessage("Serial COM Port Error",critMessage);
        return false;
    }


    //Set the Times Out for the Serial COM Port

    COMMTIMEOUTS timeout = comPortTO;
    //Set the Timeouts to the Serial Port, and fail handling
    if (!SetCommTimeouts(comPort, &timeout))
    {
        COMSTAT status;
        DWORD errors;
        DWORD lastError = GetLastError();

        ClearCommError(comPort, &errors, &status);

        QString critMessage;

        critMessage = "Serial COM Port failed to set TimeOuts, on COM Port: "+comName+". Please check you Serial COM Port connections.\nError: "+QString::number(errors)+"\nLast error: "+QString::number(lastError);
        emit ShowErrorMessage("Serial COM Port Error",critMessage);
        return false;
    }

    isCOMPortOpen = true;

    return true;
}

void ALEDStripLC::CloseCOMPort()
{
    if(isCOMPortOpen == true)
    {
        CloseHandle(comPort);
        isCOMPortOpen = false;
    }
}



bool ALEDStripLC::COMWrite(QByteArray writeData)
{
    if(isCOMPortOpen == true)
    {
        quint32 size = writeData.size ();
        char* charArray = new char[size + 1];
        std::memcpy(charArray, writeData.constData (), size);
        charArray[size] = '\0';
        quint8 retry = 0;
        DWORD dwWrite = 0;
        bool writeOutput;
        COMSTAT status;
        DWORD errors;
        bool didReconnect = true;

        //QString string = QString::fromUtf8(writeData);
        //qDebug() << "Writing to ALED Strip:" << string;

        writeOutput = WriteFile(comPort, charArray, size, &dwWrite, NULL);

        //If Failed Write, Check if COM Port is Still Open
        if(!writeOutput)
        {
            DWORD lastError = GetLastError();

            //qDebug() << "Last Error" << lastError;

            if(lastError == ERROR_GEN_FAILURE || lastError == ERROR_OPERATION_ABORTED || lastError == ERROR_NO_SUCH_DEVICE || lastError == ERROR_BAD_COMMAND)
            {
                DCB dcb = {0};
                dcb.DCBlength = sizeof(DCB);

                if(!GetCommState(comPort, &dcb))
                {
                    CloseHandle(comPort);
                    isCOMPortOpen = false;

                    didReconnect = ReconnectCOMPort();

                    //qDebug() << "COM Port Reconnect is" << didReconnect;

                    SetUpALEDStrips();
                }
            }
        }

        //If Failed Write, then Retry
        while(!writeOutput && retry != WRITERETRYATTEMPTS && didReconnect)
        {
            //Clean Out Error & Set dwWrite back to 0
            ClearCommError(comPort, &errors, &status);
            dwWrite = 0;

            //Retry Write Again
            writeOutput = WriteFile(comPort, charArray, size, &dwWrite, NULL);

            retry++;
        }

        delete [] charArray;

        if(!didReconnect)
        {
            ClearCommError(comPort, &errors, &status);

            QString critMessage = "Tried to Reconnect to "+comName+", but failed. Please check you Serial COM Port connections. Error: "+QString::number(errors);
            emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
            return false;
        }
        //If Write Failed
        else if (!writeOutput)
        {
            ClearCommError(comPort, &errors, &status);

            QString critMessage = "Serial COM Port write failed on Port: "+comName+". Please check you Serial COM Port connections. Error: "+QString::number(errors);
            emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
            return false;
        }
        //If Size Doesn't Match Byte Written
        else if(dwWrite < size)
        {
            ClearCommError(comPort, &errors, &status);

            QString critMessage = "Serial COM Port write failed on number of bytes written on Port: "+comName+". Please check you Serial COM Port connections. Error: "+QString::number(errors);
            emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
            return false;
        }

        return true;
    }

    return false;
}




QString ALEDStripLC::COMRead(bool *didRead)
{
    QString readData;

    if(isCOMPortOpen == true)
    {
        char readBuffer[65];
        DWORD bytesRead = 0;
        bool success;
        COMSTAT status;
        DWORD errors;

        success = ReadFile(
            comPort,           // The serial port handle
            readBuffer,      // The buffer to receive data
            sizeof(readBuffer) - 1, // Maximum number of bytes to read
            &bytesRead,      // Number of bytes actually read
            NULL             // Use NULL for non-overlapped I/O
            );

        //If Write Failed
        if (!success)
        {
            ClearCommError(comPort, &errors, &status);

            QString critMessage = "Serial COM Port read failed on Port: "+comName+". Please check you Serial COM Port connections. Error: "+QString::number(errors);
            emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
            *didRead = false;
            return readData;
        }

        //qDebug() << "Read Data:" << readData;
        //qDebug() << "Bytes Read:" << bytesRead;

        if (bytesRead > 0)
        {
            readBuffer[bytesRead] = '\0';
            readData = QString::fromUtf8(readBuffer, bytesRead);
            *didRead = true;
            return readData;
        }
        else
        {
            *didRead = false;
            return readData;
        }
    }
    *didRead = false;
    return readData;
}


void ALEDStripLC::SetUpALEDStrips()
{
    quint16 count = 0;

    bool setElements, setPat, setMsgChk, setChk;
    QString readMsg;

    const char msgChk[] = {HOTRCHECK, ENDCHAR, ENDCHAR};
    QByteArray msgChkBA(msgChk, sizeof(msgChk));

    //Send out Check Message to ALED Strip Controller
    setChk = COMWrite(msgChkBA);

    if(setChk)
    {
        QThread::msleep(5);

        //Read Message From ALED Strip Controller
        readMsg = COMRead(&setMsgChk);

        while(!setMsgChk && count < 10)
        {
            readMsg = COMRead(&setMsgChk);
            count++;
            QThread::msleep(5);
        }

        //qDebug() << "Check Message is:" << readMsg;
        //qDebug() << "setMsgChk is:" << setMsgChk << "Count:" << count;

        if(!setMsgChk)
        {
            QString critMessage = "Failed on the Serial Port Write on "+comName+". Couldn't set up ALED Strip controller.";
            emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
        }
    }

    //If Message says Needs Strip Init
    if(readMsg == HOTRCHECKMSGSU)
    {
        //qDebug() << "Setting Up ALED Strings";

        QString numStrings = LEDSTRINGNUMBER + QString::number(numberStrips) + ENDCHAR;

        bool setStrings = COMWrite(numStrings.toUtf8());

        if(setStrings)
        {
            QString elementData;
            for(quint8 i = 0; i < numberStrips; i++)
            {
                QString tempData = LEDSTRINGELEMENTS + QString::number(elementsStripList[i]) + ENDCHAR;
                elementData.append(tempData);
            }

            setElements = COMWrite(elementData.toUtf8());

            if(!setElements)
            {
                QString critMessage = "Failed to Set ALED Strip Elements. Failed on the Serial Port Write on "+comName+".";
                emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
            }
        }

        //Set Light Controller Groups to the Number of Strips
        numberGroups = numberStrips;

        for(quint8 i = 0; i < numberStrips; i++)
        {
            QList<quint8> temp;
            temp << i;
            regularLEDMap.insert(i,temp);
            rgbLEDMap.insert(i,temp);
        }

        //Let ALED Strip Controller Init
        QThread::msleep(10);
    }

    //Sends What Pattern to Use
    if(readMsg == HOTRCHECKMSGSU || readMsg == HOTRCHECKMSGASU)
    {
        //qDebug() << "Setting Up Pattern";

        QString pattern = SELECTPATTERN + QString::number(aledPattern) + ENDCHAR;

        setPat = COMWrite(pattern.toUtf8());

        if(!setPat)
        {
            QString critMessage = "Failed to Set ALED Strip pattern. Failed on the Serial Port Write on "+comName+".";
            emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
        }
    }
    else
    {
        QString critMessage = "Failed to Set ALED Strip numbers. Failed on the Serial Port Write on "+comName+".";
        emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
    }
}

void ALEDStripLC::RedoSetUpALEDStrips()
{
    const char setup[] = {BACKTOSETUP, ENDCHAR, ENDCHAR};
    QByteArray setupDataBA(setup, sizeof(setup));

    bool goSetup = COMWrite(setupDataBA);

    // Wait for Pico2 to Reset
    QThread::msleep(2500);

    bool reconnect = ReconnectCOMPort();

    if(goSetup && reconnect)
        SetUpALEDStrips();
}


bool ALEDStripLC::SetGroupFile(QString filePath)
{
    groupFilePath = filePath;

    rgbFastMode = false;

    noRegularGroups = false;

    noRGBGroups = false;

    didGroupFileLoad = false;

    //Load the Group File
    LoadGroupFile();

    return didGroupFileLoad;
}

void ALEDStripLC::UpdateALEDPattern()
{
    QString pattern = SELECTPATTERN + QString::number(aledPattern) + ENDCHAR;

    bool setPat = COMWrite(pattern.toUtf8());

    if(!setPat)
    {
        QString critMessage = "Failed to Set ALED Strip pattern. Failed on the Serial Port Write on "+comName+".";
        emit ShowErrorMessage("ALED Strip Serial COM Port Error",critMessage);
    }
}



void ALEDStripLC::GameStarted()
{
    const char setup[] = {GAMESTARTSTRIP, ENDCHAR, ENDCHAR};
    QByteArray setupDataBA(setup, sizeof(setup));

    COMWrite(setupDataBA);

    QThread::msleep(10);
}

void ALEDStripLC::GameEnded()
{
    const char setup[] = {GAMEENDED, ENDCHAR, ENDCHAR};
    QByteArray setupDataBA(setup, sizeof(setup));

    COMWrite(setupDataBA);

    QThread::msleep(5);

    //Set Counts back to Zero
    flashCount = 0;
    rndFlashCount = 0;
    sequentialCount = 0;

    //Set Display Range Values to 0
    for(quint8 i = 0; i < MAXNUMALEDSTRIPS; i++)
        displayRangeValue[i] = 0;
}



void ALEDStripLC::SetUpDisplayRange(QList<quint8> stps, quint16 mRange, quint8 numSteps, quint16 tOff, QString cMap, bool enSeqR, quint16 tDelay, quint8 numLEDs)
{
    strips = stps;
    maxRange = mRange;
    numberSteps = numSteps;
    quint16 timeOff = tOff;
    QStringList colors = rgbColorMapMap[cMap];
    quint8 colorIndex = 0;
    quint8 colorMax = colors.count();
    QString sendNoColor, sendColor;

    for(quint8 i = 0; i < strips.count(); i++)
    {
        displayRangeMax[strips[i]] = mRange;

        //Make Data into 1 String
        QString stripData = DISPLAYRANGE + QString::number(strips[i]) + ENDCHAR;
        sendNoColor = stripData;

        QString rangeData = DISPLAYRANGEMAX + QString::number(maxRange) + ENDCHAR;
        sendNoColor.append (rangeData);

        QString stepsData = STEPNUMBERS + QString::number(numberSteps) + ENDCHAR;
        sendNoColor.append (stepsData);

        QString tOffData = TIMEOFF + QString::number(timeOff) + ENDCHAR;
        sendNoColor.append (tOffData);

        QString enSeqReload;
        enSeqReload.append (RELOADSEQ);

        if(enSeqR)
            enSeqReload.append ('1');
        else
            enSeqReload.append ('0');

        enSeqReload.append (ENDCHAR);

        sendNoColor.append (enSeqReload);

        if(enSeqR)
        {
            QString tDData = TIMEON + QString::number(tDelay) + ENDCHAR;
            sendNoColor.append (tDData);

            QString numLEDSData = RELOADNUMLEDS + QString::number(numLEDs) + ENDCHAR;
            sendNoColor.append (numLEDSData);
        }

        //Send Data to ALED Strip Controller
        COMWrite(sendNoColor.toUtf8());

        //Send Color Data
        for(quint8 j = 0; j < numberSteps; j++)
        {
            RGBColor color = rgbColorMap[colors[colorIndex]];

            colorIndex++;
            if(colorIndex >= colorMax)
                colorIndex = 0;

            //Send Red
            QString redData = REDCOLOR + QString::number(color.r) + ENDCHAR;
            sendColor = redData;

            //Send Green
            QString greenData = GREENCOLOR + QString::number(color.g) + ENDCHAR;
            sendColor.append (greenData);

            //Send Blue
            QString blueData = BLUECOLOR + QString::number(color.b) + ENDCHAR;
            sendColor.append (blueData);

            //Send Color Data
            COMWrite(sendColor.toUtf8());
        }

        //Give a little time
        QThread::msleep(5);
    }

}


void ALEDStripLC::UpdateDisplayRange(QList<quint8> stps, quint16 value)
{
    quint16 updateValue;
    bool writeData = false;
    QString updateData;

    //Check the Max Value
    if(value > displayRangeMax[stps[0]])
        updateValue = displayRangeMax[stps[0]];
    else
        updateValue = value;

    if(updateValue != displayRangeValue[stps[0]])
    {
        updateData = UPDATEDISPLAYRANGE + QString::number(stps[0]) + QString::number(updateValue) + ENDCHAR;
        writeData = true;
        displayRangeValue[stps[0]] = updateValue;
    }

    //If there is 2 or more ALED Strips to Update
    if(stps.count() > 1)
    {
        for(quint8 i = 1; i < stps.count(); i++)
        {
            //Check the Max Value
            if(value > displayRangeMax[stps[i]])
                updateValue = displayRangeMax[stps[i]];
            else
                updateValue = value;

            if(updateValue != displayRangeValue[stps[i]])
            {
                updateData.append (UPDATEDISPLAYRANGE + QString::number(stps[i]) + QString::number(updateValue) + ENDCHAR);
                writeData = true;
                displayRangeValue[stps[i]] = updateValue;
            }
        }
    }

    //Write Data to ALED Strip Controller, if value has changed
    if(writeData)
        COMWrite(updateData.toUtf8());
}


void ALEDStripLC::SetUpStripFlash(quint8 structN, quint8 stp, quint16 timeOn, quint16 timeOff, quint8 numFlash, QString color)
{
    if(flashCount == structN)
        flashCount++;
    else
    {
        QString message = "Struct Count is off, for the ALED Strip controller Flash command. Flash Count is "+QString::number(flashCount)+" and structNumber is "+QString::number(structN)+".";
        emit ShowErrorMessage("ALED Strip Controller Error", message);
    }

    QString sendData = FLASHSTRING + QString::number(stp) + ENDCHAR;

    QString tOnData = TIMEON + QString::number(timeOn) + ENDCHAR;
    sendData.append (tOnData);

    QString tOffData = TIMEOFF + QString::number(timeOff) + ENDCHAR;
    sendData.append (tOffData);

    QString numFlData = FLASHTIMES + QString::number(numFlash) + ENDCHAR;
    sendData.append (numFlData);

    RGBColor colorRGB = rgbColorMap[color];

    //Send Red
    QString redData = REDCOLOR + QString::number(colorRGB.r) + ENDCHAR;
    sendData.append (redData);

    //Send Green
    QString greenData = GREENCOLOR + QString::number(colorRGB.g) + ENDCHAR;
    sendData.append (greenData);

    //Send Blue
    QString blueData = BLUECOLOR + QString::number(colorRGB.b) + ENDCHAR;
    sendData.append (blueData);

    //Send Data to ALED Strip Controller
    COMWrite(sendData.toUtf8());

    //Give a little time
    QThread::msleep(5);
}

void ALEDStripLC::DoStripFlash(quint8 structN)
{
    QString flashData = FLASH + QString::number(structN) + ENDCHAR;

    //Send Data to ALED Strip Controller
    COMWrite(flashData.toUtf8());
}

void ALEDStripLC::DoStripFlashWait(quint8 structN)
{
    QString flashWaitData = FLASHWAIT + QString::number(structN) + ENDCHAR;

    //Send Data to ALED Strip Controller
    COMWrite(flashWaitData.toUtf8());
}

void ALEDStripLC::SetUpStripRndFlash(quint8 structN, quint8 stp, quint8 numLEDs, quint16 timeOn, quint16 timeOff, quint8 numFlash, QString color, bool enable2nd, quint8 prob, QString color2)
{
    if(rndFlashCount == structN)
        rndFlashCount++;
    else
    {
        QString message = "Struct Count is off, for the ALED Strip controller Random Flash command. Random Flash Count is "+QString::number(rndFlashCount)+" and structNumber is "+QString::number(structN)+".";
        emit ShowErrorMessage("ALED Strip Controller Error", message);
    }

    QString sendData = RNDFLASHSTRING + QString::number(stp) + ENDCHAR;

    QString numLEDData = RNDFLASHNUMLED + QString::number(numLEDs-1) + ENDCHAR;
    sendData.append (numLEDData);

    QString tOnData = TIMEON + QString::number(timeOn) + ENDCHAR;
    sendData.append (tOnData);

    QString tOffData = TIMEOFF + QString::number(timeOff) + ENDCHAR;
    sendData.append (tOffData);

    QString numFlData = FLASHTIMES + QString::number(numFlash) + ENDCHAR;
    sendData.append (numFlData);

    if(enable2nd)
    {
        QString en2ndC = RNDFLASH2NDCOLOR + QString::number(1) + ENDCHAR;
        sendData.append (en2ndC);

        quint8 probability = prob - 2;
        QString prob2ndC = RNDFLSH2NDNUMBER + QString::number(probability) + ENDCHAR;
        sendData.append (prob2ndC);

        RGBColor color2RGB = rgbColorMap[color2];

        QString redData2 = REDCOLOR + QString::number(color2RGB.r) + ENDCHAR;
        sendData.append (redData2);

        QString greenData2 = GREENCOLOR + QString::number(color2RGB.g) + ENDCHAR;
        sendData.append (greenData2);

        QString blueData2 = BLUECOLOR + QString::number(color2RGB.b) + ENDCHAR;
        sendData.append (blueData2);
    }


    RGBColor colorRGB = rgbColorMap[color];

    //Send Red
    QString redData = REDCOLOR + QString::number(colorRGB.r) + ENDCHAR;
    sendData.append (redData);

    //Send Green
    QString greenData = GREENCOLOR + QString::number(colorRGB.g) + ENDCHAR;
    sendData.append (greenData);

    //Send Blue
    QString blueData = BLUECOLOR + QString::number(colorRGB.b) + ENDCHAR;
    sendData.append (blueData);

    //Send Data to ALED Strip Controller
    COMWrite(sendData.toUtf8());

    //Give a little time
    QThread::msleep(5);
}

void ALEDStripLC::DoStripRndFlash(quint8 structN)
{
    QString flashRndData = RNDFLASH + QString::number(structN) + ENDCHAR;

    //Send Data to ALED Strip Controller
    COMWrite(flashRndData.toUtf8());
}

void ALEDStripLC::SetUpStripSequential(quint8 structN, quint8 stp, quint16 timeDelay, QString color, quint8 numLEDs)
{
    if(sequentialCount == structN)
        sequentialCount++;
    else
    {
        QString message = "Struct Count is off, for the ALED Strip controller Sequential command. Sequential Count is "+QString::number(sequentialCount)+" and structNumber is "+QString::number(structN)+".";
        emit ShowErrorMessage("ALED Strip Controller Error", message);
    }

    QString sendData = SEQUENTIALSTRING + QString::number(stp) + ENDCHAR;

    QString tDelayData = TIMEOFF + QString::number(timeDelay) + ENDCHAR;
    sendData.append (tDelayData);

    QString numLEDSData = RELOADNUMLEDS + QString::number(numLEDs) + ENDCHAR;
    sendData.append (numLEDSData);

    RGBColor colorRGB = rgbColorMap[color];

    //Send Red
    QString redData = REDCOLOR + QString::number(colorRGB.r) + ENDCHAR;
    sendData.append (redData);

    //Send Green
    QString greenData = GREENCOLOR + QString::number(colorRGB.g) + ENDCHAR;
    sendData.append (greenData);

    //Send Blue
    QString blueData = BLUECOLOR + QString::number(colorRGB.b) + ENDCHAR;
    sendData.append (blueData);

    //Send Data to ALED Strip Controller
    COMWrite(sendData.toUtf8());

    //Give a little time
    QThread::msleep(8);
}

void ALEDStripLC::DoStripSequential(quint8 structN)
{
    QString seqData = SEQUENTIAL + QString::number(structN) + ENDCHAR;

    //Send Data to ALED Strip Controller
    COMWrite(seqData.toUtf8());
}
