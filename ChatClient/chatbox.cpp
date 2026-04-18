#include "chatbox.h"
#include "ui_chatbox.h"
#include <QDebug>

chatBox::chatBox(NetworkClient *client, const QString &username, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::chatBox)
    , isLoggedIn(true)
    , isConnected(false)
    , client(client)
    , user(username)
{
    qDebug() << "chatBox opened, client =" << client;

    ui->setupUi(this);

    connect(client, &NetworkClient::statusChanged, this, &chatBox::updateStatus);

    connect(client, &NetworkClient::messageReceived, this, [this](const QString &message) {
        ui->chatTextEdit->append("Received: " + message);
    });
}


chatBox::~chatBox()
{
    delete ui;
}
void chatBox::on_connectButton_clicked()
{
    qDebug() << "connect pressed, client =" << client;

    if (isConnected) {
        ui->statusLabel->setText("Already connected.");
        ui->statusLabel->adjustSize();
        return;
    }

    client->connectToServer();
}
void chatBox::updateStatus(const QString &status)
{
    qDebug() << "status =" << status;

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

void chatBox::on_sendButton_clicked()
{
    if(!canSendMessages()) {
        return;
    }

    QString username = user ;
    QString message = ui->messageLineEdit->text();

    if (username.trimmed().isEmpty()) {
        ui->statusLabel->setText("Enter a username first.");
        ui->statusLabel->adjustSize();
        return;
    }

    if (message.trimmed().isEmpty()) {
        ui->statusLabel->setText("Enter a message first.");
        ui->statusLabel->adjustSize();
        return;
    }

    client->sendChatMessage(username, message);

    ui->chatTextEdit->append(username + ": " + message);
    ui->messageLineEdit->clear();
}


void chatBox::on_privateSendButton_clicked()

{

    if (!canSendMessages()) {
        return;
    }


    QString username = user ;
    QString receiver = ui->recipientLineEdit->text();
    QString message = ui->messageLineEdit->text();


    if (receiver.trimmed().isEmpty()) {

        ui->statusLabel->setText("Enter a recipient.");
        ui->statusLabel->adjustSize();
        return;

    }



    if (message.trimmed().isEmpty()) {

        ui->statusLabel->setText("Enter a message first.");
        ui->statusLabel->adjustSize();
        return;

    }



    client->sendPrivateMessage(username, receiver, message);

    ui->chatTextEdit->append("[Private to " + receiver + "] " + username + ": " + message);

    ui->messageLineEdit->clear();

}


bool chatBox::canSendMessages()
{
    if (!isConnected) {
        ui->statusLabel->setText("Connect to the server first.");
        ui->statusLabel->adjustSize();
        return false;
    }

    if (!isLoggedIn) {
        ui->statusLabel->setText("You must log in first.");
        ui->statusLabel->adjustSize();
        return false;
    }

    return true;
}
