
#include "hookOfTheReaper.h"

#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HookOfTheReaper w;
    w.setWindowIcon (QIcon(":/data/icons/hOTRIcon256.ico"));
    w.show();
    return a.exec();
}
