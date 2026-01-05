#include "ChatWidget.h"
#include "HexUtils.h"
#include "SerialPortManager.h"
#include "MessageManager.h"
#include "ChatGroup.h"
#include <QScrollBar>
#include <QTimer>
#include <QMessageBox>

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
    , m_portManager(nullptr)
    , m_messageManager(nullptr)
    , m_currentGroup(nullptr)
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

void ChatWidget::setCurrentGroup(ChatGroup* group)
{
    m_currentGroup = group;
    if (group) {
        m_groupId = group->id();
        m_isGroupMode = true;
        updateHeader();
        updateTargetList();
    }
}

void ChatWidget::setCurrentPort(const QString& portName)
{
    m_currentPort = portName;
    m_isGroupMode = false;
    m_currentGroup = nullptr;
    m_groupId.clear();
    updateHeader();
    
    // Hide target selection for single port mode
    m_targetWidget->hide();
    
    // Load messages for this port
    if (m_messageManager) {
        loadMessages(m_messageManager->getMessages(portName));
    }
}

void ChatWidget::setGroupMode(bool enabled)
{
    m_isGroupMode = enabled;
    m_targetWidget->setVisible(enabled);
    updateHeader();
}

void ChatWidget::setGroupId(const QString& groupId)
{
    m_groupId = groupId;
    m_isGroupMode = true;
    m_currentPort.clear();
    updateHeader();
    updateTargetList();
    
    // Show target selection for group mode
    m_targetWidget->show();
    
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
    
    if (m_isGroupMode) {
        QStringList targets = getSelectedTargets();
        emit sendGroupDataRequested(m_groupId, data, targets);
    } else {
        emit sendDataRequested(m_currentPort, data);
    }
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
    
    m_titleButton = new QPushButton(tr("Select a port to chat"), m_headerWidget);
    m_titleButton->setIcon(QIcon(":/icons/settings.png"));
    m_titleButton->setStyleSheet("QPushButton { border: none; background: transparent; text-align: left; font-size: 14px; font-weight: bold; padding: 5px; } QPushButton:hover { background-color: #E0E0E0; border-radius: 5px; }");
    m_titleButton->setCursor(Qt::PointingHandCursor);
    connect(m_titleButton, &QPushButton::clicked, this, &ChatWidget::onTitleClicked);
    
    m_statusButton = new QPushButton(m_headerWidget);
    m_statusButton->setStyleSheet("QPushButton { border: 1px solid #E0E0E0; border-radius: 12px; padding: 4px 12px; font-size: 11px; } QPushButton:hover { background-color: #E0E0E0; }");
    m_statusButton->setCursor(Qt::PointingHandCursor);
    m_statusButton->hide();
    connect(m_statusButton, &QPushButton::clicked, this, &ChatWidget::onStatusClicked);
    
    m_membersButton = new QPushButton(m_headerWidget);
    m_membersButton->setIcon(QIcon(":/icons/group.png"));
    m_membersButton->setToolTip(tr("Manage group members"));
    m_membersButton->setStyleSheet("QPushButton { border: none; background: transparent; padding: 5px; } QPushButton:hover { background-color: #E0E0E0; border-radius: 5px; }");
    m_membersButton->setCursor(Qt::PointingHandCursor);
    m_membersButton->hide();
    connect(m_membersButton, &QPushButton::clicked, this, &ChatWidget::onMembersButtonClicked);
    
    m_formatCombo = new QComboBox(m_headerWidget);
    m_formatCombo->addItem(tr("Text"));
    m_formatCombo->addItem(tr("Hex"));
    m_formatCombo->setFixedWidth(80);
    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChatWidget::onFormatChanged);
    
    m_headerLayout->addWidget(m_titleButton);
    m_headerLayout->addWidget(m_statusButton);
    m_headerLayout->addWidget(m_membersButton);
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
    
    // Target selection for group mode
    setupTargetSelection();
    
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
    m_sendButton->setIcon(QIcon(":/icons/send.png"));
    m_sendButton->setStyleSheet("QPushButton { padding: 8px 30px; border: none; border-radius: 5px; background-color: #07C160; color: white; font-weight: bold; } QPushButton:hover { background-color: #06AD56; }");
    connect(m_sendButton, &QPushButton::clicked, this, &ChatWidget::onSendClicked);
    
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_clearButton);
    m_buttonLayout->addWidget(m_sendButton);
    
    m_inputLayout->addWidget(m_targetWidget);
    m_inputLayout->addLayout(m_inputTopLayout);
    m_inputLayout->addWidget(m_inputEdit);
    m_inputLayout->addLayout(m_buttonLayout);
    
    m_mainLayout->addWidget(m_inputWidget);
}

void ChatWidget::setupTargetSelection()
{
    m_targetWidget = new QWidget(m_inputWidget);
    m_targetWidget->hide();
    
    m_targetLayout = new QHBoxLayout(m_targetWidget);
    m_targetLayout->setContentsMargins(0, 0, 0, 5);
    m_targetLayout->setSpacing(10);
    
    m_targetLabel = new QLabel(tr("Send to:"), m_targetWidget);
    
    m_sendToAllCheckBox = new QCheckBox(tr("All members"), m_targetWidget);
    m_sendToAllCheckBox->setChecked(true);
    connect(m_sendToAllCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        m_targetList->setEnabled(!checked);
        if (checked) {
            for (int i = 0; i < m_targetList->count(); ++i) {
                m_targetList->item(i)->setCheckState(Qt::Checked);
            }
        }
    });
    
    m_targetList = new QListWidget(m_targetWidget);
    m_targetList->setMaximumHeight(60);
    m_targetList->setFlow(QListView::LeftToRight);
    m_targetList->setStyleSheet("QListWidget { border: 1px solid #E0E0E0; border-radius: 5px; }");
    m_targetList->setEnabled(false);
    
    m_targetLayout->addWidget(m_targetLabel);
    m_targetLayout->addWidget(m_sendToAllCheckBox);
    m_targetLayout->addWidget(m_targetList, 1);
}

void ChatWidget::scrollToBottom()
{
    QScrollBar* scrollBar = m_scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWidget::updateHeader()
{
    if (m_isGroupMode) {
        QString groupName = m_groupId;
        if (m_currentGroup) {
            groupName = m_currentGroup->name();
        }
        m_titleButton->setText(tr("Group: %1").arg(groupName));
        
        // Show group forwarding status
        bool forwarding = m_currentGroup ? m_currentGroup->isForwardingEnabled() : false;
        if (forwarding) {
            m_statusButton->setText(tr("Forwarding ON"));
            m_statusButton->setStyleSheet("QPushButton { border: 1px solid #07C160; border-radius: 12px; padding: 4px 12px; font-size: 11px; color: #07C160; background-color: #E8F5E9; } QPushButton:hover { background-color: #C8E6C9; }");
        } else {
            m_statusButton->setText(tr("Forwarding OFF"));
            m_statusButton->setStyleSheet("QPushButton { border: 1px solid #757575; border-radius: 12px; padding: 4px 12px; font-size: 11px; color: #757575; } QPushButton:hover { background-color: #E0E0E0; }");
        }
        m_statusButton->show();
        m_membersButton->show();
    } else if (m_currentPort.isEmpty()) {
        m_titleButton->setText(tr("Select a port to chat"));
        m_statusButton->hide();
        m_membersButton->hide();
    } else {
        m_titleButton->setText(m_currentPort);
        m_membersButton->hide();
        
        if (m_portManager) {
            SerialPortUser* user = m_portManager->getUser(m_currentPort);
            if (user && user->isOnline()) {
                m_statusButton->setText(tr("Online - Click to Disconnect"));
                m_statusButton->setStyleSheet("QPushButton { border: 1px solid #07C160; border-radius: 12px; padding: 4px 12px; font-size: 11px; color: #07C160; background-color: #E8F5E9; } QPushButton:hover { background-color: #FFEBEE; border-color: #F44336; color: #F44336; }");
                m_statusButton->show();
            } else {
                m_statusButton->setText(tr("Offline - Click to Connect"));
                m_statusButton->setStyleSheet("QPushButton { border: 1px solid #757575; border-radius: 12px; padding: 4px 12px; font-size: 11px; color: #757575; } QPushButton:hover { background-color: #E8F5E9; border-color: #07C160; color: #07C160; }");
                m_statusButton->show();
            }
        }
    }
}

void ChatWidget::updateTargetList()
{
    m_targetList->clear();
    
    if (!m_currentGroup || !m_portManager) {
        return;
    }
    
    QStringList members = m_currentGroup->members();
    for (const QString& portName : members) {
        QListWidgetItem* item = new QListWidgetItem(portName, m_targetList);
        item->setData(Qt::UserRole, portName);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        
        // Add status indicator
        SerialPortUser* user = m_portManager->getUser(portName);
        if (user && user->isOnline()) {
            item->setIcon(QIcon(":/icons/online.png"));
        } else {
            item->setIcon(QIcon(":/icons/offline.png"));
        }
    }
}

QStringList ChatWidget::getSelectedTargets()
{
    QStringList targets;
    
    if (m_sendToAllCheckBox->isChecked()) {
        // Return all members
        for (int i = 0; i < m_targetList->count(); ++i) {
            targets.append(m_targetList->item(i)->data(Qt::UserRole).toString());
        }
    } else {
        // Return only checked items
        for (int i = 0; i < m_targetList->count(); ++i) {
            QListWidgetItem* item = m_targetList->item(i);
            if (item->checkState() == Qt::Checked) {
                targets.append(item->data(Qt::UserRole).toString());
            }
        }
    }
    
    return targets;
}

void ChatWidget::onTitleClicked()
{
    if (m_isGroupMode && !m_groupId.isEmpty()) {
        emit groupSettingsRequested(m_groupId);
    } else if (!m_currentPort.isEmpty()) {
        emit portSettingsRequested(m_currentPort);
    }
}

void ChatWidget::onStatusClicked()
{
    if (m_isGroupMode) {
        // Toggle group forwarding
        bool currentState = m_currentGroup ? m_currentGroup->isForwardingEnabled() : false;
        emit groupForwardingToggled(m_groupId, !currentState);
        if (m_currentGroup) {
            m_currentGroup->setForwardingEnabled(!currentState);
        }
        updateHeader();
    } else if (!m_currentPort.isEmpty()) {
        // Toggle port connection
        if (m_portManager) {
            SerialPortUser* user = m_portManager->getUser(m_currentPort);
            if (user && user->isOnline()) {
                emit disconnectPortRequested(m_currentPort);
            } else {
                emit connectPortRequested(m_currentPort);
            }
        }
    }
}

void ChatWidget::onMembersButtonClicked()
{
    if (m_isGroupMode && !m_groupId.isEmpty()) {
        emit groupSettingsRequested(m_groupId);
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
