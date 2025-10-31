#include "serialport.h"

SerialPort::SerialPort(QObject *parent)
    : QObject(parent)
    , m_baudRate(9600)
    , m_serialPort(new QSerialPort(this))
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialPort::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &SerialPort::handleError);
}

SerialPort::SerialPort(const QString &portName, QObject *parent)
    : QObject(parent)
    , m_portName(portName)
    , m_baudRate(9600)
    , m_serialPort(new QSerialPort(this))
{
    m_serialPort->setPortName(portName);
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialPort::handleReadyRead);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &SerialPort::handleError);
}

SerialPort::~SerialPort()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
}

void SerialPort::setPortName(const QString &portName)
{
    if (m_portName != portName) {
        m_portName = portName;
        m_serialPort->setPortName(portName);
        emit portNameChanged();
    }
}

void SerialPort::setBaudRate(qint32 baudRate)
{
    if (m_baudRate != baudRate) {
        m_baudRate = baudRate;
        m_serialPort->setBaudRate(baudRate);
        emit baudRateChanged();
    }
}

bool SerialPort::isOpen() const
{
    return m_serialPort->isOpen();
}

bool SerialPort::open()
{
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        m_serialPort->setBaudRate(m_baudRate);
        emit connected();
        return true;
    }
    return false;
}

void SerialPort::close()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
        emit disconnected();
    }
}

void SerialPort::sendData(const QByteArray &data)
{
    if (m_serialPort->isOpen()) {
        m_serialPort->write(data);
    }
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
    if (error != QSerialPort::NoError) {
        emit errorOccurred(m_serialPort->errorString());
    }
}
