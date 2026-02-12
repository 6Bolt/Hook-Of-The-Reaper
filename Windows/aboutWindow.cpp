#include "aboutWindow.h"
#include "Windows/ui_aboutWindow.h"

aboutWindow::aboutWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::aboutWindow)
{
    ui->setupUi(this);

    QString versionInfo = "Version: ";
    versionInfo.append (VERSION);

    ui->versionLabel->setText (versionInfo);
}

aboutWindow::~aboutWindow()
{
    delete ui;
}
