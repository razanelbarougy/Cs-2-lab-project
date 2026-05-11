QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += core gui widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatbox.cpp \
    log_in.cpp \
    main.cpp \
    mainwindow.cpp \
    networkclient.cpp \
    reactiongame.cpp \
    scoreboard.cpp \
    tictactoe.cpp

HEADERS += \
    chatbox.h \
    log_in.h \
    mainwindow.h \
    networkclient.h \
    reactiongame.h \
    scoreboard.h \
    tictactoe.h

FORMS += \
    chatbox.ui \
    log_in.ui \
    mainwindow.ui \
    reactiongame.ui \
    scoreboard.ui \
    tictactoe.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
