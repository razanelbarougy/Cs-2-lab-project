#include "mock_networkclient.h"
#include <QCoreApplication>
#include <QSignalSpy>
#include <QTest>

class ClientTests : public QObject {
  Q_OBJECT

private:
  MockNetworkClient mock;

private slots:
  void init() {
    mock.reset();
  }

  void testValidCredentialsCallsSendSigninRequest() {
    mock.sendSigninRequest("alice", "pass123");
    QCOMPARE(mock.signinCallCount, 1);
    QCOMPARE(mock.lastSigninUsername, QStringLiteral("alice"));
    QCOMPARE(mock.lastSigninPassword, QStringLiteral("pass123"));
  }

  void testSuccessSignalCarriesCorrectPayload() {
    QSignalSpy spy(&mock, &MockNetworkClient::signupResult);
    mock.emitSignupResult(true, QStringLiteral("Account created"));

    QVERIFY(spy.wait(100));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toBool(), true);
    QCOMPARE(spy[0][1].toString(), QStringLiteral("Account created"));
  }

  void testFailureSignalCarriesSuccessFalse() {
    QSignalSpy spy(&mock, &MockNetworkClient::signupResult);
    mock.emitSignupResult(false, QStringLiteral("Username already taken"));

    QVERIFY(spy.wait(100));
    QCOMPARE(spy[0][0].toBool(), false);
  }

  void testDuplicateUsernameServerReturnsFalseWithMessage() {
    QSignalSpy spy(&mock, &MockNetworkClient::signupResult);
    mock.emitSignupResult(false, QStringLiteral("Username already exists"));

    QVERIFY(spy.wait(100));
    QCOMPARE(spy[0][0].toBool(), false);
    QVERIFY(!spy[0][1].toString().isEmpty());
  }

  void testValidLoginEmitsSuccessSignal() {
    QSignalSpy spy(&mock, &MockNetworkClient::loginResult);
    mock.emitLoginResult(true, QStringLiteral("Welcome back!"));

    QVERIFY(spy.wait(100));
    QCOMPARE(spy[0][0].toBool(), true);
  }

  void testWrongPasswordEmitsFailureSignal() {
    QSignalSpy spy(&mock, &MockNetworkClient::loginResult);
    mock.emitLoginResult(false, QStringLiteral("Incorrect username or password"));

    QVERIFY(spy.wait(100));
    QCOMPARE(spy[0][0].toBool(), false);
  }

  void testSendLoginRequestUsesCorrectCredentials() {
    mock.sendLoginRequest("bob", "secret");
    QCOMPARE(mock.loginCallCount, 1);
    QCOMPARE(mock.lastLoginUsername, QStringLiteral("bob"));
    QCOMPARE(mock.lastLoginPassword, QStringLiteral("secret"));
  }

  void testBroadcastMessageCallsSendChatMessage() {
    mock.sendChatMessage("alice", "Hello everyone!");
    QCOMPARE(mock.chatMessageCallCount, 1);
    QCOMPARE(mock.lastChatSender, QStringLiteral("alice"));
    QCOMPARE(mock.lastChatText, QStringLiteral("Hello everyone!"));
  }

  void testPrivateMessageCallsSendPrivateMessage() {
    mock.sendPrivateMessage("alice", "bob", "Hey Bob!");
    QCOMPARE(mock.privateMessageCallCount, 1);
    QCOMPARE(mock.lastPrivateSender, QStringLiteral("alice"));
    QCOMPARE(mock.lastPrivateReceiver, QStringLiteral("bob"));
    QCOMPARE(mock.lastPrivateText, QStringLiteral("Hey Bob!"));
  }

  void testIncomingBroadcastSignalContainsSenderAndText() {
    QSignalSpy spy(&mock, &MockNetworkClient::messageReceived);
    mock.emitMessageReceived(QStringLiteral("alice: Hello everyone!"));

    QVERIFY(spy.wait(100));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toString().contains("alice"), true);
    QCOMPARE(spy[0][0].toString().contains("Hello everyone!"), true);
  }

  void testIncomingPrivateMessageSignalHasPrivatePrefix() {
    QSignalSpy spy(&mock, &MockNetworkClient::messageReceived);
    mock.emitMessageReceived(QStringLiteral("[Private] bob -> alice: Hey Alice!"));

    QVERIFY(spy.wait(100));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toString().startsWith("[Private]"), true);
  }

  void testFetchUsersCalledOnce() {
    mock.fetchOnlineUsers();
    QCOMPARE(mock.fetchOnlineUsersCallCount, 1);
  }

  void testUsersReceivedSignalContainsExpectedUsers() {
    QSignalSpy spy(&mock, &MockNetworkClient::onlineUsersReceived);
    const QStringList expected = {QStringLiteral("alice"), QStringLiteral("bob"), QStringLiteral("carol")};
    mock.emitOnlineUsersReceived(expected);

    QVERIFY(spy.wait(100));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toStringList(), expected);
  }

  void testEmptyUserListHandledGracefully() {
    QSignalSpy spy(&mock, &MockNetworkClient::onlineUsersReceived);
    mock.emitOnlineUsersReceived({});

    QVERIFY(spy.wait(100));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy[0][0].toStringList().isEmpty(), true);
  }

  void testNotConnectedReturnsFalse() {
    mock.setConnected(false);
    QCOMPARE(mock.isConnected(), false);
  }

  void testConnectedReturnsTrue() {
    mock.setConnected(true);
    QCOMPARE(mock.isConnected(), true);
  }
};

QTEST_MAIN(ClientTests)
#include "Client_Tests.moc"
