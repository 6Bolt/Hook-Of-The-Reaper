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

HookTCPSocket::~HookTCPSocket()
{
    p_waitingForConnection->stop();
    delete p_waitingForConnection;
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

        //Check if Game Has Stopped
        if(splitData[0].size() == 9 && inGame)
        {
            if(splitData[0][5] == 's' && splitData[0][6] == 't' && splitData[0][8] == 'p')
            {
                emit GameHasStopped();
                inGame = false;
            }
        }


        if(inGame)
        {
            //qDebug() << "Socket Read After Game, signal:" << splitData[0] << "data:" << splitData[1];

            //Check if Light Guns and Light Controllers using Output Signal
            if(bothOutputSig)
            {
                if(outputSignalsBoth.contains(splitData[0]))
                    emit FilteredOutputSignalsBoth(splitData[0], splitData[1]);
            }

            //Check if Light Guns using Output Signal
            if(lgOutputSig)
            {
                if(outputSignalsFilter.contains(splitData[0]))
                    emit FilteredOutputSignals(splitData[0], splitData[1]);
            }

            //Check if Light Controllers using Output Signal
            if(lcOutputSig)
            {
                if(outputSignalsLight.contains(splitData[0]))
                    emit FilteredOutputSignalsLight(splitData[0], splitData[1]);
            }

            if(!isMinimized)
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
    lgOutputSig = false;
    lcOutputSig = false;
    bothOutputSig = false;
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
    lgOutputSig = false;
    lcOutputSig = false;
    bothOutputSig = false;

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
    lgOutputSig = true;

    if(lcOutputSig)
        CombineOutputSignals();
}


void HookTCPSocket::GameStartLight(const QStringList &outputSignals)
{
    outputSignalsLight = outputSignals;

    inGame = true;
    lcOutputSig = true;

    if(lgOutputSig)
        CombineOutputSignals();
}


/*
void HookTCPSocket::GameStopSocket()
{
    inGame = false;
    //qDebug() << "HE told TCP Game Has Stopped";
}
*/

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


void HookTCPSocket::CombineOutputSignals()
{
    quint8 i;
    quint8 lgCount = outputSignalsFilter.count();
    quint8 lcCount = outputSignalsLight.count();
    quint8 count = 0;
    QStringList tempSignals;

    //Check if LG List is Bigger
    if(lgCount > lcCount)
    {
        for(i = 0; i < lgCount; i++)
        {
            if(outputSignalsLight.contains(outputSignalsFilter[i]))
            {
                //Found in Both String List. Move to New List and Remove from Old 2 Lists
                outputSignalsBoth << outputSignalsFilter[i];
                //QString tempS = outputSignalsFilter[i];
                //outputSignalsFilter.removeOne(tempS);
                //outputSignalsLight.removeOne(tempS);
                count++;
            }
            else
                tempSignals << outputSignalsFilter[i];
        }

        //Make New Light Gun List to Regular List, Which has Both Signals Removed
        outputSignalsFilter = tempSignals;

        //Remove Both Signals from Light Controller Signals List
        for(i = 0; i < outputSignalsBoth.count(); i++)
            outputSignalsLight.removeOne(outputSignalsBoth[i]);
    }
    else
    {
        for(i = 0; i < lcCount; i++)
        {
            if(outputSignalsFilter.contains(outputSignalsLight[i]))
            {
                //Found in Both String List. Move to New List and Remove from Old 2 Lists
                outputSignalsBoth << outputSignalsLight[i];
                count++;
            }
            else
                tempSignals << outputSignalsLight[i];
        }

        //Make New Light Controller List to Regular List, Which has Both Signals Removed
        outputSignalsLight = tempSignals;

        //Remove Both Signals from Light Controller Signals List
        for(i = 0; i < outputSignalsBoth.count(); i++)
            outputSignalsFilter.removeOne(outputSignalsBoth[i]);
    }

    if(count > 0)
    {
        bothOutputSig = true;
        //qDebug() << "Both" << outputSignalsBoth;
        //qDebug() << "Light Gun" << outputSignalsFilter;
        //qDebug() << "Light Controller" << outputSignalsLight;
    }
    else
        bothOutputSig = false;

}

