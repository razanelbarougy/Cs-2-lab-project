QT += core gui widgets network testlib
CONFIG += console
CONFIG += c++17

TEMPLATE = app
TARGET = ChatClient_Tests

SOURCES += \
chatbox.cpp \
    log_in.cpp \
    mainwindow.cpp \
    networkclient.cpp \
    Client_Tests.cpp \
    tictactoe.cpp \
    reactiongame.cpp \
    scoreboard.cpp
HEADERS += \
    chatbox.h \
    log_in.h \
    mainwindow.h \
    mock_networkclient.h \
    networkclient.h \
    tictactoe.h \
    reactiongame.h \
    scoreboard.h

FORMS += \
    chatbox.ui \
    log_in.ui \
    mainwindow.ui \
    tictactoe.ui \
    reactiongame.ui \
    scoreboard.ui