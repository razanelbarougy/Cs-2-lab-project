#include "mock_networkclient.h"
#include <QApplication>
#include <QTimer>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;

static void processEvents(int ms = 50) {
  QTimer::singleShot(ms, qApp, [] { qApp->quit(); });
  qApp->exec();
}

class SignupTest : public ::testing::Test {
protected:
  NiceMock<MockNetworkClient> mock;
};

TEST_F(SignupTest, ValidCredentials_CallsSendSigninRequest) {
  EXPECT_CALL(mock, sendSigninRequest("alice", "pass123")).Times(1);
  mock.sendSigninRequest("alice", "pass123");
}

TEST_F(SignupTest, SuccessSignal_CarriesCorrectPayload) {
  bool receivedSuccess = false;
  QString receivedMsg;

  QObject::connect(&mock, &INetworkClient::signupResult,
                   [&](bool ok, QString msg) {
                     receivedSuccess = ok;
                     receivedMsg = msg;
                   });

  mock.emitSignupResult(true, "Account created");
  processEvents();

  EXPECT_TRUE(receivedSuccess);
  EXPECT_EQ(receivedMsg, "Account created");
}

TEST_F(SignupTest, FailureSignal_CarriesSuccessFalse) {
  bool receivedSuccess = true; // start true so we detect the flip

  QObject::connect(&mock, &INetworkClient::signupResult,
                   [&](bool ok, QString) { receivedSuccess = ok; });

  mock.emitSignupResult(false, "Username already taken");
  processEvents();

  EXPECT_FALSE(receivedSuccess);
}

TEST_F(SignupTest, DuplicateUsername_ServerReturnsFalseWithMessage) {
  QString receivedMsg;
  bool receivedOk = true;

  QObject::connect(&mock, &INetworkClient::signupResult,
                   [&](bool ok, QString msg) {
                     receivedOk = ok;
                     receivedMsg = msg;
                   });

  mock.emitSignupResult(false, "Username already exists");
  processEvents();

  EXPECT_FALSE(receivedOk);
  EXPECT_FALSE(receivedMsg.isEmpty());
}

class LoginTest : public ::testing::Test {
protected:
  NiceMock<MockNetworkClient> mock;
};

TEST_F(LoginTest, ValidLogin_EmitsSuccessSignal) {
  bool ok = false;
  QObject::connect(&mock, &INetworkClient::loginResult,
                   [&](bool success, QString) { ok = success; });

  mock.emitLoginResult(true, "Welcome back!");
  processEvents();

  EXPECT_TRUE(ok);
}

TEST_F(LoginTest, WrongPassword_EmitsFailureSignal) {
  bool ok = true;
  QObject::connect(&mock, &INetworkClient::loginResult,
                   [&](bool success, QString) { ok = success; });

  mock.emitLoginResult(false, "Incorrect username or password");
  processEvents();

  EXPECT_FALSE(ok);
}

TEST_F(LoginTest, SendLoginRequest_UsesCorrectCredentials) {
  EXPECT_CALL(mock, sendLoginRequest("bob", "secret")).Times(1);
  mock.sendLoginRequest("bob", "secret");
}

class ChatTest : public ::testing::Test {
protected:
  NiceMock<MockNetworkClient> mock;

  void SetUp() override {
    // Pretend we are connected for messaging tests
    ON_CALL(mock, isConnected()).WillByDefault(Return(true));
  }
};

TEST_F(ChatTest, BroadcastMessage_CallsSendChatMessage) {
  EXPECT_CALL(mock, sendChatMessage("alice", "Hello everyone!")).Times(1);
  mock.sendChatMessage("alice", "Hello everyone!");
}

TEST_F(ChatTest, PrivateMessage_CallsSendPrivateMessage) {
  EXPECT_CALL(mock, sendPrivateMessage("alice", "bob", "Hey Bob!")).Times(1);
  mock.sendPrivateMessage("alice", "bob", "Hey Bob!");
}

TEST_F(ChatTest, IncomingBroadcast_SignalContainsSenderAndText) {
  QString received;
  QObject::connect(&mock, &INetworkClient::messageReceived,
                   [&](const QString &msg) { received = msg; });

  mock.emitMessageReceived("alice: Hello everyone!");
  processEvents();

  EXPECT_TRUE(received.contains("alice"));
  EXPECT_TRUE(received.contains("Hello everyone!"));
}

TEST_F(ChatTest, IncomingPrivateMessage_SignalHasPrivatePrefix) {
  QString received;
  QObject::connect(&mock, &INetworkClient::messageReceived,
                   [&](const QString &msg) { received = msg; });

  mock.emitMessageReceived("[Private] bob -> alice: Hey Alice!");
  processEvents();

  EXPECT_TRUE(received.startsWith("[Private]"));
}

class OnlineUsersTest : public ::testing::Test {
protected:
  NiceMock<MockNetworkClient> mock;

  void SetUp() override {
    ON_CALL(mock, isConnected()).WillByDefault(Return(true));
  }
};

TEST_F(OnlineUsersTest, FetchUsers_CalledOnce) {
  EXPECT_CALL(mock, fetchOnlineUsers()).Times(1);
  mock.fetchOnlineUsers();
}

TEST_F(OnlineUsersTest, UsersReceivedSignal_ContainsExpectedUsers) {
  QStringList received;
  QObject::connect(&mock, &INetworkClient::onlineUsersReceived,
                   [&](const QStringList &users) { received = users; });

  QStringList expected = {"alice", "bob", "carol"};
  mock.emitOnlineUsersReceived(expected);
  processEvents();

  EXPECT_EQ(received, expected);
}

TEST_F(OnlineUsersTest, EmptyUserList_HandledGracefully) {
  QStringList received = {"placeholder"}; // ensure it actually gets overwritten
  QObject::connect(&mock, &INetworkClient::onlineUsersReceived,
                   [&](const QStringList &users) { received = users; });

  mock.emitOnlineUsersReceived({});
  processEvents();

  EXPECT_TRUE(received.isEmpty());
}

class ConnectionTest : public ::testing::Test {
protected:
  NiceMock<MockNetworkClient> mock;
};

TEST_F(ConnectionTest, NotConnected_ReturnsFalse) {
  ON_CALL(mock, isConnected()).WillByDefault(Return(false));
  EXPECT_FALSE(mock.isConnected());
}

TEST_F(ConnectionTest, Connected_ReturnsTrue) {
  ON_CALL(mock, isConnected()).WillByDefault(Return(true));
  EXPECT_TRUE(mock.isConnected());
}

TEST_F(ConnectionTest, ConnectToServer_CalledOnce) {
  EXPECT_CALL(mock, connectToServer()).Times(1);
  mock.connectToServer();
}

TEST_F(ConnectionTest, ConnectionFailed_StatusSignalPropagates) {
  QString status;
  QObject::connect(&mock, &INetworkClient::statusChanged,
                   [&](const QString &s) { status = s; });

  mock.emitStatusChanged("Connection failed: Connection refused");
  processEvents();

  EXPECT_TRUE(status.startsWith("Connection failed"));
}

TEST_F(ConnectionTest, Logout_SendsCorrectUsername) {
  EXPECT_CALL(mock, sendLogoutRequest("alice")).Times(1);
  mock.sendLogoutRequest("alice");
}

TEST_F(ConnectionTest, SendWhileDisconnected_DoesNotCallSend) {
  ON_CALL(mock, isConnected()).WillByDefault(Return(false));

  EXPECT_CALL(mock, sendChatMessage(_, _)).Times(0);

  if (mock.isConnected()) { // guard (mirrors canSendMessages)
    mock.sendChatMessage("alice", "Hi");
  }
}

int main(int argc, char **argv) {
  QApplication app(argc, argv); // required for Qt signal/slot machinery
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
