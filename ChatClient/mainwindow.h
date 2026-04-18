#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "networkclient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void on_loginButton_clicked();
    void updateStatus(const QString &status);
    void on_sendButton_clicked();

private:
    Ui::MainWindow *ui;
    NetworkClient *client;
    bool isLoggedIn;
    bool isConnected;
};

#endif // MAINWINDOW_H
