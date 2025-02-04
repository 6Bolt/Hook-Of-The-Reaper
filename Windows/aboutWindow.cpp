#include "aboutWindow.h"
#include "ui_aboutWindow.h"

aboutWindow::aboutWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::aboutWindow)
{
    ui->setupUi(this);
}

aboutWindow::~aboutWindow()
{
    delete ui;
}
