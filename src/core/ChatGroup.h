#ifndef CHAT_GROUP_H
#define CHAT_GROUP_H

#include <QObject>
#include <QList>
#include "ChatGroupInfo.h"
#include "Message.h"
#include "SerialPortUser.h"

class SerialPortManager;

/**
 * @brief Manages a chat group where multiple serial ports can communicate
 * 
 * This class handles:
 * - Message forwarding between group members
 * - Group message history
 * - Member management
 */
class ChatGroup : public QObject {
    Q_OBJECT

public:
    ChatGroup(const ChatGroupInfo& info, SerialPortManager* manager, QObject* parent = nullptr);
    ~ChatGroup() override;
    
    // Group information
    ChatGroupInfo info() const { return m_info; }
    QString id() const { return m_info.id(); }
    QString name() const { return m_info.name(); }
    
    // Member management
    void addMember(const QString& portName);
    void removeMember(const QString& portName);
    QStringList members() const { return m_info.members(); }
    bool hasMember(const QString& portName) const;
    
    // Forwarding control
    bool isForwardingEnabled() const { return m_info.isForwardingEnabled(); }
    void setForwardingEnabled(bool enabled);
    
    // Message history
    QList<Message> messageHistory() const { return m_messageHistory; }
    void clearHistory();
    
    // Settings
    void setInfo(const ChatGroupInfo& info);
    void setName(const QString& name);
    void setDescription(const QString& description);

signals:
    void messageReceived(const Message& message);
    void messageForwarded(const QString& fromPort, const QString& toPort, const Message& message);
    void memberAdded(const QString& portName);
    void memberRemoved(const QString& portName);
    void infoChanged();

private slots:
    void onMemberMessageReceived(const QString& portName, const Message& message);

private:
    ChatGroupInfo m_info;
    SerialPortManager* m_portManager;
    QList<Message> m_messageHistory;
    
    void forwardMessage(const QString& fromPort, const QByteArray& data);
    void connectMember(const QString& portName);
    void disconnectMember(const QString& portName);
};

#endif // CHAT_GROUP_H
