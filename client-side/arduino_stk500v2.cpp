#include "arduino_stk500v2.h"

//http://www.atmel.com/Images/doc2591.pdf

ArduinoStk500v2::ArduinoStk500v2(QObject *parent) :
    QSerialPort(parent)
{
    this->m_readbuffer_size = 100;
    this->m_writebuffer_size = 256;
    this->m_command_sequence = 0;
}

/**
 * @brief ArduinoStk500v2::PgmInit
 * @return
 */
bool ArduinoStk500v2::PgmInit()
{
    this->open(QSerialPort::ReadWrite);
    this->setBaudRate(QSerialPort::Baud115200);
    this->setDataBits(QSerialPort::Data8);
    this->setParity(QSerialPort::NoParity);
    this->setStopBits(QSerialPort::OneStop);

    if (!this->isOpen()) {
        qWarning()<<"SerialPort not open";
        return false;
    }

    this->setDataTerminalReady(false);
    this->setRequestToSend(false);

    Sleep(500);

    this->setDataTerminalReady(true);
    this->setRequestToSend(true);

    Sleep(500);

    return PgmSeek(0);
}

/**
 * @brief ArduinoStk500v2::PgmClose
 * @return
 */
bool ArduinoStk500v2::PgmClose()
{
    char closeMessage[] = {0x11};
    PgmSendPacket(QByteArray::fromRawData(closeMessage, sizeof(closeMessage)));
    QByteArray data = PgmReadPacket();
    if (data.length() >= 2 && (char)data[0] == 0x11 && (char)data[1] == 0x00) {
        if (this->isOpen()) {
            this->close();
        }
        this->setDataTerminalReady(false);
        this->setRequestToSend(false);
        return true;
    }
    return false;
}

/**
 * @brief ArduinoStk500v2::PgmSeek CMD_LOAD_ADDRESS
 * @param position
 * @return
 */
bool ArduinoStk500v2::PgmSeek(int address)
{
    if (!this->isOpen()) {
        qWarning()<<"SerialPort not open";
        return false;
    }

    if (address % 2 == 1) {
        qWarning()<<"Address must be an even number";
        return false;
    }

    int addr = address / 2;
    char seekMessage[] = { 0x6, (char)((addr >> 24) & 0xff), (char)((addr >> 16) & 0xff), (char)((addr >> 8) & 0xff), (char)((addr >> 0) & 0xff) };
    PgmSendPacket(QByteArray::fromRawData(seekMessage, sizeof(seekMessage)));
    QByteArray data = PgmReadPacket();
    if (data.length() >= 2 && (char)data[0] == 0x06 && (char)data[1] == 0x00) {
        return true;
    }
    return false;
}

/**
 * @brief ArduinoStk500v2::PgmWrite CMD_PROGRAM_FLASH_ISP
 * @param data
 * @return
 */
bool ArduinoStk500v2::PgmWrite(QByteArray data)
{
    int total = data.length();
    while(data.length() > 0) {
        int blockLength = data.length() > this->m_writebuffer_size ? this->m_writebuffer_size : data.length();
        char writeMessage[] = {
            0x13,
            (char)((blockLength >> 8) ),
            (char)(blockLength & 0xff),
            0x00, //mode
            0x00, //delay
            0x00, //cmd1 AVR_OP_WRITE_LO or AVR_OP_LOADPAGE_LO
            0x00, //cmd2 0 or AVR_OP_WRITEPAGE
            0x00, //cmd3 AVR_OP_READ_LO
            0x00, //pol1
            0x00 //pol2
        };
        QByteArray blockData = QByteArray::fromRawData(writeMessage, sizeof(writeMessage));
        blockData.append(data.left(blockLength));
        data = data.mid(blockLength);
        emit writingFlash(total - data.length(), total);

        PgmSendPacket(blockData);
        QByteArray resultData = PgmReadPacket();
        if (resultData.length() < 2 || (resultData.length() >= 2 && ((char)resultData[0] != 0x13 || (char)resultData[1] != 0x00))) {
            qWarning()<<"Write faild";
            return false;
        }
    }

    return true;
}

/**
 * @brief ArduinoStk500v2::PgmRead CMD_READ_FLASH_ISP
 * @param length
 * @return
 */
QByteArray ArduinoStk500v2::PgmRead(int length)
{
    QByteArray fullData;
    while(1) {
        int blockLength = length > this->m_readbuffer_size ? this->m_readbuffer_size : length;
        length -= blockLength;

        char readMessage[] = { 0x14, (char)((blockLength >> 8) & 0xff), (char)((blockLength >> 0) & 0xff) };
        PgmSendPacket(QByteArray::fromRawData(readMessage, sizeof(readMessage)));
        QByteArray data = PgmReadPacket();

        if (data.length() >= 2 && (char)data[0] == 0x14 && (char)data[1] == 0x00) {
            QByteArray tmp = data.mid(2, blockLength);
            fullData.append(tmp);
            emit readingFlash(fullData.size(), fullData.size() + length);
        } else {
            qWarning()<<"Read faild";
            return QByteArray();
        }

        if (length <= 0) {
            break;
        }
    }

    return fullData;
}

/**
 * @brief ArduinoStk500v2::PgmGetChipType
 * @return
 */
QByteArray ArduinoStk500v2::PgmGetChipType()
{
    QByteArray chipCodes;

    char command[] =  { 0x1b, 0, 0, 0, 0 };
    PgmSendPacket(QByteArray::fromRawData(command, sizeof(command)));
    QByteArray data = PgmReadPacket();
    chipCodes.append(data[2]);

    command[4] = 1;
    PgmSendPacket(QByteArray::fromRawData(command, sizeof(command)));
    data = PgmReadPacket();
    chipCodes.append(data[2]);

    command[4] = 2;
    PgmSendPacket(QByteArray::fromRawData(command, sizeof(command)));
    data = PgmReadPacket();
    chipCodes.append(data[2]);
    return chipCodes;
}

/**
 * @brief ArduinoStk500v2::PgmCalculateChecksum
 * @param data
 * @return
 */
char ArduinoStk500v2::PgmCalculateChecksum(QByteArray data)
{
    char checksum = 0;
    for (int i = 0; i < data.length(); i++) {
        checksum ^= data[i];
    }
    return checksum;
}

/**
 * @brief ArduinoStk500v2::PgmSendPacket
 * @param message
 * @return
 */
bool ArduinoStk500v2::PgmSendPacket(QByteArray message)
{
    this->m_command_sequence++;
    if (this->m_command_sequence > 0xff) {
        this->m_command_sequence = 0x01;
    }

    char messageData[] = {
        0x1b,
        (char)this->m_command_sequence,
        (char)((message.length() >> 8) & 0xff),
        (char)(message.length() & 0xff),
        0x0e,
    };

    QByteArray data = QByteArray::fromRawData(messageData, sizeof(messageData));
    data.append(message);
    data.append(PgmCalculateChecksum(data));
    data.append('\0');

    qDebug()<<">>"<<data.length()<<" "<<data.toHex();
    return this->write(data, data.length()) == data.length();
}

/**
 * @brief ArduinoStk500v2::PgmReadPacket
 * @return
 */
QByteArray ArduinoStk500v2::PgmReadPacket()
{
    QByteArray data;
    QByteArray message;
    int messageLength = 0;
    while(1) {
        this->waitForReadyRead(1000);
        QByteArray tmp = this->readAll();
        data.append(tmp);

        //Check for header
        while(data.length() >= 2) {
            if (!((char)data[0] == 0x1b && (char)data[1] == this->m_command_sequence)) {
                data = data.mid(1);
            } else {
                break;
            }
        }

        //Extracting message
        if (data.length() >= 4 && ((char)data[0] == 0x1b && (char)data[1] == this->m_command_sequence)) {
            messageLength = (data[2] << 8) + data[3];
            //message + header + checksum
            if (data.length() >= messageLength + 6) {
                char checksum = data.mid(5 + messageLength, 1)[0];
                if (checksum != PgmCalculateChecksum(data.left(messageLength + 5))) {
                    qWarning()<<"Checksum wrong";
                    qDebug()<<data.toHex();
                    break;
                }
                if (messageLength > 0) {
                    message = data.mid(5, messageLength);
                }
                data = data.mid(messageLength + 6);
                break;
            }
        }
    }

    qDebug()<<"<<"<<message.length()<<" "<<message.toHex();
    return message;
}
