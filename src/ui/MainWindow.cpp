#include "MainWindow.h"
#include "ChatGroupDialog.h"
#include "SerialPortRemarkDialog.h"
#include "SerialPortSettingsDialog.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_portManager(new SerialPortManager(this)), m_messageManager(new MessageManager(this)),
      m_dataPersistence(new DataPersistence(this)) {
    setupUi();
    setupMenuBar();
    setupStatusBar();
    setupConsoleDock();
    setupConnections();
    loadData();

    // Start auto-refresh
    m_portManager->setAutoRefresh(true);
    m_statusTimer->start(1000);

    logMessage(tr("Serial Chat started"));
}

MainWindow::~MainWindow() {
    saveData();
    qDeleteAll(m_chatGroups);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveData();
    m_portManager->disconnectAll();
    event->accept();
}

void MainWindow::onAddPortRequested() {
    SerialPortSettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        SerialPortInfo info = dialog.portInfo();
        if (!info.portName().isEmpty()) {
            m_portManager->createUser(info);
            m_portManager->updatePortSettings(info);
            m_friendListWidget->refreshList();
        }
    }
}

void MainWindow::onPortSelected(const QString &portName) {
    m_chatWidget->setCurrentPort(portName);

    // Clear unread count for this port
    m_friendListWidget->clearUnread(portName);

    // Load message history
    QList<Message> messages = m_messageManager->getMessages(portName);
    m_chatWidget->loadMessages(messages);
}

void MainWindow::onGroupSelected(const QString &groupId) {
    ChatGroup *group = getChatGroup(groupId);
    if (group) {
        m_chatWidget->setCurrentGroup(group);
    } else {
        m_chatWidget->setGroupId(groupId);
    }

    // Load group message history
    QList<Message> messages = m_messageManager->getGroupMessages(groupId);
    m_chatWidget->loadMessages(messages);
}

void MainWindow::onConnectRequested(const QString &portName) {
    if (m_portManager->connectPort(portName)) {
        m_friendListWidget->refreshList();
    }
}

void MainWindow::onDisconnectRequested(const QString &portName) {
    m_portManager->disconnectPort(portName);
    m_friendListWidget->refreshList();
}

void MainWindow::onPortSettingsRequested(const QString &portName) {
    SerialPortUser *user = m_portManager->getUser(portName);
    if (!user) {
        return;
    }

    SerialPortSettingsDialog dialog(user->info(), this);
    if (dialog.exec() == QDialog::Accepted) {
        m_portManager->updatePortSettings(dialog.portInfo());
        m_friendListWidget->refreshList();
    }
}

void MainWindow::onPortRemarkRequested(const QString &portName) {
    SerialPortUser *user = m_portManager->getUser(portName);
    QString currentRemark = user ? user->info().remark() : QString();

    SerialPortRemarkDialog dialog(portName, currentRemark, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_portManager->setPortRemark(portName, dialog.remark());
        m_friendListWidget->refreshList();
    }
}

void MainWindow::onCreateGroupRequested() {
    ChatGroupDialog dialog(m_portManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        ChatGroupInfo info = dialog.groupInfo();
        createChatGroup(info);
        m_friendListWidget->addGroup(info);
        saveData();
    }
}

void MainWindow::onSendDataRequested(const QString &portName, const QByteArray &data) {
    SerialPortUser *user = m_portManager->getUser(portName);
    if (!user) {
        user = m_portManager->createUser(portName);
    }

    if (!user->isOnline()) {
        if (!user->connect()) {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Failed to connect to %1: %2").arg(portName, user->errorString()));
            return;
        }
        m_friendListWidget->refreshList();
    }

    if (user->sendData(data)) {
        // Message will be added via signal
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Failed to send data: %1").arg(user->errorString()));
    }
}

void MainWindow::onClearHistoryRequested(const QString &portName) { m_messageManager->clearMessages(portName); }

void MainWindow::onUserStatusChanged(const QString &portName, PortStatus status) {
    Q_UNUSED(status)
    m_friendListWidget->refreshList();

    // Update chat widget if this is the current port
    if (m_chatWidget->currentPort() == portName) {
        m_chatWidget->updateHeader();
    }

    updateStatusBar();
}

void MainWindow::onUserMessageReceived(const QString &portName, const Message &message) {
    // Add to message manager
    m_messageManager->addMessage(message);

    // Update chat widget based on current mode
    if (m_chatWidget->isGroupMode()) {
        // In group mode, only show messages from group members
        QString currentGroupId = m_chatWidget->groupId();
        ChatGroup *group = getChatGroup(currentGroupId);
        if (group && group->hasMember(portName)) {
            m_chatWidget->addMessage(message);
        }
    } else if (m_chatWidget->currentPort() == portName) {
        // In single port mode
        m_chatWidget->addMessage(message);
    } else {
        // Increment unread count for non-active ports
        m_friendListWidget->incrementUnread(portName);
    }

    // Update last message in friend list
    QString preview = message.data().left(50);
    m_friendListWidget->updateLastMessage(portName, preview);

    // Forward to chat groups (for storage only)
    for (ChatGroup *group : m_chatGroups) {
        if (group->hasMember(portName)) {
            m_messageManager->addGroupMessage(group->id(), message);
        }
    }
}

void MainWindow::onUserMessageSent(const QString &portName, const Message &message) {
    // Add to message manager
    m_messageManager->addMessage(message);

    // Update chat widget based on current mode
    if (m_chatWidget->isGroupMode()) {
        // In group mode, only show if this port is a member
        QString currentGroupId = m_chatWidget->groupId();
        ChatGroup *group = getChatGroup(currentGroupId);
        if (group && group->hasMember(portName)) {
            m_chatWidget->addMessage(message);
        }
    } else if (m_chatWidget->currentPort() == portName) {
        // In single port mode
        m_chatWidget->addMessage(message);
    }

    // Update last message in friend list (show what we sent)
    QString preview = QString("[%1] ").arg(tr("Sent")) + message.data().left(40);
    m_friendListWidget->updateLastMessage(portName, preview);
}

void MainWindow::onDeletePortRequested(const QString &portName) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Delete Port"),
        tr("Are you sure you want to delete '%1'?\nThis will also clear its message history.").arg(portName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Disconnect if connected
        m_portManager->disconnectPort(portName);

        // Remove from friend list
        m_portManager->removeUser(portName);
        m_friendListWidget->removeFriend(portName);

        // Clear message history
        m_messageManager->clearMessages(portName);

        // Clear chat if this was selected
        if (m_chatWidget->currentPort() == portName) {
            m_chatWidget->clearMessages();
            m_chatWidget->setCurrentPort(QString());
        }

        logMessage(tr("Deleted port '%1'").arg(portName));
        saveData();
    }
}

void MainWindow::onRefreshPorts() { m_portManager->refreshAvailablePorts(); }

void MainWindow::onDisconnectAll() {
    m_portManager->disconnectAll();
    m_friendListWidget->refreshList();
}

void MainWindow::onClearAllHistory() {
    QMessageBox::StandardButton reply =
        QMessageBox::question(this, tr("Clear History"), tr("Are you sure you want to clear all message history?"),
                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_messageManager->clearAllMessages();
        m_chatWidget->clearMessages();
    }
}

void MainWindow::onExportHistory() {
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Export History"), QString(), tr("JSON Files (*.json);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    }

    // Export all messages to JSON
    QJsonObject root;
    root["exportTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["version"] = APP_VERSION;

    // Export port messages
    QJsonObject portMessages;
    QList<SerialPortInfo> friends = m_portManager->friendList();
    for (const SerialPortInfo &info : friends) {
        QList<Message> messages = m_messageManager->getMessages(info.portName());
        QJsonArray msgArray;
        for (const Message &msg : messages) {
            msgArray.append(msg.toJson());
        }
        if (!msgArray.isEmpty()) {
            portMessages[info.portName()] = msgArray;
        }
    }
    root["portMessages"] = portMessages;

    // Export group messages
    QJsonObject groupMessages;
    for (auto it = m_chatGroups.begin(); it != m_chatGroups.end(); ++it) {
        QList<Message> messages = m_messageManager->getGroupMessages(it.key());
        QJsonArray msgArray;
        for (const Message &msg : messages) {
            msgArray.append(msg.toJson());
        }
        if (!msgArray.isEmpty()) {
            QJsonObject groupObj;
            groupObj["name"] = it.value()->name();
            groupObj["messages"] = msgArray;
            groupMessages[it.key()] = groupObj;
        }
    }
    root["groupMessages"] = groupMessages;

    // Write to file
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(root);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        logMessage(tr("History exported to %1").arg(fileName));
        QMessageBox::information(this, tr("Export"), tr("History exported successfully"));
    } else {
        logError(tr("Failed to export history: %1").arg(file.errorString()));
        QMessageBox::warning(this, tr("Export"), tr("Failed to export history: %1").arg(file.errorString()));
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, tr("About Serial Chat"),
                       tr("<h3>Serial Chat</h3>"
                          "<p>Version %1</p>"
                          "<p>A serial port communication tool with a chat-like interface.</p>"
                          "<p>Serial ports are treated as 'users' that can send and receive messages.</p>"
                          "<p>Copyright © 2026</p>")
                           .arg(APP_VERSION));
}

void MainWindow::updateStatusBar() {
    int online = m_portManager->onlineCount();
    int total = m_portManager->totalCount();
    m_connectionLabel->setText(tr("Online: %1/%2").arg(online).arg(total));
}

void MainWindow::setupUi() {
    setWindowTitle(tr("Serial Chat"));
    setMinimumSize(900, 600);
    resize(1100, 700);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QHBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_splitter = new QSplitter(Qt::Horizontal, m_centralWidget);

    m_friendListWidget = new FriendListWidget(m_splitter);
    m_friendListWidget->setPortManager(m_portManager);

    m_chatWidget = new ChatWidget(m_splitter);
    m_chatWidget->setPortManager(m_portManager);
    m_chatWidget->setMessageManager(m_messageManager);

    m_splitter->addWidget(m_friendListWidget);
    m_splitter->addWidget(m_chatWidget);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setSizes({300, 700});

    m_mainLayout->addWidget(m_splitter);
}

void MainWindow::setupMenuBar() {
    // File menu
    m_fileMenu = menuBar()->addMenu(tr("&File"));

    m_refreshAction = m_fileMenu->addAction(tr("&Refresh Ports"));
    m_refreshAction->setIcon(QIcon(":/icons/refresh.png"));
    m_refreshAction->setShortcut(QKeySequence::Refresh);
    connect(m_refreshAction, &QAction::triggered, this, &MainWindow::onRefreshPorts);

    m_fileMenu->addSeparator();

    m_exportHistoryAction = m_fileMenu->addAction(tr("&Export History..."));
    connect(m_exportHistoryAction, &QAction::triggered, this, &MainWindow::onExportHistory);

    m_fileMenu->addSeparator();

    m_exitAction = m_fileMenu->addAction(tr("E&xit"));
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &QMainWindow::close);

    // Port menu
    m_portMenu = menuBar()->addMenu(tr("&Port"));

    m_addPortAction = m_portMenu->addAction(tr("&Add Port..."));
    m_addPortAction->setIcon(QIcon(":/icons/add.png"));
    m_addPortAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    connect(m_addPortAction, &QAction::triggered, this, &MainWindow::onAddPortRequested);

    m_portMenu->addSeparator();

    m_connectAllAction = m_portMenu->addAction(tr("&Connect All"));
    m_connectAllAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C));
    connect(m_connectAllAction, &QAction::triggered, this, &MainWindow::onConnectAll);

    m_disconnectAllAction = m_portMenu->addAction(tr("&Disconnect All"));
    connect(m_disconnectAllAction, &QAction::triggered, this, &MainWindow::onDisconnectAll);

    // View menu
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    m_clearHistoryAction = m_viewMenu->addAction(tr("&Clear All History"));
    connect(m_clearHistoryAction, &QAction::triggered, this, &MainWindow::onClearAllHistory);

    m_viewMenu->addSeparator();

    m_toggleConsoleAction = m_viewMenu->addAction(tr("&Console"));
    m_toggleConsoleAction->setCheckable(true);
    m_toggleConsoleAction->setChecked(false);
    m_toggleConsoleAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_QuoteLeft));
    connect(m_toggleConsoleAction, &QAction::triggered, this, &MainWindow::onToggleConsole);

    // Help menu
    m_helpMenu = menuBar()->addMenu(tr("&Help"));

    m_aboutAction = m_helpMenu->addAction(tr("&About"));
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::setupStatusBar() {
    m_statusLabel = new QLabel(tr("Ready"));
    statusBar()->addWidget(m_statusLabel, 1);

    m_connectionLabel = new QLabel(tr("Online: 0/0"));
    statusBar()->addPermanentWidget(m_connectionLabel);

    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
}

void MainWindow::setupConnections() {
    // Friend list connections
    connect(m_friendListWidget, &FriendListWidget::portSelected, this, &MainWindow::onPortSelected);
    connect(m_friendListWidget, &FriendListWidget::groupSelected, this, &MainWindow::onGroupSelected);
    connect(m_friendListWidget, &FriendListWidget::addPortRequested, this, &MainWindow::onAddPortRequested);
    connect(m_friendListWidget, &FriendListWidget::createGroupRequested, this, &MainWindow::onCreateGroupRequested);
    connect(m_friendListWidget, &FriendListWidget::connectRequested, this, &MainWindow::onConnectRequested);
    connect(m_friendListWidget, &FriendListWidget::disconnectRequested, this, &MainWindow::onDisconnectRequested);
    connect(m_friendListWidget, &FriendListWidget::portSettingsRequested, this, &MainWindow::onPortSettingsRequested);
    connect(m_friendListWidget, &FriendListWidget::portRemarkRequested, this, &MainWindow::onPortRemarkRequested);

    // Chat widget connections
    connect(m_chatWidget, &ChatWidget::sendDataRequested, this, &MainWindow::onSendDataRequested);
    connect(m_chatWidget, &ChatWidget::sendGroupDataRequested, this, &MainWindow::onSendGroupDataRequested);
    connect(m_chatWidget, &ChatWidget::clearHistoryRequested, this, &MainWindow::onClearHistoryRequested);
    connect(m_chatWidget, &ChatWidget::portSettingsRequested, this, &MainWindow::onPortSettingsRequested);
    connect(m_chatWidget, &ChatWidget::groupSettingsRequested, this, &MainWindow::onGroupSettingsRequested);
    connect(m_chatWidget, &ChatWidget::connectPortRequested, this, &MainWindow::onConnectRequested);
    connect(m_chatWidget, &ChatWidget::disconnectPortRequested, this, &MainWindow::onDisconnectRequested);
    connect(m_chatWidget, &ChatWidget::groupForwardingToggled, this, &MainWindow::onGroupForwardingToggled);

    // Port manager connections
    connect(m_portManager, &SerialPortManager::userStatusChanged, this, &MainWindow::onUserStatusChanged);
    connect(m_portManager, &SerialPortManager::userMessageReceived, this, &MainWindow::onUserMessageReceived);
    connect(m_portManager, &SerialPortManager::userMessageSent, this, &MainWindow::onUserMessageSent);

    // Delete port handling
    connect(m_friendListWidget, &FriendListWidget::deletePortRequested, this, &MainWindow::onDeletePortRequested);

    // Group signals from friend list
    connect(m_friendListWidget, &FriendListWidget::deleteGroupRequested, this, &MainWindow::onDeleteGroupRequested);
    connect(m_friendListWidget, &FriendListWidget::groupSettingsRequested, this, &MainWindow::onGroupSettingsRequested);
}

void MainWindow::loadData() {
    // Load friend list
    QList<SerialPortInfo> friends = m_dataPersistence->loadFriendList();
    for (const SerialPortInfo &info : friends) {
        m_portManager->addToFriendList(info);
    }
    m_friendListWidget->refreshList();

    // Load chat groups
    QList<ChatGroupInfo> groups = m_dataPersistence->loadChatGroups();
    for (const ChatGroupInfo &info : groups) {
        createChatGroup(info);
        m_friendListWidget->addGroup(info);
    }
}

void MainWindow::saveData() {
    // Save friend list
    m_dataPersistence->saveFriendList(m_portManager->friendList());

    // Save chat groups
    QList<ChatGroupInfo> groups;
    for (ChatGroup *group : m_chatGroups) {
        groups.append(group->info());
    }
    m_dataPersistence->saveChatGroups(groups);
}

void MainWindow::createChatGroup(const ChatGroupInfo &info) {
    if (m_chatGroups.contains(info.id())) {
        return;
    }

    ChatGroup *group = new ChatGroup(info, m_portManager, this);
    m_chatGroups.insert(info.id(), group);

    // Connect group signals
    connect(group, &ChatGroup::messageReceived, this, [this, group](const Message &message) {
        m_messageManager->addGroupMessage(group->id(), message);
        if (m_chatWidget->isGroupMode() && m_chatWidget->groupId() == group->id()) {
            m_chatWidget->addMessage(message);
        }
    });
}

ChatGroup *MainWindow::getChatGroup(const QString &groupId) { return m_chatGroups.value(groupId, nullptr); }

void MainWindow::setupConsoleDock() {
    m_consoleDock = new QDockWidget(tr("Console"), this);
    m_consoleDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

    m_consoleOutput = new QTextEdit(m_consoleDock);
    m_consoleOutput->setReadOnly(true);
    m_consoleOutput->setStyleSheet("QTextEdit { background-color: #1E1E1E; color: #D4D4D4; font-family: 'Consolas', "
                                   "'Courier New', monospace; font-size: 12px; }");
    m_consoleOutput->setMinimumHeight(100);
    m_consoleOutput->setMaximumHeight(200);

    m_consoleDock->setWidget(m_consoleOutput);
    addDockWidget(Qt::BottomDockWidgetArea, m_consoleDock);
    m_consoleDock->hide();
}

void MainWindow::onConnectAll() {
    int connected = 0;
    int failed = 0;

    QList<SerialPortInfo> friends = m_portManager->friendList();
    for (const SerialPortInfo &info : friends) {
        if (!info.isOnline()) {
            if (m_portManager->connectPort(info.portName())) {
                connected++;
                logMessage(tr("Connected to %1").arg(info.portName()));
            } else {
                failed++;
                logWarning(tr("Failed to connect to %1").arg(info.portName()));
            }
        }
    }

    m_friendListWidget->refreshList();
    logMessage(tr("Connect All completed: %1 connected, %2 failed").arg(connected).arg(failed));
}

void MainWindow::onToggleConsole() {
    m_consoleDock->setVisible(!m_consoleDock->isVisible());
    m_toggleConsoleAction->setChecked(m_consoleDock->isVisible());
}

void MainWindow::logMessage(const QString &message) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_consoleOutput->append(QString("<span style='color: #6A9955;'>[%1]</span> %2").arg(timestamp, message));
}

void MainWindow::logError(const QString &message) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_consoleOutput->append(
        QString("<span style='color: #F14C4C;'>[%1] ERROR:</span> <span style='color: #F14C4C;'>%2</span>")
            .arg(timestamp, message));
}

void MainWindow::logWarning(const QString &message) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_consoleOutput->append(QString("<span style='color: #CCA700;'>[%1] WARNING:</span> %2").arg(timestamp, message));
}

void MainWindow::onGroupSettingsRequested(const QString &groupId) {
    ChatGroup *group = getChatGroup(groupId);
    if (!group) {
        return;
    }

    ChatGroupDialog dialog(group->info(), m_portManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        group->setInfo(dialog.groupInfo());
        m_friendListWidget->updateGroup(dialog.groupInfo());
        saveData();
        logMessage(tr("Group '%1' settings updated").arg(group->name()));
    }
}

void MainWindow::onGroupForwardingToggled(const QString &groupId, bool enabled) {
    ChatGroup *group = getChatGroup(groupId);
    if (group) {
        group->setForwardingEnabled(enabled);
        saveData();
        logMessage(tr("Group '%1' forwarding %2").arg(group->name(), enabled ? tr("enabled") : tr("disabled")));
    }
}

void MainWindow::onDeleteGroupRequested(const QString &groupId) {
    ChatGroup *group = getChatGroup(groupId);
    if (!group) {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Delete Group"), tr("Are you sure you want to delete group '%1'?").arg(group->name()),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QString groupName = group->name();

        // Remove from UI
        m_friendListWidget->removeGroup(groupId);

        // Clear chat if this was selected
        if (m_chatWidget->isGroupMode() && m_chatWidget->groupId() == groupId) {
            m_chatWidget->clearMessages();
            m_chatWidget->setCurrentPort(QString());
        }

        // Remove from internal map
        m_chatGroups.remove(groupId);
        delete group;

        // Clear group message history
        m_messageManager->clearGroupMessages(groupId);

        logMessage(tr("Deleted group '%1'").arg(groupName));
        saveData();
    }
}

void MainWindow::onSendGroupDataRequested(const QString &groupId, const QByteArray &data,
                                          const QStringList &targetPorts) {
    ChatGroup *group = getChatGroup(groupId);
    if (!group) {
        logError(tr("Group not found: %1").arg(groupId));
        return;
    }

    for (const QString &portName : targetPorts) {
        SerialPortUser *user = m_portManager->getUser(portName);
        if (!user) {
            user = m_portManager->createUser(portName);
        }

        if (!user->isOnline()) {
            if (!user->connect()) {
                logWarning(tr("Failed to connect to %1: %2").arg(portName, user->errorString()));
                continue;
            }
            m_friendListWidget->refreshList();
        }

        if (user->sendData(data)) {
            Message msg(portName, data, MessageDirection::Sent);
            m_messageManager->addGroupMessage(groupId, msg);
            if (m_chatWidget->isGroupMode() && m_chatWidget->groupId() == groupId) {
                m_chatWidget->addMessage(msg);
            }
        } else {
            logWarning(tr("Failed to send to %1: %2").arg(portName, user->errorString()));
        }
    }
}
