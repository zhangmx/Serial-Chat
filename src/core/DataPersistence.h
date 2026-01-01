#ifndef DATA_PERSISTENCE_H
#define DATA_PERSISTENCE_H

#include <QObject>
#include <QString>
#include <QList>
#include "SerialPortInfo.h"
#include "ChatGroupInfo.h"
#include "Message.h"

/**
 * @brief Handles saving and loading application data
 */
class DataPersistence : public QObject {
    Q_OBJECT

public:
    explicit DataPersistence(QObject* parent = nullptr);
    ~DataPersistence() override;
    
    // Data directory
    QString dataDirectory() const;
    void setDataDirectory(const QString& path);
    
    // Serial port friends
    bool saveFriendList(const QList<SerialPortInfo>& friends);
    QList<SerialPortInfo> loadFriendList();
    
    // Chat groups
    bool saveChatGroups(const QList<ChatGroupInfo>& groups);
    QList<ChatGroupInfo> loadChatGroups();
    
    // Message history
    bool saveMessages(const QString& portName, const QList<Message>& messages);
    QList<Message> loadMessages(const QString& portName);
    bool saveGroupMessages(const QString& groupId, const QList<Message>& messages);
    QList<Message> loadGroupMessages(const QString& groupId);
    
    // Clear data
    void clearAllData();
    void clearMessages();
    
    // Auto-save
    void setAutoSave(bool enabled);
    bool autoSave() const { return m_autoSave; }

signals:
    void dataLoaded();
    void dataSaved();
    void error(const QString& message);

private:
    QString m_dataDirectory;
    bool m_autoSave;
    
    QString friendListPath() const;
    QString chatGroupsPath() const;
    QString messagesPath(const QString& portName) const;
    QString groupMessagesPath(const QString& groupId) const;
    
    bool ensureDirectoryExists(const QString& path);
    bool writeJsonFile(const QString& path, const QJsonDocument& doc);
    QJsonDocument readJsonFile(const QString& path);
};

#endif // DATA_PERSISTENCE_H
