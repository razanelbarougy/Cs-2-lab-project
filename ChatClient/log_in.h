#ifndef LOG_IN_H
#define LOG_IN_H

#include "chatbox.h"
#include "networkclient.h"
#include <QDialog>
using namespace std;

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
    NetworkClient *client;
    bool isLoggedIn;
    chatBox *chatWindow;

    QString username ;

private slots:
    void on_connectButton_clicked();
    void updateStatus(const QString &status);
    void on_lg_loginPushButton_clicked();
    void handleServerResponse(bool success) ;
};

bool uExists(string username) ;

#endif // LOG_IN_H
