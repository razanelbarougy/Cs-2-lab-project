#ifndef CHATBOX_H
#define CHATBOX_H

#include "networkclient.h"
#include <QDialog>

namespace Ui {
class chatBox;
}

class chatBox : public QDialog
{
    Q_OBJECT

public:
    explicit chatBox(NetworkClient *client, const QString &username, QWidget *parent = nullptr);
    ~chatBox();

private:
    Ui::chatBox *ui;
    bool isLoggedIn;
    bool isConnected;
    bool canSendMessages();
    NetworkClient *client;

    QString user ;

private slots:
    void on_sendButton_clicked();
    void on_privateSendButton_clicked();
    void on_connectButton_clicked();
    void updateStatus(const QString &status);
    void on_fetchUsersButton_clicked();
    void updateOnlineUsers(const QStringList &users);
};

#endif // CHATBOX_H
