#ifndef CHAT_WIDGET_H
#define CHAT_WIDGET_H

#include "ChatBubble.h"
#include "Message.h"
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

class SerialPortManager;
class MessageManager;
class ChatGroup;

/**
 * @brief Chat widget for serial port communication
 *
 * Displays messages in a chat-like interface with bubbles
 * and provides input controls for sending data.
 */
class ChatWidget : public QWidget {
    Q_OBJECT

  public:
    explicit ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget() override;

    // Set managers
    void setPortManager(SerialPortManager *manager);
    void setMessageManager(MessageManager *manager);
    void setCurrentGroup(ChatGroup *group);

    // Current port
    void setCurrentPort(const QString &portName);
    QString currentPort() const { return m_currentPort; }

    // Group mode
    void setGroupMode(bool enabled);
    void setGroupId(const QString &groupId);
    bool isGroupMode() const { return m_isGroupMode; }
    QString groupId() const { return m_groupId; }

    // Display settings
    void setDisplayFormat(MessageFormat format);
    MessageFormat displayFormat() const { return m_displayFormat; }

    // Messages
    void addMessage(const Message &message);
    void clearMessages();
    void loadMessages(const QList<Message> &messages);

    // UI updates
    void updateHeader();

  signals:
    void sendDataRequested(const QString &portName, const QByteArray &data);
    void sendGroupDataRequested(const QString &groupId, const QByteArray &data, const QStringList &targetPorts);
    void clearHistoryRequested(const QString &portName);
    void portSettingsRequested(const QString &portName);
    void groupSettingsRequested(const QString &groupId);
    void connectPortRequested(const QString &portName);
    void disconnectPortRequested(const QString &portName);
    void groupForwardingToggled(const QString &groupId, bool enabled);

  public slots:
    void onMessageReceived(const Message &message);

  private slots:
    void onSendClicked();
    void onClearClicked();
    void onFormatChanged(int index);
    void onHexModeChanged(bool checked);
    void onTitleClicked();
    void onStatusClicked();
    void onMembersButtonClicked();

  private:
    // Managers
    SerialPortManager *m_portManager;
    MessageManager *m_messageManager;
    ChatGroup *m_currentGroup;

    // Current state
    QString m_currentPort;
    QString m_groupId;
    bool m_isGroupMode;
    MessageFormat m_displayFormat;
    bool m_sendAsHex;

    // UI Components
    QVBoxLayout *m_mainLayout;

    // Header
    QWidget *m_headerWidget;
    QHBoxLayout *m_headerLayout;
    QPushButton *m_titleButton;
    QPushButton *m_statusButton;
    QPushButton *m_membersButton;
    QComboBox *m_formatCombo;

    // Chat area
    QScrollArea *m_scrollArea;
    QWidget *m_chatContainer;
    QVBoxLayout *m_chatLayout;
    QList<ChatBubble *> m_bubbles;

    // Input area
    QWidget *m_inputWidget;
    QVBoxLayout *m_inputLayout;
    QHBoxLayout *m_inputTopLayout;
    QTextEdit *m_inputEdit;
    QHBoxLayout *m_buttonLayout;
    QCheckBox *m_hexCheckBox;
    QPushButton *m_clearButton;
    QPushButton *m_sendButton;

    // Group target selection
    QWidget *m_targetWidget;
    QHBoxLayout *m_targetLayout;
    QLabel *m_targetLabel;
    QListWidget *m_targetList;
    QCheckBox *m_sendToAllCheckBox;

    void setupUi();
    void setupHeader();
    void setupChatArea();
    void setupInputArea();
    void setupTargetSelection();
    void scrollToBottom();
    void updateTargetList();
    QByteArray prepareData(const QString &text);
    QStringList getSelectedTargets();
};

#endif // CHAT_WIDGET_H
