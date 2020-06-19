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
    QTimer  *timer_update_current_time;
    explicit configDialog(QWidget *parent = 0);
    int set_config(CONFIG *cfg, NetClientThread *socket);
    ~configDialog();
    QString get_local_ip();
    uint8_t get_ip_array_form_QString(QString in,unsigned char *out);
    char convertHexChar(char ch);
    void stringToHex(QString str, QByteArray &senddata);
    quint8 hexstrToInt(QString str);
    QString arrayToHex(QByteArray array);
    quint8 hexstrToInt(QByteArray array);

private slots:
    void on_timer_update_current_time();

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
