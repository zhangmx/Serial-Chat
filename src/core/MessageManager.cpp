#include "MessageManager.h"

MessageManager::MessageManager(QObject* parent)
    : QObject(parent)
    , m_maxMessagesPerPort(1000)
{
}

MessageManager::~MessageManager()
{
}

void MessageManager::addMessage(const Message& message)
{
    QString portName = message.portName();
    m_messages[portName].append(message);
    trimMessages(portName);
    emit messageAdded(portName, message);
}

void MessageManager::addMessage(const QString& portName, const QByteArray& data, MessageDirection direction)
{
    Message msg(portName, data, direction);
    addMessage(msg);
}

QList<Message> MessageManager::getMessages(const QString& portName) const
{
    return m_messages.value(portName);
}

QList<Message> MessageManager::getMessages(const QString& portName, int limit) const
{
    QList<Message> messages = m_messages.value(portName);
    if (limit > 0 && messages.size() > limit) {
        return messages.mid(messages.size() - limit);
    }
    return messages;
}

QList<Message> MessageManager::getAllMessages() const
{
    QList<Message> all;
    for (const auto& messages : m_messages) {
        all.append(messages);
    }
    
    // Sort by timestamp
    std::sort(all.begin(), all.end(), [](const Message& a, const Message& b) {
        return a.timestamp() < b.timestamp();
    });
    
    return all;
}

Message MessageManager::getLastMessage(const QString& portName) const
{
    QList<Message> messages = m_messages.value(portName);
    if (messages.isEmpty()) {
        return Message();
    }
    return messages.last();
}

int MessageManager::messageCount(const QString& portName) const
{
    return m_messages.value(portName).size();
}

int MessageManager::totalMessageCount() const
{
    int total = 0;
    for (const auto& messages : m_messages) {
        total += messages.size();
    }
    return total;
}

void MessageManager::clearMessages(const QString& portName)
{
    m_messages.remove(portName);
    emit messagesCleared(portName);
}

void MessageManager::clearAllMessages()
{
    m_messages.clear();
    emit allMessagesCleared();
}

void MessageManager::addGroupMessage(const QString& groupId, const Message& message)
{
    m_groupMessages[groupId].append(message);
    emit groupMessageAdded(groupId, message);
}

QList<Message> MessageManager::getGroupMessages(const QString& groupId) const
{
    return m_groupMessages.value(groupId);
}

void MessageManager::clearGroupMessages(const QString& groupId)
{
    m_groupMessages.remove(groupId);
}

void MessageManager::trimMessages(const QString& portName)
{
    if (!m_messages.contains(portName)) {
        return;
    }
    
    QList<Message>& messages = m_messages[portName];
    while (messages.size() > m_maxMessagesPerPort) {
        messages.removeFirst();
    }
}
