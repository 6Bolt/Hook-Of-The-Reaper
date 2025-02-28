#include "HookCOMPort.h"
#include "../Global.h"

//Constructor
HookCOMPort::HookCOMPort(QObject *parent)
    : QObject{parent}
{
    //qDebug() << "HookCOMPort Started";

    numPortOpen = 0;
    isPortOpen = false;

    for(quint8 i = 0; i < MAXCOMPORTS; i++)
    {
        p_ComPortArray[i] = nullptr;
        comPortOpen[i] = false;
    }


}

//Deconstructor
HookCOMPort::~HookCOMPort()
{
    //Close and Delete Serial COM Ports
    for(quint8 i = 0; i < MAXCOMPORTS; i++)
    {
        if(comPortOpen[i])
            p_ComPortArray[i]->close ();

        if(p_ComPortArray[i] != nullptr)
            delete p_ComPortArray[i];
    }
}


//public slots

void HookCOMPort::Connect(const quint8 &comPortNum, const QString &comPortName, const qint32 &comPortBaud, const quint8 &comPortData, const quint8 &comPortParity, const quint8 &comPortStop, const quint8 &comPortFlow, const bool &isWriteOnly)
{

    //qDebug() << "Creating a New Serial Com Port at: " << comPortNum << " With the name of: " << comPortName;

    //Check if it is Already Open, if so, do nothing
    if(comPortOpen[comPortNum] == false)
    {
        bool isOpen;


        p_ComPortArray[comPortNum] = new QSerialPort(comPortName);

        if(isWriteOnly)
            p_ComPortArray[comPortNum]->setBaudRate ((QSerialPort::BaudRate) comPortBaud, QSerialPort::Output);
        else
            p_ComPortArray[comPortNum]->setBaudRate ((QSerialPort::BaudRate) comPortBaud);

        p_ComPortArray[comPortNum]->setDataBits ((QSerialPort::DataBits) comPortData);
        p_ComPortArray[comPortNum]->setParity ((QSerialPort::Parity) comPortParity);
        p_ComPortArray[comPortNum]->setStopBits ((QSerialPort::StopBits) comPortStop);
        p_ComPortArray[comPortNum]->setFlowControl ((QSerialPort::FlowControl) comPortFlow);

        QSerialPortInfo newPortInfo(comPortName);
        p_ComPortArray[comPortNum]->setPort (newPortInfo);

        if(isWriteOnly)
            isOpen = p_ComPortArray[comPortNum]->open(QIODevice::WriteOnly);
        else
            isOpen = p_ComPortArray[comPortNum]->open(QIODevice::ReadWrite);

        if(!isOpen)
        {
            //If Failed to Open COM Port
            QSerialPort::SerialPortError portError = p_ComPortArray[comPortNum]->error();
            QString critMessage = "Can not open the Serial COM Port: "+comPortName+" on Port: "+QString::number(comPortNum)+". Make sure a light gun or a COM device is on that port number. Serial Port Error: "+QString::number(portError);
            emit ErrorMessage("Serial COM Port Error",critMessage);
            //qDebug() << critMessage;
            return;
        }
        else
        {
            //If Open COM Port
            comPortOpen[comPortNum] = true;

            if(!isWriteOnly)
                connect(p_ComPortArray[comPortNum],SIGNAL(readyRead()),this,SLOT(ReadData()));

            numPortOpen++;
            isPortOpen = true;
        }

    }
    //qDebug() << "Done with Connecting Port";

}

void HookCOMPort::Disconnect(const quint8 &comPortNum)
{
    if(comPortOpen[comPortNum])
    {


        if(p_ComPortArray[comPortNum]->bytesToWrite () > 0)
            p_ComPortArray[comPortNum]->flush ();

        //bool writeDone = p_ComPortArray[comPortNum]->waitForBytesWritten (COMPORTWAITFORWRITE);

        //qDebug() << "Closing COM Port #" << comPortNum << " writeDone: " << writeDone;

        p_ComPortArray[comPortNum]->close ();

        //delete p_ComPortArray[comPortNum];
        //p_ComPortArray[comPortNum] = nullptr;

        comPortOpen[comPortNum] = false;

        if(numPortOpen > 0)
            numPortOpen--;

        if(numPortOpen == 0)
            isPortOpen = false;
    }

}

void HookCOMPort::WriteData(const quint8 &comPortNum, const QByteArray &writeData)
{
    qint64 bytesWritten;
    bool writeDone = false;

    if(comPortOpen[comPortNum])
    {


        //qDebug() << "Port is Open: " << p_ComPortArray[comPortNum]->isOpen() << " and Port Number is: " << comPortNum;

        bytesWritten = p_ComPortArray[comPortNum]->write(writeData);

        //qDebug() << "bytesWritten is : " << bytesWritten << " and QByteArray size is: " << writeData.size();

        //qDebug() << "Data to be Written: " << QString::fromStdString (writeData.toStdString ()) << " to Port #" << comPortNum << " bytesWritten:" << bytesWritten;

        //If the data has not been Written, flush & wait 100 milli-secs
        if(bytesWritten != writeData.size())
        {
            p_ComPortArray[comPortNum]->flush ();
            writeDone = p_ComPortArray[comPortNum]->waitForBytesWritten (COMPORTWAITFORWRITE);
        }
        else
            writeDone = true;

        if(!writeDone)
        {
            //If Failed to Write then emit Error Signal
            QSerialPort::SerialPortError portError = p_ComPortArray[comPortNum]->error();
            QString critMessage = "A write failed on the Serial COM Port: "+QString::number(comPortNum)+". Make sure a light gun or a COM device is on that port number. Serial Port Error: "+QString::number(portError);
            emit ErrorMessage("Serial COM Port Error",critMessage);
            //qDebug() << critMessage;
        }
    }
}

void HookCOMPort::ReadData()
{
    quint8 tempNum = UNASSIGN;
    QByteArray tempReadData;

    for(quint8 i = 0; i < MAXCOMPORTS; i++)
    {
        if(p_ComPortArray[i]->canReadLine())
        {
            tempNum = i;
            break;
        }
    }

    if(tempNum != UNASSIGN)
    {
        tempReadData = p_ComPortArray[tempNum]->readAll ();
        emit ReadDataSig(tempNum, tempReadData);
    }
}

void HookCOMPort::DisconnectAll()
{
    if(isPortOpen)
    {
        //Close All Connections
        for(quint8 i = 0; i < MAXCOMPORTS; i++)
        {
            if(comPortOpen[i])
            {
                p_ComPortArray[i]->close ();
                comPortOpen[i] = false;
            }
        }
        isPortOpen = false;
    }
}






