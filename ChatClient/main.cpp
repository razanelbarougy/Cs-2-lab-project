#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    NetworkClient *client = new NetworkClient ;
    client -> connectToServer() ;

    MainWindow w(client);
    w.show();
    return a.exec();
}
