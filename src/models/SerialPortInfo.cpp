#include "SerialPortInfo.h"

SerialPortInfo::SerialPortInfo()
    : m_baudRate(115200)
    , m_dataBits(QSerialPort::Data8)
    , m_stopBits(QSerialPort::OneStop)
    , m_parity(QSerialPort::NoParity)
    , m_flowControl(QSerialPort::NoFlowControl)
    , m_status(PortStatus::Offline)
{
}

SerialPortInfo::SerialPortInfo(const QString& portName)
    : m_portName(portName)
    , m_baudRate(115200)
    , m_dataBits(QSerialPort::Data8)
    , m_stopBits(QSerialPort::OneStop)
    , m_parity(QSerialPort::NoParity)
    , m_flowControl(QSerialPort::NoFlowControl)
    , m_status(PortStatus::Offline)
{
}

QString SerialPortInfo::displayName() const
{
    if (m_remark.isEmpty()) {
        return m_portName;
    }
    return QString("%1 (%2)").arg(m_remark, m_portName);
}

void SerialPortInfo::applyToPort(QSerialPort* port) const
{
    if (!port) return;
    
    port->setPortName(m_portName);
    port->setBaudRate(m_baudRate);
    port->setDataBits(m_dataBits);
    port->setStopBits(m_stopBits);
    port->setParity(m_parity);
    port->setFlowControl(m_flowControl);
}

QJsonObject SerialPortInfo::toJson() const
{
    QJsonObject json;
    json["portName"] = m_portName;
    json["remark"] = m_remark;
    json["baudRate"] = m_baudRate;
    json["dataBits"] = static_cast<int>(m_dataBits);
    json["stopBits"] = static_cast<int>(m_stopBits);
    json["parity"] = static_cast<int>(m_parity);
    json["flowControl"] = static_cast<int>(m_flowControl);
    json["lastActiveTime"] = m_lastActiveTime.toString(Qt::ISODate);
    return json;
}

SerialPortInfo SerialPortInfo::fromJson(const QJsonObject& json)
{
    SerialPortInfo info;
    info.m_portName = json["portName"].toString();
    info.m_remark = json["remark"].toString();
    info.m_baudRate = json["baudRate"].toInt(115200);
    info.m_dataBits = static_cast<QSerialPort::DataBits>(json["dataBits"].toInt(8));
    info.m_stopBits = static_cast<QSerialPort::StopBits>(json["stopBits"].toInt(1));
    info.m_parity = static_cast<QSerialPort::Parity>(json["parity"].toInt(0));
    info.m_flowControl = static_cast<QSerialPort::FlowControl>(json["flowControl"].toInt(0));
    info.m_lastActiveTime = QDateTime::fromString(json["lastActiveTime"].toString(), Qt::ISODate);
    info.m_status = PortStatus::Offline;
    return info;
}

bool SerialPortInfo::operator==(const SerialPortInfo& other) const
{
    return m_portName == other.m_portName;
}
