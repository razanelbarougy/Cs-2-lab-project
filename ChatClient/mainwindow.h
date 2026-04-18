#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "chatbox.h"
#include "log_in.h"
#include "networkclient.h"
#include <QMainWindow>
#include <QtNetwork/qtcpsocket.h>

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
    ~MainWindow() override;

private slots:

    void on_signupButton_clicked();

    void on_mw_login_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    bool isLoggedin ;
    bool isSignedin ;
    bool isConnected ;

    NetworkClient *client;
    Log_in *loginWindow;
    chatBox *chatWindow;

    QString user ;

};

#endif // MAINWINDOW_H