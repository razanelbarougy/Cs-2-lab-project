#ifndef REACTIONGAME_H
#define REACTIONGAME_H

#include "networkclient.h"
#include <QDialog>
#include <QDateTime>

namespace Ui {
class reactiongame;
}

class reactiongame : public QDialog
{
    Q_OBJECT

public:
    explicit reactiongame(NetworkClient *client, const QString &username, QWidget *parent = nullptr);
    ~reactiongame();

private slots:
    void on_reactButton_clicked();
    void handleGoSignal();
    void handleGameEnd(QString winner);

private:
    Ui::reactiongame *ui;
    NetworkClient *client;
    QString username;
    bool gameStarted;
    QDateTime signalTime;
};

#endif // REACTIONGAME_H
