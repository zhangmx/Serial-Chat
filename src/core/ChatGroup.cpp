#include "ChatGroup.h"
#include "SerialPortManager.h"

ChatGroup::ChatGroup(const ChatGroupInfo& info, SerialPortManager* manager, QObject* parent)
    : QObject(parent)
    , m_info(info)
    , m_portManager(manager)
{
    // Connect to existing members
    for (const QString& portName : m_info.members()) {
        connectMember(portName);
    }
}

ChatGroup::~ChatGroup()
{
    // Disconnect from all members
    for (const QString& portName : m_info.members()) {
        disconnectMember(portName);
    }
}

void ChatGroup::addMember(const QString& portName)
{
    if (m_info.hasMember(portName)) {
        return;
    }
    
    m_info.addMember(portName);
    connectMember(portName);
    emit memberAdded(portName);
    emit infoChanged();
}

void ChatGroup::removeMember(const QString& portName)
{
    if (!m_info.hasMember(portName)) {
        return;
    }
    
    disconnectMember(portName);
    m_info.removeMember(portName);
    emit memberRemoved(portName);
    emit infoChanged();
}

bool ChatGroup::hasMember(const QString& portName) const
{
    return m_info.hasMember(portName);
}

void ChatGroup::setForwardingEnabled(bool enabled)
{
    if (m_info.isForwardingEnabled() != enabled) {
        m_info.setForwardingEnabled(enabled);
        emit infoChanged();
    }
}

void ChatGroup::clearHistory()
{
    m_messageHistory.clear();
}

void ChatGroup::setInfo(const ChatGroupInfo& info)
{
    // Disconnect old members
    for (const QString& portName : m_info.members()) {
        if (!info.hasMember(portName)) {
            disconnectMember(portName);
        }
    }
    
    // Connect new members
    for (const QString& portName : info.members()) {
        if (!m_info.hasMember(portName)) {
            connectMember(portName);
        }
    }
    
    m_info = info;
    emit infoChanged();
}

void ChatGroup::setName(const QString& name)
{
    m_info.setName(name);
    emit infoChanged();
}

void ChatGroup::setDescription(const QString& description)
{
    m_info.setDescription(description);
    emit infoChanged();
}

void ChatGroup::onMemberMessageReceived(const QString& portName, const Message& message)
{
    if (!m_info.hasMember(portName)) {
        return;
    }
    
    // Add to history
    m_messageHistory.append(message);
    emit messageReceived(message);
    
    // Forward to other members if enabled
    if (m_info.isForwardingEnabled()) {
        forwardMessage(portName, message.data());
    }
}

void ChatGroup::forwardMessage(const QString& fromPort, const QByteArray& data)
{
    for (const QString& portName : m_info.members()) {
        if (portName == fromPort) {
            continue;
        }
        
        SerialPortUser* user = m_portManager->getUser(portName);
        if (user && user->isOnline()) {
            if (user->sendData(data)) {
                Message forwardedMsg(portName, data, MessageDirection::Sent);
                emit messageForwarded(fromPort, portName, forwardedMsg);
            }
        }
    }
}

void ChatGroup::connectMember(const QString& portName)
{
    if (!m_portManager) {
        return;
    }
    
    // We rely on the manager's signal for message reception
    QObject::connect(m_portManager, &SerialPortManager::userMessageReceived,
                     this, &ChatGroup::onMemberMessageReceived);
}

void ChatGroup::disconnectMember(const QString& portName)
{
    Q_UNUSED(portName)
    // Note: We don't disconnect the signal here because it's a shared connection
    // The message filter in onMemberMessageReceived handles member-specific filtering
}
