#ifndef HOOKTCPSOCKET_H
#define HOOKTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QRegularExpression>

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

    //When a Game Starts, get String List of Output Signals, then filter output to those Signals
    void GameStartSocket(const QStringList &outputSignals);

    //When a Game has Stopped and Stop Filtering the Output Data
    void GameStopSocket();

    //Tells TCP Socket if HOTR is Minimized
    void WindowStateTCP(const bool &isMin);

signals:

    //Signal Sent to Hooker Engine with Read Data
    //void DataRead(const QByteArray &readBA);
    void DataRead(const QStringList &readSL);

    //Signal Sent to Hooker Engine to tell it is connected
    void SocketConnectedSignal();

    //Signal Sent to Hooker Engine to tell it is disconnected
    void SocketDisconnectedSignal();

    //Tells Hooker Engine that it can Stop Filtering Data
    void StopFilteringTCPReadData();

    //Send out No Used Data
    void FilteredTCPData(const QStringList &readSL);

private:


    ///////////////////////////////////////////////////////////////////////////

    //read Data
    QByteArray readData;

    //Output Signal that Hooker Engine is Looking for
    QStringList outputSignalsFilter;

    //If a Game is going on or not
    bool inGame;

    //Send Stop Filtering String
    bool sentStopFiltering;

    //HOTR is Minimized, so Don't Send Unused Data
    bool isMinimized;

};

#endif // HOOKTCPSOCKET_H
