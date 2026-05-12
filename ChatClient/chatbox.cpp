#include "chatbox.h"
#include "mainwindow.h"
#include "reactiongame.h"
#include "scoreboard.h"
#include "tictactoe.h"
#include "ui_chatbox.h"
#include <QMessageBox>

chatBox::chatBox(NetworkClient *client, const QString &username, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::chatBox)
    , isLoggedIn(true)
    , client(client)
    , user(username)
{

    ui->setupUi(this);

    connect(client, &NetworkClient::statusChanged, this, &chatBox::updateStatus);

    connect(client, &NetworkClient::messageReceived, this, [this](const QString &message) {
        ui->chatTextEdit->append("Received: " + message);
    });

    connect(client, &NetworkClient::onlineUsersReceived, this, &chatBox::updateOnlineUsers);

    connect(client, &NetworkClient::createRoomResult, this, &chatBox::handleCreateRoomResult);
    connect(client, &NetworkClient::joinRoomResult, this, &chatBox::handleJoinRoomResult);
    connect(client, &NetworkClient::leaveRoomResult, this, &chatBox::handleLeaveRoomResult);
    connect(client, &NetworkClient::roomsReceived, this, &chatBox::updateRooms);
    connect(client, &NetworkClient::roomUsersReceived, this, &chatBox::updateRoomUsers);
    connect(client, &NetworkClient::roomMessageReceived, this, &chatBox::handleRoomMessage);

    setAttribute(Qt::WA_DeleteOnClose);
}


chatBox::~chatBox()
{
    delete ui;
}
void chatBox::on_connectButton_clicked()
{
    if (client->isConnected()==true) {
        ui->statusLabel->setText("Already connected.");
        ui->statusLabel->adjustSize();
        return;
    }

    client->connectToServer();
}
void chatBox::updateStatus(const QString &status)
{

    ui->statusLabel->setText(status);
    ui->statusLabel->adjustSize();

    if (status.startsWith("Connection failed")) {
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
    if (!client->isConnected()) {
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
void chatBox::on_fetchUsersButton_clicked()
{
    if(!canSendMessages()) {
        return;
    }

    client->fetchOnlineUsers();

}
void chatBox::updateOnlineUsers(const QStringList &users)
{
    ui->onlineUsersListWidget->clear();
    ui->onlineUsersListWidget->addItems(users);
}

void chatBox::on_logoutPushButton_clicked()
{
    client ->sendLogoutRequest(user) ;
    MainWindow *MW = new MainWindow(client, nullptr);
    MW->show();

    this->close();
}
void chatBox::on_playTicTacToeButton_clicked()
{
    tictactoe *game = new tictactoe(client, user, this);
    game->show();
}

void chatBox::on_playReactionButton_clicked()
{
    reactiongame *game = new reactiongame(client, user, this);
    game->show();
}
void chatBox::on_showScoreboardButton_clicked()
{
    scoreboard *sb = new scoreboard(client, user, this);
    sb->show();
}

void chatBox::on_createRoomButton_clicked()
{
    if (!canSendMessages()) return;

    QString roomName = ui->roomNameLineEdit->text().trimmed();
    if (roomName.isEmpty()) {
        ui->statusLabel->setText("Enter a room name.");
        return;
    }

    client->sendCreateRoom(roomName, user);
}

void chatBox::on_joinRoomButton_clicked()
{
    if (!canSendMessages()) return;

    QString roomName = ui->roomNameLineEdit->text().trimmed();
    if (roomName.isEmpty()) {
        ui->statusLabel->setText("Enter a room name.");
        return;
    }

    client->sendJoinRoom(roomName, user);
}

void chatBox::on_leaveRoomButton_clicked()
{
    if (!canSendMessages()) return;

    QString roomName = ui->roomNameLineEdit->text().trimmed();
    if (roomName.isEmpty()) {
        ui->statusLabel->setText("Enter a room name.");
        return;
    }

    client->sendLeaveRoom(roomName, user);
}

void chatBox::on_sendRoomMessageButton_clicked()
{
    if (!canSendMessages()) return;

    QString roomName = ui->roomNameLineEdit->text().trimmed();
    QString message = ui->messageLineEdit->text().trimmed();

    if (roomName.isEmpty()) {
        ui->statusLabel->setText("Enter a room name.");
        return;
    }

    if (message.isEmpty()) {
        ui->statusLabel->setText("Enter a message.");
        return;
    }

    client->sendRoomMessage(roomName, user, message);
    ui->chatTextEdit->append("[" + roomName + "] " + user + ": " + message);
    ui->messageLineEdit->clear();
}

void chatBox::on_fetchRoomsButton_clicked()
{
    if (!canSendMessages()) return;

    client->fetchRooms();
}

void chatBox::on_fetchRoomUsersButton_clicked()
{
    if (!canSendMessages()) return;

    QString roomName = ui->roomNameLineEdit->text().trimmed();
    if (roomName.isEmpty()) {
        ui->statusLabel->setText("Enter a room name.");
        return;
    }

    client->fetchRoomUsers(roomName);
}

void chatBox::handleCreateRoomResult(bool success, QString message)
{
    if (success) {
        QMessageBox::information(this, "Success", message);
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}

void chatBox::handleJoinRoomResult(bool success, QString message)
{
    if (success) {
        QMessageBox::information(this, "Success", message);
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}

void chatBox::handleLeaveRoomResult(bool success, QString message)
{
    if (success) {
        QMessageBox::information(this, "Success", message);
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}

void chatBox::updateRooms(const QStringList &rooms)
{
    ui->roomsListWidget->clear();
    ui->roomsListWidget->addItems(rooms);
}

void chatBox::updateRoomUsers(QString room, QStringList users)
{
    ui->roomUsersListWidget->clear();
    ui->roomUsersListWidget->addItems(users);
    ui->statusLabel->setText("Users in room: " + room);
}

void chatBox::handleRoomMessage(QString room, QString sender, QString text)
{
    ui->chatTextEdit->append("[" + room + "] " + sender + ": " + text);
}
