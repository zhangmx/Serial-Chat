#ifndef SERIAL_PORT_INFO_H
#define SERIAL_PORT_INFO_H

#include <QString>
#include <QJsonObject>
#include <QSerialPort>
#include <QDateTime>

/**
 * @brief Serial port connection status
 */
enum class PortStatus {
    Offline,    // Port is not connected
    Online,     // Port is connected and ready
    Error       // Port has an error
};

/**
 * @brief Contains information about a serial port "user"
 */
class SerialPortInfo {
public:
    SerialPortInfo();
    explicit SerialPortInfo(const QString& portName);
    
    // Port identification
    QString portName() const { return m_portName; }
    QString remark() const { return m_remark; }
    QString displayName() const;
    
    // Port settings
    qint32 baudRate() const { return m_baudRate; }
    QSerialPort::DataBits dataBits() const { return m_dataBits; }
    QSerialPort::StopBits stopBits() const { return m_stopBits; }
    QSerialPort::Parity parity() const { return m_parity; }
    QSerialPort::FlowControl flowControl() const { return m_flowControl; }
    
    // Status
    PortStatus status() const { return m_status; }
    bool isOnline() const { return m_status == PortStatus::Online; }
    QDateTime lastActiveTime() const { return m_lastActiveTime; }
    
    // Setters
    void setPortName(const QString& name) { m_portName = name; }
    void setRemark(const QString& remark) { m_remark = remark; }
    void setBaudRate(qint32 baudRate) { m_baudRate = baudRate; }
    void setDataBits(QSerialPort::DataBits dataBits) { m_dataBits = dataBits; }
    void setStopBits(QSerialPort::StopBits stopBits) { m_stopBits = stopBits; }
    void setParity(QSerialPort::Parity parity) { m_parity = parity; }
    void setFlowControl(QSerialPort::FlowControl flowControl) { m_flowControl = flowControl; }
    void setStatus(PortStatus status) { m_status = status; }
    void updateLastActiveTime() { m_lastActiveTime = QDateTime::currentDateTime(); }
    
    // Serialization
    QJsonObject toJson() const;
    static SerialPortInfo fromJson(const QJsonObject& json);
    
    // Apply settings to a QSerialPort
    void applyToPort(QSerialPort* port) const;
    
    // Operators
    bool operator==(const SerialPortInfo& other) const;
    bool operator!=(const SerialPortInfo& other) const { return !(*this == other); }

private:
    QString m_portName;
    QString m_remark;
    qint32 m_baudRate;
    QSerialPort::DataBits m_dataBits;
    QSerialPort::StopBits m_stopBits;
    QSerialPort::Parity m_parity;
    QSerialPort::FlowControl m_flowControl;
    PortStatus m_status;
    QDateTime m_lastActiveTime;
};

#endif // SERIAL_PORT_INFO_H
