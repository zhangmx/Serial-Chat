#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include <QObject>
#include <QMap>
#include <QList>
#include "Message.h"

/**
 * @brief Manages message history for all serial port conversations
 */
class MessageManager : public QObject {
    Q_OBJECT

public:
    explicit MessageManager(QObject* parent = nullptr);
    ~MessageManager() override;
    
    // Message storage
    void addMessage(const Message& message);
    void addMessage(const QString& portName, const QByteArray& data, MessageDirection direction);
    
    // Message retrieval
    QList<Message> getMessages(const QString& portName) const;
    QList<Message> getMessages(const QString& portName, int limit) const;
    QList<Message> getAllMessages() const;
    Message getLastMessage(const QString& portName) const;
    
    // Message count
    int messageCount(const QString& portName) const;
    int totalMessageCount() const;
    
    // Clear history
    void clearMessages(const QString& portName);
    void clearAllMessages();
    
    // Group messages
    void addGroupMessage(const QString& groupId, const Message& message);
    QList<Message> getGroupMessages(const QString& groupId) const;
    void clearGroupMessages(const QString& groupId);

signals:
    void messageAdded(const QString& portName, const Message& message);
    void messagesCleared(const QString& portName);
    void allMessagesCleared();
    void groupMessageAdded(const QString& groupId, const Message& message);

private:
    QMap<QString, QList<Message>> m_messages;
    QMap<QString, QList<Message>> m_groupMessages;
    int m_maxMessagesPerPort;
    
    void trimMessages(const QString& portName);
};

#endif // MESSAGE_MANAGER_H
