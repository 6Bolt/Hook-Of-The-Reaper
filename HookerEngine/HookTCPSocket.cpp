#include "HookTCPSocket.h"
#include "../Global.h"


HookTCPSocket::HookTCPSocket(QObject *parent)
    : QObject{parent}
{
    //qDebug() << "HookTCPSocket Started";

    inGame = false;

    //HOTR Starts out Minimized
    isMinimized = true;

    //Create the New TCP Socket
    p_hookSocket = new QTcpSocket(this);

    //Connect Signal of when there is read data, to the slot that will read it
    connect(p_hookSocket,SIGNAL(readyRead()), this, SLOT(TCPReadData()));
    connect(p_hookSocket,SIGNAL(connected()), this, SLOT(SocketConnected()));
    connect(p_hookSocket,SIGNAL(disconnected()), this, SLOT(SocketDisconnected()));

}

void HookTCPSocket::TCPReadData()
{
    quint8 i;
    QStringList readDataSL, filterData;

    //Read the TCP Socket Data
    readData = p_hookSocket->readAll ();

    QString message = QString::fromStdString (readData.toStdString ());

    //Remove the \r at the end
    message.chop(1);

    //qDebug() << message;

    //If Multiple Data Lines, they will be seperated into lines, using \r or \n
    //If it had 2 data lines together, then \r would be at end which is chopped off, and middle
    //QRegularExpression endLines("[\r\n]");
    QStringList tcpSocketReadData = message.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);

    for(i = 0; i < tcpSocketReadData.count(); i++)
    {
        //qDebug() << "Socket Read:" << tcpSocketReadData[i];

        if(inGame)
        {
            if(!sentStopFiltering)
            {
                emit StopFilteringTCPReadData();
                sentStopFiltering = true;
            }

            //Get the Output Signal Name
            QStringList splitData = tcpSocketReadData[i].split(" = ", Qt::SkipEmptyParts);

            //qDebug() << "Socket Read Signal:" << splitData[0];

            //Search for Output Signal
            if(outputSignalsFilter.contains(splitData[0]))
                readDataSL << tcpSocketReadData[i];
            else if(!isMinimized)
                filterData << tcpSocketReadData[i];
        }
        else
        {
            //None Filter Data
            readDataSL << tcpSocketReadData[i];
        }

    }

    //Send Data to Hooker Engine
    emit DataRead(readDataSL);

    //Sent No Used Data Out, if Not Minimized
    if(!isMinimized)
        emit FilteredTCPData(filterData);
}


void HookTCPSocket::Connect()
{
    //qDebug() << "Waiting for a TCP Connection - Connect";

    //Set the Address for the TCP Socket
    //p_hookSocket->connectToHost (TCPHOSTNAME, TCPHOSTPORT);
    //p_hookSocket->connectToHost (QHostAddress::SpecialAddress::LocalHost, TCPHOSTPORT);
    p_hookSocket->connectToHost (QHostAddress("127.0.0.1"), TCPHOSTPORT);

    //Wait for Connection
    p_hookSocket->waitForConnected (TIMETOWAIT);

}


void HookTCPSocket::Disconnect()
{
    //qDebug() << "Closed TCP Connection - Disconnect";

    //Close TCP Socket
    p_hookSocket->close ();

    //TCP Socket Closed, so game has Stopped
    inGame = false;
    sentStopFiltering = false;
}

//Used for MultiThreading
void HookTCPSocket::SocketConnected()
{
    emit SocketConnectedSignal();
}

void HookTCPSocket::SocketDisconnected()
{
    emit SocketDisconnectedSignal();
}


void HookTCPSocket::GameStartSocket(const QStringList &outputSignals)
{
    outputSignalsFilter = outputSignals;

    inGame = true;
}


void HookTCPSocket::GameStopSocket()
{
    inGame = false;
}

void HookTCPSocket::WindowStateTCP(const bool &isMin)
{
    isMinimized = isMin;
}




