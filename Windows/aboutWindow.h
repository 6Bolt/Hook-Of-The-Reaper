#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QDialog>


#include "../Global.h"

namespace Ui {
class aboutWindow;
}

class aboutWindow : public QDialog
{
    Q_OBJECT

public:
    explicit aboutWindow(QWidget *parent = nullptr);
    ~aboutWindow();

private:
    Ui::aboutWindow *ui;
};

#endif // ABOUTWINDOW_H
