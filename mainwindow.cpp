#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFtp>
#include <QDebug>
#include <QFile>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QTreeWidgetItem>
#include <QXmlStreamReader>
#include <QAbstractItemView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("SATOOT ftpClient for OMAL138");
    this->isFileAutoLoad = ui->checkBox_autoload->isChecked();
    this->initDisplay();
    this->showLocalFile();
    this->initQwt();
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showLocalTreeViewMenu(QPoint)));
}
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initQwt()
{
    QwtText title;
    QFont font;
    double sinTable[1024];
    QBrush brush2(QColor(128,128,128));
    QwtPlotGrid* gridCh = new QwtPlotGrid();
    QwtPlotGrid* gridFft = new QwtPlotGrid();

    QwtPlotPicker *m_picker_ch = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
                                                    QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                                    ui->qwt_ch->canvas() );
    QwtPlotPicker *m_picker_fft = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
                                                     QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                                     ui->qwt_fft->canvas() );

    brush2.setStyle(Qt::NoBrush);
    title.setText("时域波形图");
    ui->qwt_ch->setAxisTitle(2,"伏特(V)");
    ui->qwt_ch->setTitle(title);
    title.setText("频域波形图");
    ui->qwt_fft->setTitle(title);
    ui->qwt_fft->setAxisTitle(2,"Hz");
    ui->qwt_ch->insertLegend(new QwtLegend(), QwtPlot::TopLegend);
    ui->qwt_fft->insertLegend(new QwtLegend(), QwtPlot::TopLegend);
    ui->qwt_ch->setStyleSheet("background-color:rgb(255,255,255)");
    ui->qwt_fft->setStyleSheet("background-color:rgb(255,255,255)");
    this->qwtCurve1Ch1 = new QwtPlotCurve("通道1（单位V）");
    this->qwtCurve1Ch2 = new QwtPlotCurve("通道2（单位V）");
    this->qwtCurve1Ch3 = new QwtPlotCurve("通道3（单位V）");
    this->qwtCurve1Ch4 = new QwtPlotCurve("通道4（单位V）");
    this->qwtCurve1Ch1Fft = new QwtPlotCurve("CH1 FFT");
    this->qwtCurve1Ch2Fft = new QwtPlotCurve("CH2 FFT");
    this->qwtCurve1Ch3Fft = new QwtPlotCurve("CH3 FFT");
    this->qwtCurve1Ch4Fft = new QwtPlotCurve("CH4 FFT");
    this->qwtCurve1Ch1->setPen(QColor(157,97,169),2.5,Qt::SolidLine);
    this->qwtCurve1Ch1Fft->setPen(QColor(157,97,169),2.5,Qt::SolidLine);
    this->qwtCurve1Ch2->setPen(QColor(241,196,86),2.5,Qt::SolidLine);
    this->qwtCurve1Ch2Fft->setPen(QColor(241,196,86),2.5,Qt::SolidLine);
    this->qwtCurve1Ch3->setPen(QColor(50,141,202),2.5,Qt::SolidLine);
    this->qwtCurve1Ch3Fft->setPen(QColor(50,141,202),2.5,Qt::SolidLine);
    this->qwtCurve1Ch4->setPen(QColor(220,95,47),2.5,Qt::SolidLine);
    this->qwtCurve1Ch4Fft->setPen(QColor(220,95,47),2.5,Qt::SolidLine);
    this->qwtCurve1Ch1->setBrush(brush2);
    this->qwtCurve1Ch2->setBrush(brush2);
    this->qwtCurve1Ch3->setBrush(brush2);
    this->qwtCurve1Ch4->setBrush(brush2);
    this->qwtCurve1Ch1Fft->setBrush(brush2);
    this->qwtCurve1Ch2Fft->setBrush(brush2);
    this->qwtCurve1Ch3Fft->setBrush(brush2);
    this->qwtCurve1Ch4Fft->setBrush(brush2);
    this->qwtCurve1Ch1->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch2->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch3->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch4->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch1Fft->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch2Fft->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch3Fft->setCurveAttribute(QwtPlotCurve::Fitted, true);
    this->qwtCurve1Ch4Fft->setCurveAttribute(QwtPlotCurve::Fitted, true);
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
            if (this->isFileAutoLoad == true) {
                ui->lineEditLoadData->setText(this->hookFileAddr);
            }
            currentIndex ++;
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
    QString userName = "carlos";
    QString passWord = "123456";
    ftp->connectToHost("127.0.0.1", SERVERPORT);
    qDebug() << "connect!";
    ftp->login(userName,passWord);
    ui->statusBar->showMessage("和信号板建立FTP连接中...", 5000);
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

    item->setText(1, fileSize);
    item->setText(2, urlInfo.lastModified().toString("MMM dd yyyy"));
    item->setText(3, urlInfo.owner());
    item->setText(4, urlInfo.group());
    QPixmap pixmap(urlInfo.isDir() ? "./dir.png" : "./file.png");
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
        ftp->cd(_ToSpecialEncoding(name));
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
        ftp->cd("/");
    } else {
        ftp->cd(_ToSpecialEncoding(currentPath));
    }
    ftp->list();
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
    ui->downloadButton->setEnabled(false);
    currentIndex = 0;
    this->downloadFtpFile(currentIndex);
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
    ftp->get(_ToSpecialEncoding(indexList.at(rowIndex).data().toString()), file);
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
        if(isDirectory.value(fileName))
            ftp->rmdir(_ToSpecialEncoding(fileName));
        else
            ftp->remove(_ToSpecialEncoding(fileName));
    }
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
//            QMessageBox::information(this,
//                                     tr("Remove"),
//                                     tr("Failed to remove %1").arg(dirModel->fileName(indexList.at(i))));
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


void MainWindow::on_net_plot_read(quint32 *block, quint32 length)
{
    qint32 channel_a[500];
    qint32 channel_b[500];
    qint32 channel_c[500];
    qint32 channel_d[500];

    double channel_a_d[500];
    double channel_b_d[500];
    double channel_c_d[500];
    double channel_d_d[500];
    //qDebug() << "slot plot!";

    for (quint32 i = 0; i < length/4; i ++) {
        channel_a[i] = (qint32) block[i] << 8;
        channel_b[i] = (qint32) block[(length/4)+i] << 8;// & (0xFFFFFF)) * (((block[500+i] & 0x800000) >> 23)?1:-1);
        channel_c[i] = (qint32) block[2*(length/4)+i] << 8;// & (0xFFFFFF))* (((block[2*500+i] & 0x800000) >> 23)?1:-1);
        channel_d[i] = (qint32) block[3*(length/4)+i] << 8;// & (0xFFFFFF))* (((block[3*500+i] & 0x800000) >> 23)?1:-1);
    }

    for (quint32 i = 0; i < (length/4); i ++) {
        channel_a_d[i] = channel_d[i] / 256 / 1000000000.0 * 488.0;
        channel_b_d[i] = channel_c[i] / 256 / 1000000000.0 * 488.0;
        channel_c_d[i] = channel_b[i] / 256 / 1000000000.0 * 488.0;
        channel_d_d[i] = channel_a[i] / 256 / 1000000000.0 * 488.0;
        //qDebug() << "sample: " << channel_a_d[i];
    }

    if (ui->checkBox_ch1_time->isChecked()) {
        this->qwtCurve1Ch1->attach(ui->qwt_ch);
        qwtPlotWave(CHANNEL_0, channel_a_d, (length/4));
    }else{
        this->qwtCurve1Ch1->detach();
    }
    if (ui->checkBox_ch2_time->isChecked()) {
        this->qwtCurve1Ch2->attach(ui->qwt_ch);
        qwtPlotWave(CHANNEL_1, channel_b_d, (length/4));
    }else{
        this->qwtCurve1Ch2->detach();
    }

    if (ui->checkBox_ch3_time->isChecked()) {
        this->qwtCurve1Ch3->attach(ui->qwt_ch);
        qwtPlotWave(CHANNEL_2, channel_c_d, (length/4));
    }else{
        this->qwtCurve1Ch3->detach();
    }
    if (ui->checkBox_ch4_time->isChecked()) {
        this->qwtCurve1Ch4->attach(ui->qwt_ch);
        qwtPlotWave(CHANNEL_3, channel_d_d, (length/4));
    }else{
        this->qwtCurve1Ch4->detach();
    }


    if (ui->checkBox_ch1_fft->isChecked()) {
        this->qwtCurve1Ch1Fft->attach(ui->qwt_fft);
        qwtPlotFft(CHANNEL_0, channel_a_d, (length/4));
    }else{
        this->qwtCurve1Ch1Fft->detach();
    }

    if (ui->checkBox_ch2_fft->isChecked()) {
        this->qwtCurve1Ch2Fft->attach(ui->qwt_fft);
        qwtPlotFft(CHANNEL_1, channel_b_d, (length/4));
    }else{
        this->qwtCurve1Ch2Fft->detach();
    }

    if (ui->checkBox_ch3_fft->isChecked()) {
        this->qwtCurve1Ch3Fft->attach(ui->qwt_fft);
        qwtPlotFft(CHANNEL_2, channel_c_d, (length/4));
    }else{
        this->qwtCurve1Ch3Fft->detach();
    }

    if (ui->checkBox_ch4_fft->isChecked()) {
        this->qwtCurve1Ch4Fft->attach(ui->qwt_fft);
        qwtPlotFft(CHANNEL_3, channel_d_d, (length/4));
    }else{
        this->qwtCurve1Ch4Fft->detach();
    }

}

void MainWindow::qwtPlotFft(int channel, double *rom, int NP)
{
    fftwf_plan p;
    fftwf_complex  *in1_c = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex)* NP);;
    fftwf_complex  *out1_c = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * NP);
    float ui_sample_freq = 250000.0;
    QVector<QPointF> vector;
    double current_fft_value;

    for (int i = 0; i < NP; i ++) {
        in1_c[i][0] = *(rom + i);
        in1_c[i][1] = 0;
    }
    p = fftwf_plan_dft_1d(NP, in1_c, out1_c, FFTW_FORWARD, FFTW_ESTIMATE);
    fftwf_execute(p);
    int step=120;
    for( quint64 i = 0; i < step ; i++ ){
        QPointF point;
        current_fft_value = sqrt(out1_c[i][0] * out1_c[i][0] + out1_c[i][1] * out1_c[i][1]);
        point.setX((ui_sample_freq/500)*i);
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
    ui->qwt_fft->replot();
    ui->qwt_fft->show();

}
void MainWindow::qwtPlotWave(unsigned int channel, double *data, unsigned long length)
{

    QVector<QPointF> vector;
    for( quint64 i = 0; i < length ; i++ ){
        QPointF point;
        point.setX( i );
        point.setY( data[i] );
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
    ui->qwt_ch->replot();
    ui->qwt_ch->show();
}

void MainWindow::on_checkBox_autoload_clicked(bool checked)
{
    this->isFileAutoLoad = checked;
}
