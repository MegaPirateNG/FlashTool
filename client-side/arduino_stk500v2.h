#ifndef ARDUINO_STK500V2_H
#define ARDUINO_STK500V2_H

#include <QSerialPort>
#include <QtGui>

class ArduinoStk500v2 : public QSerialPort
{
    Q_OBJECT
public:
    explicit ArduinoStk500v2(QObject *parent = 0);

    bool PgmInit();
    bool PgmClose();
    QByteArray PgmGetChipType();
    bool PgmSeek(int address);
    bool PgmWrite(QByteArray data);
    QByteArray PgmRead(int length);

signals:
    void readingFlash(int bytesRead, int bytesTotal);
    void writingFlash(int bytesWritten, int bytesTotal);

public slots:

private:
    char PgmCalculateChecksum(QByteArray data);
    bool PgmSendPacket(QByteArray message);
    QByteArray PgmReadPacket();

    int m_readbuffer_size;
    int m_writebuffer_size;
    char m_command_sequence;
};

#endif // ARDUINO_STK500V2_H
