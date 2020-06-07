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
#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QByteArray>

#define         CHANNEL_3           0
#define         CHANNEL_2           1
#define         CHANNEL_1           2
#define         CHANNEL_0           3

#define         FTP_PORT            21
#define         TCP_PORT            5000

#define         CMD_HEADER_1            0xA5
#define         CMD_HEADER_2            0xBB
#define         CMD_SET_FREQ            0x01
#define         CMD_SET_GAIN            0x03
#define         CMD_ALL_START           0x04
#define         CMD_ALL_STOP            0x08
#define         CMD_DA_START            0x09
#define         CMD_DA_CLR_BUFFER       0x0A
#define         CMD_DA_PAUSE            0x0B
#define         CMD_DA_RECALL           0x0C
#define         CMD_CLOSE_REMOTE        0xFF
#define         CMD_SET_FS              0xA1


typedef struct config_t {
    QString boardIp;
    QString gate;
    QString mask;
    int tcpPort;
    int ftpPort;
    quint8 sampleRateKhz;
    quint16 gainFirst;
    quint16 gainSecond;

} CONFIG;

#endif // CONFIG_H
