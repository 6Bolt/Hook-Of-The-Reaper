#ifndef ULTIMARCLC_H
#define ULTIMARCLC_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QDebug>
//#include <QMessageBox>
//#include <QTimer>
#include <QRandomGenerator>


#include "LightController.h"
//#include "LightExecution.h"
//#include "LightBackground.h"

#include "../Global.h"



class UltimarcLC : public LightController
{
    Q_OBJECT
public:

    explicit UltimarcLC(quint8 cntlrNum, UltimarcLC const &other, QObject *parent = nullptr);

    explicit UltimarcLC(quint8 cntlrNum, UltimarcData dataU, QObject *parent = nullptr);


    ~UltimarcLC();



};

#endif // ULTIMARCLC_H
