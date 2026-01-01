#ifndef CHAT_WIDGET_H
#define CHAT_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QList>
#include "Message.h"
#include "ChatBubble.h"

class SerialPortManager;
class MessageManager;

/**
 * @brief Chat widget for serial port communication
 * 
 * Displays messages in a chat-like interface with bubbles
 * and provides input controls for sending data.
 */
class ChatWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChatWidget(QWidget* parent = nullptr);
    ~ChatWidget() override;
    
    // Set managers
    void setPortManager(SerialPortManager* manager);
    void setMessageManager(MessageManager* manager);
    
    // Current port
    void setCurrentPort(const QString& portName);
    QString currentPort() const { return m_currentPort; }
    
    // Group mode
    void setGroupMode(bool enabled);
    void setGroupId(const QString& groupId);
    bool isGroupMode() const { return m_isGroupMode; }
    
    // Display settings
    void setDisplayFormat(MessageFormat format);
    MessageFormat displayFormat() const { return m_displayFormat; }
    
    // Messages
    void addMessage(const Message& message);
    void clearMessages();
    void loadMessages(const QList<Message>& messages);

signals:
    void sendDataRequested(const QString& portName, const QByteArray& data);
    void clearHistoryRequested(const QString& portName);

public slots:
    void onMessageReceived(const Message& message);

private slots:
    void onSendClicked();
    void onClearClicked();
    void onFormatChanged(int index);
    void onHexModeChanged(bool checked);

private:
    // Managers
    SerialPortManager* m_portManager;
    MessageManager* m_messageManager;
    
    // Current state
    QString m_currentPort;
    QString m_groupId;
    bool m_isGroupMode;
    MessageFormat m_displayFormat;
    bool m_sendAsHex;
    
    // UI Components
    QVBoxLayout* m_mainLayout;
    
    // Header
    QWidget* m_headerWidget;
    QHBoxLayout* m_headerLayout;
    QLabel* m_titleLabel;
    QLabel* m_statusLabel;
    QComboBox* m_formatCombo;
    
    // Chat area
    QScrollArea* m_scrollArea;
    QWidget* m_chatContainer;
    QVBoxLayout* m_chatLayout;
    QList<ChatBubble*> m_bubbles;
    
    // Input area
    QWidget* m_inputWidget;
    QVBoxLayout* m_inputLayout;
    QHBoxLayout* m_inputTopLayout;
    QTextEdit* m_inputEdit;
    QHBoxLayout* m_buttonLayout;
    QCheckBox* m_hexCheckBox;
    QPushButton* m_clearButton;
    QPushButton* m_sendButton;
    
    void setupUi();
    void setupHeader();
    void setupChatArea();
    void setupInputArea();
    void scrollToBottom();
    void updateHeader();
    QByteArray prepareData(const QString& text);
};

#endif // CHAT_WIDGET_H
