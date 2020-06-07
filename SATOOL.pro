#-------------------------------------------------
#
# Project created by QtCreator 2020-06-01T16:59:01
#
#-------------------------------------------------


LINUX_QWT_DIR=../SATOOL/lib/libqwt/qwt-gcc-6.1.3
LINUX_FFTW_DIR=./SATOOL/lib/libfftw/fftw-gcc

WIN_QWT_DIR=..\SATOOL\lib\libqwt\qwt-mingw32\qwt-6.1.3
WIN_FFTW_DIR=..\SATOOL\lib\libfftw\fftw-mingw32\b32

ARM_QWT_DIR=./SATOOL/lib/libqwt/qwt-arch64-6.1.3
ARM_FFTW_DIR=./SATOOL/lib/libfftw/fftw-arch64
# add file: qwt.dll file: libfftw3f-3.dll to *.exe path

QT          += core gui sql xml widgets printsupport
QT          += network
QT          += ftp
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SATOOL
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    netclientthread.cpp \
    configdialog.cpp

HEADERS += \
        mainwindow.h \
    config.h \
    netclientthread.h \
    configdialog.h

FORMS += \
        mainwindow.ui \
    configdialog.ui

win32 {
    INCLUDEPATH += $${WIN_QWT_DIR}\include
    INCLUDEPATH += $${WIN_FFTW_DIR}
    LIBS += -L"$${WIN_QWT_DIR}\lib" -lqwt
    LIBS += "$${WIN_FFTW_DIR}\libfftw3f-3.dll"
}

linux-oe-g++ {
    INCLUDEPATH += $${ARM_QWT_DIR}/include
    INCLUDEPATH += $${ARM_FFTW_DIR}/include
    LIBS += -L"$${ARM_QWT_DIR}/lib/" -lqwt
    LIBS += $${ARM_FFTW_DIR}/lib/libfftw3f.a
}

linux-g++ {
    INCLUDEPATH += $${LINUX_QWT_DIR}/include
    INCLUDEPATH += $${LINUX_FFTW_DIR}/include
    LIBS += -L"$${LINUX_QWT_DIR}/lib/" -lqwt
    LIBS += $${LINUX_FFTW_DIR}/lib/libfftw3f.a
}

RESOURCES += \
    source.qrc
