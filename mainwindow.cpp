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
/*                                                                          */
/*  @File       : main.c                                                    */
/*  @Revision   : Ver 1.0.                                                  */
/*  @Date       : 2020.06.07 Realse.                                        */
/*  @Belong     : PROJECT.                                                  */
/*  @GitHub     :                                                           */
/*  @ASCII : (GBK/GB2312) in Qt Windows.                                    */
/****************************************************************************/
/*  @Attention:                                                             */
/*  ---------------------------------------------------------------------   */
/*  |    Data    |  Behavior |     Offer     |          Content         |   */
/*  |------------|-----------|---------------|--------------------------|   */
/*  | 2020.06.01 |   create  |Carlos Wei(M)  | add all device driver.   |   */
/*  ---------------------------------------------------------------------   */
/*                                                            MULTIBEANS.   */
/****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int ret = 0;
    ui->setupUi(this);
    this->setWindowTitle("SATOOT ftpClient for OMAL138");
    this->isFileAutoLoad = ui->checkBox_autoload->isChecked();
    this->initDisplay();
    this->showLocalFile();
    dataRomPointerCh0 = NULL;
    dataRomPointerCh1 = NULL;
    dataRomPointerCh2 = NULL;
    dataRomPointerCh3 = NULL;
    merge_downloads = false;

    this->get_ini_file_data();

    this->initQwt();
    this->globalConfig.boardIp = ui->ftpServerLineEdit->text();
    this->globalConfig.ftpPort = FTP_PORT;
    //    this->glabalConfig.tcpPort = TCP_PORT;
    //    this->glabalConfig.sampleRateKhz = 10;
    //    this->glabalConfig.mask = "255.255.255.0";
    //    this->glabalConfig.gate = "192.168.1.1";
    this->globalConfig.gainFirst = 1;
    this->globalConfig.gainSecond = 1;
    this->voltageGroup = new QButtonGroup(this);
    this->voltageGroup->addButton( ui->radioButton5v );
    this->voltageGroup->addButton( ui->radioButton10v );
    if(this->voltage5v == false)
        ui->radioButton5v->setChecked(false);
    else
        ui->radioButton5v->setChecked(true);

    this->isFileAutoLoad = ui->checkBox_autoload->isChecked();
    this->socket = new NetClientThread(this->globalConfig.boardIp, this->globalConfig.tcpPort);
    //this->on_radioButton10v_clicked(true);
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showLocalTreeViewMenu(QPoint)));
    ret = this->socket->set_connect(this->globalConfig.boardIp, this->globalConfig.tcpPort);
    if (ret != true) {
        QMessageBox::critical(this, "错误", "与下位机TCP通信失败，请检查与板子网络连接！");
    }
    ui->statusBar->showMessage("与板子TCP通信成功", 5000);

    timerAntiDiscon = new QTimer();
    //connect(timerAntiDiscon, SIGNAL(timeout()), this, SLOT(on_timerAntiConn_timeout()));
    //timerAntiDiscon->setInterval(10000);
    this->ui->radioButton4ch->setChecked(true);
    this->is_ch1_display = ui->radioButton1ch->isChecked();
    this->is_ch4_display = ui->radioButton4ch->isChecked();
}

MainWindow::~MainWindow()
{
    delete timerAntiDiscon;
    delete ui;
}

void MainWindow::on_timerAntiConn_timeout()
{
    ftpCd(".");
    qDebug("send alive signal to ftp.");
}

void MainWindow::ftpCd(QString path)
{
    ftp->cd(path);
    this->curFtpPath = path;
    qDebug() << "ftp cd: "  << path;
}

void MainWindow::get_ini_file_data()
{
    QFileInfo file("config.ini");
    if(file.exists() == false)
    {
        qDebug() << "不存在配置文件";

        QSettings *configIniWrite = new QSettings("config.ini",QSettings::IniFormat);
        configIniWrite->setValue("net/ip","192.168.1.10");
        configIniWrite->setValue("net/mask","255.255.255.0");
        configIniWrite->setValue("net/gate","192.168.1.1");
        configIniWrite->setValue("net/port","5000");
        configIniWrite->setValue("sample/rate","10");
        configIniWrite->setValue("voltage/range","5V");
        delete configIniWrite;
    }

    QSettings *configIniRead = new QSettings("config.ini",QSettings::IniFormat);
    if(configIniRead->value("net/ip").isNull() == true)
        configIniRead->setValue("net/ip","192.168.1.10");
    ui->ftpServerLineEdit->setText( configIniRead->value("net/ip").toString() );
    if(configIniRead->value("net/mask").isNull() == true)
        configIniRead->setValue("net/mask","255.255.255.0");
    this->globalConfig.mask = configIniRead->value("net/mask").toString();
    if(configIniRead->value("net/gate").isNull() == true)
        configIniRead->setValue("net/gate","192.168.1.1");
    this->globalConfig.gate = configIniRead->value("net/gate").toString();
    if(configIniRead->value("net/port").isNull() == true)
        configIniRead->setValue("net/port","5000");
    this->globalConfig.tcpPort = configIniRead->value("net/port").toULongLong();
    if(configIniRead->value("sample/rate").isNull() == true)
        configIniRead->setValue("sample/rate","10");
    this->globalConfig.sampleRateKhz = configIniRead->value("sample/rate").toUInt();
    if(configIniRead->value("voltage/range").isNull() == true)
        configIniRead->setValue("voltage/range","5V");
    if(configIniRead->value("voltage/range").toString() == "5V"){
        this->voltage5v = true;
        qDebug() << "5V";
    }else
        this->voltage5v = false;
    delete configIniRead;

    ui->checkBoxChannel0->setEnabled(true);
    ui->checkBoxChannel1->setEnabled(true);
    ui->checkBoxChannel2->setEnabled(true);
    ui->checkBoxChannel3->setEnabled(true);
    ui->checkBoxChannel0->setChecked(true);
    ui->checkBoxChannel1->setChecked(false);
    ui->checkBoxChannel2->setChecked(false);
    ui->checkBoxChannel3->setChecked(false);
}

void MainWindow::initQwt()
{
    QwtText title;
    QFont font;
    double sinTable[1024];
    QBrush brush1(QColor(92,184,92));
    QBrush brush2(QColor(91,192,222));
    QBrush brush3(QColor(240,173,78));
    QBrush brush4(QColor(217,83,79));
    QPen ch1Pen(QColor(92,184,92), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen ch2Pen(QColor(91,192,222), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen ch3Pen(QColor(240,173,78), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen ch4Pen(QColor(217,83,79), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QwtPlotGrid* gridCh = new QwtPlotGrid();
    QwtPlotGrid* gridFft = new QwtPlotGrid();

    QwtPlotPicker *m_picker_ch = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
                                                    QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                                    ui->qwt_ch->canvas() );
    QwtPlotPicker *m_picker_fft = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
                                                     QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                                     ui->qwt_fft->canvas() );
    SAXYDataTracker *tracker = new SAXYDataTracker(ui->qwt_fft->canvas());
    tracker->setRubberBandPen( QPen( Qt::red) );
    brush1.setStyle(Qt::Dense7Pattern);
    brush2.setStyle(Qt::Dense7Pattern);
    brush3.setStyle(Qt::Dense7Pattern);
    brush4.setStyle(Qt::Dense7Pattern);
    title.setText("Time Domain");
    ui->qwt_ch->setAxisTitle(2,"Voltage(V)");
    ui->qwt_ch->setTitle(title);
    title.setText("FFT");
    ui->qwt_fft->setTitle(title);
    ui->qwt_fft->setAxisTitle(2,"Hz");
    ui->qwt_ch->insertLegend(new QwtLegend(), QwtPlot::TopLegend);
    ui->qwt_fft->insertLegend(new QwtLegend(), QwtPlot::TopLegend);
    ui->qwt_ch->setStyleSheet("background-color:rgb(255,255,255)");
    ui->qwt_fft->setStyleSheet("background-color:rgb(255,255,255)");
    if(this->voltage5v == false)
    {
        ui->qwt_ch->setAxisScale(QwtPlot::xBottom, 0, ui->spinBoxBlockSize->value());
        ui->qwt_ch->setAxisScale(QwtPlot::yLeft, -12.1, 12.1);
    } else {
        ui->qwt_ch->setAxisScale(QwtPlot::xBottom, 0, ui->spinBoxBlockSize->value());
        ui->qwt_ch->setAxisScale(QwtPlot::yLeft, -6.1, 6.1);
    }

    this->qwtCurve1Ch1 = new QwtPlotCurve("Channel1");
    this->qwtCurve1Ch2 = new QwtPlotCurve("Channel2");
    this->qwtCurve1Ch3 = new QwtPlotCurve("Channel3");
    this->qwtCurve1Ch4 = new QwtPlotCurve("Channel4");
    this->qwtCurve1Ch1Fft = new QwtPlotCurve("CH1 FFT");
    this->qwtCurve1Ch2Fft = new QwtPlotCurve("CH2 FFT");
    this->qwtCurve1Ch3Fft = new QwtPlotCurve("CH3 FFT");
    this->qwtCurve1Ch4Fft = new QwtPlotCurve("CH4 FFT");
    this->qwtCurve1Ch1->setPen(ch1Pen);
    this->qwtCurve1Ch1Fft->setPen(ch1Pen);
    this->qwtCurve1Ch2->setPen(ch2Pen);
    this->qwtCurve1Ch2Fft->setPen(ch2Pen);
    this->qwtCurve1Ch3->setPen(ch3Pen);
    this->qwtCurve1Ch3Fft->setPen(ch3Pen);
    this->qwtCurve1Ch4->setPen(ch4Pen);
    this->qwtCurve1Ch4Fft->setPen(ch4Pen);
    this->qwtCurve1Ch1->setBrush(brush1);
    this->qwtCurve1Ch2->setBrush(brush2);
    this->qwtCurve1Ch3->setBrush(brush3);
    this->qwtCurve1Ch4->setBrush(brush4);
    this->qwtCurve1Ch1Fft->setBrush(brush1);
    this->qwtCurve1Ch2Fft->setBrush(brush2);
    this->qwtCurve1Ch3Fft->setBrush(brush3);
    this->qwtCurve1Ch4Fft->setBrush(brush4);
    this->qwtCurve1Ch1->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch2->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch3->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch4->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch1Fft->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch2Fft->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch3Fft->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch4Fft->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch1->setLegendAttribute(this->qwtCurve1Ch1->LegendShowLine);
    this->qwtCurve1Ch2->setLegendAttribute(this->qwtCurve1Ch1->LegendShowLine);
    this->qwtCurve1Ch3->setLegendAttribute(this->qwtCurve1Ch1->LegendShowLine);
    this->qwtCurve1Ch4->setLegendAttribute(this->qwtCurve1Ch1->LegendShowLine);
    this->qwtCurve1Ch1Fft->setLegendAttribute(this->qwtCurve1Ch1Fft->LegendShowLine);
    this->qwtCurve1Ch2Fft->setLegendAttribute(this->qwtCurve1Ch2Fft->LegendShowLine);
    this->qwtCurve1Ch3Fft->setLegendAttribute(this->qwtCurve1Ch3Fft->LegendShowLine);
    this->qwtCurve1Ch4Fft->setLegendAttribute(this->qwtCurve1Ch4Fft->LegendShowLine);
    this->qwtCurve1Ch1->attach(ui->qwt_ch);
    this->qwtCurve1Ch2->attach(ui->qwt_ch);
    this->qwtCurve1Ch3->attach(ui->qwt_ch);
    this->qwtCurve1Ch4->attach(ui->qwt_ch);
    this->qwtCurve1Ch1Fft->attach(ui->qwt_fft);
    this->qwtCurve1Ch2Fft->attach(ui->qwt_fft);
    this->qwtCurve1Ch3Fft->attach(ui->qwt_fft);
    this->qwtCurve1Ch4Fft->attach(ui->qwt_fft);
    gridCh->setPen(QColor(220,220,220),1, Qt::DashLine);
    gridFft->setPen(QColor(220,220,220),1,Qt::DashLine);
    gridCh->attach(ui->qwt_ch);
    gridFft->attach(ui->qwt_fft);

}

void MainWindow::initDisplay()
{
    ui->cdToParentButton->setEnabled(false);
    ui->downloadButton->setEnabled(false);
    ui->uploadButton->setEnabled(false);
    ui->progressBar->setValue(0);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    //fileLish 可使用右键菜单
    ui->fileList->setContextMenuPolicy(Qt::CustomContextMenu);
    this->mServerMenu = new QMenu(this);

    QAction *server_rm = new QAction(QObject::tr("删除"),this);
    this->mServerMenu->addAction(server_rm);
}

void MainWindow::ftpCommandStarted(int)
{
    int id = ftp->currentCommand();
    switch (id)
    {
    case QFtp::ConnectToHost :
        ui->label->setText(tr("正在连接到信号板…"));
        break;
    case QFtp::Login :
        ui->label->setText(tr("正在登录…"));
        break;
    case QFtp::Get :
        ui->label->setText(tr("正在下载…"));
        break;
    case QFtp::Close :
        ui->label->setText(tr("正在关闭连接…"));
        this->timerAntiDiscon->stop();
        break;
    case QFtp::Unconnected:
        // There is no connection to the host.
        ui->label->setText( "Status(%d) = disconnected status");
        break;
    }
}

void MainWindow::ftpCommandFinished(int, bool error)
{
    if(ftp->currentCommand() == QFtp::ConnectToHost) {
        if (error){
            ui->label->setText(tr("连接信号板出现错误：%1").arg(ftp->errorString()));
        }
        else{
            ui->connectButton->setDisabled(true);
            ui->connectButton->setText("已连接");
            ui->label->setText(tr("连接到信号板成功"));
            ui->statusBar->showMessage("连接到信号板成功", 5000);
        }
    } else if (ftp->currentCommand() == QFtp::Login) {
        if (error){
            ui->label->setText(tr("连接出现错误：%1").arg(ftp->errorString()));
        }
        else {
            ui->downloadButton->setEnabled(true);
            ui->uploadButton->setEnabled(true);
            ui->label->setText(tr("登录成功"));
            ui->statusBar->showMessage("连接到信号板成功", 5000);
            ftp->list();
        }
    } else if (ftp->currentCommand() == QFtp::Get) {
        if(error) ui->label->setText(tr("下载出现错误：%1").arg(ftp->errorString()));
        else {
            file->close();
            ui->label->setText(tr("下载完成"));
            ui->statusBar->showMessage("下载完成", 5000);
            ui->labelRemind->setText("文件" + this->hookFileAddr + " 传输完毕");
            if (this->isFileAutoLoad == true) {
                ui->lineEditLoadData->setText(this->hookFileAddr);
                ui->spinBoxBlockSize->setEnabled(true);
                qDebug() << "auto load: " << this->hookFileAddr;
            }
            currentIndex ++;

            if(merge_downloads == true && currentIndex == indexCount){
                qDebug() << "开始合并";
                qDebug() << merge_downoads_files_list;
                qSort(merge_downoads_files_list.begin(),merge_downoads_files_list.end());//按照升序进行排序
                qDebug() << merge_downoads_files_list;
                //新建一个新的文件名，格式为: 选中的第一个文件名 + _merge.txt
                //qDebug() << "merge_downloads_files: " << merge_downloads_files;
                QString new_merge_file = merge_downoads_files_list.at(0) + "_merge.txt";
                this->hookFileAddr = new_merge_file;
                qDebug() << "new_merge_file: " << new_merge_file;
                ui->lineEditLoadData->setText(this->hookFileAddr);
                QFile file( new_merge_file );
                if (file.open(QIODevice::ReadWrite) == true)
                {
                    QString current_merge_file;
                    QByteArray read_data;
                    QDataStream out(&file);
                    for(int i = 0; i < indexCount; i++)
                    {
                        current_merge_file = merge_downoads_files_list.at(i) + ".txt";
                        qDebug() << current_merge_file;
                        QFile current_file(current_merge_file);
                        if(current_file.open(QIODevice::ReadWrite) == true)
                        {
                            read_data = current_file.readAll();
                            qDebug() << read_data.length();
                            file.write(read_data);
                            current_file.close();
                        }
                    }
                    file.flush();
                    file.close();
                }
                merge_downloads = false;
            }

            if(currentIndex < indexCount){
                this->downloadFtpFile(currentIndex);
            }
            else{
                currentIndex = 0;
                dirModel->refresh();
            }
        }
        ui->downloadButton->setEnabled(true);
    } else if (ftp->currentCommand() == QFtp::List){
        if (isDirectory.isEmpty())
        {
            ui->fileList->addTopLevelItem(
                        new QTreeWidgetItem(QStringList()<< tr("<empty>")));
            ui->fileList->setEnabled(false);
            ui->label->setText(tr("该目录为空"));
        }
    }else if (ftp->currentCommand() == QFtp::Put) {
        if(error) ui->label->setText(tr("上传出现错误：检查文件是否重名！").arg(ftp->errorString()));
        else {
            ui->label->setText(tr("上传完成"));
            ui->statusBar->showMessage("上传完成", 5000);
            file->close();
            currentIndex ++;
            if(currentIndex < indexCount){
                this->uploadLocalFile(currentIndex);
            }
            else{
                currentIndex = 0;
                //刷新显示列表
                isDirectory.clear();
                ui->fileList->clear();
                ftp->list();
            }
        }
    }else if (ftp->currentCommand() == QFtp::Mkdir){
        ui->label->setText(tr("新建文件夹完成"));
        //刷新显示列表
        isDirectory.clear();
        ui->fileList->clear();
        ftp->list();
    }else if (ftp->currentCommand() == QFtp::Remove){
        currentIndex++;
        if(currentIndex >= indexCount){
            ui->label->setText(tr("删除完成！"));
            isDirectory.clear();
            ui->fileList->clear();
            ftp->list();
        }
    }else if(ftp->currentCommand() == QFtp::Rmdir){
        currentIndex++;
        if(currentIndex >= indexCount){
            ui->label->setText(tr("删除完成！"));
            isDirectory.clear();
            ui->fileList->clear();
            ftp->list();
        }
    }
    else if (ftp->currentCommand() == QFtp::Close) {
        ui->label->setText(tr("已经关闭连接"));
    }
}

// 连接按钮
void MainWindow::on_connectButton_clicked()
{
    ui->fileList->clear();
    currentPath.clear();
    isDirectory.clear();

    ftp = new QFtp;
    ftp->setTransferMode(QFtp::Active);
    connect(ftp, SIGNAL(commandStarted(int)), this, SLOT(ftpCommandStarted(int)));
    connect(ftp, SIGNAL(commandFinished(int, bool)),
            this, SLOT(ftpCommandFinished(int, bool)));
    connect(ftp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(addToList(QUrlInfo)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)),
            this, SLOT(updateDataTransferProgress(qint64, qint64)));
    connect(ui->fileList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showFtpTreeViewMenu(QPoint)));
    connect(ui->fileList, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(processItem(QTreeWidgetItem*, int)));

    QString ftpServer = ui->ftpServerLineEdit->text();
    QString userName = "root";
    QString passWord = "";
    ftp->connectToHost(ftpServer, FTP_PORT);
    ftp->login(userName,passWord);
    ui->statusBar->showMessage("和信号板建立FTP连接中...", 5000);
    ftpCd("/");
    ftpCd("/mnt");
    //ftpCd("/mnt/mmcblk0p1");
    this->timerAntiDiscon->start();
}

void MainWindow::addToList(const QUrlInfo &urlInfo)
{
    //
    if(_FromSpecialEncoding(urlInfo.name()).startsWith("."))
        return;
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, _FromSpecialEncoding(urlInfo.name()));
    double  dFileSize = ((int)(urlInfo.size()/1024.0*100))/100.0;
    QString fileSize = QString::number(dFileSize,'g',10)+"KB";
    qDebug() << "fileSize: " << fileSize;
    if(fileSize == "0KB")
        return;
    item->setText(1, fileSize);
    item->setText(2, urlInfo.lastModified().toString("MMM dd yyyy"));
    item->setText(3, urlInfo.owner());
    item->setText(4, urlInfo.group());
    QPixmap pixmap(urlInfo.isDir() ? ":/dir.png" : ":/file.png");
    item->setIcon(0, pixmap);
    isDirectory[_FromSpecialEncoding(urlInfo.name())] = urlInfo.isDir();
    ui->fileList->addTopLevelItem(item);
    if (!ui->fileList->currentItem()) {
        ui->fileList->setCurrentItem(ui->fileList->topLevelItem(0));
        ui->fileList->setEnabled(true);
    }
}

void MainWindow::processItem(QTreeWidgetItem *item, int)
{
    if(item->isDisabled())
        return;
    QString name = item->text(0);
    // 如果这个文件是个目录，则打开
    if (isDirectory.value(name)) {
        ui->fileList->clear();
        isDirectory.clear();
        currentPath += "/";
        currentPath += name;
        ftpCd(_ToSpecialEncoding(name));
        ftp->list();    //重新显示文件列表
        ui->cdToParentButton->setEnabled(true);
    }
}

// 返回上级目录按钮
void MainWindow::on_cdToParentButton_clicked()
{
    qDebug()<<"re:currentPath:"<<currentPath;
    ui->fileList->clear();
    isDirectory.clear();
    currentPath = currentPath.left(currentPath.lastIndexOf('/'));
    qDebug()<<"now:currentPath:"<<currentPath;
    if (currentPath.isEmpty()) {
        ui->cdToParentButton->setEnabled(false);
        ftpCd("/");
    } else {
        ftpCd(_ToSpecialEncoding(currentPath));
    }
    ftp->list();
}

//合并下载
void MainWindow::on_pushButton_merge_downloads_clicked()
{
    merge_downloads = true;
    merge_downloads_files.clear();
    merge_downoads_files_list.clear();
    on_downloadButton_clicked();
}

// 下载按钮
void MainWindow::on_downloadButton_clicked()
{
    if(isDirectory.isEmpty())
        return;
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) {
        ui->label->setText("请选择正确的下载路径！");
        return;
    }
    indexCount = ui->fileList->selectionModel()->selectedRows().count();
    if (indexCount <= 0) {
        return;
    }
    qDebug() << "indexCount:" << indexCount;
    ui->downloadButton->setEnabled(false);
    currentIndex = 0;
    this->downloadFtpFile(currentIndex);
    if (this->dataRomPointerCh0 != NULL) {
        delete(this->dataRomPointerCh0);
        this->dataRomPointerCh0 = NULL;
    }
    if (this->dataRomPointerCh1 != NULL) {
        delete(this->dataRomPointerCh1);
        this->dataRomPointerCh1 = NULL;
    }
    if (this->dataRomPointerCh2 != NULL) {
        delete(this->dataRomPointerCh2);
        this->dataRomPointerCh2 = NULL;
    }
    if (this->dataRomPointerCh3 != NULL) {
        delete(this->dataRomPointerCh3);
        this->dataRomPointerCh3 = NULL;
    }
}
void MainWindow::downloadFtpFile(int rowIndex)
{
    QModelIndex index = ui->treeView->currentIndex();
    QModelIndexList indexList = ui->fileList->selectionModel()->selectedRows();
    QFileInfo fileInfo = dirModel->fileInfo(index);
    QString fileName;
    if(fileInfo.isDir())
        fileName = fileInfo.absoluteFilePath() +"/"+ indexList.at(rowIndex).data().toString();
    else
        fileName = fileInfo.absolutePath() + "/" + indexList.at(rowIndex).data().toString();
    file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly)) {
        delete file;
        return;
    }
    ui->labelRemind->setText("文件" + fileName + " 传输中...");
    ui->statusBar->showMessage("ftp busy...");
    this->hookFileAddr = fileName;
    qDebug() << "hook name is :" << this->hookFileAddr;
    qDebug() << ftp->get(_ToSpecialEncoding(indexList.at(rowIndex).data().toString()), file);
    //加入合并下载
    if(merge_downloads == true)
    {
        merge_downoads_files_list.append(fileName.replace(QString(".txt"),QString("")));
        merge_downloads_files.append(fileName.replace(QString(".txt"),QString("")));
        merge_downloads_files.append(",");
    }
}

void MainWindow::updateDataTransferProgress(qint64 readBytes,qint64 totalBytes)
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(readBytes);
}

//本地目录操作
void MainWindow::showLocalFile()
{
    dirModel = new QDirModel;

    dirModel->setReadOnly(false);
    dirModel->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);

    //QTreeView treeView = new QTreeView;
    ui->treeView->setModel(dirModel);
    // ui->treeView->setRootIndex(dirModel->index("c:\\"));
    ui->treeView->header()->setStretchLastSection(true);
    ui->treeView->header()->setSortIndicator(0, Qt::AscendingOrder);
    ui->treeView->header()->setSortIndicatorShown(true);
    //ui->treeView->header()->setClickable(true);

    QModelIndex index = dirModel->index(QDir::currentPath());
    ui->treeView->expand(index);
    ui->treeView->scrollTo(index);
    ui->treeView->resizeColumnToContents(0);

    QPushButton *createBtn = new QPushButton(tr("Create Directory..."));
    QPushButton *delBtn = new QPushButton(tr("Remove"));

    connect(createBtn, SIGNAL(clicked()), this, SLOT(mkdir()));
    connect(delBtn, SIGNAL(clicked()), this, SLOT(rm()));
}

void MainWindow::mkdir()
{
    QModelIndex index = ui->treeView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QString dirName = QInputDialog::getText(this,
                                            tr("新建文件夹"),
                                            tr("文件夹名称"));
    if (!dirName.isEmpty()) {
        if (!dirModel->mkdir(index, dirName).isValid()) {
            QMessageBox::information(this,
                                     tr("新建文件夹"),
                                     tr("创建文件夹失败"));
        }
    }
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{

}
//文件上传
void MainWindow::on_uploadButton_clicked()
{
    indexCount = ui->treeView->selectionModel()->selectedRows().count();
    if (indexCount <= 0) {
        return;
    }
    currentIndex = 0;
    this->uploadLocalFile(currentIndex);
}

void MainWindow::uploadLocalFile(int rowIndex)
{
    QModelIndexList indexList = ui->treeView->selectionModel()->selectedRows();
    QFileInfo fileInfo = dirModel->fileInfo(indexList.at(rowIndex));
    QString fileName = fileInfo.absoluteFilePath();
    file = new QFile(fileName);
    if (!file->open(QIODevice::ReadOnly)) {
        delete file;
        return;
    }
    ftp->put(file,_ToSpecialEncoding(fileInfo.fileName()));
}

//有FTP端编码转本地编码
QString MainWindow::_FromSpecialEncoding(const QString &InputStr)
{
#ifdef Q_OS_WIN
    return  QString::fromLocal8Bit(InputStr.toLatin1());
#else
    QTextCodec *codec = QTextCodec::codecForName("gbk");
    if (codec)
    {
        return codec->toUnicode(InputStr.toLatin1());
    }
    else
    {
        return QString("");
    }
#endif
}
//put get等上传文件时，转换为FTP端编码
QString MainWindow::_ToSpecialEncoding(const QString &InputStr)
{
#ifdef Q_OS_WIN
    return QString::fromLatin1(InputStr.toLocal8Bit());
#else
    QTextCodec *codec= QTextCodec::codecForName("gbk");
    if (codec)
    {
        return QString::fromLatin1(codec->fromUnicode(InputStr));
    }
    else
    {
        return QString("");
    }
#endif
}
//fileList 右键菜单
void MainWindow::showFtpTreeViewMenu(const QPoint &pos)
{

    QMenu* menu=new QMenu;
    menu->addAction(QString(tr("新建文件夹")),this,SLOT(slotMkdir()));
    menu->addAction(QString(tr("刷新")),this,SLOT(slotRefreshFtpList()));
    menu->addAction(QString(tr("下载")),this,SLOT(on_downloadButton_clicked()));
    menu->addAction(QString(tr("删除")),this,SLOT(slotDeleteFile()));
    menu->exec(QCursor::pos());
}

void MainWindow::slotRefreshFtpList()
{
    isDirectory.clear();
    ui->fileList->clear();
    ftp->list();
}

void MainWindow::slotMkdir()
{
    QString dirName = QInputDialog::getText(this,
                                            tr("新建文件夹"),
                                            tr("文件夹名称"));
    if (!dirName.isEmpty()) {
        ftp->mkdir(_ToSpecialEncoding(dirName));
    }
}

void MainWindow::slotDeleteFile()
{
    QModelIndexList indexList = ui->fileList->selectionModel()->selectedRows();
    QString fileName;
    currentIndex = 0;
    indexCount = indexList.count();
    ui->label->setText("正在删除中......请稍等！");
    for(int i = 0;i < indexCount;i++)
    {
        fileName = indexList.at(i).data().toString();
        if(isDirectory.value(fileName)) {
            //deleteFtpDirectory(ftp, _ToSpecialEncoding(fileName));
            ftp->rmdir(_ToSpecialEncoding(fileName));
        } else {
            ftp->remove(_ToSpecialEncoding(fileName));
        }
    }
}

// not finish deleteFTP dir.
bool MainWindow::deleteFtpDirectory(QFtp *ftp, const QString &path)
{
    if (ftp == NULL) {
        return false;
    }
    quint32 pageCount = 0, i = 0, j = 0, indexCounter = 0;
    QString fileName;
    ftp->cd(path);
    qDebug() << "cd to " << path;
    ui->fileList->clear();
    isDirectory.clear();
    ftp->list();
    QTreeWidgetItemIterator it(ui->fileList);
    qDebug() << "delete count " << indexCounter;
    while(*it) {
        //QTreeWidgetItem  *twi = *it;
        //QModelIndex index = twi->child(0);
        //fileName = index.data().toString();
        qDebug() << "file name" << fileName;
        if(isDirectory.value(fileName)) {
            deleteFtpDirectory(ftp, _ToSpecialEncoding(fileName));
            ftp->rmdir(_ToSpecialEncoding(fileName));
        } else {
            ftp->remove(_ToSpecialEncoding(fileName));
        }
        i ++;
        it ++;
    }
    return true;
}

//本地文件 右键操作
void MainWindow::showLocalTreeViewMenu(const QPoint &pos)
{
    QMenu* menu=new QMenu;
    menu->addAction(QString(tr("新建文件夹")),this,SLOT(mkdir()));
    menu->addAction(QString(tr("删除")),this,SLOT(rm()));
    menu->addAction(QString(tr("刷新")),this,SLOT(localDirRefresh()));
    menu->exec(QCursor::pos());
}

void MainWindow::rm()
{
    QModelIndexList indexList = ui->treeView->selectionModel()->selectedRows();
    if (indexList.count() == 0) {
        return;
    }
    bool ok;
    for(int i=0;i<indexList.count();i++)
    {
        QModelIndex index = indexList.at(i);
        if (dirModel->fileInfo(index).isDir())
        {
            ok = dirModel->rmdir(index);
        } else {
            ok = dirModel->remove(index);
        }
        if (!ok) {
            QMessageBox::information(this,
                                     tr("Remove"),
                                     tr("Failed to remove %1").arg(dirModel->fileName(indexList.at(i))));
        }
    }
}

//本地目录刷新
void MainWindow::localDirRefresh(){
    dirModel->refresh();
}

//按键事件
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //按回车键，默认执行“连接”按钮操作
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        on_connectButton_clicked();
}


void MainWindow::qwtPlotFft(int channel, double *rom, int NP)
{
    fftwf_plan p;
    fftwf_complex  *in1_c = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)* NP);;
    fftwf_complex  *out1_c = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * NP);
    float ui_sample_freq = (float)this->globalConfig.sampleRateKhz * 1000.0f;
    QVector<QPointF> vector;
    double current_fft_value;


    for (int i = 0; i < NP; i ++) {
        in1_c[i][0] = *(rom + i);
        in1_c[i][1] = 0;
    }
    p = fftwf_plan_dft_1d(NP, in1_c, out1_c, FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(p);
    int step=NP/2;
    for( quint64 i = 0; i < step ; i++ ){
        QPointF point;
        current_fft_value = sqrt(out1_c[i][0] * out1_c[i][0] + out1_c[i][1] * out1_c[i][1]);
        point.setX((ui_sample_freq/ui->spinBoxBlockSize->value())*i);
        if ( i < 2 )
            point.setY(0);
        else
            point.setY(current_fft_value);
        vector.append(point);
    }
    fftwf_free(in1_c);
    fftwf_free(out1_c);
    fftwf_destroy_plan(p);
    fftwf_cleanup();

    QwtPointSeriesData* series = new QwtPointSeriesData(vector);

    if (channel == CHANNEL_0) {
        qwtCurve1Ch1Fft->setData(series);
    } else if (channel == CHANNEL_1) {
        qwtCurve1Ch2Fft->setData(series);
    } else if (channel == CHANNEL_2) {
        qwtCurve1Ch3Fft->setData(series);
    } else if (channel == CHANNEL_3) {
        qwtCurve1Ch4Fft->setData(series);
    }
}

void MainWindow::qwtPlotWave(unsigned int channel, double *data, unsigned long length)
{

    QVector<QPointF> vector;
    for( quint64 i = 0; i < length ; i++ ){
        QPointF point;
        point.setX( i );
        point.setY( data[i] );
        //qDebug() << i << ": " << data[i] << ",";
        vector.append(point);
    }
    QwtPointSeriesData* series = new QwtPointSeriesData(vector);

    switch (channel) {

    case CHANNEL_0:
        qwtCurve1Ch1->setData(series);
        break;
    case CHANNEL_1:
        qwtCurve1Ch2->setData(series);
        break;
    case CHANNEL_2:
        qwtCurve1Ch3->setData(series);
        break;
    case CHANNEL_3:
        qwtCurve1Ch4->setData(series);
        break;

    default:
        break;
    }
}
void MainWindow::qwtCleanChannel()
{
    double a = 0,b = 0;
    qwtCurve1Ch1->setSamples(&a,&b,1);
    qwtCurve1Ch2->setSamples(&a,&b,1);
    qwtCurve1Ch3->setSamples(&a,&b,1);
    qwtCurve1Ch4->setSamples(&a,&b,1);
    qwtCurve1Ch1Fft->setSamples(&a,&b,1);
    qwtCurve1Ch2Fft->setSamples(&a,&b,1);
    qwtCurve1Ch3Fft->setSamples(&a,&b,1);
    qwtCurve1Ch4Fft->setSamples(&a,&b,1);
    ui->qwt_ch->replot();
    ui->qwt_fft->replot();
}
void MainWindow::qwtShow()
{
    ui->qwt_ch->replot();
    ui->qwt_ch->show();
    ui->qwt_fft->replot();
    ui->qwt_fft->show();
}

void MainWindow::on_checkBox_autoload_clicked(bool checked)
{
    this->isFileAutoLoad = checked;
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QModelIndexList indexList = ui->treeView->selectionModel()->selectedRows();
    QFileInfo fileInfo = dirModel->fileInfo(indexList.at(0));
    QString fileName = fileInfo.absoluteFilePath();
    ui->currentPathText->setText(fileName);
}

void MainWindow::on_pushButtonDraw_clicked()
{
    if (dataRomPointerCh0 != NULL) {
        delete dataRomPointerCh0;
    }
    if (dataRomPointerCh1 != NULL) {
        delete dataRomPointerCh1;
    }
    if (dataRomPointerCh2 != NULL) {
        delete dataRomPointerCh2;
    }
    if (dataRomPointerCh3 != NULL) {
        delete dataRomPointerCh3;
    }
    QString dataFile = ui->lineEditLoadData->text();
    if (dataFile.isEmpty()) {
        return;
    }
    QFile file(dataFile);
    QFileInfo fileInfo;
    quint64 dataLen;
    float A = 0;

    fileInfo.setFile(dataFile);
    dataLen = fileInfo.size();
    qDebug() <<"file size : " << dataLen;
    bool isOk = file.open(QIODevice::ReadOnly);
    QByteArray fileData;
    fileData.clear();
    fileData = file.read(dataLen);
    file.close();
    /**
     * Logic of dealing single channel data
     * the name is xxx.txt
     * \note
     * it is difference from xxx.hex signle channel data.
     */
    if (dataFile.contains(".txt")) {
        dataLen = dataLen/2;
        qint16 dataTemp = 0;
        double dataTempDouble = 0.0;
        quint8 dataLow,dataHigh;
        int range = dataLen / ui->spinBoxBlockSize->value() - 1;
        ui->spinBox->setRange(0,range);
        ui->horizontalSlider_do->setRange(0, range);
        this->dataRange = range;
        double *dataRom = new double[dataLen + 1];
        if (this->voltage5v == true) {
            A = 5.0f;
        }else {
            A = 10.0f;
        }
        for(quint64 i = 0; i < dataLen; i ++) {
            dataTemp = 0;

            dataLow = fileData.at(2*i);
            dataHigh = fileData.at(2*i+1);
            dataTemp = dataLow;
            dataTemp |= dataHigh << 8;

            dataTempDouble = dataTemp * 1.0;
            dataRom[i] = dataTempDouble / 32768.0 * A;
        }

        this->dataRomPointerCh0 = dataRom;
        ui->spinBoxBlockSize->setEnabled(false);
        on_spinBox_valueChanged(0);
    /* Logic of dealing xxx.hex data file */
    /**
     * \brief Logic of dealing xxx.hex data file.
     * 1) get hex type from is_ch1/4_display.
     * 2) ch1 is single channel data file.
     * 3) ch4 is four channle data file.
     */
    } else if (dataFile.contains(".hex")) {
        if (is_ch4_display) {
            quint32 j = 0;
            dataLen = dataLen/8;
            qint16 dataTemp[4];
            double dataTempDouble[4];
            quint8 dataLow[4],dataHigh[4];
            int range = dataLen / ui->spinBoxBlockSize->value() - 1;
            ui->spinBox->setRange(0,range);
            ui->horizontalSlider_do->setRange(0, range);
            this->dataRange = range;
            double *dataRom1 = new double[dataLen + 1];
            double *dataRom2 = new double[dataLen + 1];
            double *dataRom3 = new double[dataLen + 1];
            double *dataRom4 = new double[dataLen + 1];
            if (this->voltage5v == true) {
                A = 5.0f;
            }else {
                A = 10.0f;
            }
            for(quint64 i = 0; i < dataLen; i ++) {
                memset(dataTemp, 0, 4);
                for (j = 0; j < 4; j ++) {
                    dataLow[j] = fileData.at(8*i + 2*j);
                    dataHigh[j] = fileData.at(8*i + 2*j + 1);
                    dataTemp[j] = dataHigh[j];
                    dataTemp[j] |= dataLow[j] << 8;
                    dataTempDouble[j] = dataTemp[j] * 1.0;
                }
                dataRom1[i] = dataTempDouble[0] / 32768.0 * A;
                dataRom2[i] = dataTempDouble[1] / 32768.0 * A;
                dataRom3[i] = dataTempDouble[2] / 32768.0 * A;
                dataRom4[i] = dataTempDouble[3] / 32768.0 * A;
            }

            this->dataRomPointerCh0 = dataRom1;
            this->dataRomPointerCh1 = dataRom2;
            this->dataRomPointerCh2 = dataRom3;
            this->dataRomPointerCh3 = dataRom4;
            ui->spinBoxBlockSize->setEnabled(false);
            on_spinBox_valueChanged(0);
        } else {
            dataLen = dataLen/8;
            qint16 dataTemp;
            double dataTempDouble;
            quint8 dataLow,dataHigh;
            int range = dataLen / ui->spinBoxBlockSize->value() - 1;
            ui->spinBox->setRange(0,range);
            ui->horizontalSlider_do->setRange(0, range);
            this->dataRange = range;
            double *dataRom1 = new double[dataLen + 1];
            if (this->voltage5v == true) {
                A = 5.0f;
            }else {
                A = 10.0f;
            }
            for(quint64 i = 0; i < dataLen; i ++) {
                dataTemp = 0;
                dataLow = fileData.at(2*i);
                dataHigh = fileData.at(2*i + 1);
                dataTemp = dataHigh;
                dataTemp |= dataLow << 8;
                dataTempDouble = dataTemp * 1.0;
                dataRom1[i] = dataTempDouble / 32768.0 * A;
            }

            this->dataRomPointerCh0 = dataRom1;
            ui->spinBoxBlockSize->setEnabled(false);
            on_spinBox_valueChanged(0);
        }
    }
}


void MainWindow::on_horizontalSlider_do_sliderMoved(int position)
{
    ui->spinBox->setValue(position);
    this->drawData(position);
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    ui->horizontalSlider_do->setValue(arg1);
    this->drawData(arg1);
}

void MainWindow::drawData(int pos)
{
    int seg = 0;
    qwtCleanChannel();
    if (ui->checkBoxChannel0->isChecked() && dataRomPointerCh0 != NULL) {
        qwtPlotWave(CHANNEL_0, (double*)(&dataRomPointerCh0[ui->spinBoxBlockSize->value()*pos] ), ui->spinBoxBlockSize->value());
        qwtPlotFft(CHANNEL_0, (double*)(&dataRomPointerCh0[ui->spinBoxBlockSize->value()*pos]), ui->spinBoxBlockSize->value());
        seg ++;
    } else {

    }
    if (ui->checkBoxChannel1->isChecked() && dataRomPointerCh1 != NULL) {
        qwtPlotWave(CHANNEL_1, (double*)(&dataRomPointerCh1[ui->spinBoxBlockSize->value()*pos] ), ui->spinBoxBlockSize->value());
        qwtPlotFft(CHANNEL_1, (double*)(&dataRomPointerCh1[ui->spinBoxBlockSize->value()*pos]), ui->spinBoxBlockSize->value());
        seg ++;
    }
    if (ui->checkBoxChannel2->isChecked() && dataRomPointerCh2 != NULL) {
        qwtPlotWave(CHANNEL_2, (double*)(&dataRomPointerCh2[ui->spinBoxBlockSize->value()*pos] ), ui->spinBoxBlockSize->value());
        qwtPlotFft(CHANNEL_2, (double*)(&dataRomPointerCh2[ui->spinBoxBlockSize->value()*pos]), ui->spinBoxBlockSize->value());
        seg ++;
    }
    if (ui->checkBoxChannel3->isChecked() && dataRomPointerCh3 != NULL) {
        qwtPlotWave(CHANNEL_3, (double*)(&dataRomPointerCh3[ui->spinBoxBlockSize->value()*pos] ), ui->spinBoxBlockSize->value());
        qwtPlotFft(CHANNEL_3, (double*)(&dataRomPointerCh3[ui->spinBoxBlockSize->value()*pos]), ui->spinBoxBlockSize->value());
        seg ++;
    }
    if (seg != 0)
        this->qwtShow();
}

void MainWindow::on_drawButton_clicked()
{
    QByteArray Doc;
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     tr("Open File"),
                                                     this->hookFileAddr,
                                                     "四通道数据(*.hex);;单通道数据(*.txt)",
                                                     0);
    if (!file_name.isNull()) {
        qDebug() << file_name ;
    }else{
        qDebug() << "cancel " ;
        return;
    }
    ui->lineEditLoadData->setText(file_name);
    ui->spinBoxBlockSize->setEnabled(true);
}

void MainWindow::on_actionProfile_triggered()
{
    int ret = 0;
    configDialog *dialog = new configDialog(this);
    dialog->setWindowTitle("配置窗口");
    ret = dialog->set_config(&this->globalConfig, this->socket);
    if (ret != 0) {
        QMessageBox::critical(   this,
                                 tr("错误提示"),
                                 tr("与下位机TCP网络通信失败！"));
        delete dialog;
        return;
    }
    dialog->setModal(false);
    dialog->show();
}

void MainWindow::on_radioButton5v_clicked(bool checked)
{
    this->voltage5v = checked;
    if (this->voltage5v == true)
        qDebug() << "current set 5V";
    else
        qDebug() << "current set 10V";
    QSettings *configIniWrite = new QSettings("config.ini",QSettings::IniFormat);
    configIniWrite->setValue("voltage/range","5V");
    delete configIniWrite;
    ui->qwt_ch->setAxisScale(QwtPlot::xBottom, 0, ui->spinBoxBlockSize->value());
    ui->qwt_ch->setAxisScale(QwtPlot::yLeft, -6.1, 6.1);
    ui->qwt_ch->replot();
}

void MainWindow::on_radioButton10v_clicked(bool checked)
{
    this->voltage5v = !checked;
    if (this->voltage5v == true)
        qDebug() << "current set 5V";
    else
        qDebug() << "current set 10V";
    QSettings *configIniWrite = new QSettings("config.ini",QSettings::IniFormat);
    configIniWrite->setValue("voltage/range","10V");
    delete configIniWrite;
    ui->qwt_ch->setAxisScale(QwtPlot::xBottom, 0, ui->spinBoxBlockSize->value());
    ui->qwt_ch->setAxisScale(QwtPlot::yLeft, -12.1, 12.1);
    ui->qwt_ch->replot();
}

void MainWindow::on_actionstartSample_triggered(bool checked)
{
    if (checked) {
        QByteArray cmd;
        bool state;
        cmd.append( (char)0x55 );
        for (quint8 i = 0; i < 12; i ++) {
            cmd.append((char)0);
        }
        state = this->socket->send_cmd_to_remote( (uint8_t*)cmd.data(), cmd.length());
        if (!state) {
            QMessageBox::critical(this, "错误提示", "网络数据校验失败，没有配置成功");
            ui->statusBar->showMessage("启动失败！", 5000);
            ui->actionstartSample->setChecked(false);
        }else{
            QMessageBox::information(this, "提示", "启动采样成功");
            ui->statusBar->showMessage("启动成功！", 5000);
            ui->actionstartSample->setChecked(true);
        }
    } else {
        on_actionstopSample_triggered();
    }
}

void MainWindow::on_actionstopSample_triggered()
{
    QByteArray cmd;
    bool state;
    cmd.append( (char)0x56 );
    for (quint8 i = 0; i < 12; i ++) {
        cmd.append((char)0);
    }
    state = this->socket->send_cmd_to_remote( (uint8_t*)cmd.data() , cmd.length());
    if (!state) {
        ui->statusBar->showMessage("关闭采样失败！", 5000);
        QMessageBox::critical(this, "错误提示", "网络数据校验失败，没有配置成功");
    }else{
        ui->statusBar->showMessage("关闭采样成功！", 5000);
        QMessageBox::information(this, "提示", "关闭采样成功！");
        ui->actionstartSample->setChecked(false);
    }
}

void MainWindow::on_actionlinkTcp_triggered()
{
    on_connectButton_clicked();
}

void MainWindow::on_actionDownloadFile_triggered()
{
    on_downloadButton_clicked();
}

void MainWindow::on_actionconfig_triggered()
{
    on_actionProfile_triggered();
}


void MainWindow::on_horizontalSlider_do_actionTriggered(int action)
{

}


void MainWindow::on_radioButton1ch_clicked()
{
    this->is_ch1_display = ui->radioButton1ch->isChecked();
    this->is_ch4_display = ui->radioButton4ch->isChecked();
}

void MainWindow::on_radioButton4ch_clicked()
{
    this->is_ch1_display = ui->radioButton1ch->isChecked();
    this->is_ch4_display = ui->radioButton4ch->isChecked();
}

void MainWindow::on_comboBox_sample_rate_currentIndexChanged(int index)
{
    switch(index) {
    case 0:
        this->globalConfig.sampleRateKhz = 10;
        break;
    case 1:
        this->globalConfig.sampleRateKhz = 50;
        break;
    case 2:
        this->globalConfig.sampleRateKhz = 100;
        break;
    case 3:
        this->globalConfig.sampleRateKhz = 150;
        break;
    case 4:
        this->globalConfig.sampleRateKhz = 180;
        break;
    case 5:
        this->globalConfig.sampleRateKhz = 200;
        break;
    }
}
