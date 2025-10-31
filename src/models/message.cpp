#include "message.h"

Message::Message(QObject *parent)
    : QObject(parent)
    , m_timestamp(QDateTime::currentDateTime())
    , m_direction(System)
    , m_sourcePort(nullptr)
{
}

Message::Message(const QByteArray &content, Direction direction, SerialPort *sourcePort, QObject *parent)
    : QObject(parent)
    , m_content(content)
    , m_timestamp(QDateTime::currentDateTime())
    , m_direction(direction)
    , m_sourcePort(sourcePort)
{
}

void Message::setContent(const QByteArray &content)
{
    if (m_content != content) {
        m_content = content;
        emit contentChanged();
    }
}

void Message::setTimestamp(const QDateTime &timestamp)
{
    if (m_timestamp != timestamp) {
        m_timestamp = timestamp;
        emit timestampChanged();
    }
}

void Message::setDirection(Direction direction)
{
    if (m_direction != direction) {
        m_direction = direction;
        emit directionChanged();
    }
}

void Message::setSourcePort(SerialPort *port)
{
    m_sourcePort = port;
}
