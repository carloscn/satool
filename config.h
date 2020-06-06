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
