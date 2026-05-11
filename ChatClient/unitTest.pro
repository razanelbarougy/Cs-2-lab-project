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
    Client_Tests.cpp

HEADERS += \
    chatbox.h \
    log_in.h \
    mainwindow.h \
    mock_networkclient.h \
    networkclient.h

FORMS += \
    chatbox.ui \
    log_in.ui \
    mainwindow.ui
