#include "networkclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

NetworkClient::NetworkClient(QObject *parent)
    : QObject(parent), socket(new QTcpSocket(this))
{

    connect(socket, &QTcpSocket::connected,this, &NetworkClient::onConnected);

    connect(socket, &QTcpSocket::errorOccurred,this, &NetworkClient::onErrorOccurred);

    connect(socket, &QTcpSocket::readyRead, this, &NetworkClient::onReadyRead);
}

void NetworkClient::connectToServer()
{
    if (socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Already connected to server.";
        return;
    }

    if (socket->state() == QAbstractSocket::ConnectingState) {
        qDebug() << "Connection already in progress.";
        return;
    }

    emit statusChanged("Connecting to server...");
    socket->connectToHost("127.0.0.1", 54321);
}

bool NetworkClient::isConnected() const
{
    return socket->state()== QAbstractSocket::ConnectedState ;
}

void NetworkClient::sendSigninRequest( const QString &username, const QString &password )
{
    if (username.trimmed().isEmpty()) {
        emit statusChanged("Username cannot be empty.");
        return;
    }

    if (password.trimmed().isEmpty()) {
        emit statusChanged("Username cannot be empty.");
        return;
    }


    QJsonObject json;
    json["type"] = "signup";
    json["username"] = username;
    json["password"] = password ;

    QJsonDocument doc(json);
    socket->write(doc.toJson(QJsonDocument::Compact));
    socket->write("\n");

    emit statusChanged("Sign up request sent.");
}

void NetworkClient::sendLoginRequest(const QString &username, const QString &password)
{
    if (username.trimmed().isEmpty()) {
        emit statusChanged("Username cannot be empty.");
        return;
    }

    if (password.trimmed().isEmpty()) {
        emit statusChanged("Password cannot be empty.");
        return;
    }

    QJsonObject json;
    json["type"] = "login";
    json["username"] = username;
    json["password"] = password;

    sendJsonMessage(json);
    emit statusChanged("Login request sent.");
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

void NetworkClient::sendJsonMessage(const QJsonObject &message)
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        emit statusChanged("Not connected to server.");
        return;
    }

    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');

    socket->write(data);
}
void NetworkClient::sendTestMessage()
{
    QJsonObject payload;
    payload["text"] = "Hello from client";

    QJsonObject message;
    message["type"] = "sendMessage";
    message["sender"] = "Sandra";
    message["payload"] = payload;

    sendJsonMessage(message);
}
void NetworkClient::sendChatMessage(const QString &sender, const QString &text)
{
    if (sender.trimmed().isEmpty()) {
        emit statusChanged("Username cannot be empty.");
        return;
    }

    if (text.trimmed().isEmpty()) {
        emit statusChanged("Message cannot be empty.");
        return;
    }

    QJsonObject payload;
    payload["text"] = text;

    QJsonObject message;
    message["type"] = "sendMessage";
    message["sender"] = sender;
    message["payload"] = payload;

    sendJsonMessage(message);
    emit statusChanged("Message sent.");
}
void NetworkClient::sendPrivateMessage(const QString &sender, const QString &receiver, const QString &text)
{
    if (sender.trimmed().isEmpty()) {
        emit statusChanged("Username cannot be empty.");
        return;
    }

    if (receiver.trimmed().isEmpty()) {
        emit statusChanged("Recipient cannot be empty.");
        return;
    }

    if (text.trimmed().isEmpty()) {
        emit statusChanged("Message cannot be empty.");
        return;
    }

    QJsonObject payload;
    payload["receiver"] = receiver;
    payload["text"] = text;

    QJsonObject message;
    message["type"] = "privateMessage";
    message["sender"] = sender;
    message["payload"] = payload;

    sendJsonMessage(message);
    emit statusChanged("Private message sent.");
}
void NetworkClient::onReadyRead()
{
    QByteArray data = socket->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return;
    }

    QJsonObject obj = doc.object();

    QString type = obj["type"].toString();
    QString sender = obj["sender"].toString();
    QString status = obj["status"].toString() ;
    QString message = obj["message"].toString() ;

    if (type == "signupResult")
    {
        if(status == "success")
        {
            emit signupResult(true,message) ;
        }
        else
        {
            emit signupResult(false,message) ;
        }
    }

    if(type== "loginResult")
    {
        if(status=="success")
        {
            emit loginResult(true,message) ;
        }
        else
        {
            emit loginResult(false,message) ;
        }
    }

    if (type == "sendMessage") {
        QJsonObject payload = obj["payload"].toObject();
        QString text = payload["text"].toString();

        emit messageReceived(sender + ": " + text);
    }
    else if (type == "privateMessage") {
        QJsonObject payload = obj["payload"].toObject();
        QString text = payload["text"].toString();
        QString receiver = payload["receiver"].toString();

        emit messageReceived("[Private] " + sender + " -> " + receiver + ": " + text);
    }
    else if (type == "onlineUsersResponse") {
        QJsonArray usersArray = obj["payload"].toArray();
        QStringList users;

        for (const QJsonValue &value : std::as_const(usersArray)) {
            users.append(value.toString());
        }

        emit onlineUsersReceived(users);
    }
}

void NetworkClient::fetchOnlineUsers()
{
    QJsonObject message;
    message["type"] = "fetchOnlineUsers";
    message["sender"] = "";
    message["payload"] = "Request online users";

    sendJsonMessage(message);
    emit statusChanged("Online users request sent.");
}

void NetworkClient::sendLogoutRequest(const QString& username)
{
    QJsonObject object;
    object["type"] = "logout";
    object["username"] = username;;

    QJsonDocument doc(object);
    socket->write(doc.toJson(QJsonDocument::Compact));
    socket->write("\n");
}