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
    void sendTestMessage();
    void sendChatMessage(const QString &sender, const QString &text);
    void sendPrivateMessage(const QString &sender, const QString &reciever, const QString &text);

signals:
    void statusChanged(const QString &status);
    void messageReceived(const QString &message);

private slots:
    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();

private:
    void sendJsonMessage(const QJsonObject &message);
    QTcpSocket *socket;
};

#endif // NETWORKCLIENT_H
