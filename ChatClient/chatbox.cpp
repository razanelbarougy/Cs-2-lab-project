#include "chatbox.h"
#include "mainwindow.h"
#include "reactiongame.h"
#include "scoreboard.h"
#include "tictactoe.h"
#include "ui_chatbox.h"

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
