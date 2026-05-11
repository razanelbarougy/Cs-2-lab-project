#include "tictactoe.h"
#include "ui_tictactoe.h"

tictactoe::tictactoe(NetworkClient *client, const QString &username, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::tictactoe)
    , client(client)
    , username(username)
    , mySymbol("")
    , currentTurn("X")
    , board({"","","","","","","","",""})
{
    ui->setupUi(this);

    connect(client, &NetworkClient::ticTacToeStarted, this, &tictactoe::handleGameStart);
    connect(client, &NetworkClient::ticTacToeUpdated, this, &tictactoe::handleBoardUpdate);
    connect(client, &NetworkClient::ticTacToeEnded, this, &tictactoe::handleGameEnd);

    // Send join request
    client->sendTicTacToeJoin(username);
}

tictactoe::~tictactoe()
{
    delete ui;
}

void tictactoe::makeMove(int row, int col)
{
    int index = row * 3 + col;

    if (currentTurn != mySymbol) {
        ui->statusLabel->setText("Not your turn!");
        return;
    }

    if (board[index] != "") {
        ui->statusLabel->setText("Cell already taken!");
        return;
    }

    client->sendTicTacToeMove(username, row, col);
}

void tictactoe::updateBoard()
{
    QList<QPushButton*> buttons = {
        ui->btn00, ui->btn01, ui->btn02,
        ui->btn10, ui->btn11, ui->btn12,
        ui->btn20, ui->btn21, ui->btn22
    };

    for (int i = 0; i < 9; i++) {
        buttons[i]->setText(board[i]);
    }
}

void tictactoe::disableAll()
{
    ui->btn00->setEnabled(false); ui->btn01->setEnabled(false); ui->btn02->setEnabled(false);
    ui->btn10->setEnabled(false); ui->btn11->setEnabled(false); ui->btn12->setEnabled(false);
    ui->btn20->setEnabled(false); ui->btn21->setEnabled(false); ui->btn22->setEnabled(false);
}

void tictactoe::handleGameStart(QString symbol, QString opponent)
{
    mySymbol = symbol;
    currentTurn = "X";
    board = {"","","","","","","","",""};
    updateBoard();
    ui->statusLabel->setText("Game started! You are " + symbol + " vs " + opponent + ". X goes first.");
}

void tictactoe::handleBoardUpdate(QStringList newBoard, QString turn)
{
    board = newBoard;
    currentTurn = turn;
    updateBoard();

    if (turn == mySymbol) {
        ui->statusLabel->setText("Your turn!");
    } else {
        ui->statusLabel->setText("Opponent's turn...");
    }
}

void tictactoe::handleGameEnd(QString winner)
{
    disableAll();
    if (winner == username) {
        ui->statusLabel->setText("You win!");
    } else if (winner == "draw") {
        ui->statusLabel->setText("It's a draw!");
    } else {
        ui->statusLabel->setText("You lose!");
    }
}

void tictactoe::on_btn00_clicked() { makeMove(0, 0); }
void tictactoe::on_btn01_clicked() { makeMove(0, 1); }
void tictactoe::on_btn02_clicked() { makeMove(0, 2); }
void tictactoe::on_btn10_clicked() { makeMove(1, 0); }
void tictactoe::on_btn11_clicked() { makeMove(1, 1); }
void tictactoe::on_btn12_clicked() { makeMove(1, 2); }
void tictactoe::on_btn20_clicked() { makeMove(2, 0); }
void tictactoe::on_btn21_clicked() { makeMove(2, 1); }
void tictactoe::on_btn22_clicked() { makeMove(2, 2); }
