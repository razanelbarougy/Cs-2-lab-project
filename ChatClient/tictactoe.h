#ifndef TICTACTOE_H
#define TICTACTOE_H

#include "networkclient.h"
#include <QDialog>

namespace Ui {
class tictactoe;
}

class tictactoe : public QDialog
{
    Q_OBJECT

public:
    explicit tictactoe(NetworkClient *client, const QString &username, QWidget *parent = nullptr);
    ~tictactoe();

private slots:
    void on_btn00_clicked();
    void on_btn01_clicked();
    void on_btn02_clicked();
    void on_btn10_clicked();
    void on_btn11_clicked();
    void on_btn12_clicked();
    void on_btn20_clicked();
    void on_btn21_clicked();
    void on_btn22_clicked();

    void handleGameStart(QString symbol, QString opponent);
    void handleBoardUpdate(QStringList board, QString turn);
    void handleGameEnd(QString winner);

private:
    Ui::tictactoe *ui;
    NetworkClient *client;
    QString username;
    QString mySymbol;
    QString currentTurn;
    QStringList board;

    void makeMove(int row, int col);
    void updateBoard();
    void disableAll();
};

#endif // TICTACTOE_H