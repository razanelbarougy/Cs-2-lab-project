#include "networkclient.h"
#include <QJsonDocument>

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent), socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::connected,
            this, &NetworkClient::onConnected);

    connect(socket, &QTcpSocket::errorOccurred,
            this, &NetworkClient::onErrorOccurred);
}

void NetworkClient::connectToServer()
{
    emit statusChanged("Connecting to server...");
    socket->connectToHost("127.0.0.1", 54321);
}

void NetworkClient::sendLoginRequest(const QString &username)
{
    if (username.trimmed().isEmpty()) {
        emit statusChanged("Username cannot be empty.");
        return;
    }

    if (socket->state() != QAbstractSocket::ConnectedState) {
        emit statusChanged("Not connected to server.");
        return;
    }

    QJsonObject json;
    json["type"] = "login";
    json["sender"] = username;
    json["payload"] = "Login request";

    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');

    socket->write(data);
    emit statusChanged("Login JSON sent.");
}

void NetworkClient::onConnected()
{
    emit statusChanged("Connected successfully.");
}

void NetworkClient::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit statusChanged("Connection failed: " + socket->errorString());
}
