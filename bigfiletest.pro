QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += console

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    add_link_dialog.cpp \
    encrypt/simple_encrypt.cpp \
    http_thread.cpp \
    main.cpp \
    mainwindow.cpp \
    oxfold_wrapper.cpp \
    page_downloaded.cpp \
    page_downloading.cpp \
    page_shared.cpp \
    page_sharing.cpp \
    sharing_dialog.cpp

HEADERS += \
    ZeroTierSockets.h \
    add_link_dialog.h \
    encrypt/simple_encrypt.h \
    http_thread.h \
    httplib.h \
    mainwindow.h \
    oxfold_wrapper.h \
    page_downloaded.h \
    page_downloading.h \
    page_shared.h \
    page_sharing.h \
    sharing_dialog.h

FORMS += \
    add_link_dialog.ui \
    mainwindow.ui \
    page_downloaded.ui \
    page_downloading.ui \
    page_shared.ui \
    page_sharing.ui \
    sharing_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    res.qrc

macx: LIBS += -L$$PWD/libs/macos-x86_64/ -lzt

INCLUDEPATH += $$PWD/libs/macos-x86_64
DEPENDPATH += $$PWD/libs/macos-x86_64

macx: PRE_TARGETDEPS += $$PWD/libs/macos-x86_64/libzt.a


win32: LIBS += -LZ:/test/3rdparty/lib/ -lzt-shared
win32: LIBS += -LZ:/test/3rdparty/lib/ -lztcore

# INCLUDEPATH += Z:/test//3rdparty/include
DEPENDPATH  += Z:/test//3rdparty/lib
