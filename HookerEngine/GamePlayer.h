#ifndef GAMEPLAYER_H
#define GAMEPLAYER_H

#include <QObject>

#include "../Global.h"

class GamePlayer : public QObject
{
    Q_OBJECT
public:
    explicit GamePlayer(QObject *parent = nullptr);

    void SetPlayer(quint8 player);
    void SetWriteNumber(quint8 number);
    void SetHIDInfo(HIDInfo playerHIDInfo);

    void Connect(bool noInit);
    void Disconnect(bool noInit, bool InitOnly);
    void Write(QString data);


signals:
    void ConnectToLG(quint8 playerNum, bool noInit);
    void DisconnectFromLG(quint8 playerNum, bool noInit, bool initOnly);
    void WriteToLG(quint8 playerNum, QString cpData);

private:
    quint8          playerNumber;
    quint8          writeNumber;
    bool            openNoInit;
    bool            closeNoInit;
    bool            closeInitOnly;
    QString         writeData;
    HIDInfo         lgHIDInfo;
};

#endif // GAMEPLAYER_H
