#include "GamePlayer.h"

GamePlayer::GamePlayer(QObject *parent)
    : QObject{parent}
{}


void GamePlayer::SetPlayer(quint8 player)
{
    playerNumber = player;
}

void GamePlayer::SetWriteNumber(quint8 number)
{
    writeNumber = number;
}

void GamePlayer::SetHIDInfo(HIDInfo playerHIDInfo)
{
    lgHIDInfo = playerHIDInfo;
}


void GamePlayer::Connect(bool noInit)
{
    openNoInit = noInit;

    emit ConnectToLG(playerNumber, openNoInit);
}

void GamePlayer::Disconnect(bool noInit, bool InitOnly)
{
    closeNoInit = noInit;
    closeInitOnly = InitOnly;

    emit DisconnectFromLG(playerNumber, closeNoInit, closeInitOnly);
}

void GamePlayer::Write(QString data)
{
    writeData = data;

    emit WriteToLG(writeNumber, writeData);
}
