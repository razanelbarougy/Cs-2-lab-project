#ifndef MOCK_NETWORKCLIENT_H
#define MOCK_NETWORKCLIENT_H

#include <QObject>
#include <QString>
#include <QStringList>

class MockNetworkClient : public QObject {
  Q_OBJECT

public:
  explicit MockNetworkClient(QObject *parent = nullptr)
      : QObject(parent), m_connected(false) {}

  void connectToServer() {
    connectCallCount++;
  }

  bool isConnected() const {
    return m_connected;
  }

  void setConnected(bool connected) {
    m_connected = connected;
  }

  void sendSigninRequest(const QString &username,
                         const QString &password) {
    signinCallCount++;
    lastSigninUsername = username;
    lastSigninPassword = password;
  }

  void sendLoginRequest(const QString &username,
                        const QString &password) {
    loginCallCount++;
    lastLoginUsername = username;
    lastLoginPassword = password;
  }

  void sendLogoutRequest(const QString &username) {
    logoutCallCount++;
    lastLogoutUsername = username;
  }

  void sendChatMessage(const QString &sender, const QString &text) {
    chatMessageCallCount++;
    lastChatSender = sender;
    lastChatText = text;
  }

  void sendPrivateMessage(const QString &sender,
                          const QString &receiver,
                          const QString &text) {
    privateMessageCallCount++;
    lastPrivateSender = sender;
    lastPrivateReceiver = receiver;
    lastPrivateText = text;
  }

  void fetchOnlineUsers() {
    fetchOnlineUsersCallCount++;
  }

  void emitSignupResult(bool ok, const QString &msg) {
    emit signupResult(ok, msg);
  }

  void emitLoginResult(bool ok, const QString &msg) {
    emit loginResult(ok, msg);
  }

  void emitStatusChanged(const QString &s) {
    emit statusChanged(s);
  }

  void emitMessageReceived(const QString &m) {
    emit messageReceived(m);
  }

  void emitOnlineUsersReceived(const QStringList &users) {
    emit onlineUsersReceived(users);
  }

  void reset() {
    connectCallCount = 0;
    signinCallCount = 0;
    loginCallCount = 0;
    logoutCallCount = 0;
    chatMessageCallCount = 0;
    privateMessageCallCount = 0;
    fetchOnlineUsersCallCount = 0;
    m_connected = false;
    lastSigninUsername.clear();
    lastSigninPassword.clear();
    lastLoginUsername.clear();
    lastLoginPassword.clear();
    lastLogoutUsername.clear();
    lastChatSender.clear();
    lastChatText.clear();
    lastPrivateSender.clear();
    lastPrivateReceiver.clear();
    lastPrivateText.clear();
  }

  int connectCallCount = 0;
  int signinCallCount = 0;
  int loginCallCount = 0;
  int logoutCallCount = 0;
  int chatMessageCallCount = 0;
  int privateMessageCallCount = 0;
  int fetchOnlineUsersCallCount = 0;

  QString lastSigninUsername;
  QString lastSigninPassword;
  QString lastLoginUsername;
  QString lastLoginPassword;
  QString lastLogoutUsername;
  QString lastChatSender;
  QString lastChatText;
  QString lastPrivateSender;
  QString lastPrivateReceiver;
  QString lastPrivateText;

signals:
  void statusChanged(const QString &status);
  void messageReceived(const QString &message);
  void onlineUsersReceived(const QStringList &users);
  void signupResult(bool success, QString message);
  void loginResult(bool success, QString message);

private:
  bool m_connected;
};

#endif // MOCK_NETWORKCLIENT_H
