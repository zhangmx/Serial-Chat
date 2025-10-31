#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>
#include <QDateTime>
#include <QObject>

// Forward declaration
class SerialPort;

class Message : public QObject
{
    Q_OBJECT

public:
    enum Direction {
        Incoming,
        Outgoing,
        System
    };
    Q_ENUM(Direction)

    explicit Message(QObject *parent = nullptr);
    Message(const QByteArray &content, Direction direction, SerialPort *sourcePort = nullptr, QObject *parent = nullptr);

    QByteArray content() const { return m_content; }
    void setContent(const QByteArray &content);

    QDateTime timestamp() const { return m_timestamp; }
    void setTimestamp(const QDateTime &timestamp);

    Direction direction() const { return m_direction; }
    void setDirection(Direction direction);

    SerialPort* sourcePort() const { return m_sourcePort; }
    void setSourcePort(SerialPort *port);

Q_SIGNALS:
    void contentChanged();
    void timestampChanged();
    void directionChanged();
    void sourcePortChanged();

private:
    QByteArray m_content;
    QDateTime m_timestamp;
    Direction m_direction;
    SerialPort *m_sourcePort;
};

#endif // MESSAGE_H
