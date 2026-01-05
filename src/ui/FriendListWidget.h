#ifndef FRIEND_LIST_WIDGET_H
#define FRIEND_LIST_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QEvent>
#include "SerialPortInfo.h"
#include "ChatGroupInfo.h"
#include "FriendListItem.h"

class SerialPortManager;

/**
 * @brief Widget displaying the list of serial port "friends" and chat groups
 */
class FriendListWidget : public QWidget {
    Q_OBJECT

public:
    explicit FriendListWidget(QWidget* parent = nullptr);
    ~FriendListWidget() override;
    
    // Manager
    void setPortManager(SerialPortManager* manager);
    
    // Friend list management
    void addFriend(const SerialPortInfo& info);
    void removeFriend(const QString& portName);
    void updateFriend(const SerialPortInfo& info);
    void setFriends(const QList<SerialPortInfo>& friends);
    
    // Group management
    void addGroup(const ChatGroupInfo& group);
    void removeGroup(const QString& groupId);
    void updateGroup(const ChatGroupInfo& group);
    void setGroups(const QList<ChatGroupInfo>& groups);
    
    // Selection
    void selectPort(const QString& portName);
    void selectGroup(const QString& groupId);
    QString selectedPort() const { return m_selectedPort; }
    QString selectedGroup() const { return m_selectedGroup; }
    
    // Refresh
    void refreshList();

signals:
    void portSelected(const QString& portName);
    void groupSelected(const QString& groupId);
    void addPortRequested();
    void createGroupRequested();
    void portSettingsRequested(const QString& portName);
    void portRemarkRequested(const QString& portName);
    void connectRequested(const QString& portName);
    void disconnectRequested(const QString& portName);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onItemClicked(const QString& portName);
    void onSearchTextChanged(const QString& text);
    void onAddButtonClicked();
    void onGroupButtonClicked();

private:
    SerialPortManager* m_portManager;
    
    QString m_selectedPort;
    QString m_selectedGroup;
    
    // UI Components
    QVBoxLayout* m_mainLayout;
    
    // Header with search
    QWidget* m_headerWidget;
    QVBoxLayout* m_headerLayout;
    QLabel* m_titleLabel;
    QLineEdit* m_searchEdit;
    
    // Action buttons
    QWidget* m_buttonWidget;
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_addButton;
    QPushButton* m_groupButton;
    
    // Online section
    QWidget* m_onlineSection;
    QVBoxLayout* m_onlineLayout;
    QLabel* m_onlineLabel;
    
    // Offline section  
    QWidget* m_offlineSection;
    QVBoxLayout* m_offlineLayout;
    QLabel* m_offlineLabel;
    
    // Groups section
    QWidget* m_groupSection;
    QVBoxLayout* m_groupLayout;
    QLabel* m_groupLabel;
    
    // Scroll area
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QVBoxLayout* m_scrollLayout;
    
    // Item tracking
    QMap<QString, FriendListItem*> m_friendItems;
    QMap<QString, QWidget*> m_groupItems;
    
    void setupUi();
    void setupHeader();
    void setupSections();
    void clearSelection();
    void updateSectionLabels();
    FriendListItem* createFriendItem(const SerialPortInfo& info);
    QWidget* createGroupItem(const ChatGroupInfo& group);
};

#endif // FRIEND_LIST_WIDGET_H
