#include "HookTCPSocket.h"
#include "../Global.h"


HookTCPSocket::HookTCPSocket(QObject *parent)
    : QObject{parent}
{
    //qDebug() << "HookTCPSocket Started";

    //Create the New TCP Socket
    p_hookSocket = new QTcpSocket(this);

    //Connect Signal of when there is read data, to the slot that will read it
    connect(p_hookSocket,SIGNAL(readyRead()), this, SLOT(TCPReadData()));
    connect(p_hookSocket,SIGNAL(connected()), this, SLOT(SocketConnected()));
    connect(p_hookSocket,SIGNAL(disconnected()), this, SLOT(SocketDisconnected()));

}

void HookTCPSocket::TCPReadData()
{
    //Read the TCP Socket Data
    readData = p_hookSocket->readAll ();

    //Move the Data over Securly, Since Multi-Threaded
    emit DataRead(readData);
}


void HookTCPSocket::Connect()
{
    //qDebug() << "Waiting for a TCP Connection - Connect";

    //Set the Address for the TCP Socket
    p_hookSocket->connectToHost (TCPHOSTNAME, TCPHOSTPORT);

    //Wait for Connection
    p_hookSocket->waitForConnected (TIMETOWAIT);

}


void HookTCPSocket::Disconnect()
{
    //qDebug() << "Closed TCP Connection - Disconnect";

    //Close TCP Socket
    p_hookSocket->close ();
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
