#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QDateTime>
#include <QByteArray>
#include <QJsonObject>

/**
 * @brief Message direction enum
 */
enum class MessageDirection {
    Sent,       // Message sent from this port
    Received    // Message received by this port
};

/**
 * @brief Message display format enum
 */
enum class MessageFormat {
    Text,   // Display as text
    Hex     // Display as hexadecimal
};

/**
 * @brief Represents a single message in the chat
 */
class Message {
public:
    Message();
    Message(const QString& portName, const QByteArray& data, 
            MessageDirection direction, const QDateTime& timestamp = QDateTime::currentDateTime());
    
    // Getters
    QString id() const { return m_id; }
    QString portName() const { return m_portName; }
    QByteArray data() const { return m_data; }
    MessageDirection direction() const { return m_direction; }
    QDateTime timestamp() const { return m_timestamp; }
    
    // Display methods
    QString toText() const;
    QString toHex() const;
    QString displayText(MessageFormat format = MessageFormat::Text) const;
    QString formattedTime() const;
    
    // Setters
    void setPortName(const QString& portName) { m_portName = portName; }
    void setData(const QByteArray& data) { m_data = data; }
    void setDirection(MessageDirection direction) { m_direction = direction; }
    void setTimestamp(const QDateTime& timestamp) { m_timestamp = timestamp; }
    
    // Serialization
    QJsonObject toJson() const;
    static Message fromJson(const QJsonObject& json);
    
    // Operators
    bool operator==(const Message& other) const;
    bool operator!=(const Message& other) const { return !(*this == other); }

private:
    QString m_id;
    QString m_portName;
    QByteArray m_data;
    MessageDirection m_direction;
    QDateTime m_timestamp;
    
    static QString generateId();
};

#endif // MESSAGE_H
