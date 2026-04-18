#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->ticTacToeButton, &QPushButton::clicked, this, [](){
        qDebug() <<
            R"({
"type": "tictactoe_move",
"player": "abdullah",
"row": 1,
"col": 1
})";
    });

    connect(ui->reactionGameButton, &QPushButton::clicked, this, [](){
        qDebug() <<
            R"({
"type": "reaction_start",
"player": "abdullah"
})";
    });

    connect(ui->scoreboardButton, &QPushButton::clicked, this, [](){
        qDebug() <<
            R"({
"type": "get_scoreboard"
})";
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}
