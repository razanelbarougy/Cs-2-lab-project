#include "scoreboard.h"
#include "ui_scoreboard.h"
#include <QMessageBox>

scoreboard::scoreboard(NetworkClient *client, const QString &username, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::scoreboard)
    , client(client)
    , username(username)
{
    ui->setupUi(this);

    connect(client, &NetworkClient::scoreboardReceived, this, &scoreboard::handleScoreboard);

    client->sendFetchScoreboard();
}

scoreboard::~scoreboard()
{
    delete ui;
}

void scoreboard::on_refreshButton_clicked()
{
    ui->scoreListWidget->clear();
    ui->statusLabel->setText("Refreshing...");
    client->sendFetchScoreboard();
}

void scoreboard::handleScoreboard(QStringList scores)
{
    ui->scoreListWidget->clear();
    ui->scoreListWidget->addItems(scores);
    ui->statusLabel->setText("Scoreboard");
}