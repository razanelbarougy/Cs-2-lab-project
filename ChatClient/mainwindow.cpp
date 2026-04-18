#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , client(new NetworkClient(this))
{
    ui->setupUi(this);
    isLoggedin = false ;
    isSignedin = false ;
    isConnected = false ;
}


MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_signupButton_clicked()
{

    QString username = ui->sign_up_usernameLineEdit->text() ;
    QString password = ui->sign_up_passwordLineEdit-> text() ;
    username = username.trimmed() ;
    password = password.trimmed() ;

    if(username=="")
    {
        QMessageBox::critical(this, "Error", "Username Field is Empty") ;
        return ;
    }
    if(password=="")
    {
        QMessageBox::critical(this, "Error", "Password Field is Empty") ;
        return ;
    }

    user = username ;
    isSignedin = true ;

    client->sendSigninRequest(user) ;

    chatWindow = new chatBox(client,user,this) ;
    this->hide() ;
    chatWindow -> show() ;
}


void MainWindow::on_mw_login_pushButton_clicked()
{
    Log_in* l = new Log_in(client,this) ;
    l -> show() ;
    this->hide() ;
}

