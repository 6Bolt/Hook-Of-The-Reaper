#ifndef HOOKTCPSOCKET_H
#define HOOKTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>



#include <QByteArray>
#include <QDebug>




class HookTCPSocket : public QObject
{
        Q_OBJECT

public:
    explicit HookTCPSocket(QObject *parent = nullptr);

    ///////////////////////////////////////////////////////////////////////////

    //TCP Socket Class
    QTcpSocket *p_hookSocket;

    ///////////////////////////////////////////////////////////////////////////

public slots:

    //Connects to TCP Socket and Waits for Connection
    void Connect();
    //Disconnects the TCP Socket
    void Disconnect();

    //When Signal readReady, this Slot Reads the TCP Socket and Forwards it to
    //the Hooker Engine
    void TCPReadData();

    //When TCP Socket Connects, it calls this Slot, which emit another Signal to
    //Hooker Engine to Let it Know it Connected
    void SocketConnected();
    //When TCP Socket Disconnects, it calls this Slot, which emit another Signal to
    //Hooker Engine to Let it Know it Disconnected
    void SocketDisconnected();


signals:

    //Signal Sent to Hooker Engine with Read Data
    void DataRead(const QByteArray &readBA);

    //Signal Sent to Hooker Engine to tell it is connected
    void SocketConnectedSignal();

    //Signal Sent to Hooker Engine to tell it is disconnected
    void SocketDisconnectedSignal();


private:


    ///////////////////////////////////////////////////////////////////////////

    //read Data
    QByteArray readData;



};

#endif // HOOKTCPSOCKET_H
