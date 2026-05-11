#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "networkclient.h"
#include <QDialog>

namespace Ui {
class scoreboard;
}

class scoreboard : public QDialog
{
    Q_OBJECT

public:
    explicit scoreboard(NetworkClient *client, const QString &username, QWidget *parent = nullptr);
    ~scoreboard();

private slots:
    void on_refreshButton_clicked();
    void handleScoreboard(QStringList scores);

private:
    Ui::scoreboard *ui;
    NetworkClient *client;
    QString username;
};

#endif // SCOREBOARD_H
