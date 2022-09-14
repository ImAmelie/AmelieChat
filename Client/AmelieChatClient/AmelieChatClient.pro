QT       += core gui sql network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatwindow.cc \
    linkwindow.cc \
    loginwindow.cc \
    main.cc \
    mainwindow.cc \
    registerwindow.cc \
    settingswindow.cc \
    tcpsocket.cc \
    utils.cc

HEADERS += \
    chatwindow.h \
    def.h \
    linkwindow.h \
    loginwindow.h \
    mainwindow.h \
    registerwindow.h \
    settingswindow.h \
    tcpsocket.h \
    utils.h

FORMS += \
    chatwindow.ui \
    linkwindow.ui \
    loginwindow.ui \
    mainwindow.ui \
    registerwindow.ui \
    settingswindow.ui

TRANSLATIONS += \
    zh_CN.ts \
    en_US.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# hiredis
INCLUDEPATH += "D:\Develop\lib\hiredis\include"
LIBS += -L"D:\Develop\lib\hiredis\lib" -lhiredis
# OpenCV
INCLUDEPATH += "D:\Develop\OpenCV\opencv\build\include"
Debug:LIBS += -L"D:\Develop\OpenCV\opencv\build\x64\vc15\lib" -lopencv_world460d
Release:LIBS += -L"D:\Develop\OpenCV\opencv\build\x64\vc15\lib" -lopencv_world460

# MSVC解决UTF-8中文乱码问题
QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
