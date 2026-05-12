#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class NetworkClient : public QObject
{
    Q_OBJECT

public:
    NetworkClient(QObject *parent = nullptr);

    void connectToServer();
    void sendSigninRequest( const QString &username, const QString &password) ;
    void sendLoginRequest(const QString &username, const QString &password);
    void sendLogoutRequest(const QString& username);
    void sendTestMessage();
    void sendChatMessage(const QString &sender, const QString &text);
    void sendPrivateMessage(const QString &sender, const QString &reciever, const QString &text);
    void fetchOnlineUsers();
    void sendTicTacToeJoin(const QString &username);
    void sendTicTacToeMove(const QString &username, int row, int col);
    void sendReactionGameJoin(const QString &username);
    void sendReactionResponse(const QString &username);
    void sendFetchScoreboard();
    void sendCreateRoom(const QString &roomName, const QString &username);
    void sendJoinRoom(const QString &roomName, const QString &username);
    void sendLeaveRoom(const QString &roomName, const QString &username);
    void sendRoomMessage(const QString &roomName, const QString &username, const QString &text);
    void fetchRooms();
    void fetchRoomUsers(const QString &roomName);

    bool isConnected() const;

signals:
    void statusChanged(const QString &status);
    void messageReceived(const QString &message);
    void onlineUsersReceived(const QStringList &users);
    void signupResult(bool success, QString message) ;
    void loginResult(bool success, QString message) ;
    void ticTacToeStarted(QString symbol, QString opponent);
    void ticTacToeUpdated(QStringList board, QString turn);
    void ticTacToeEnded(QString winner);
    void reactionGoSignal();
    void reactionGameEnded(QString winner);
    void scoreboardReceived(QStringList scores);
    void createRoomResult(bool success, QString message);
    void joinRoomResult(bool success, QString message);
    void leaveRoomResult(bool success, QString message);
    void roomMessageReceived(QString room, QString sender, QString text);
    void roomsReceived(QStringList rooms);
    void roomUsersReceived(QString room, QStringList users);


private slots:
    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();
private:
    void sendJsonMessage(const QJsonObject &message);
    QTcpSocket *socket;
};

#endif // NETWORKCLIENT_H
