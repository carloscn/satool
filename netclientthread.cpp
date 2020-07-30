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
#include "netclientthread.h"

#define             RING_BUFFER_SIZE    19200ul
#define             pow(x) x*x
#define             UINT_FORMAT_SAVE       0
#define             KEY_ENABLE

NetClientThread::NetClientThread( QString server_ip, int server_port )
{
    socket = new QTcpSocket();
    server = new QTcpServer();
    sample_level = 1;
    array_length = 0;
    mutex = new QMutex();
    count = 0;
    is_head = false;
    packet_number = 0;
    kcount = 0;
    is_enable_socket_read = false;
    left_length = 0;
    array_rom.clear();
}

bool NetClientThread::set_connect(QString server_ip, int server_port)
{
    socket->connectToHost( server_ip, server_port , QIODevice::ReadWrite );
    QObject::connect((QObject*) socket,SIGNAL(readyRead()),(QObject*)this,SLOT(on_read_message()));
    if( !socket->waitForConnected(1000) ) {
        return false;
    }else {
        return true;
    }
}

bool NetClientThread::is_network_setup()
{
    return socket->isOpen();
}

void NetClientThread::set_disconnet()
{
    socket->flush();
    socket->close();
}

float NetClientThread::bcd_decoding(uint8_t *dHandle)
{
    float *df;
    uint32_t temp;

    for ( int i = 0; i < 4; i ++ ) {
        temp |= *(dHandle + 4 - i) & 0xFF;
        temp = temp << 8;
    }
    df = (float *)&temp;
    return *df;
}

void NetClientThread::bcd_encoding(float df, uint8_t *distChar)
{
    uint8_t* dHandle;

    dHandle = (uint8_t *)&df;
    *distChar = *dHandle & 0xFF;
    *(distChar + 1) = *(dHandle + 1) & 0xFF ;
    *(distChar + 2) = *(dHandle + 2) & 0xFF ;
    *(distChar + 3) = *(dHandle + 3) & 0xFF ;
}

bool NetClientThread::send_cmd_to_remote(uint8_t *users, quint16 length)
{
    bool state = false;
    QByteArray cmd;
    QByteArray recv;
    quint8  checkSum = 0;
    quint64 error_count = 0;
    cmd.clear();
    cmd.append(CMD_HEADER_1);
    for (int i = 0; i < length; i ++)
        cmd.append(users[i]);
    qDebug() << "send :" << cmd;
    cmd.append(0xE9);
    socket->write(cmd);
    socket->flush();
    qDebug() << "cmd socket series number: ( " << cmd << "\"" << "length" << cmd.length() << ")";
    socket->waitForReadyRead(300);
    for (quint64 i = 0; i < cmd.length(); i ++) {
        if (cmd.at(i) != recv_bytes.at(i)) {
            error_count ++;
        }
    }
    if (error_count > 0) {
        state = false;
        qDebug() << "rec socket series number: (  " << recv_bytes << "  )";
        qCritical("error recv != send");
    }else {
        state = true;
    }
    return state;
}

void NetClientThread::run()
{

}

void NetClientThread::socket_write_byte_array(QByteArray array)
{
    socket->write(array);
}

void NetClientThread::on_read_message()
{
    recv_bytes = socket->readAll();
    qDebug() << "received : " << recv_bytes;
}


void NetClientThread::enable_socket_read(bool state)
{
    is_enable_socket_read = state;
}

QString NetClientThread::gethostMac()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();
    int nCnt = nets.count();
    QString strMacAddr = "";
    for(int i = 0; i < nCnt; i ++) {
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning)
                && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            strMacAddr = nets[i].hardwareAddress();
            break;
        }
    }
    return strMacAddr;

}
NetClientThread::~NetClientThread()
{
}
