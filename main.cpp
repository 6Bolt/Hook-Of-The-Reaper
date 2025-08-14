
#include "hookOfTheReaper.h"

#include <QApplication>
#include <QCoreApplication>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Code to Check if Program Already Running or Not
    //First Get Application Name
    QString programName = QCoreApplication::applicationName();

    //Make an Unique Program Name for Shared Memory and Semaphore
    QString shardMemoryName = programName+"6969DEADBEEF";
    QString semaphoreName = programName+"DEADBEEF6969";

    //Create Semaphore with semaphoreName
    QSystemSemaphore semaphore(semaphoreName, 1);

    semaphore.acquire();

    //Create Share Memory with shardMemoryName
    QSharedMemory sharedMemory(shardMemoryName);
    bool isRunning;

    //If Memory can Be Created, then no other HOTR Running
    //If false, then HOTR is Already Running
    if (sharedMemory.create(1))
        isRunning = false;
    else
        isRunning = true;

    //Release semaphore before program Starts or Closes
    semaphore.release();


    //If Not Running, Start the Code, or Close if Running
    if(!isRunning)
    {
        //If Not Running, then Get Going
        HookOfTheReaper w;
        //w.setWindowIcon (QIcon(":/data/icons/hOTRIcon256.ico"));
        w.setWindowIcon (QIcon("./data/icons/hOTRIcon256.ico"));

        //w.setVisible (false);
        //w.hide();
        //w.show();
        return a.exec();
    }
    else
    {
        //Close, if already running
        return 1;
    }
}







