#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QWidget>
#include <QDirModel>
#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QkeyEvent>
#include <QHash>
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
#include "config.h"

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
    QMenu *mServerMenu;
    QMenu *mClientMenu;
    int indexCount;
    int currentIndex;
    quint32 plotCount;
    QPen pen;
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
    void on_net_plot_read(quint32 *block, quint32 length);
    void on_checkBox_autoload_clicked(bool checked);
    void on_connectButton_clicked();
    void on_cdToParentButton_clicked();
    void on_downloadButton_clicked();
};

#endif // MAINWINDOW_H
