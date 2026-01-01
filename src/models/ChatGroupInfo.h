#ifndef CHAT_GROUP_INFO_H
#define CHAT_GROUP_INFO_H

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QDateTime>

/**
 * @brief Contains information about a custom chat group
 * 
 * A chat group allows multiple serial ports to communicate with each other.
 * Messages from any port in the group can be forwarded to other ports.
 */
class ChatGroupInfo {
public:
    ChatGroupInfo();
    explicit ChatGroupInfo(const QString& name);
    
    // Identification
    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString description() const { return m_description; }
    
    // Members
    QStringList members() const { return m_members; }
    int memberCount() const { return m_members.size(); }
    bool hasMember(const QString& portName) const;
    
    // Settings
    bool isForwardingEnabled() const { return m_forwardingEnabled; }
    QDateTime createdTime() const { return m_createdTime; }
    
    // Setters
    void setName(const QString& name) { m_name = name; }
    void setDescription(const QString& description) { m_description = description; }
    void setForwardingEnabled(bool enabled) { m_forwardingEnabled = enabled; }
    
    // Member management
    void addMember(const QString& portName);
    void removeMember(const QString& portName);
    void clearMembers();
    void setMembers(const QStringList& members) { m_members = members; }
    
    // Serialization
    QJsonObject toJson() const;
    static ChatGroupInfo fromJson(const QJsonObject& json);
    
    // Operators
    bool operator==(const ChatGroupInfo& other) const;
    bool operator!=(const ChatGroupInfo& other) const { return !(*this == other); }

private:
    QString m_id;
    QString m_name;
    QString m_description;
    QStringList m_members;
    bool m_forwardingEnabled;
    QDateTime m_createdTime;
    
    static QString generateId();
};

#endif // CHAT_GROUP_INFO_H
