#ifndef LOG_IN_H
#define LOG_IN_H

#include "chatbox.h"
#include "networkclient.h"
#include <QDialog>

namespace Ui {
class Log_in;
}

class Log_in : public QDialog
{
    Q_OBJECT

public:
    explicit Log_in(NetworkClient *client, QWidget *parent = nullptr);
    ~Log_in();

private:
    Ui::Log_in *ui;
    bool isConnected;
    NetworkClient *client;
    bool isLoggedIn;
    chatBox *chatWindow;

    QString username ;

private slots:
    void on_loginButton_clicked();
    void on_connectButton_clicked();
    void updateStatus(const QString &status);
};

#endif // LOG_IN_H
