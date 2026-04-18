#include "log_in.h"
#include "networkclient.h"
#include "ui_log_in.h"

Log_in::Log_in(NetworkClient *client, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Log_in)
    , isConnected(false)
    , client(client)
    , isLoggedIn(false)
    , chatWindow(nullptr)
{
    ui->setupUi(this);

    connect(this->client, &NetworkClient::statusChanged,
            this, &Log_in::updateStatus);
}

Log_in::~Log_in()
{
    delete ui;
}

void Log_in::on_connectButton_clicked()
{
    if (isConnected) {
        ui->statusLabel->setText("Already connected.");
        ui->statusLabel->adjustSize();
        return;
    }

    client->connectToServer();
}

void Log_in::on_loginButton_clicked()
{

    if (!isConnected) {
        ui->statusLabel->setText("Connect to the server first.");
        ui->statusLabel->adjustSize();
        return;
    }

    if(isLoggedIn) {
        ui->statusLabel->setText("Already logged in.");
        ui->statusLabel->adjustSize();
        return;
    }

    QString username = ui->lg_sign_up_usernameLineEdit->text();

    if (username.trimmed().isEmpty()) {
        ui->statusLabel->setText("Enter a username first.");
        ui->statusLabel->adjustSize();
        return;
    }

    client->sendLoginRequest(username);
    isLoggedIn = true;

    chatWindow = new chatBox(client,username, this) ;
    this -> hide() ;
    chatWindow -> open() ;
}

void Log_in::updateStatus(const QString &status)
{
    ui->statusLabel->setText(status);
    ui->statusLabel->adjustSize();

    if (status == "Connected successfully.") {
        isConnected = true;
    }

    if (status.startsWith("Connection failed")) {
        isConnected = false;
        isLoggedIn = false;
    }
}
