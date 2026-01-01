#ifndef SERIAL_PORT_USER_H
#define SERIAL_PORT_USER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QTimer>
#include "SerialPortInfo.h"
#include "Message.h"

/**
 * @brief Represents a serial port "user" that can send and receive messages
 * 
 * This class wraps a QSerialPort and provides chat-like functionality,
 * treating the serial port as a user in a messaging system.
 */
class SerialPortUser : public QObject {
    Q_OBJECT

public:
    explicit SerialPortUser(QObject* parent = nullptr);
    explicit SerialPortUser(const SerialPortInfo& info, QObject* parent = nullptr);
    ~SerialPortUser() override;
    
    // Port information
    SerialPortInfo info() const { return m_info; }
    QString portName() const { return m_info.portName(); }
    QString displayName() const { return m_info.displayName(); }
    
    // Status
    bool isOnline() const { return m_port && m_port->isOpen(); }
    PortStatus status() const;
    QString errorString() const { return m_errorString; }
    
    // Settings
    void setInfo(const SerialPortInfo& info);
    void setRemark(const QString& remark);
    
    // Connection management
    bool connect();
    void disconnect();
    
    // Data transmission
    bool sendData(const QByteArray& data);
    bool sendText(const QString& text);
    bool sendHex(const QString& hexString);
    
    // Receive buffer
    QByteArray readAll();
    bool hasData() const { return !m_receiveBuffer.isEmpty(); }

signals:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray& data);
    void messageSent(const Message& message);
    void messageReceived(const Message& message);
    void statusChanged(PortStatus status);
    void errorOccurred(const QString& error);

private slots:
    void onReadyRead();
    void onErrorOccurred(QSerialPort::SerialPortError error);

private:
    QSerialPort* m_port;
    SerialPortInfo m_info;
    QByteArray m_receiveBuffer;
    QString m_errorString;
    
    void setupConnections();
    void updateStatus(PortStatus status);
};

#endif // SERIAL_PORT_USER_H
