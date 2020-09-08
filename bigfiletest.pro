QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += console

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

################################################
# enable oxfold service, de-comment below code.
# DEFINES += ENABLE_OXFOLD

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    3rd/http/civetweb.c \
    3rd/http/civetweb_main.c \
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
    3rd/http/civetweb/include/civetweb.h \
    3rd/http/civetweb/include/civetweb_main.h \
    3rd/http/civetweb/src/handle_form.inl \
    3rd/http/civetweb/src/md5.inl \
    3rd/http/civetweb/src/sha1.inl \
    3rd/http/civetweb/src/timer.inl \
    add_link_dialog.h \
    encrypt/simple_encrypt.h \
    http_thread.h \
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

INCLUDEPATH += ./3rd/http/civetweb/include
INCLUDEPATH += ./3rd/http/civetweb/src/
INCLUDEPATH += ./3rd/oxfold/include






macx: LIBS += -L$$PWD/3rd/oxfold/macos-x86_64/ -lzt

INCLUDEPATH += $$PWD/3rd/oxfold/macos-x86_64
DEPENDPATH += $$PWD/3rd/oxfold/macos-x86_64
