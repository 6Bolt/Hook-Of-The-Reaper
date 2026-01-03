#include "HookTCPSocket.h"
#include "../Global.h"


HookTCPSocket::HookTCPSocket(QObject *parent)
    : QObject{parent}
{
    //qDebug() << "HookTCPSocket Started";

    inGame = false;

    //HOTR Starts out Minimized
    isMinimized = true;

    //Is the TCP Socket Connected
    isConnected = false;

    //Is TCP Socket trying to Connect
    isConnecting = false;

    //Stop Connecting to TCP Server
    stopConnecting = false;

    //Create the New TCP Socket
    p_hookSocket = new QTcpSocket(this);

    //Connect Signal of when there is read data, to the slot that will read it
    connect(p_hookSocket,SIGNAL(readyRead()), this, SLOT(TCPReadData()));
    connect(p_hookSocket,SIGNAL(connected()), this, SLOT(SocketConnected()));
    connect(p_hookSocket,SIGNAL(disconnected()), this, SLOT(SocketDisconnected()));

    //Timer Set-up
    p_waitingForConnection = new QTimer(this);
    p_waitingForConnection->setInterval (TCPTIMERTIME);
    p_waitingForConnection->setSingleShot (true);
    connect(p_waitingForConnection, SIGNAL(timeout()), this, SLOT(TCPConnectionTimeOut()));

}

void HookTCPSocket::TCPReadData()
{
    quint8 i;

    //Read the TCP Socket Data
    readData = p_hookSocket->readAll ();

    //Convert to Byte Array to String
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
        //Get the Output Signal Name
        QStringList splitData = tcpSocketReadData[i].split(" = ", Qt::SkipEmptyParts);

        //qDebug() << "Socket Read, signal:" << splitData[0] << "data:" << splitData[1];

        if(inGame)
        {
            //qDebug() << "Socket Read After Game, signal:" << splitData[0] << "data:" << splitData[1];

            //Search for Output Signal
            if(outputSignalsFilter.contains(splitData[0]))
            {
                if(splitData[0].size() == 9)
                {
                    if(splitData[0][5] == 's' && splitData[0][8] == 'p')
                    {
                        emit GameHasStopped();
                        inGame = false;
                        //qDebug() << "Socket Read After Game, Game Has Stopped";
                    }
                    else
                        emit FilteredOutputSignals(splitData[0], splitData[1]);
                }
                else
                    emit FilteredOutputSignals(splitData[0], splitData[1]);
            }
            else if(!isMinimized)
                emit FilteredTCPData(splitData[0], splitData[1]);
        }
        else
        {
            //qDebug() << "Socket Read Before Game, signal:" << splitData[0] << "data:" << splitData[1];

            //Check for Game Starting
            if(splitData[0] == MAMESTART)
            {
                if(splitData[1] == MAMEEMPTY)
                    emit EmptyGameHasStarted();
                else
                    emit GameHasStarted(splitData[1]);
            }
            else if(splitData[0] == GAMESTART)
                emit GameHasStarted(splitData[1]);
            else
            {
                if(splitData[0][0] == 'M' && splitData[0][1] == 'a' && splitData[0][2] == 'm')
                {
                    if(splitData[0][4] == 'P' && splitData[0].size() == 9)
                        splitData[0] = PAUSE;
                    else if(splitData[0][4] == 'O' && splitData[0].size() == 15)
                        splitData[0].replace(MAMEORIENTATION, ORIENTATION);
                }

                emit DataRead(splitData[0], splitData[1]);
            }
        }

    }
}


void HookTCPSocket::Connect()
{
    stopConnecting = false;
    if(!isConnected && !isConnecting)
    {
        //qDebug() << "Waiting for a TCP Connection - Connect";

        //Set the Address for the TCP Socket
        //p_hookSocket->connectToHost (TCPHOSTNAME, TCPHOSTPORT);
        //p_hookSocket->connectToHost (QHostAddress::SpecialAddress::LocalHost, TCPHOSTPORT);
        p_hookSocket->connectToHost (QHostAddress("127.0.0.1"), TCPHOSTPORT);
        //p_hookSocket->connectToHost ("localhost", TCPHOSTPORT);

        //Start Timer for Connection
        p_waitingForConnection->start ();

        //Set the Is Connecting Bool
        isConnecting = true;

        //Wait for Connection
        p_hookSocket->waitForConnected (TIMETOWAIT);
    }
}


void HookTCPSocket::Disconnect()
{
    //Set to stop TCP Socket from trying to Connect again
    stopConnecting = true;

    p_waitingForConnection->stop();

    //Close TCP Socket
    p_hookSocket->close ();

    isConnected = false;
    isConnecting = false;

    //TCP Socket Closed, so game has Stopped
    inGame = false;
}

//Used for MultiThreading
void HookTCPSocket::SocketConnected()
{
    isConnected = true;
    isConnecting = false;

    p_waitingForConnection->stop();

    emit SocketConnectedSignal();

    //qDebug() << "TCP Socket Connected";
}

void HookTCPSocket::SocketDisconnected()
{
    isConnected = false;
    isConnecting = false;
    inGame = false;

    emit SocketDisconnectedSignal();

    //qDebug() << "TCP Socket Disconnected";

    //if(!stopConnecting)
    //    Connect();
}


void HookTCPSocket::GameStartSocket(const QStringList &outputSignals)
{
    outputSignalsFilter = outputSignals;

    //qDebug() << "Stop Filtering Data: Sent Signal to Hooker Engine";

    inGame = true;
}


void HookTCPSocket::GameStopSocket()
{
    inGame = false;
    //qDebug() << "HE told TCP Game Has Stopped";
}

void HookTCPSocket::WindowStateTCP(const bool &isMin)
{
    isMinimized = isMin;
}

void HookTCPSocket::TCPConnectionTimeOut()
{
    if(!isConnected && !stopConnecting && isConnecting)
    {
        if(p_hookSocket->state() != QAbstractSocket::ConnectedState)
        {
            p_hookSocket->connectToHost (QHostAddress("127.0.0.1"), TCPHOSTPORT);

            p_waitingForConnection->start ();

            //Wait for Connection
            p_hookSocket->waitForConnected (TIMETOWAIT);
        }
    }
}


