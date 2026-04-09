#include "UltimarcLC.h"

UltimarcLC::UltimarcLC(quint8 cntlrNum, UltimarcLC const &other, QObject *parent)
    : LightController{cntlrNum, other, parent}
{


}


UltimarcLC::UltimarcLC(quint8 cntlrNum, UltimarcData dataU, QObject *parent)
    : LightController{cntlrNum, dataU, parent}
{

}


UltimarcLC::~UltimarcLC()
{

}

