#include "log_in.h"
#include "networkclient.h"
#include "ui_log_in.h"
#include <QMessageBox>

using namespace std ;

Log_in::Log_in(NetworkClient *client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Log_in)
    , client(client)
    , isLoggedIn(false)
    , chatWindow(nullptr)
{
    ui->setupUi(this);

    connect(this->client, &NetworkClient::statusChanged,this, &Log_in::updateStatus);
    connect(client, &NetworkClient::loginResult, this, &Log_in::handleServerResponse);
}

Log_in::~Log_in()
{
    delete ui;
}

void Log_in::on_connectButton_clicked()
{
    if (client->isConnected() == true) {
        ui->statusLabel->setText("Already connected.");
        ui->statusLabel->adjustSize();
        return;
    }

    client->connectToServer();
}

void Log_in::updateStatus(const QString &status)
{
    ui->statusLabel->setText(status);
    ui->statusLabel->adjustSize();

    if (status.startsWith("Connection failed")) {
        isLoggedIn = false;
    }
}

void Log_in::on_lg_loginPushButton_clicked()
{
    qDebug() << "Login clicked";

    if (!client->isConnected()) {
        ui->statusLabel->setText("Connect to the server first.");
        ui->statusLabel->adjustSize();
        return;
    }

    if(isLoggedIn) {
        ui->statusLabel->setText("Already logged in.");
        ui->statusLabel->adjustSize();
        return;
    }


    QString username = ui->lg_sign_up_usernameLineEdit->text().trimmed();
    QString password = ui->lg_sign_up_passwordLineEdit->text().trimmed();

    if (username.isEmpty()) {
        ui->statusLabel->setText("Enter a username first.");
        ui->statusLabel->adjustSize();
        return;
    }

    if (password.isEmpty()) {
        ui->statusLabel->setText("Enter a password first.");
        ui->statusLabel->adjustSize();
        return;
    }

    client->sendLoginRequest(username, password);
}
void Log_in::handleServerResponse(bool success)
{
    if (success) {
        isLoggedIn = true;
        chatWindow = new chatBox(client, ui->lg_sign_up_usernameLineEdit->text().trimmed(), this);
        this->hide();
        chatWindow->show();
    }
    else{
        isLoggedIn = false;
        QMessageBox::critical(this, "Error", "Incorrect username or password");
    }
}