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
#include "configdialog.h"
#include "ui_configdialog.h"

configDialog::configDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configDialog)
{
    ui->setupUi(this);
    this->cfg = NULL;
}


configDialog::~configDialog()
{
    delete ui;
}

QString configDialog::get_local_ip()
{
    QString ip = "";
    QProcess cmd_pro ;
    QString cmd_str = QString("ipconfig");
    cmd_pro.start("cmd.exe", QStringList() << "/c" << cmd_str);
    cmd_pro.waitForStarted();
    cmd_pro.waitForFinished();
    QString result = cmd_pro.readAll();
    QString pattern("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}");
    QRegExp rx(pattern);
    int pos = 0;
    bool found = false;
    while((pos = rx.indexIn(result, pos)) != -1){
        QString tmp = rx.cap(0);
        if(-1 == tmp.indexOf("255")){
            if(ip != "" && -1 != tmp.indexOf(ip.mid(0,ip.lastIndexOf(".")))){
                found = true;
                break;
            }
            ip = tmp;
        }
        pos += rx.matchedLength();
    }
    qDebug()<<"local ip: " << ip;
    return ip;
}

void configDialog::on_pushButton_set_to_server_clicked()
{
    bool allowFlag = false;
    QMessageBox message1(QMessageBox::Critical, "警告",
                         "确定要更新IP信息，如果更改IP信息则需要重新配置软件网络，是否确定更改？",
                         QMessageBox::Yes | QMessageBox::No, NULL);
    if(message1.exec() == QMessageBox::Yes) {
        QMessageBox message2(QMessageBox::Critical, "警告",
                             "请再一次确认？要更改IP，谨慎操作！",
                             QMessageBox::Yes | QMessageBox::No, NULL);
        if(message2.exec() == QMessageBox::Yes)
            allowFlag = true;
    }
    if(allowFlag == false)
        return;
    QByteArray ip;
    QByteArray mask;
    QByteArray gate;
    QStringList list;
    uint8_t temp[4];
    CONFIG backConfig;
    memcpy(&backConfig, this->cfg, sizeof(CONFIG));
    this->get_ip_array_form_QString( ui->lineEdit_set_ip->text(), temp);
    cfg->boardIp = ui->lineEdit_set_ip->text();
    for (quint8 i = 0; i < 4; i ++) {
        ip.append(temp[i]);
    }

    this->get_ip_array_form_QString( ui->lineEdit_set_mask->text(), temp);
    cfg->mask = ui->lineEdit_set_mask->text();
    for (quint8 i = 0; i < 4; i ++) {
        mask.append(temp[i]);
    }

    this->get_ip_array_form_QString( ui->lineEdit_set_gate->text(), temp);
    cfg->gate = ui->lineEdit_set_gate->text();
    for (quint8 i = 0; i < 4; i ++) {
        gate.append(temp[i]);
    }
    qDebug() << "ip :" << ip;
    qDebug() << "mask :" << mask;
    qDebug() << "gate :" << gate;

    QByteArray cmd;
    cmd.append( 0x51 );
    cmd.append( ip );
    cmd.append( mask );
    cmd.append( gate );
    QSettings *configIniWrite = new QSettings("config.ini",QSettings::IniFormat);
    configIniWrite->setValue("net/ip",ui->lineEdit_set_ip->text());
    configIniWrite->setValue("net/mask",ui->lineEdit_set_mask->text());
    configIniWrite->setValue("net/gate",ui->lineEdit_set_gate->text());
    configIniWrite->setValue("net/port",ui->lineEdit_set_port->text());
    configIniWrite->setValue("sample/rate",ui->comboBox_sample_rate->currentText());
    delete configIniWrite;

    if (!this->net_socket->send_cmd_to_remote( (uint8_t*)cmd.data() , cmd.length())) {
        memcpy(this->cfg, &backConfig, sizeof(CONFIG));
        QMessageBox::critical(this, "错误提示", "网络数据校验失败，没有配置成功");
    }else{
        QSettings *configIniWrite = new QSettings("config.ini",QSettings::IniFormat);
        configIniWrite->setValue("net/ip",ui->lineEdit_set_ip->text());
        configIniWrite->setValue("net/mask",ui->lineEdit_set_mask->text());
        configIniWrite->setValue("net/gate",ui->lineEdit_set_gate->text());
        configIniWrite->setValue("net/port",ui->lineEdit_set_port->text());
        configIniWrite->setValue("sample/rate",ui->lineEdit_freq->text());
        delete configIniWrite;
        QMessageBox::information(this, "提示", "已写入IP配置，请重启板子和该软件，并输入正确IP");
    }
}

uint8_t configDialog::get_ip_array_form_QString(QString in,unsigned char *out)
{
    uint8_t next_ip_separate_symbol_index = 0;
    uint8_t now_ip_separate_symbol_index = 0;

    for(uint8_t i=0;i<4;i++){
        next_ip_separate_symbol_index = (i!=3)? (in.indexOf(".",next_ip_separate_symbol_index+1)):(in.length());
        if((next_ip_separate_symbol_index-now_ip_separate_symbol_index>0)&&(next_ip_separate_symbol_index-now_ip_separate_symbol_index<= 4)){
            *(out+i) = in.mid((i==0)? now_ip_separate_symbol_index:(now_ip_separate_symbol_index+1), \
                              (i==0)? next_ip_separate_symbol_index: \
                                      (next_ip_separate_symbol_index-now_ip_separate_symbol_index-1)).toInt(0,10);
            now_ip_separate_symbol_index = next_ip_separate_symbol_index;
        }else{
            return 0;
        }
    }

    return 1;
}

void configDialog::on_pushButton_set_clear_clicked()
{

}

int configDialog::set_config(CONFIG *cfg, NetClientThread *socket)
{
    int ret = 0;

    this->tcp_target_ip = cfg->boardIp;
    this->tcp_target_port = cfg->tcpPort;
    this->net_socket = socket;
    this->cfg = cfg;

    ui->lineEdit_set_gate->setText(cfg->gate);
    ui->lineEdit_set_ip->setText(cfg->boardIp);
    ui->lineEdit_set_mask->setText(cfg->mask);
    ui->lineEdit_set_port->setText(QString::number(cfg->tcpPort));

    switch (cfg->sampleRateKhz) {
    case 10:
        ui->comboBox_sample_rate->setCurrentIndex(0);
        break;
    case 50:
        ui->comboBox_sample_rate->setCurrentIndex(1);
        break;
    case 100:
        ui->comboBox_sample_rate->setCurrentIndex(2);
        break;
    case 150:
        ui->comboBox_sample_rate->setCurrentIndex(3);
        break;
    case 180:
        ui->comboBox_sample_rate->setCurrentIndex(4);
        break;
    case 200:
        ui->comboBox_sample_rate->setCurrentIndex(5);
        break;
    default:
        break;
    }

    QDateTime curDateTime = QDateTime::currentDateTime();
    ui->dateTimeEdit->sectionText(QDateTimeEdit::YearSection);
    ui->dateTimeEdit->sectionText(QDateTimeEdit::MonthSection );
    ui->dateTimeEdit->sectionText(QDateTimeEdit::DaySection );
    ui->dateTimeEdit->sectionText(QDateTimeEdit::HourSection );
    ui->dateTimeEdit->sectionText(QDateTimeEdit::MinuteSection  );
    ui->dateTimeEdit->sectionText(QDateTimeEdit::SecondSection  );
    ui->dateTimeEdit->setDateTime(curDateTime);

    timer_update_current_time = new QTimer();
    timer_update_current_time->setInterval(1000);
    connect(timer_update_current_time, SIGNAL(timeout()),this,SLOT(on_timer_update_current_time()));
    timer_update_current_time->start();

    return ret;
}

void configDialog::on_timer_update_current_time()
{
    //qDebug() << "update current time";
    QDateTime curDateTime = QDateTime::currentDateTime();
    ui->dateTimeEdit->sectionText(QDateTimeEdit::YearSection);
    ui->dateTimeEdit->sectionText(QDateTimeEdit::MonthSection );
    ui->dateTimeEdit->sectionText(QDateTimeEdit::DaySection );
    ui->dateTimeEdit->sectionText(QDateTimeEdit::HourSection );
    ui->dateTimeEdit->sectionText(QDateTimeEdit::MinuteSection  );
    ui->dateTimeEdit->sectionText(QDateTimeEdit::SecondSection  );
    ui->dateTimeEdit->setDateTime(curDateTime);
}

void configDialog::on_pushButton_set_to_time_clicked()
{
    QByteArray cmd;
    QString str;

    // 20/04/05/ 12:20:50
    cmd.clear();
    cmd.append( (char)0x53 );
    cmd.append( (char)(ui->dateTimeEdit->sectionText(QDateTimeEdit::YearSection).toUInt()&0xFF)  );
    cmd.append( (char)(ui->dateTimeEdit->sectionText(QDateTimeEdit::MonthSection).toUInt()&0xFF)  );
    cmd.append( (char)(ui->dateTimeEdit->sectionText(QDateTimeEdit::DaySection).toUInt()&0xFF)  );
    cmd.append( (char)(ui->dateTimeEdit->sectionText(QDateTimeEdit::HourSection).toUInt()&0xFF)  );
    cmd.append( (char)(ui->dateTimeEdit->sectionText(QDateTimeEdit::MinuteSection ).toUInt()&0xFF)  );
    cmd.append( (char)(ui->dateTimeEdit->sectionText(QDateTimeEdit::SecondSection ).toUInt()&0xFF)  );
    qDebug() << "time " << cmd;
    qDebug() << "ui" << ui->dateTimeEdit->sectionText(QDateTimeEdit::YearSection).toInt();
    for(int i = cmd.length(); i < 13; i ++)
    {
        cmd.append((char)0x00);
    }
    qDebug() << "time : " << this->arrayToHex( cmd );
    if (!this->net_socket->send_cmd_to_remote( (uint8_t*)cmd.data(), cmd.length() )) {
        QMessageBox::critical(this, "错误提示", "网络数据校验失败，同步时钟失败");
    }else{
        QMessageBox::information(this, "提示", "同步时钟成功");
    }
}
/**
 * @ingroup sec_soft
 * @brief	private function: hex(ASCII) to value
 * @param[out]  value
 * @param[in]	hex(ASCII)
 */

quint8 configDialog::hexstrToInt(QByteArray array)
{
    quint8 i;
    quint8 n = 0;
    quint8 len = 2;

    for ( i = 0; i < len; ++ i) {
        if (array.at(i) > '9') {
            n = 16 * n + (10 + array.at(i) - 'A');
        }
        else {
            n = 16 * n +( array.at(i) - '0');
        }
    }
    return n;
}

/**
 * @ingroup sec_soft
 * @brief	private function: array to hex
 * @param[out]  QString: Hex string
 * @param[in]	QByteArray: array
 */

QString configDialog::arrayToHex(QByteArray array)
{
    QByteArray temp1, temp2;
    QString str;
    temp1 = array.toHex().toUpper();

    for(int i = 0; i < temp1.length()/2;i++) {
        temp2 += temp1.mid(i*2,2) + " ";
    }
    str.append(temp2);

    return str;
}

/**
 * @ingroup sec_soft
 * @brief	private function: hex string to int
 * @param[out]  quint8: int value
 * @param[in]	QString: str hex(ascii)
 */

quint8 configDialog::hexstrToInt(QString str)
{
    quint8 i;
    quint8 n = 0;
    quint8 len = 2;

    for ( i = 0; i < len; ++ i) {
        if (str.at(i).toLatin1() > '9') {
            n = 16 * n + (10 + str.at(i).toLatin1() - 'A');
        }
        else {
            n = 16 * n +( str.at(i).toLatin1() - '0');
        }
    }
    return n;
}

/**
 * @ingroup sec_soft
 * @brief	private function: hex string to int
 * @param[out]  quint8: int value
 * @param[in]	QString: str hex(ascii)
 */

void configDialog::stringToHex(QString str, QByteArray &senddata)
{
    senddata.clear();
    int     hexdata,lowhexdata;
    int     hexdatalen = 0;
    int     len = str.length();
    char    lstr, hstr;
    senddata.resize(len/2);
    for( int i = 0; i < len ; ) {
        //char lstr,
        hstr    = str[i].toLatin1();
        if( hstr == ' ' ) {
            i ++;
            continue;
        }
        i ++ ;
        if( i >= len )
            break;
        lstr    = str[i].toLatin1();
        hexdata = convertHexChar(hstr);
        lowhexdata  = convertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata     = hexdata*16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

/**
 * @ingroup sec_soft
 * @brief	private function: hex to char
 * @param[out]  char '9'
 * @param[in]	char 0x9
 */
char configDialog::convertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

void configDialog::on_pushButton_reboot_clicked()
{
    QByteArray cmd;
    bool state;
    cmd.append( 0x53 );
    for (quint8 i = 0; i < 8; i ++) {
        cmd.append( (char)0x00 );
    }
    state = this->net_socket->send_cmd_to_remote( (uint8_t*)cmd.data(), cmd.length() );
    if (!state) {
        QMessageBox::critical(this, "错误提示", "网络数据校验失败，没有配置成功");
        ui->textBrowser->append("重启中...");
    }else{
        QMessageBox::information(this, "提示", "已写入配置");
        ui->textBrowser->append("重启失败！");
    }
}

void configDialog::on_pushButton_fs_set_clicked()
{
    QByteArray cmd;
    bool state;
    CONFIG cfgBackup;
    quint8 backUp = this->cfg->sampleRateKhz;
    QString sampleRateKHz;
    memcpy(&cfgBackup, this->cfg, sizeof(CONFIG));
    cmd.append( (char)0x54 );

    switch( ui->comboBox_sample_rate->currentIndex() ) {
    case 0:
        cmd.append( (char)10 );
        this->cfg->sampleRateKhz = 10;
        sampleRateKHz = "10";
        break;
    case 1:
        cmd.append( (char)50 );
        this->cfg->sampleRateKhz = 50;
        sampleRateKHz = "10";
        break;
    case 2:
        cmd.append( (char)100 );
        this->cfg->sampleRateKhz = 100;
        sampleRateKHz = "100";
        break;
    case 3:
        cmd.append( (char)150 );
        this->cfg->sampleRateKhz = 150;
        sampleRateKHz = "150";
        break;
    case 4:
        cmd.append( (char)180 );
        this->cfg->sampleRateKhz = 180;
        sampleRateKHz = "180";
        break;
    case 5:
        cmd.append( (char)200 );
        this->cfg->sampleRateKhz = 200;
        sampleRateKHz = "200";
        break;
    }
    for (quint8 i = 0; i < 11; i ++) {
        cmd.append((char)0);
    }
    state = this->net_socket->send_cmd_to_remote( (uint8_t*)cmd.data() , cmd.length());
    if (!state) {
        memcpy(this->cfg, &cfgBackup, sizeof(CONFIG));
        QMessageBox::critical(this, "错误提示", "网络数据校验失败，没有配置成功");
        ui->textBrowser->append("设置采样率失败");
    }else{
        QSettings *configIniWrite = new QSettings("config.ini",QSettings::IniFormat);
        configIniWrite->setValue("sample/rate",sampleRateKHz);
        delete configIniWrite;
        QMessageBox::information(this, "提示", "已写入配置");
        ui->textBrowser->append("设置采样率成功");
    }
}

void configDialog::on_pushButton_sample_clicked()
{
    QByteArray cmd;
    bool state;
    cmd.append( (char)0x55 );
    for (quint8 i = 0; i < 12; i ++) {
        cmd.append((char)0);
    }
    state = this->net_socket->send_cmd_to_remote( (uint8_t*)cmd.data() , cmd.length());
    if (!state) {
        QMessageBox::critical(this, "错误提示", "网络数据校验失败，启动采样失败");
        ui->textBrowser->append("启动失败！");
    }else{
        QMessageBox::information(this, "提示", "启动采样成功！");
        ui->textBrowser->append("启动采样成功！");
    }
}

void configDialog::on_pushButton_close_sample_clicked()
{
    QByteArray cmd;
    bool state;
    cmd.append( (char)0x56 );
    for (quint8 i = 0; i < 12; i ++) {
        cmd.append((char)0);
    }
    state = this->net_socket->send_cmd_to_remote( (uint8_t*)cmd.data() , cmd.length());
    if (!state) {
        ui->textBrowser->append("关闭采样失败！");
        QMessageBox::critical(this, "错误提示", "网络数据校验失败，关闭采样失败");
    }else{
        ui->textBrowser->append("关闭采样成功！");
        QMessageBox::information(this, "提示", "关闭采样成功");
    }
}
