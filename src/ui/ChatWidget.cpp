#include "ChatWidget.h"
#include "HexUtils.h"
#include "SerialPortManager.h"
#include "MessageManager.h"
#include <QScrollBar>
#include <QTimer>
#include <QMessageBox>

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , m_portManager(nullptr)
    , m_messageManager(nullptr)
    , m_isGroupMode(false)
    , m_displayFormat(MessageFormat::Text)
    , m_sendAsHex(false)
{
    setupUi();
}

ChatWidget::~ChatWidget()
{
}

void ChatWidget::setPortManager(SerialPortManager* manager)
{
    m_portManager = manager;
}

void ChatWidget::setMessageManager(MessageManager* manager)
{
    m_messageManager = manager;
}

void ChatWidget::setCurrentPort(const QString& portName)
{
    m_currentPort = portName;
    m_isGroupMode = false;
    updateHeader();
    
    // Load messages for this port
    if (m_messageManager) {
        loadMessages(m_messageManager->getMessages(portName));
    }
}

void ChatWidget::setGroupMode(bool enabled)
{
    m_isGroupMode = enabled;
    updateHeader();
}

void ChatWidget::setGroupId(const QString& groupId)
{
    m_groupId = groupId;
    m_isGroupMode = true;
    updateHeader();
    
    // Load group messages
    if (m_messageManager) {
        loadMessages(m_messageManager->getGroupMessages(groupId));
    }
}

void ChatWidget::setDisplayFormat(MessageFormat format)
{
    m_displayFormat = format;
    for (ChatBubble* bubble : m_bubbles) {
        bubble->setFormat(format);
    }
}

void ChatWidget::addMessage(const Message& message)
{
    ChatBubble* bubble = new ChatBubble(message, m_displayFormat, m_chatContainer);
    m_bubbles.append(bubble);
    m_chatLayout->insertWidget(m_chatLayout->count() - 1, bubble);
    
    // Scroll to bottom after adding
    QTimer::singleShot(50, this, &ChatWidget::scrollToBottom);
}

void ChatWidget::clearMessages()
{
    for (ChatBubble* bubble : m_bubbles) {
        m_chatLayout->removeWidget(bubble);
        delete bubble;
    }
    m_bubbles.clear();
}

void ChatWidget::loadMessages(const QList<Message>& messages)
{
    clearMessages();
    for (const Message& msg : messages) {
        addMessage(msg);
    }
}

void ChatWidget::onMessageReceived(const Message& message)
{
    // Only add if it's for our current port or group
    if (!m_isGroupMode && message.portName() == m_currentPort) {
        addMessage(message);
    }
}

void ChatWidget::onSendClicked()
{
    QString text = m_inputEdit->toPlainText();
    if (text.isEmpty()) {
        return;
    }
    
    if (m_currentPort.isEmpty() && !m_isGroupMode) {
        QMessageBox::warning(this, tr("Warning"), tr("No port selected"));
        return;
    }
    
    QByteArray data = prepareData(text);
    if (data.isEmpty() && !text.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Invalid hex format"));
        return;
    }
    
    emit sendDataRequested(m_currentPort, data);
    m_inputEdit->clear();
}

void ChatWidget::onClearClicked()
{
    clearMessages();
    emit clearHistoryRequested(m_currentPort);
}

void ChatWidget::onFormatChanged(int index)
{
    m_displayFormat = (index == 0) ? MessageFormat::Text : MessageFormat::Hex;
    setDisplayFormat(m_displayFormat);
}

void ChatWidget::onHexModeChanged(bool checked)
{
    m_sendAsHex = checked;
    if (checked) {
        m_inputEdit->setPlaceholderText(tr("Enter hex data (e.g., 48 65 6C 6C 6F)"));
    } else {
        m_inputEdit->setPlaceholderText(tr("Enter message..."));
    }
}

void ChatWidget::setupUi()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    setupHeader();
    setupChatArea();
    setupInputArea();
}

void ChatWidget::setupHeader()
{
    m_headerWidget = new QWidget(this);
    m_headerWidget->setStyleSheet("background-color: #F5F5F5; border-bottom: 1px solid #E0E0E0;");
    m_headerWidget->setFixedHeight(50);
    
    m_headerLayout = new QHBoxLayout(m_headerWidget);
    m_headerLayout->setContentsMargins(15, 5, 15, 5);
    
    m_titleLabel = new QLabel(tr("Select a port to chat"), m_headerWidget);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    
    m_statusLabel = new QLabel(m_headerWidget);
    m_statusLabel->setStyleSheet("color: #757575;");
    
    m_formatCombo = new QComboBox(m_headerWidget);
    m_formatCombo->addItem(tr("Text"));
    m_formatCombo->addItem(tr("Hex"));
    m_formatCombo->setFixedWidth(80);
    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChatWidget::onFormatChanged);
    
    m_headerLayout->addWidget(m_titleLabel);
    m_headerLayout->addWidget(m_statusLabel);
    m_headerLayout->addStretch();
    m_headerLayout->addWidget(m_formatCombo);
    
    m_mainLayout->addWidget(m_headerWidget);
}

void ChatWidget::setupChatArea()
{
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #EBEBEB; }");
    
    m_chatContainer = new QWidget(m_scrollArea);
    m_chatContainer->setStyleSheet("background-color: #EBEBEB;");
    
    m_chatLayout = new QVBoxLayout(m_chatContainer);
    m_chatLayout->setContentsMargins(10, 10, 10, 10);
    m_chatLayout->setSpacing(5);
    m_chatLayout->addStretch();
    
    m_scrollArea->setWidget(m_chatContainer);
    m_mainLayout->addWidget(m_scrollArea, 1);
}

void ChatWidget::setupInputArea()
{
    m_inputWidget = new QWidget(this);
    m_inputWidget->setStyleSheet("background-color: #F5F5F5; border-top: 1px solid #E0E0E0;");
    
    m_inputLayout = new QVBoxLayout(m_inputWidget);
    m_inputLayout->setContentsMargins(10, 10, 10, 10);
    m_inputLayout->setSpacing(8);
    
    m_inputTopLayout = new QHBoxLayout();
    
    m_hexCheckBox = new QCheckBox(tr("Hex Mode"), m_inputWidget);
    connect(m_hexCheckBox, &QCheckBox::toggled, this, &ChatWidget::onHexModeChanged);
    
    m_inputTopLayout->addWidget(m_hexCheckBox);
    m_inputTopLayout->addStretch();
    
    m_inputEdit = new QTextEdit(m_inputWidget);
    m_inputEdit->setPlaceholderText(tr("Enter message..."));
    m_inputEdit->setMaximumHeight(80);
    m_inputEdit->setStyleSheet("QTextEdit { border: 1px solid #E0E0E0; border-radius: 5px; padding: 5px; background-color: white; }");
    
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(10);
    
    m_clearButton = new QPushButton(tr("Clear"), m_inputWidget);
    m_clearButton->setStyleSheet("QPushButton { padding: 8px 20px; border: 1px solid #E0E0E0; border-radius: 5px; background-color: white; } QPushButton:hover { background-color: #F0F0F0; }");
    connect(m_clearButton, &QPushButton::clicked, this, &ChatWidget::onClearClicked);
    
    m_sendButton = new QPushButton(tr("Send"), m_inputWidget);
    m_sendButton->setStyleSheet("QPushButton { padding: 8px 30px; border: none; border-radius: 5px; background-color: #07C160; color: white; font-weight: bold; } QPushButton:hover { background-color: #06AD56; }");
    connect(m_sendButton, &QPushButton::clicked, this, &ChatWidget::onSendClicked);
    
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_clearButton);
    m_buttonLayout->addWidget(m_sendButton);
    
    m_inputLayout->addLayout(m_inputTopLayout);
    m_inputLayout->addWidget(m_inputEdit);
    m_inputLayout->addLayout(m_buttonLayout);
    
    m_mainLayout->addWidget(m_inputWidget);
}

void ChatWidget::scrollToBottom()
{
    QScrollBar* scrollBar = m_scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWidget::updateHeader()
{
    if (m_isGroupMode) {
        m_titleLabel->setText(tr("Group: %1").arg(m_groupId));
        m_statusLabel->setText(tr("Group Chat"));
    } else if (m_currentPort.isEmpty()) {
        m_titleLabel->setText(tr("Select a port to chat"));
        m_statusLabel->clear();
    } else {
        m_titleLabel->setText(m_currentPort);
        if (m_portManager) {
            SerialPortUser* user = m_portManager->getUser(m_currentPort);
            if (user && user->isOnline()) {
                m_statusLabel->setText(tr("Online"));
                m_statusLabel->setStyleSheet("color: #07C160;");
            } else {
                m_statusLabel->setText(tr("Offline"));
                m_statusLabel->setStyleSheet("color: #757575;");
            }
        }
    }
}

QByteArray ChatWidget::prepareData(const QString& text)
{
    if (m_sendAsHex) {
        return HexUtils::hexStringToByteArray(text);
    } else {
        return text.toUtf8();
    }
}
