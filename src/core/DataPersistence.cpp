#include "DataPersistence.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>

DataPersistence::DataPersistence(QObject* parent)
    : QObject(parent)
    , m_autoSave(true)
{
    // Default data directory
    m_dataDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    ensureDirectoryExists(m_dataDirectory);
}

DataPersistence::~DataPersistence()
{
}

QString DataPersistence::dataDirectory() const
{
    return m_dataDirectory;
}

void DataPersistence::setDataDirectory(const QString& path)
{
    m_dataDirectory = path;
    ensureDirectoryExists(m_dataDirectory);
}

bool DataPersistence::saveFriendList(const QList<SerialPortInfo>& friends)
{
    QJsonArray array;
    for (const auto& info : friends) {
        array.append(info.toJson());
    }
    
    QJsonDocument doc(array);
    if (writeJsonFile(friendListPath(), doc)) {
        emit dataSaved();
        return true;
    }
    return false;
}

QList<SerialPortInfo> DataPersistence::loadFriendList()
{
    QList<SerialPortInfo> friends;
    QJsonDocument doc = readJsonFile(friendListPath());
    
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        for (const QJsonValue& value : array) {
            if (value.isObject()) {
                friends.append(SerialPortInfo::fromJson(value.toObject()));
            }
        }
    }
    
    return friends;
}

bool DataPersistence::saveChatGroups(const QList<ChatGroupInfo>& groups)
{
    QJsonArray array;
    for (const auto& group : groups) {
        array.append(group.toJson());
    }
    
    QJsonDocument doc(array);
    if (writeJsonFile(chatGroupsPath(), doc)) {
        emit dataSaved();
        return true;
    }
    return false;
}

QList<ChatGroupInfo> DataPersistence::loadChatGroups()
{
    QList<ChatGroupInfo> groups;
    QJsonDocument doc = readJsonFile(chatGroupsPath());
    
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        for (const QJsonValue& value : array) {
            if (value.isObject()) {
                groups.append(ChatGroupInfo::fromJson(value.toObject()));
            }
        }
    }
    
    return groups;
}

bool DataPersistence::saveMessages(const QString& portName, const QList<Message>& messages)
{
    QString dirPath = m_dataDirectory + "/messages";
    ensureDirectoryExists(dirPath);
    
    QJsonArray array;
    for (const auto& msg : messages) {
        array.append(msg.toJson());
    }
    
    QJsonDocument doc(array);
    return writeJsonFile(messagesPath(portName), doc);
}

QList<Message> DataPersistence::loadMessages(const QString& portName)
{
    QList<Message> messages;
    QJsonDocument doc = readJsonFile(messagesPath(portName));
    
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        for (const QJsonValue& value : array) {
            if (value.isObject()) {
                messages.append(Message::fromJson(value.toObject()));
            }
        }
    }
    
    return messages;
}

bool DataPersistence::saveGroupMessages(const QString& groupId, const QList<Message>& messages)
{
    QString dirPath = m_dataDirectory + "/group_messages";
    ensureDirectoryExists(dirPath);
    
    QJsonArray array;
    for (const auto& msg : messages) {
        array.append(msg.toJson());
    }
    
    QJsonDocument doc(array);
    return writeJsonFile(groupMessagesPath(groupId), doc);
}

QList<Message> DataPersistence::loadGroupMessages(const QString& groupId)
{
    QList<Message> messages;
    QJsonDocument doc = readJsonFile(groupMessagesPath(groupId));
    
    if (doc.isArray()) {
        QJsonArray array = doc.array();
        for (const QJsonValue& value : array) {
            if (value.isObject()) {
                messages.append(Message::fromJson(value.toObject()));
            }
        }
    }
    
    return messages;
}

void DataPersistence::clearAllData()
{
    QDir dir(m_dataDirectory);
    dir.removeRecursively();
    ensureDirectoryExists(m_dataDirectory);
}

void DataPersistence::clearMessages()
{
    QDir messagesDir(m_dataDirectory + "/messages");
    messagesDir.removeRecursively();
    
    QDir groupMessagesDir(m_dataDirectory + "/group_messages");
    groupMessagesDir.removeRecursively();
}

void DataPersistence::setAutoSave(bool enabled)
{
    m_autoSave = enabled;
}

QString DataPersistence::friendListPath() const
{
    return m_dataDirectory + "/friends.json";
}

QString DataPersistence::chatGroupsPath() const
{
    return m_dataDirectory + "/groups.json";
}

QString DataPersistence::messagesPath(const QString& portName) const
{
    // Sanitize port name for filename
    QString safeName = portName;
    safeName.replace("/", "_").replace("\\", "_").replace(":", "_");
    return m_dataDirectory + "/messages/" + safeName + ".json";
}

QString DataPersistence::groupMessagesPath(const QString& groupId) const
{
    return m_dataDirectory + "/group_messages/" + groupId + ".json";
}

bool DataPersistence::ensureDirectoryExists(const QString& path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}

bool DataPersistence::writeJsonFile(const QString& path, const QJsonDocument& doc)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        emit error(tr("Cannot open file for writing: %1").arg(path));
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

QJsonDocument DataPersistence::readJsonFile(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonDocument();
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        emit this->error(tr("JSON parse error: %1").arg(error.errorString()));
        return QJsonDocument();
    }
    
    return doc;
}
