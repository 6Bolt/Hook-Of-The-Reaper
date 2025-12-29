#ifndef HOOKTCPSOCKET_H
#define HOOKTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QRegularExpression>
#include <QTimer>

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

    //When Timer Runs Out
    void TCPConnectionTimeOut();

signals:

    //Signal Sent to Hooker Engine with Read Data
    //void DataRead(const QByteArray &readBA);
    //void DataRead(const QStringList &readSL);
    void DataRead(const QString &signal, const QString &data);

    //Signal Sent to Hooker Engine to tell it is connected
    void SocketConnectedSignal();

    //Signal Sent to Hooker Engine to tell it is disconnected
    void SocketDisconnectedSignal();

    //Send out No Used Data
    //void FilteredTCPData(const QStringList &readSL);
    void FilteredTCPData(const QString &signal, const QString &data);

    //When Game Has Begon (got mame_start or game)
    void GameHasStarted(const QString &data);

    //When an Empty Game has Started
    void EmptyGameHasStarted();

    //When Game Has Stopped (got mame_stop or game_stop)
    void GameHasStopped();

    //Filtered Output Signals with their Data Value, Sent to ProcessLGCommands or ProcessINIComands
    void FilteredOutputSignals(const QString &signal, const QString &data);

private:


    ///////////////////////////////////////////////////////////////////////////

    //Timer
    QTimer *p_waitingForConnection;

    bool isConnected;

    //read Data
    QByteArray readData;

    //Output Signal that Hooker Engine is Looking for
    QStringList outputSignalsFilter;

    //If a Game is going on or not
    bool inGame;

    //HOTR is Minimized, so Don't Send Unused Data
    bool isMinimized;

    //Stop Connecting to TCP Server
    bool stopConnecting;


};

#endif // HOOKTCPSOCKET_H
