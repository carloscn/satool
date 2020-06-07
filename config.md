## Add QFTP to your Qt5

### code

git clone https://code.qt.io/qt/qtftp.git

### makefile prepare

install perl http://strawberryperl.com/download/5.30.2.1/strawberry-perl-5.30.2.1-64bit.msi

### modify code

modify ./qtftp/src/qftp/qftp.pro

CONFIG += shared  

### build (win32)

* use Qt 5.xx.xx mingw32 command line.
* qmake qtftp.pro
* mingw32-make
* mingw32-make install

### reboot your qt creator