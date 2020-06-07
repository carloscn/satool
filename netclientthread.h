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
#ifndef NETCLIENTTHREAD_H
#define NETCLIENTTHREAD_H

#include <QThread>
#include <QMutex>
#include <QtNetwork>
#include <QByteArray>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QString>
#include <QDebug>
#include "config.h"
#define             ONE_PACKET_LENGTH           (8010ul)
#define             ADC_MODE                    0
#define             DAC_MODE                    1

struct bcd_sper_t{
    uint8_t bit24_32;
    uint8_t bit16_24;
    uint8_t bit8_16;
    uint8_t bit0_8;
};
union bcd_code_t{

    struct bcd_sper_t bcdSperUint8;
    float fd;
    uint32_t uintd;
};

struct data_packet_t{

    quint32 number;
    quint32 da;
    quint32 db;
    quint32 dc;
    quint32 dd;

};


class NetClientThread:public QThread
{
    Q_OBJECT
public:
    NetClientThread( QString server_ip, int server_port );
    ~NetClientThread();

    void stop();
    bool set_connect( QString server_ip, int server_port );
    void set_disconnet();
    void bcd_encoding( float , uint8_t * );
    float bcd_decoding( uint8_t * );
    union bcd_code_t bcdCode;
    bool send_cmd_to_remote( uint8_t *cmd, quint16 length );
    void enable_socket_read(bool);
    void socket_write_byte_array(QByteArray array);
    bool is_network_setup();
    QByteArray recv_bytes;
protected:
    void run();

private slots:
    void on_read_message();

signals:

    void data_prepared( float* array_datas, uint length );

    void data_plot( double* array_datas );

    void net_data_ready(QByteArray block);

    void net_data_ready(char* block, quint32 length);

    void net_data_plot(quint32 *block, quint32 length);

    void net_add_doc_list(QString);



private :
    struct data_packet_t *data_packet;
    QMutex *mutex;
    QTcpServer *server;
    QTcpSocket *socket;
    bool data_prepared_flag;
    unsigned long count;
    bool is_head;
    bool is_packet_num;
    QByteArray buffer;
    quint32 buffer_index;
    quint64 packet_number;
    QByteArray array_rom;
    quint32 array_length = 0;
    quint32 head_index = 0;
    quint32 vector_counter = 0;
    quint64 read_counter = 0;
    quint8* socket_buffer;
    quint8 left_rom[8010];
    quint8 right_rom[8010];
    quint64 left_length;
    quint64 right_length;
    quint64 displat_counter=0;
//    FileManager *file_ctr;
    quint64 kcount;
    quint8 plot_buffer[ONE_PACKET_LENGTH];
    bool is_enable_socket_read;

public:
    quint16 sample_level;

private:

    qint8 check_packet(quint8* array, quint64 length);
    void check_packet(QByteArray array);
    QString gethostMac();
    bool check_key();

};

#endif // NETCLIENTTHREAD_H
