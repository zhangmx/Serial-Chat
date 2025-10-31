#include "SerialPort.h"

SerialPort::SerialPort(QObject *parent)
    : QObject(parent)
    , m_serialPort(new QSerialPort(this))
    , m_baudRate(QSerialPort::Baud9600)
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialPort::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &SerialPort::handleError);
}

SerialPort::~SerialPort()
{
    closePort();
}

bool SerialPort::openPort(const QString &portName, qint32 baudRate)
{
    closePort();

    m_portName = portName;
    m_baudRate = baudRate;

    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    bool opened = m_serialPort->open(QIODevice::ReadWrite);
    emit connectionStatusChanged(opened);
    
    return opened;
}

void SerialPort::closePort()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
        emit connectionStatusChanged(false);
    }
}

bool SerialPort::isOpen() const
{
    return m_serialPort->isOpen();
}

bool SerialPort::writeData(const QByteArray &data)
{
    if (!m_serialPort->isOpen()) {
        return false;
    }

    qint64 bytesWritten = m_serialPort->write(data);
    return bytesWritten == data.size();
}

QByteArray SerialPort::readData()
{
    if (!m_serialPort->isOpen()) {
        return QByteArray();
    }

    return m_serialPort->readAll();
}

QString SerialPort::portName() const
{
    return m_portName;
}

qint32 SerialPort::baudRate() const
{
    return m_baudRate;
}

QList<QSerialPortInfo> SerialPort::availablePorts()
{
    return QSerialPortInfo::availablePorts();
}

void SerialPort::handleReadyRead()
{
    QByteArray data = m_serialPort->readAll();
    if (!data.isEmpty()) {
        emit dataReceived(data);
    }
}

void SerialPort::handleError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError && error != QSerialPort::TimeoutError) {
        emit errorOccurred(m_serialPort->errorString());
    }
}
