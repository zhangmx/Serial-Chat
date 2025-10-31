#include "Message.h"

Message::Message(QObject *parent)
    : QObject(parent)
    , m_direction(Incoming)
    , m_timestamp(QDateTime::currentDateTime())
{
}

Message::Message(const QString &content, Direction direction, QObject *parent)
    : QObject(parent)
    , m_content(content)
    , m_direction(direction)
    , m_timestamp(QDateTime::currentDateTime())
{
}

QString Message::content() const
{
    return m_content;
}

void Message::setContent(const QString &content)
{
    if (m_content != content) {
        m_content = content;
        emit contentChanged();
    }
}

Message::Direction Message::direction() const
{
    return m_direction;
}

void Message::setDirection(Direction direction)
{
    if (m_direction != direction) {
        m_direction = direction;
        emit directionChanged();
    }
}

QDateTime Message::timestamp() const
{
    return m_timestamp;
}

void Message::setTimestamp(const QDateTime &timestamp)
{
    m_timestamp = timestamp;
}
