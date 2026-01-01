#include "SerialPortUser.h"
#include "HexUtils.h"

SerialPortUser::SerialPortUser(QObject* parent)
    : QObject(parent)
    , m_port(nullptr)
{
}

SerialPortUser::SerialPortUser(const SerialPortInfo& info, QObject* parent)
    : QObject(parent)
    , m_port(nullptr)
    , m_info(info)
{
}

SerialPortUser::~SerialPortUser()
{
    disconnect();
    if (m_port) {
        delete m_port;
        m_port = nullptr;
    }
}

PortStatus SerialPortUser::status() const
{
    if (!m_port) {
        return PortStatus::Offline;
    }
    if (m_port->isOpen()) {
        return PortStatus::Online;
    }
    if (m_port->error() != QSerialPort::NoError) {
        return PortStatus::Error;
    }
    return PortStatus::Offline;
}

void SerialPortUser::setInfo(const SerialPortInfo& info)
{
    bool wasOpen = isOnline();
    if (wasOpen) {
        disconnect();
    }
    
    m_info = info;
    
    if (wasOpen) {
        connect();
    }
}

void SerialPortUser::setRemark(const QString& remark)
{
    m_info.setRemark(remark);
}

bool SerialPortUser::connect()
{
    if (isOnline()) {
        return true;
    }
    
    if (!m_port) {
        m_port = new QSerialPort(this);
        setupConnections();
    }
    
    m_info.applyToPort(m_port);
    
    if (!m_port->open(QIODevice::ReadWrite)) {
        m_errorString = m_port->errorString();
        updateStatus(PortStatus::Error);
        emit errorOccurred(m_errorString);
        return false;
    }
    
    m_errorString.clear();
    m_info.updateLastActiveTime();
    updateStatus(PortStatus::Online);
    emit connected();
    return true;
}

void SerialPortUser::disconnect()
{
    if (m_port && m_port->isOpen()) {
        m_port->close();
        updateStatus(PortStatus::Offline);
        emit disconnected();
    }
}

bool SerialPortUser::sendData(const QByteArray& data)
{
    if (!isOnline()) {
        m_errorString = tr("Port is not open");
        emit errorOccurred(m_errorString);
        return false;
    }
    
    qint64 bytesWritten = m_port->write(data);
    if (bytesWritten == -1) {
        m_errorString = m_port->errorString();
        emit errorOccurred(m_errorString);
        return false;
    }
    
    m_port->flush();
    m_info.updateLastActiveTime();
    
    Message msg(m_info.portName(), data, MessageDirection::Sent);
    emit messageSent(msg);
    
    return true;
}

bool SerialPortUser::sendText(const QString& text)
{
    return sendData(text.toUtf8());
}

bool SerialPortUser::sendHex(const QString& hexString)
{
    QByteArray data = HexUtils::hexStringToByteArray(hexString);
    if (data.isEmpty() && !hexString.isEmpty()) {
        m_errorString = tr("Invalid hex string");
        emit errorOccurred(m_errorString);
        return false;
    }
    return sendData(data);
}

QByteArray SerialPortUser::readAll()
{
    QByteArray data = m_receiveBuffer;
    m_receiveBuffer.clear();
    return data;
}

void SerialPortUser::setupConnections()
{
    if (!m_port) return;
    
    QObject::connect(m_port, &QSerialPort::readyRead,
                     this, &SerialPortUser::onReadyRead);
    QObject::connect(m_port, &QSerialPort::errorOccurred,
                     this, &SerialPortUser::onErrorOccurred);
}

void SerialPortUser::onReadyRead()
{
    QByteArray data = m_port->readAll();
    if (!data.isEmpty()) {
        m_receiveBuffer.append(data);
        m_info.updateLastActiveTime();
        
        Message msg(m_info.portName(), data, MessageDirection::Received);
        emit dataReceived(data);
        emit messageReceived(msg);
    }
}

void SerialPortUser::onErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        return;
    }
    
    m_errorString = m_port->errorString();
    
    if (error == QSerialPort::ResourceError) {
        // Device was removed
        disconnect();
    }
    
    updateStatus(PortStatus::Error);
    emit errorOccurred(m_errorString);
}

void SerialPortUser::updateStatus(PortStatus status)
{
    m_info.setStatus(status);
    emit statusChanged(status);
}
