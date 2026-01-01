#include "Message.h"
#include <QUuid>
#include <QJsonArray>

Message::Message()
    : m_id(generateId())
    , m_direction(MessageDirection::Received)
    , m_timestamp(QDateTime::currentDateTime())
{
}

Message::Message(const QString& portName, const QByteArray& data, 
                 MessageDirection direction, const QDateTime& timestamp)
    : m_id(generateId())
    , m_portName(portName)
    , m_data(data)
    , m_direction(direction)
    , m_timestamp(timestamp)
{
}

QString Message::generateId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString Message::toText() const
{
    return QString::fromUtf8(m_data);
}

QString Message::toHex() const
{
    return QString(m_data.toHex(' ').toUpper());
}

QString Message::displayText(MessageFormat format) const
{
    switch (format) {
        case MessageFormat::Hex:
            return toHex();
        case MessageFormat::Text:
        default:
            return toText();
    }
}

QString Message::formattedTime() const
{
    return m_timestamp.toString("hh:mm:ss");
}

QJsonObject Message::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["portName"] = m_portName;
    json["data"] = QString(m_data.toBase64());
    json["direction"] = static_cast<int>(m_direction);
    json["timestamp"] = m_timestamp.toString(Qt::ISODate);
    return json;
}

Message Message::fromJson(const QJsonObject& json)
{
    Message msg;
    msg.m_id = json["id"].toString();
    msg.m_portName = json["portName"].toString();
    msg.m_data = QByteArray::fromBase64(json["data"].toString().toUtf8());
    msg.m_direction = static_cast<MessageDirection>(json["direction"].toInt());
    msg.m_timestamp = QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate);
    return msg;
}

bool Message::operator==(const Message& other) const
{
    return m_id == other.m_id;
}
