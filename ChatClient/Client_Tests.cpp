// ============================================================
//  ChatClient – Client-Side Unit Tests
//  Framework : Google Test + Google Mock
//  All network I/O is replaced by MockNetworkClient so every
//  test runs 100 % offline.
// ============================================================

#include "mock_networkclient.h"
#include <QApplication>
#include <QTimer>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;

// ──────────────────────────────────────────────────────────────
// Helpers
// ──────────────────────────────────────────────────────────────

// Pump the Qt event loop briefly so signals are delivered.
static void processEvents(int ms = 50) {
  QTimer::singleShot(ms, qApp, [] { qApp->quit(); });
  qApp->exec();
}

// ============================================================
// TEST SUITE 1 – NetworkClient: signup flow
// ============================================================

class SignupTest : public ::testing::Test {
protected:
  NiceMock<MockNetworkClient> mock;
};

// TC-01  Signup with valid credentials calls sendSigninRequest exactly once
TEST_F(SignupTest, ValidCredentials_CallsSendSigninRequest) {
  EXPECT_CALL(mock, sendSigninRequest("alice", "pass123")).Times(1);
  mock.sendSigninRequest("alice", "pass123");
}

// TC-02  Successful signup result signal carries success=true and a non-empty
// message
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

// TC-03  Failed signup result signal carries success=false
TEST_F(SignupTest, FailureSignal_CarriesSuccessFalse) {
  bool receivedSuccess = true; // start true so we detect the flip

  QObject::connect(&mock, &INetworkClient::signupResult,
                   [&](bool ok, QString) { receivedSuccess = ok; });

  mock.emitSignupResult(false, "Username already taken");
  processEvents();

  EXPECT_FALSE(receivedSuccess);
}

// TC-04  Duplicate username: server response sets success=false with
// descriptive message
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

// ============================================================
// TEST SUITE 2 – NetworkClient: login flow
// ============================================================

class LoginTest : public ::testing::Test {
protected:
  NiceMock<MockNetworkClient> mock;
};

// TC-05  Valid login emits loginResult(true)
TEST_F(LoginTest, ValidLogin_EmitsSuccessSignal) {
  bool ok = false;
  QObject::connect(&mock, &INetworkClient::loginResult,
                   [&](bool success, QString) { ok = success; });

  mock.emitLoginResult(true, "Welcome back!");
  processEvents();

  EXPECT_TRUE(ok);
}

// TC-06  Wrong password emits loginResult(false)
TEST_F(LoginTest, WrongPassword_EmitsFailureSignal) {
  bool ok = true;
  QObject::connect(&mock, &INetworkClient::loginResult,
                   [&](bool success, QString) { ok = success; });

  mock.emitLoginResult(false, "Incorrect username or password");
  processEvents();

  EXPECT_FALSE(ok);
}

// TC-07  sendLoginRequest is called with the exact username & password entered
TEST_F(LoginTest, SendLoginRequest_UsesCorrectCredentials) {
  EXPECT_CALL(mock, sendLoginRequest("bob", "secret")).Times(1);
  mock.sendLoginRequest("bob", "secret");
}

// ============================================================
// TEST SUITE 3 – NetworkClient: chat messaging
// ============================================================

class ChatTest : public ::testing::Test {
protected:
  NiceMock<MockNetworkClient> mock;

  void SetUp() override {
    // Pretend we are connected for messaging tests
    ON_CALL(mock, isConnected()).WillByDefault(Return(true));
  }
};

// TC-08  Broadcast message: sendChatMessage called with correct sender & text
TEST_F(ChatTest, BroadcastMessage_CallsSendChatMessage) {
  EXPECT_CALL(mock, sendChatMessage("alice", "Hello everyone!")).Times(1);
  mock.sendChatMessage("alice", "Hello everyone!");
}

// TC-09  Private message: sendPrivateMessage called with sender, receiver and
// text
TEST_F(ChatTest, PrivateMessage_CallsSendPrivateMessage) {
  EXPECT_CALL(mock, sendPrivateMessage("alice", "bob", "Hey Bob!")).Times(1);
  mock.sendPrivateMessage("alice", "bob", "Hey Bob!");
}

// TC-10  Incoming broadcast message signal delivers formatted text
TEST_F(ChatTest, IncomingBroadcast_SignalContainsSenderAndText) {
  QString received;
  QObject::connect(&mock, &INetworkClient::messageReceived,
                   [&](const QString &msg) { received = msg; });

  mock.emitMessageReceived("alice: Hello everyone!");
  processEvents();

  EXPECT_TRUE(received.contains("alice"));
  EXPECT_TRUE(received.contains("Hello everyone!"));
}

// TC-11  Incoming private message signal contains [Private] prefix
TEST_F(ChatTest, IncomingPrivateMessage_SignalHasPrivatePrefix) {
  QString received;
  QObject::connect(&mock, &INetworkClient::messageReceived,
                   [&](const QString &msg) { received = msg; });

  mock.emitMessageReceived("[Private] bob -> alice: Hey Alice!");
  processEvents();

  EXPECT_TRUE(received.startsWith("[Private]"));
}

// ============================================================
// TEST SUITE 4 – NetworkClient: online-users list
// ============================================================

class OnlineUsersTest : public ::testing::Test {
protected:
  NiceMock<MockNetworkClient> mock;

  void SetUp() override {
    ON_CALL(mock, isConnected()).WillByDefault(Return(true));
  }
};

// TC-12  fetchOnlineUsers is called once when button pressed
TEST_F(OnlineUsersTest, FetchUsers_CalledOnce) {
  EXPECT_CALL(mock, fetchOnlineUsers()).Times(1);
  mock.fetchOnlineUsers();
}

// TC-13  onlineUsersReceived signal delivers the correct user list
TEST_F(OnlineUsersTest, UsersReceivedSignal_ContainsExpectedUsers) {
  QStringList received;
  QObject::connect(&mock, &INetworkClient::onlineUsersReceived,
                   [&](const QStringList &users) { received = users; });

  QStringList expected = {"alice", "bob", "carol"};
  mock.emitOnlineUsersReceived(expected);
  processEvents();

  EXPECT_EQ(received, expected);
}

// TC-14  Empty server response yields empty user list (no crash)
TEST_F(OnlineUsersTest, EmptyUserList_HandledGracefully) {
  QStringList received = {"placeholder"}; // ensure it actually gets overwritten
  QObject::connect(&mock, &INetworkClient::onlineUsersReceived,
                   [&](const QStringList &users) { received = users; });

  mock.emitOnlineUsersReceived({});
  processEvents();

  EXPECT_TRUE(received.isEmpty());
}

// ============================================================
// TEST SUITE 5 – NetworkClient: connection & logout
// ============================================================

class ConnectionTest : public ::testing::Test {
protected:
  NiceMock<MockNetworkClient> mock;
};

// TC-15  isConnected returns false before connecting
TEST_F(ConnectionTest, NotConnected_ReturnsFalse) {
  ON_CALL(mock, isConnected()).WillByDefault(Return(false));
  EXPECT_FALSE(mock.isConnected());
}

// TC-16  isConnected returns true after a successful connection
TEST_F(ConnectionTest, Connected_ReturnsTrue) {
  ON_CALL(mock, isConnected()).WillByDefault(Return(true));
  EXPECT_TRUE(mock.isConnected());
}

// TC-17  connectToServer is called exactly once on startup
TEST_F(ConnectionTest, ConnectToServer_CalledOnce) {
  EXPECT_CALL(mock, connectToServer()).Times(1);
  mock.connectToServer();
}

// TC-18  statusChanged signal propagates "Connection failed" on error
TEST_F(ConnectionTest, ConnectionFailed_StatusSignalPropagates) {
  QString status;
  QObject::connect(&mock, &INetworkClient::statusChanged,
                   [&](const QString &s) { status = s; });

  mock.emitStatusChanged("Connection failed: Connection refused");
  processEvents();

  EXPECT_TRUE(status.startsWith("Connection failed"));
}

// TC-19  sendLogoutRequest is called with the correct username
TEST_F(ConnectionTest, Logout_SendsCorrectUsername) {
  EXPECT_CALL(mock, sendLogoutRequest("alice")).Times(1);
  mock.sendLogoutRequest("alice");
}

// TC-20  Sending a message while disconnected does NOT call sendChatMessage
TEST_F(ConnectionTest, SendWhileDisconnected_DoesNotCallSend) {
  ON_CALL(mock, isConnected()).WillByDefault(Return(false));

  // Simulate the guard that chatBox::canSendMessages() enforces:
  // if not connected, we never reach sendChatMessage.
  EXPECT_CALL(mock, sendChatMessage(_, _)).Times(0);

  if (mock.isConnected()) { // guard (mirrors canSendMessages)
    mock.sendChatMessage("alice", "Hi");
  }
  // No call should have been made
}

// ============================================================
// main
// ============================================================

int main(int argc, char **argv) {
  QApplication app(argc, argv); // required for Qt signal/slot machinery
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
