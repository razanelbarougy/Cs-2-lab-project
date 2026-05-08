#pragma once
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <gmock/gmock.h>

// We define a minimal abstract interface so GoogleMock can mock it.
// In production code, NetworkClient already has these methods – the mock
// replaces the real TCP socket with fake in-process behaviour.

class INetworkClient : public QObject {
  Q_OBJECT
public:
  explicit INetworkClient(QObject *parent = nullptr) : QObject(parent) {}
  virtual ~INetworkClient() = default;

  virtual void connectToServer() = 0;
  virtual bool isConnected() const = 0;
  virtual void sendSigninRequest(const QString &username,
                                 const QString &password) = 0;
  virtual void sendLoginRequest(const QString &username,
                                const QString &password) = 0;
  virtual void sendLogoutRequest(const QString &username) = 0;
  virtual void sendChatMessage(const QString &sender, const QString &text) = 0;
  virtual void sendPrivateMessage(const QString &sender,
                                  const QString &receiver,
                                  const QString &text) = 0;
  virtual void fetchOnlineUsers() = 0;

signals:
  void statusChanged(const QString &status);
  void messageReceived(const QString &message);
  void onlineUsersReceived(const QStringList &users);
  void signupResult(bool success, QString message);
  void loginResult(bool success, QString message);
};

class MockNetworkClient : public INetworkClient {
  Q_OBJECT
public:
  explicit MockNetworkClient(QObject *parent = nullptr)
      : INetworkClient(parent) {}

  MOCK_METHOD(void, connectToServer, (), (override));
  MOCK_METHOD(bool, isConnected, (), (const, override));
  MOCK_METHOD(void, sendSigninRequest, (const QString &, const QString &),
              (override));
  MOCK_METHOD(void, sendLoginRequest, (const QString &, const QString &),
              (override));
  MOCK_METHOD(void, sendLogoutRequest, (const QString &), (override));
  MOCK_METHOD(void, sendChatMessage, (const QString &, const QString &),
              (override));
  MOCK_METHOD(void, sendPrivateMessage,
              (const QString &, const QString &, const QString &), (override));
  MOCK_METHOD(void, fetchOnlineUsers, (), (override));

  // ── helpers that let tests fire Qt signals from the mock ──────────────
  void emitSignupResult(bool ok, const QString &msg) {
    emit signupResult(ok, msg);
  }
  void emitLoginResult(bool ok, const QString &msg) {
    emit loginResult(ok, msg);
  }
  void emitStatusChanged(const QString &s) { emit statusChanged(s); }
  void emitMessageReceived(const QString &m) { emit messageReceived(m); }
  void emitOnlineUsersReceived(const QStringList &users) {
    emit onlineUsersReceived(users);
  }
};
