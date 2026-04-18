#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , client(new NetworkClient(this))
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
    client->connectToServer();
}

void MainWindow::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    client->sendLoginRequest(username);
}

void MainWindow::updateStatus(const QString &status)
{
    ui->statusLabel->setText(status);
    ui->statusLabel->adjustSize();
}
