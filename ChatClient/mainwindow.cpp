#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<fstream>
#include <QMessageBox>

using namespace std;

MainWindow::MainWindow(NetworkClient *client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , client(client)
{
    ui->setupUi(this);
    isLoggedin = false ;
    isSignedin = false ;

    connect(client, &NetworkClient::signupResult, this, &MainWindow::handleSResponse);
    setAttribute(Qt::WA_DeleteOnClose);
}


MainWindow::~MainWindow()
{
    delete ui;
}

bool userExists(string username)
{
    QString projectDir = QCoreApplication::applicationDirPath().section("/build", 0, 0);
    ifstream file((projectDir + "/users.txt").toStdString());

    string storedUsername , storedPassword ;
    while(file>>storedUsername>>storedPassword)
    {
        if(storedUsername==username)
        {
            return true ;
        }
    }

    return false ;
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

    client->sendSigninRequest(username,password) ;
}


void MainWindow::on_mw_login_pushButton_clicked()
{
    Log_in* l = new Log_in(client,this) ;
    l -> show() ;
    this->hide() ;
}

void MainWindow::handleSResponse(bool success, QString message)
{
    if(success)
    {
        QMessageBox::information(this, "Success", message);
        QString usern = ui->sign_up_usernameLineEdit->text().trimmed() ;
        chatWindow = new chatBox(client, usern, nullptr);
        chatWindow->show();

        this->hide() ;
    }
    else
    {
        QMessageBox::critical(this, "Signup Failed", message);
    }
}



















