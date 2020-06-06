#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include "netclientthread.h"

namespace Ui {
class configDialog;
}

class configDialog : public QDialog
{
    Q_OBJECT

public:
    CONFIG *cfg;
    QString tcp_target_ip;
    int     tcp_target_port;
    explicit configDialog(QWidget *parent = 0);
    int set_config(CONFIG *cfg);
    ~configDialog();
    QString get_local_ip();
    uint8_t get_ip_array_form_QString(QString in,unsigned char *out);
    char convertHexChar(char ch);
    void stringToHex(QString str, QByteArray &senddata);
    quint8 hexstrToInt(QString str);
    QString arrayToHex(QByteArray array);
    quint8 hexstrToInt(QByteArray array);

private slots:
    void on_pushButton_set_to_server_clicked();

    void on_pushButton_set_clear_clicked();

    void on_pushButton_set_to_time_clicked();

    void on_pushButton_reboot_clicked();

    void on_pushButton_fs_set_clicked();

    void on_pushButton_sample_clicked();

    void on_pushButton_close_sample_clicked();

private:
    Ui::configDialog *ui;
    NetClientThread* net_socket;
};

#endif // CONFIGDIALOG_H
