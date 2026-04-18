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
    void sendLoginRequest(const QString &username);

signals:
    void statusChanged(const QString &status);

private slots:
    void onConnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
};

#endif // NETWORKCLIENT_H
