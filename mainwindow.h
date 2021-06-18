/**
 * \brief   This project about SATOOL.
 *
 * \License  THIS FILE IS PART OF MULTIBEANS PROJECT;
 *           all of the files  - The core part of the project;
 *           THIS PROGRAM IS NOT FREE SOFTWARE, NEED MULTIBEANS ORG LIC;
 *           YOU SHOULD HAVE RECEIVED A COPY OF WTFPL LICENSE.
 *
 *                ________________     ___           _________________
 *               |    __    __    |   |   |         |______     ______|
 *               |   |  |  |  |   |   |   |                |   |
 *               |   |  |__|  |   |   |   |________        |   |
 *               |___|        |___|   |____________|       |___|
 *
 *                               MULTIBEANS ORG.
 *                     Homepage: http://www.mlts.tech/
 *
 *           * You can download the license on our Github. ->
 *           * -> https://github.com/carloscn/satool  <-
 *           * Copyright (c) 2013-2020 MULTIBEANS ORG. www.mlts.tech/
 *           * Copyright (c) 2020 (Carlos Wei: # carlos@mlts.tech).
 *
 *  \note    Code license for Xiaoji Zhang of NWPU.
 ****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QWidget>
#include <QDirModel>
#include <QTreeView>
#include <QFile>
#include <QFileInfo>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QkeyEvent>
#include <QHash>
#include <QFileDialog>
#include <QButtonGroup>
#include <QFtp>
#include <QDebug>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QTreeWidgetItem>
#include <QXmlStreamReader>
#include <QAbstractItemView>
#include <QMessageBox>
#include "fftw3.h"
#include "qwt.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <math.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>
#include "saxydatatracker.h"
#include "config.h"
#include "configdialog.h"
#include <QVariant>
#include <QSettings>
#include <QtAlgorithms>

class QFtp;
class QFile;
class QUrlInfo;
class QTreeWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    QFtp *ftp;
    QHash<QString, bool> isDirectory;
    QHash<QString, bool> isLocalDirectory;
    QString currentPath;
    QDirModel *dirModel;
    QFile *file;
    QFile *merge_file;
    bool    merge_downloads;
    QString merge_downloads_files;
    QStringList merge_downoads_files_list;
    QMenu *mServerMenu;
    QMenu *mClientMenu;
    int indexCount;
    int currentIndex;
    quint32 plotCount;
    double *dataRomPointerCh0;
    double *dataRomPointerCh1;
    double *dataRomPointerCh2;
    double *dataRomPointerCh3;
    QPen pen;
    int dataRange;
    QwtPlotCurve* qwtCurve1Ch1;
    QwtPlotCurve* qwtCurve1Ch2;
    QwtPlotCurve* qwtCurve1Ch3;
    QwtPlotCurve* qwtCurve1Ch4;
    QwtPlotCurve* qwtCurve1Ch1Fft;
    QwtPlotCurve* qwtCurve1Ch2Fft;
    QwtPlotCurve* qwtCurve1Ch3Fft;
    QwtPlotCurve* qwtCurve1Ch4Fft;
    bool isFileAutoLoad;
    QString hookFileAddr;
    CONFIG glabalConfig;
    bool voltage5v;
    QButtonGroup *voltageGroup;
    NetClientThread *socket;

private:

    void initDisplay();
    void showLocalFile();
    QString _ToSpecialEncoding(const QString &InputStr);
    QString _FromSpecialEncoding(const QString &InputStr);
    void downloadFtpFile(int rowIndex);
    void uploadLocalFile(int rowIndex);
    void initQwt();
    void qwtPlotWave( unsigned int , double* , unsigned long);
    void qwtPlotFft( int, double *, int);
    void qwtCleanChannel();
    void drawData(int pos);
    void get_ini_file_data();
    void qwtShow();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void ftpCommandStarted(int);
    void ftpCommandFinished(int, bool);
    void updateDataTransferProgress(qint64, qint64 );
    void addToList(const QUrlInfo &urlInfo);
    void processItem(QTreeWidgetItem*, int);
    void mkdir();
    void rm();
    void localDirRefresh();
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_uploadButton_clicked();
    void showFtpTreeViewMenu(const QPoint &point );
    void showLocalTreeViewMenu(const QPoint &point );
    void slotMkdir();
    void slotDeleteFile();
    void slotRefreshFtpList();
    void on_checkBox_autoload_clicked(bool checked);
    void on_connectButton_clicked();
    void on_cdToParentButton_clicked();
    void on_downloadButton_clicked();
    void on_treeView_clicked(const QModelIndex &index);
    void on_pushButtonDraw_clicked();
    void on_horizontalSlider_do_sliderMoved(int position);
    void on_spinBox_valueChanged(int arg1);
    void on_drawButton_clicked();
    void on_actionProfile_triggered();
    void on_radioButton5v_clicked(bool checked);
    void on_radioButton10v_clicked(bool checked);
    void on_actionstartSample_triggered(bool checked);
    void on_actionstopSample_triggered();
    void on_actionlinkTcp_triggered();
    void on_actionDownloadFile_triggered();
    void on_actionconfig_triggered();
    void on_pushButton_merge_downloads_clicked();
    void on_horizontalSlider_do_actionTriggered(int action);
};

#endif // MAINWINDOW_H
