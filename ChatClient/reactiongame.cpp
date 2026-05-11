#include "reactiongame.h"
#include "networkclient.h"
#include "ui_reactiongame.h"

reactiongame::reactiongame(NetworkClient *client, const QString &username, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::reactiongame)
    , client(client)
    , username(username)
    , gameStarted(false)
{
    ui->setupUi(this);

    ui->reactButton->setEnabled(false);

    connect(client, &NetworkClient::reactionGoSignal, this, &reactiongame::handleGoSignal);
    connect(client, &NetworkClient::reactionGameEnded, this, &reactiongame::handleGameEnd);

    client->sendReactionGameJoin(username);
}

reactiongame::~reactiongame()
{
    delete ui;
}

void reactiongame::on_reactButton_clicked()
{
    if (!gameStarted) return;

    gameStarted = false;
    ui->reactButton->setEnabled(false);
    ui->statusLabel->setText("Reacted! Waiting for result...");

    client->sendReactionResponse(username);
}

void reactiongame::handleGoSignal()
{
    gameStarted = true;
    signalTime = QDateTime::currentDateTime();
    ui->reactButton->setEnabled(true);
    ui->statusLabel->setText("GO! CLICK NOW!");
}

void reactiongame::handleGameEnd(QString winner)
{
    ui->reactButton->setEnabled(false);
    gameStarted = false;

    if (winner == username) {
        ui->statusLabel->setText("You win! You reacted fastest!");
    } else {
        ui->statusLabel->setText("You lose! " + winner + " was faster.");
    }
}