#include "playerAssignWindow.h"
#include "Windows/ui_playerAssignWindow.h"

//Constructor
playerAssignWindow::playerAssignWindow(ComDeviceList *cdList, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::playerAssignWindow)
{
    QString tempQS;
    quint8 i;

    //Setup the Window Functions, like Combo Box
    ui->setupUi(this);

    //Fixes the Size of the Window, so it Cannot Be Expanded
    this->setFixedSize(this->size());

    //Move Over the ComDevice List and Get a Copy of the Unused COM Ports
    p_comDeviceList = cdList;

    //Load Players Combo Boxes with the Saved Light Guns; Number, Name, and COM Port. But the first entry will be blank to deassign
    numberLightGuns = p_comDeviceList->GetNumberLightGuns();

    ui->player1ComboBox->insertItem(0,"   ");
    ui->player2ComboBox->insertItem(0,"   ");
    ui->player3ComboBox->insertItem(0,"   ");
    ui->player4ComboBox->insertItem(0,"   ");
    ui->player5ComboBox->insertItem(0,"   ");
    ui->player6ComboBox->insertItem(0,"   ");
    ui->player7ComboBox->insertItem(0,"   ");
    ui->player8ComboBox->insertItem(0,"   ");

    for(i = 0; i < numberLightGuns; i++)
    {
        quint8 outputConnection = p_comDeviceList->p_lightGunList[i]->GetOutputConnection();
        tempQS = QString::number(i+1);
        tempQS.append (": ");
        tempQS.append (p_comDeviceList->p_lightGunList[i]->GetLightGunName());
        tempQS.append (" on ");
        if(outputConnection == SERIALPORT)
        {
            quint8 cpNum = p_comDeviceList->p_lightGunList[i]->GetComPortNumber();
            tempQS.append ("COM");
            tempQS.append (QString::number (cpNum));
        }
        else if(outputConnection == USBHID)
            tempQS.append ("USB");
        else if(outputConnection == BTLE)
            tempQS.append ("BTLE");
        else if(outputConnection == TCP)
        {
            tempQS.append ("TCP(");
            quint16 tempTCP = p_comDeviceList->p_lightGunList[i]->GetTCPPort ();
            tempQS.append(QString::number (tempTCP));
            tempQS.append (")");
        }

        ui->player1ComboBox->insertItem(i+1,tempQS);
        ui->player2ComboBox->insertItem(i+1,tempQS);
        ui->player3ComboBox->insertItem(i+1,tempQS);
        ui->player4ComboBox->insertItem(i+1,tempQS);
        ui->player5ComboBox->insertItem(i+1,tempQS);
        ui->player6ComboBox->insertItem(i+1,tempQS);
        ui->player7ComboBox->insertItem(i+1,tempQS);
        ui->player8ComboBox->insertItem(i+1,tempQS);
    }

    //Get Players Assignment from the COM Devices List
    for(i = 0; i < MAXPLAYERLIGHTGUNS; i++)
    {
        playersAssignment[i] = p_comDeviceList->GetPlayerLightGunAssignment(i);
    }

    //Set the Players Combo Boxes to the Right Value
    if(playersAssignment[0] == UNASSIGN)
        ui->player1ComboBox->setCurrentIndex (0);
    else
        ui->player1ComboBox->setCurrentIndex (playersAssignment[0]+1);

    if(playersAssignment[1] == UNASSIGN)
        ui->player2ComboBox->setCurrentIndex (0);
    else
        ui->player2ComboBox->setCurrentIndex (playersAssignment[1]+1);

    if(playersAssignment[2] == UNASSIGN)
        ui->player3ComboBox->setCurrentIndex (0);
    else
        ui->player3ComboBox->setCurrentIndex (playersAssignment[2]+1);

    if(playersAssignment[3] == UNASSIGN)
        ui->player4ComboBox->setCurrentIndex (0);
    else
        ui->player4ComboBox->setCurrentIndex (playersAssignment[3]+1);


    if(playersAssignment[4] == UNASSIGN)
        ui->player5ComboBox->setCurrentIndex (0);
    else
        ui->player5ComboBox->setCurrentIndex (playersAssignment[4]+1);

    if(playersAssignment[5] == UNASSIGN)
        ui->player6ComboBox->setCurrentIndex (0);
    else
        ui->player6ComboBox->setCurrentIndex (playersAssignment[5]+1);

    if(playersAssignment[6] == UNASSIGN)
        ui->player7ComboBox->setCurrentIndex (0);
    else
        ui->player7ComboBox->setCurrentIndex (playersAssignment[6]+1);

    if(playersAssignment[7] == UNASSIGN)
        ui->player8ComboBox->setCurrentIndex (0);
    else
        ui->player8ComboBox->setCurrentIndex (playersAssignment[7]+1);

}

//Deconstructor
playerAssignWindow::~playerAssignWindow()
{
    delete ui;
}


//private slots

void playerAssignWindow::on_assignPushButton_clicked()
{
    AssignPlayers();
}

void playerAssignWindow::on_okPushButton_clicked()
{
    AssignPlayers();
    //Closes Window
    accept ();
}


void playerAssignWindow::on_cancelPushButton_clicked()
{
    //Closes Window
    accept ();
}


//private

void playerAssignWindow::GetComboBoxIndexes()
{
    playersIndex[0] = ui->player1ComboBox->currentIndex ();
    playersIndex[1] = ui->player2ComboBox->currentIndex ();
    playersIndex[2] = ui->player3ComboBox->currentIndex ();
    playersIndex[3] = ui->player4ComboBox->currentIndex ();
    playersIndex[4] = ui->player5ComboBox->currentIndex ();
    playersIndex[5] = ui->player6ComboBox->currentIndex ();
    playersIndex[6] = ui->player7ComboBox->currentIndex ();
    playersIndex[7] = ui->player8ComboBox->currentIndex ();
}

void playerAssignWindow::AssignPlayers()
{
    bool assignmentError = false;
    bool passedToList;
    quint8 i, j;


    GetComboBoxIndexes();

    //Check to make sure a Light Gun is not Being Assign Multiple times
    for(i = 0; i < (MAXPLAYERLIGHTGUNS-1); i++)
    {
        for(j = (i+1); j < MAXPLAYERLIGHTGUNS; j++)
        {
            if(playersIndex[i] == playersIndex[j] && playersIndex[i] != 0)
                assignmentError = true;
        }
    }

    if(assignmentError)
    {
        QMessageBox::warning (this, tr("Light Gun Has Multiple Assignments"), "Light Gun(s) have multiple assignments. A light gun can only be assign once to a player.");
    }
    else
    {
        if(playersIndex[0] == 0)
            p_comDeviceList->DeassignPlayerLightGun(0);
        else
            passedToList = p_comDeviceList->AssignPlayerLightGun(0, playersIndex[0]-1);

        //if(passedToList == false)
        //    qDebug() << "\n" << "Player 1 Assignment Failed to Write to List: " << playersIndex[0]-1 << "\n";


        if(playersIndex[1] == 0)
            p_comDeviceList->DeassignPlayerLightGun(1);
        else
            passedToList = p_comDeviceList->AssignPlayerLightGun(1, playersIndex[1]-1);

        //if(passedToList == false)
        //    qDebug() << "\n" << "Player 2 Assignment Failed to Write to List: " << playersIndex[1]-1 << "\n";


        if(playersIndex[2] == 0)
            p_comDeviceList->DeassignPlayerLightGun(2);
        else
            passedToList = p_comDeviceList->AssignPlayerLightGun(2, playersIndex[2]-1);

        //if(passedToList == false)
        //    qDebug() << "\n" << "Player 3 Assignment Failed to Write to List: " << playersIndex[2]-1 << "\n";


        if(playersIndex[3] == 0)
            p_comDeviceList->DeassignPlayerLightGun(3);
        else
            passedToList = p_comDeviceList->AssignPlayerLightGun(3, playersIndex[3]-1);

        //if(passedToList == false)
        //    qDebug() << "\n" << "Player 4 Assignment Failed to Write to List: " << playersIndex[3]-1 << "\n";


        if(playersIndex[4] == 0)
            p_comDeviceList->DeassignPlayerLightGun(4);
        else
            passedToList = p_comDeviceList->AssignPlayerLightGun(4, playersIndex[4]-1);

        if(playersIndex[5] == 0)
            p_comDeviceList->DeassignPlayerLightGun(5);
        else
            passedToList = p_comDeviceList->AssignPlayerLightGun(5, playersIndex[5]-1);

        if(playersIndex[6] == 0)
            p_comDeviceList->DeassignPlayerLightGun(6);
        else
            passedToList = p_comDeviceList->AssignPlayerLightGun(6, playersIndex[6]-1);

        if(playersIndex[7] == 0)
            p_comDeviceList->DeassignPlayerLightGun(7);
        else
            passedToList = p_comDeviceList->AssignPlayerLightGun(7, playersIndex[7]-1);
    }

}





