#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , client(new NetworkClient(this))
    , isLoggedIn(false)
    , isConnected(false)
{
    ui->setupUi(this);

    connect(client, &NetworkClient::statusChanged,
            this, &MainWindow::updateStatus);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    if (isConnected) {
        ui->statusLabel->setText("Already connected.");
        ui->statusLabel->adjustSize();
        return;
    }

    client->connectToServer();
}

void MainWindow::on_loginButton_clicked()
{

    if (!isConnected) {
        ui->statusLabel->setText("Connect to the server first.");
        ui->statusLabel->adjustSize();
        return;
    }

    if(isLoggedIn) {
        ui->statusLabel->setText("Already logged in. .");
        ui->statusLabel->adjustSize();
        return;
    }

    QString username = ui->usernameLineEdit->text();

    if (username.trimmed().isEmpty()) {
        ui->statusLabel->setText("Enter a username first.");
        ui->statusLabel->adjustSize();
        return;
    }

    client->sendLoginRequest(username);
    isLoggedIn = true;
}

void MainWindow::updateStatus(const QString &status)
{
    ui->statusLabel->setText(status);
    ui->statusLabel->adjustSize();

    if (status == "Connected successfully.") {
        isConnected = true;
    }

    if (status.startsWith("Connection failed")) {
        isConnected = false;
        isLoggedIn = false;
    }
}

void MainWindow::on_sendButton_clicked()
{
    if (!isConnected) {
        ui->statusLabel->setText("Connect to the server first.");
        ui->statusLabel->adjustSize();
        return;
    }

    if (!isLoggedIn) {
        ui->statusLabel->setText("You must be logged in first");
        ui->statusLabel->adjustSize();
        return;
    }

    QString username = ui->usernameLineEdit->text();
    QString message = ui->messageLineEdit->text();

    if (username.trimmed().isEmpty()) {
        ui->statusLabel->setText("Enter a username first.");
        ui->statusLabel->adjustSize();
        return;
    }

    if (message.trimmed().isEmpty()) {
        ui->statusLabel->setText("Enter a message first.");
        ui->statusLabel->adjustSize();
        return;
    }

    client->sendChatMessage(username, message);

    ui->chatTextEdit->append(username + ": " + message);
    ui->messageLineEdit->clear();
}

