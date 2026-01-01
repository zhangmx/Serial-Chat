#include "MainWindow.h"
#include "SerialPortSettingsDialog.h"
#include "SerialPortRemarkDialog.h"
#include "ChatGroupDialog.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_portManager(new SerialPortManager(this))
    , m_messageManager(new MessageManager(this))
    , m_dataPersistence(new DataPersistence(this))
{
    setupUi();
    setupMenuBar();
    setupStatusBar();
    setupConnections();
    loadData();
    
    // Start auto-refresh
    m_portManager->setAutoRefresh(true);
    m_statusTimer->start(1000);
}

MainWindow::~MainWindow()
{
    saveData();
    qDeleteAll(m_chatGroups);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    saveData();
    m_portManager->disconnectAll();
    event->accept();
}

void MainWindow::onAddPortRequested()
{
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

void MainWindow::onPortSelected(const QString& portName)
{
    m_chatWidget->setCurrentPort(portName);
    
    // Load message history
    QList<Message> messages = m_messageManager->getMessages(portName);
    m_chatWidget->loadMessages(messages);
}

void MainWindow::onGroupSelected(const QString& groupId)
{
    m_chatWidget->setGroupId(groupId);
    
    // Load group message history
    QList<Message> messages = m_messageManager->getGroupMessages(groupId);
    m_chatWidget->loadMessages(messages);
}

void MainWindow::onConnectRequested(const QString& portName)
{
    if (m_portManager->connectPort(portName)) {
        m_friendListWidget->refreshList();
    }
}

void MainWindow::onDisconnectRequested(const QString& portName)
{
    m_portManager->disconnectPort(portName);
    m_friendListWidget->refreshList();
}

void MainWindow::onPortSettingsRequested(const QString& portName)
{
    SerialPortUser* user = m_portManager->getUser(portName);
    if (!user) {
        return;
    }
    
    SerialPortSettingsDialog dialog(user->info(), this);
    if (dialog.exec() == QDialog::Accepted) {
        m_portManager->updatePortSettings(dialog.portInfo());
        m_friendListWidget->refreshList();
    }
}

void MainWindow::onPortRemarkRequested(const QString& portName)
{
    SerialPortUser* user = m_portManager->getUser(portName);
    QString currentRemark = user ? user->info().remark() : QString();
    
    SerialPortRemarkDialog dialog(portName, currentRemark, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_portManager->setPortRemark(portName, dialog.remark());
        m_friendListWidget->refreshList();
    }
}

void MainWindow::onCreateGroupRequested()
{
    ChatGroupDialog dialog(m_portManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        ChatGroupInfo info = dialog.groupInfo();
        createChatGroup(info);
        m_friendListWidget->addGroup(info);
        saveData();
    }
}

void MainWindow::onSendDataRequested(const QString& portName, const QByteArray& data)
{
    SerialPortUser* user = m_portManager->getUser(portName);
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
        QMessageBox::warning(this, tr("Error"), 
            tr("Failed to send data: %1").arg(user->errorString()));
    }
}

void MainWindow::onClearHistoryRequested(const QString& portName)
{
    m_messageManager->clearMessages(portName);
}

void MainWindow::onUserStatusChanged(const QString& portName, PortStatus status)
{
    Q_UNUSED(portName)
    Q_UNUSED(status)
    m_friendListWidget->refreshList();
    updateStatusBar();
}

void MainWindow::onUserMessageReceived(const QString& portName, const Message& message)
{
    // Add to message manager
    m_messageManager->addMessage(message);
    
    // Update chat widget if this is the current port
    if (m_chatWidget->currentPort() == portName) {
        m_chatWidget->addMessage(message);
    }
    
    // Forward to chat groups
    for (ChatGroup* group : m_chatGroups) {
        if (group->hasMember(portName)) {
            m_messageManager->addGroupMessage(group->id(), message);
        }
    }
}

void MainWindow::onRefreshPorts()
{
    m_portManager->refreshAvailablePorts();
}

void MainWindow::onDisconnectAll()
{
    m_portManager->disconnectAll();
    m_friendListWidget->refreshList();
}

void MainWindow::onClearAllHistory()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        tr("Clear History"),
        tr("Are you sure you want to clear all message history?"),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_messageManager->clearAllMessages();
        m_chatWidget->clearMessages();
    }
}

void MainWindow::onExportHistory()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Export History"), QString(), tr("JSON Files (*.json);;All Files (*)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // TODO: Implement export
    QMessageBox::information(this, tr("Export"), tr("Export functionality coming soon"));
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("About Serial Chat"),
        tr("<h3>Serial Chat</h3>"
           "<p>Version 1.0.0</p>"
           "<p>A serial port communication tool with a chat-like interface.</p>"
           "<p>Serial ports are treated as 'users' that can send and receive messages.</p>"
           "<p>Copyright © 2024</p>"));
}

void MainWindow::updateStatusBar()
{
    int online = m_portManager->onlineCount();
    int total = m_portManager->totalCount();
    m_connectionLabel->setText(tr("Online: %1/%2").arg(online).arg(total));
}

void MainWindow::setupUi()
{
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

void MainWindow::setupMenuBar()
{
    // File menu
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    
    m_refreshAction = m_fileMenu->addAction(tr("&Refresh Ports"));
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
    m_addPortAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    connect(m_addPortAction, &QAction::triggered, this, &MainWindow::onAddPortRequested);
    
    m_portMenu->addSeparator();
    
    m_disconnectAllAction = m_portMenu->addAction(tr("&Disconnect All"));
    connect(m_disconnectAllAction, &QAction::triggered, this, &MainWindow::onDisconnectAll);
    
    // View menu
    m_viewMenu = menuBar()->addMenu(tr("&View"));
    
    m_clearHistoryAction = m_viewMenu->addAction(tr("&Clear All History"));
    connect(m_clearHistoryAction, &QAction::triggered, this, &MainWindow::onClearAllHistory);
    
    // Help menu
    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    
    m_aboutAction = m_helpMenu->addAction(tr("&About"));
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(tr("Ready"));
    statusBar()->addWidget(m_statusLabel, 1);
    
    m_connectionLabel = new QLabel(tr("Online: 0/0"));
    statusBar()->addPermanentWidget(m_connectionLabel);
    
    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
}

void MainWindow::setupConnections()
{
    // Friend list connections
    connect(m_friendListWidget, &FriendListWidget::portSelected,
            this, &MainWindow::onPortSelected);
    connect(m_friendListWidget, &FriendListWidget::groupSelected,
            this, &MainWindow::onGroupSelected);
    connect(m_friendListWidget, &FriendListWidget::addPortRequested,
            this, &MainWindow::onAddPortRequested);
    connect(m_friendListWidget, &FriendListWidget::createGroupRequested,
            this, &MainWindow::onCreateGroupRequested);
    connect(m_friendListWidget, &FriendListWidget::connectRequested,
            this, &MainWindow::onConnectRequested);
    connect(m_friendListWidget, &FriendListWidget::disconnectRequested,
            this, &MainWindow::onDisconnectRequested);
    connect(m_friendListWidget, &FriendListWidget::portSettingsRequested,
            this, &MainWindow::onPortSettingsRequested);
    connect(m_friendListWidget, &FriendListWidget::portRemarkRequested,
            this, &MainWindow::onPortRemarkRequested);
    
    // Chat widget connections
    connect(m_chatWidget, &ChatWidget::sendDataRequested,
            this, &MainWindow::onSendDataRequested);
    connect(m_chatWidget, &ChatWidget::clearHistoryRequested,
            this, &MainWindow::onClearHistoryRequested);
    
    // Port manager connections
    connect(m_portManager, &SerialPortManager::userStatusChanged,
            this, &MainWindow::onUserStatusChanged);
    connect(m_portManager, &SerialPortManager::userMessageReceived,
            this, &MainWindow::onUserMessageReceived);
}

void MainWindow::loadData()
{
    // Load friend list
    QList<SerialPortInfo> friends = m_dataPersistence->loadFriendList();
    for (const SerialPortInfo& info : friends) {
        m_portManager->addToFriendList(info);
    }
    m_friendListWidget->refreshList();
    
    // Load chat groups
    QList<ChatGroupInfo> groups = m_dataPersistence->loadChatGroups();
    for (const ChatGroupInfo& info : groups) {
        createChatGroup(info);
        m_friendListWidget->addGroup(info);
    }
}

void MainWindow::saveData()
{
    // Save friend list
    m_dataPersistence->saveFriendList(m_portManager->friendList());
    
    // Save chat groups
    QList<ChatGroupInfo> groups;
    for (ChatGroup* group : m_chatGroups) {
        groups.append(group->info());
    }
    m_dataPersistence->saveChatGroups(groups);
}

void MainWindow::createChatGroup(const ChatGroupInfo& info)
{
    if (m_chatGroups.contains(info.id())) {
        return;
    }
    
    ChatGroup* group = new ChatGroup(info, m_portManager, this);
    m_chatGroups.insert(info.id(), group);
    
    // Connect group signals
    connect(group, &ChatGroup::messageReceived, this, [this, group](const Message& message) {
        m_messageManager->addGroupMessage(group->id(), message);
        if (m_chatWidget->isGroupMode() && m_chatWidget->currentPort().isEmpty()) {
            // Could track current group ID and update
        }
    });
}
