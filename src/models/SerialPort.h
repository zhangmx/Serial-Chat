#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialPort : public QObject
{
    Q_OBJECT

public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort() override;

    bool openPort(const QString &portName, qint32 baudRate = QSerialPort::Baud9600);
    void closePort();
    bool isOpen() const;

    bool writeData(const QByteArray &data);
    QByteArray readData();

    QString portName() const;
    qint32 baudRate() const;

    static QList<QSerialPortInfo> availablePorts();

signals:
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);
    void connectionStatusChanged(bool connected);

private slots:
    void handleReadyRead();
    void handleError(QSerialPort::SerialPortError error);

private:
    QSerialPort *m_serialPort;
    QString m_portName;
    qint32 m_baudRate;
};

#endif // SERIALPORT_H
