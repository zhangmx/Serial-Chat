#include "ChatGroupInfo.h"
#include <QUuid>
#include <QJsonArray>

ChatGroupInfo::ChatGroupInfo()
    : m_id(generateId())
    , m_forwardingEnabled(true)
    , m_createdTime(QDateTime::currentDateTime())
{
}

ChatGroupInfo::ChatGroupInfo(const QString& name)
    : m_id(generateId())
    , m_name(name)
    , m_forwardingEnabled(true)
    , m_createdTime(QDateTime::currentDateTime())
{
}

QString ChatGroupInfo::generateId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

bool ChatGroupInfo::hasMember(const QString& portName) const
{
    return m_members.contains(portName);
}

void ChatGroupInfo::addMember(const QString& portName)
{
    if (!m_members.contains(portName)) {
        m_members.append(portName);
    }
}

void ChatGroupInfo::removeMember(const QString& portName)
{
    m_members.removeAll(portName);
}

void ChatGroupInfo::clearMembers()
{
    m_members.clear();
}

QJsonObject ChatGroupInfo::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["description"] = m_description;
    json["forwardingEnabled"] = m_forwardingEnabled;
    json["createdTime"] = m_createdTime.toString(Qt::ISODate);
    
    QJsonArray membersArray;
    for (const QString& member : m_members) {
        membersArray.append(member);
    }
    json["members"] = membersArray;
    
    return json;
}

ChatGroupInfo ChatGroupInfo::fromJson(const QJsonObject& json)
{
    ChatGroupInfo info;
    info.m_id = json["id"].toString();
    info.m_name = json["name"].toString();
    info.m_description = json["description"].toString();
    info.m_forwardingEnabled = json["forwardingEnabled"].toBool(true);
    info.m_createdTime = QDateTime::fromString(json["createdTime"].toString(), Qt::ISODate);
    
    QJsonArray membersArray = json["members"].toArray();
    for (const QJsonValue& value : membersArray) {
        info.m_members.append(value.toString());
    }
    
    return info;
}

bool ChatGroupInfo::operator==(const ChatGroupInfo& other) const
{
    return m_id == other.m_id;
}
