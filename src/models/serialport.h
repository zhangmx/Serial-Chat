#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QString>

class SerialPort : public QObject
{
    Q_OBJECT

public:
    explicit SerialPort(QObject *parent = nullptr);
    explicit SerialPort(const QString &portName, QObject *parent = nullptr);
    ~SerialPort();

    QString portName() const { return m_portName; }
    void setPortName(const QString &portName);

    qint32 baudRate() const { return m_baudRate; }
    void setBaudRate(qint32 baudRate);

    bool isOpen() const;
    bool open();
    void close();

    void sendData(const QByteArray &data);

Q_SIGNALS:
    void dataReceived(const QByteArray &data);
    void portNameChanged();
    void baudRateChanged();
    void errorOccurred(const QString &error);
    void connected();
    void disconnected();

private Q_SLOTS:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    QString m_portName;
    qint32 m_baudRate;
    QSerialPort *m_serialPort;
};

#endif // SERIALPORT_H
