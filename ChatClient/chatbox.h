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
    void on_logoutPushButton_clicked();
    void on_playTicTacToeButton_clicked();
    void on_playReactionButton_clicked();
    void on_showScoreboardButton_clicked();
    void on_createRoomButton_clicked();
    void on_joinRoomButton_clicked();
    void on_leaveRoomButton_clicked();
    void on_sendRoomMessageButton_clicked();
    void on_fetchRoomsButton_clicked();
    void on_fetchRoomUsersButton_clicked();
    void handleCreateRoomResult(bool success, QString message);
    void handleJoinRoomResult(bool success, QString message);
    void handleLeaveRoomResult(bool success, QString message);
    void updateRooms(const QStringList &rooms);
    void updateRoomUsers(QString room, QStringList users);
    void handleRoomMessage(QString room, QString sender, QString text);
};

#endif // CHATBOX_H
