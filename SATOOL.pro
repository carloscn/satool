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
# set export value in : project->
# DYLD_FRAMEWORK_PATH /Users/carlos/work/out/qwt6.1.3/lib

MAC_QWT_DIR=../SATOOL/lib/libqwt/qwt-macos
MAC_FFTW_DIR=../SATOOL/lib/libfftw/fftw-macos
macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.09
macx:QMAKE_MAC_SDK=macosx10.09
QT          += core gui sql xml widgets printsupport
QT          += network
QT          += ftp
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
DEFINES += QT_DLL QWT_DLL QWT_POLAR_DLL
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
    configdialog.cpp \
    saxydatatracker.cpp

HEADERS += \
        mainwindow.h \
    config.h \
    netclientthread.h \
    configdialog.h \
    saxydatatracker.h

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

macx-clang {
include($${MAC_QWT_DIR}/features/qwt.prf)
    INCLUDEPATH += $${MAC_QWT_DIR}/lib/qwt.framework/Headers
    INCLUDEPATH += $${MAC_FFTW_DIR}/include
    LIBS += -F"$${MAC_QWT_DIR}/lib/qwt.framework"
    LIBS += "$${MAC_FFTW_DIR}/lib/libfftw3f.a"
    #LIBS += -framework qwt
}

RESOURCES += \
    source.qrc

DISTFILES += \
    images/gif_48_001.gif \
    images/gif_48_002.gif \
    images/gif_48_003.gif \
    images/gif_48_004.gif \
    images/gif_48_005.gif \
    images/gif_48_006.gif \
    images/gif_48_007.gif \
    images/gif_48_008.gif \
    images/gif_48_009.gif \
    images/gif_48_010.gif \
    images/gif_48_011.gif \
    images/gif_48_012.gif \
    images/gif_48_013.gif \
    images/gif_48_014.gif \
    images/gif_48_015.gif \
    images/gif_48_016.gif \
    images/gif_48_017.gif \
    images/gif_48_018.gif \
    images/gif_48_019.gif \
    images/gif_48_020.gif \
    images/gif_48_021.gif \
    images/gif_48_022.gif \
    images/gif_48_023.gif \
    images/gif_48_024.gif \
    images/gif_48_025.gif \
    images/gif_48_026.gif \
    images/gif_48_027.gif \
    images/gif_48_028.gif \
    images/gif_48_029.gif \
    images/gif_48_030.gif \
    images/gif_48_031.gif \
    images/gif_48_032.gif \
    images/gif_48_033.gif \
    images/gif_48_034.gif \
    images/gif_48_035.gif \
    images/gif_48_036.gif \
    images/gif_48_037.gif \
    images/gif_48_038.gif \
    images/gif_48_039.gif \
    images/gif_48_040.gif \
    images/gif_48_041.gif \
    images/gif_48_042.gif \
    images/gif_48_043.gif \
    images/gif_48_044.gif \
    images/gif_48_045.gif \
    images/gif_48_046.gif \
    images/gif_48_047.gif \
    images/gif_48_048.gif \
    images/gif_48_049.gif \
    images/gif_48_050.gif \
    images/gif_48_051.gif \
    images/gif_48_052.gif \
    images/gif_48_053.gif \
    images/gif_48_054.gif \
    images/gif_48_055.gif \
    images/gif_48_056.gif \
    images/gif_48_057.gif \
    images/gif_48_058.gif \
    images/gif_48_059.gif \
    images/gif_48_060.gif \
    images/gif_48_061.gif \
    images/gif_48_062.gif \
    images/gif_48_063.gif \
    images/gif_48_064.gif \
    images/gif_48_065.gif \
    images/gif_48_066.gif \
    images/gif_48_067.gif \
    images/gif_48_068.gif \
    images/gif_48_069.gif \
    images/gif_48_070.gif \
    images/gif_48_071.gif \
    images/gif_48_072.gif \
    images/gif_48_073.gif \
    images/gif_48_074.gif \
    images/gif_48_075.gif \
    images/gif_48_076.gif \
    images/gif_48_077.gif \
    images/gif_48_078.gif \
    images/gif_48_079.gif \
    images/gif_48_080.gif \
    images/gif_48_081.gif \
    images/gif_48_082.gif \
    images/gif_48_083.gif \
    images/gif_48_084.gif \
    images/gif_48_085.gif \
    images/gif_48_086.gif \
    images/gif_48_087.gif \
    images/gif_48_088.gif \
    images/gif_48_089.gif \
    images/gif_48_090.gif \
    images/gif_48_091.gif \
    images/gif_48_092.gif \
    images/gif_48_093.gif \
    images/gif_48_094.gif \
    images/gif_48_095.gif \
    images/gif_48_096.gif \
    images/gif_48_097.gif \
    images/gif_48_098.gif \
    images/gif_48_099.gif \
    images/gif_48_100.gif \
    images/gif_48_101.gif \
    images/gif_48_102.gif \
    images/gif_48_103.gif \
    images/gif_48_104.gif \
    images/gif_48_105.gif \
    images/gif_48_106.gif \
    images/gif_48_107.gif \
    images/gif_48_108.gif \
    images/gif_48_109.gif \
    images/gif_48_110.gif \
    images/.svn/text-base/zoomin.png.svn-base \
    images/.svn/text-base/zoomout.png.svn-base \
    images/8.png \
    images/atom.png \
    images/chip.png \
    images/Going down two bars 128.png \
    images/Going down two bars 256.png \
    images/Going down two bars 32.png \
    images/Going down two bars 512.png \
    images/Going down two bars 64.png \
    images/processor.png \
    images/welcome.png \
    images/welcome_2.png \
    images/welcome_3.png \
    images/welcome_4.png \
    images/welcome_5.png \
    images/ring.svg \
    images/.svn/prop-base/zoomin.png.svn-base \
    images/.svn/prop-base/zoomout.png.svn-base \
    images/.svn/all-wcprops \
    images/.svn/entries
