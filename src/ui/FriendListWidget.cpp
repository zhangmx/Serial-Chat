#include "FriendListWidget.h"
#include "SerialPortManager.h"
#include <QMouseEvent>
#include <QScrollBar>

FriendListWidget::FriendListWidget(QWidget *parent) : QWidget(parent), m_portManager(nullptr) { setupUi(); }

FriendListWidget::~FriendListWidget() {}

bool FriendListWidget::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QWidget *widget = qobject_cast<QWidget *>(watched);
        if (widget) {
            QString groupId = widget->property("groupId").toString();
            if (!groupId.isEmpty()) {
                if (mouseEvent->button() == Qt::RightButton) {
                    showGroupContextMenu(groupId, mouseEvent->globalPos());
                    return true;
                } else if (mouseEvent->button() == Qt::LeftButton) {
                    selectGroup(groupId);
                    return true;
                }
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void FriendListWidget::setPortManager(SerialPortManager *manager) {
    m_portManager = manager;
    if (m_portManager) {
        QObject::connect(m_portManager, &SerialPortManager::friendListChanged, this, &FriendListWidget::refreshList);
        QObject::connect(m_portManager, &SerialPortManager::userStatusChanged, this,
                         [this](const QString &portName, PortStatus status) {
                             if (m_friendItems.contains(portName)) {
                                 m_friendItems[portName]->setStatus(status);
                             }
                         });
        refreshList();
    }
}

void FriendListWidget::addFriend(const SerialPortInfo &info) {
    if (m_friendItems.contains(info.portName())) {
        updateFriend(info);
        return;
    }

    FriendListItem *item = createFriendItem(info);
    m_friendItems.insert(info.portName(), item);

    if (info.isOnline()) {
        m_onlineLayout->addWidget(item);
    } else {
        m_offlineLayout->addWidget(item);
    }

    updateSectionLabels();
}

void FriendListWidget::removeFriend(const QString &portName) {
    if (!m_friendItems.contains(portName)) {
        return;
    }

    FriendListItem *item = m_friendItems.take(portName);
    m_onlineLayout->removeWidget(item);
    m_offlineLayout->removeWidget(item);
    delete item;

    updateSectionLabels();
}

void FriendListWidget::updateFriend(const SerialPortInfo &info) {
    if (!m_friendItems.contains(info.portName())) {
        addFriend(info);
        return;
    }

    FriendListItem *item = m_friendItems[info.portName()];
    item->setInfo(info);
}

void FriendListWidget::setFriends(const QList<SerialPortInfo> &friends) {
    // Clear existing items
    for (FriendListItem *item : m_friendItems) {
        m_onlineLayout->removeWidget(item);
        m_offlineLayout->removeWidget(item);
        delete item;
    }
    m_friendItems.clear();

    // Add new items
    for (const SerialPortInfo &info : friends) {
        addFriend(info);
    }

    updateSectionLabels();
}

void FriendListWidget::addGroup(const ChatGroupInfo &group) {
    if (m_groupItems.contains(group.id())) {
        updateGroup(group);
        return;
    }

    QWidget *item = createGroupItem(group);
    m_groupItems.insert(group.id(), item);
    m_groupLayout->addWidget(item);

    updateSectionLabels();
}

void FriendListWidget::removeGroup(const QString &groupId) {
    if (!m_groupItems.contains(groupId)) {
        return;
    }

    QWidget *item = m_groupItems.take(groupId);
    m_groupLayout->removeWidget(item);
    delete item;

    updateSectionLabels();
}

void FriendListWidget::updateGroup(const ChatGroupInfo &group) {
    Q_UNUSED(group)
    // Re-create group item
    // TODO: Implement efficient update
}

void FriendListWidget::setGroups(const QList<ChatGroupInfo> &groups) {
    // Clear existing group items
    for (QWidget *item : m_groupItems) {
        m_groupLayout->removeWidget(item);
        delete item;
    }
    m_groupItems.clear();

    // Add new items
    for (const ChatGroupInfo &group : groups) {
        addGroup(group);
    }

    updateSectionLabels();
}

void FriendListWidget::selectPort(const QString &portName) {
    clearSelection();
    m_selectedPort = portName;
    m_selectedGroup.clear();

    if (m_friendItems.contains(portName)) {
        m_friendItems[portName]->setSelected(true);
    }

    emit portSelected(portName);
}

void FriendListWidget::selectGroup(const QString &groupId) {
    clearSelection();
    m_selectedGroup = groupId;
    m_selectedPort.clear();

    // TODO: Update group item selection visual

    emit groupSelected(groupId);
}

void FriendListWidget::refreshList() {
    if (!m_portManager) {
        return;
    }

    setFriends(m_portManager->friendList());
}

void FriendListWidget::onItemClicked(const QString &portName) { selectPort(portName); }

void FriendListWidget::onSearchTextChanged(const QString &text) {
    QString searchText = text.toLower();

    // Filter friends
    for (auto it = m_friendItems.begin(); it != m_friendItems.end(); ++it) {
        FriendListItem *item = it.value();
        bool visible = searchText.isEmpty() || item->portName().toLower().contains(searchText) ||
                       item->info().remark().toLower().contains(searchText);
        item->setVisible(visible);
    }

    // Filter groups
    for (auto it = m_groupItems.begin(); it != m_groupItems.end(); ++it) {
        QWidget *item = it.value();
        QString groupId = item->property("groupId").toString();
        QLabel *nameLabel = item->findChild<QLabel *>();
        bool visible = searchText.isEmpty();
        if (nameLabel) {
            visible = visible || nameLabel->text().toLower().contains(searchText);
        }
        item->setVisible(visible);
    }
}

void FriendListWidget::onAddButtonClicked() { emit addPortRequested(); }

void FriendListWidget::onGroupButtonClicked() { emit createGroupRequested(); }

void FriendListWidget::setupUi() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    setMinimumWidth(280);
    setMaximumWidth(350);
    setStyleSheet("background-color: #FAFAFA;");

    setupHeader();
    setupSections();
}

void FriendListWidget::setupHeader() {
    m_headerWidget = new QWidget(this);
    m_headerWidget->setStyleSheet("background-color: #F5F5F5; border-bottom: 1px solid #E0E0E0;");

    m_headerLayout = new QVBoxLayout(m_headerWidget);
    m_headerLayout->setContentsMargins(15, 10, 15, 10);
    m_headerLayout->setSpacing(10);

    m_titleLabel = new QLabel(tr("Serial Ports"), m_headerWidget);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);

    m_searchEdit = new QLineEdit(m_headerWidget);
    m_searchEdit->setPlaceholderText(tr("Search..."));
    m_searchEdit->setStyleSheet(
        "QLineEdit { border: 1px solid #E0E0E0; border-radius: 15px; padding: 5px 15px; background-color: white; }");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &FriendListWidget::onSearchTextChanged);

    m_buttonWidget = new QWidget(m_headerWidget);
    m_buttonLayout = new QHBoxLayout(m_buttonWidget);
    m_buttonLayout->setContentsMargins(0, 0, 0, 0);
    m_buttonLayout->setSpacing(10);

    m_addButton = new QPushButton(tr("+ Add Port"), m_buttonWidget);
    m_addButton->setIcon(QIcon(":/icons/add.png"));
    m_addButton->setStyleSheet(
        "QPushButton { padding: 8px 15px; border: 1px solid #07C160; border-radius: 5px; background-color: white; "
        "color: #07C160; } QPushButton:hover { background-color: #E8F5E9; }");
    connect(m_addButton, &QPushButton::clicked, this, &FriendListWidget::onAddButtonClicked);

    m_groupButton = new QPushButton(tr("+ Group"), m_buttonWidget);
    m_groupButton->setIcon(QIcon(":/icons/group.png"));
    m_groupButton->setStyleSheet(
        "QPushButton { padding: 8px 15px; border: 1px solid #1976D2; border-radius: 5px; background-color: white; "
        "color: #1976D2; } QPushButton:hover { background-color: #E3F2FD; }");
    connect(m_groupButton, &QPushButton::clicked, this, &FriendListWidget::onGroupButtonClicked);

    m_buttonLayout->addWidget(m_addButton);
    m_buttonLayout->addWidget(m_groupButton);
    m_buttonLayout->addStretch();

    m_headerLayout->addWidget(m_titleLabel);
    m_headerLayout->addWidget(m_searchEdit);
    m_headerLayout->addWidget(m_buttonWidget);

    m_mainLayout->addWidget(m_headerWidget);
}

void FriendListWidget::setupSections() {
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setStyleSheet("QScrollArea { border: none; }");

    m_scrollContent = new QWidget(m_scrollArea);
    m_scrollLayout = new QVBoxLayout(m_scrollContent);
    m_scrollLayout->setContentsMargins(0, 0, 0, 0);
    m_scrollLayout->setSpacing(0);

    // Online section
    m_onlineSection = new QWidget(m_scrollContent);
    m_onlineLayout = new QVBoxLayout(m_onlineSection);
    m_onlineLayout->setContentsMargins(0, 0, 0, 0);
    m_onlineLayout->setSpacing(0);

    m_onlineLabel = new QLabel(tr("Online (0)"), m_onlineSection);
    m_onlineLabel->setStyleSheet("background-color: #EEEEEE; padding: 5px 15px; color: #666666;");
    QFont sectionFont = m_onlineLabel->font();
    sectionFont.setPointSize(10);
    m_onlineLabel->setFont(sectionFont);
    m_onlineLayout->addWidget(m_onlineLabel);

    // Offline section
    m_offlineSection = new QWidget(m_scrollContent);
    m_offlineLayout = new QVBoxLayout(m_offlineSection);
    m_offlineLayout->setContentsMargins(0, 0, 0, 0);
    m_offlineLayout->setSpacing(0);

    m_offlineLabel = new QLabel(tr("Offline (0)"), m_offlineSection);
    m_offlineLabel->setStyleSheet("background-color: #EEEEEE; padding: 5px 15px; color: #666666;");
    m_offlineLabel->setFont(sectionFont);
    m_offlineLayout->addWidget(m_offlineLabel);

    // Groups section
    m_groupSection = new QWidget(m_scrollContent);
    m_groupLayout = new QVBoxLayout(m_groupSection);
    m_groupLayout->setContentsMargins(0, 0, 0, 0);
    m_groupLayout->setSpacing(0);

    m_groupLabel = new QLabel(tr("Groups (0)"), m_groupSection);
    m_groupLabel->setStyleSheet("background-color: #EEEEEE; padding: 5px 15px; color: #666666;");
    m_groupLabel->setFont(sectionFont);
    m_groupLayout->addWidget(m_groupLabel);

    m_scrollLayout->addWidget(m_onlineSection);
    m_scrollLayout->addWidget(m_offlineSection);
    m_scrollLayout->addWidget(m_groupSection);
    m_scrollLayout->addStretch();

    m_scrollArea->setWidget(m_scrollContent);
    m_mainLayout->addWidget(m_scrollArea, 1);
}

void FriendListWidget::clearSelection() {
    for (FriendListItem *item : m_friendItems) {
        item->setSelected(false);
    }
}

void FriendListWidget::updateSectionLabels() {
    int onlineCount = 0;
    int offlineCount = 0;

    for (FriendListItem *item : m_friendItems) {
        if (item->info().isOnline()) {
            onlineCount++;
        } else {
            offlineCount++;
        }
    }

    m_onlineLabel->setText(tr("Online (%1)").arg(onlineCount));
    m_offlineLabel->setText(tr("Offline (%1)").arg(offlineCount));
    m_groupLabel->setText(tr("Groups (%1)").arg(m_groupItems.size()));
}

FriendListItem *FriendListWidget::createFriendItem(const SerialPortInfo &info) {
    FriendListItem *item = new FriendListItem(info, this);
    connect(item, &FriendListItem::clicked, this, &FriendListWidget::onItemClicked);
    connect(item, &FriendListItem::deleteRequested, this, &FriendListWidget::onDeleteRequested);
    connect(item, &FriendListItem::contextMenuRequested, this, &FriendListWidget::showPortContextMenu);
    return item;
}

void FriendListWidget::onDeleteRequested(const QString &portName) { emit deletePortRequested(portName); }

void FriendListWidget::showPortContextMenu(const QString &portName, const QPoint &pos) {
    QMenu menu(this);

    // Check if port is online
    bool isOnline = false;
    if (m_portManager) {
        SerialPortUser *user = m_portManager->getUser(portName);
        isOnline = user && user->isOnline();
    }

    if (isOnline) {
        QAction *disconnectAction = menu.addAction(QIcon(":/icons/disconnect.png"), tr("Disconnect"));
        connect(disconnectAction, &QAction::triggered, this,
                [this, portName]() { emit disconnectRequested(portName); });
    } else {
        QAction *connectAction = menu.addAction(QIcon(":/icons/connect.png"), tr("Connect"));
        connect(connectAction, &QAction::triggered, this, [this, portName]() { emit connectRequested(portName); });
    }

    menu.addSeparator();

    QAction *settingsAction = menu.addAction(QIcon(":/icons/settings.png"), tr("Settings..."));
    connect(settingsAction, &QAction::triggered, this, [this, portName]() { emit portSettingsRequested(portName); });

    QAction *remarkAction = menu.addAction(tr("Set Remark..."));
    connect(remarkAction, &QAction::triggered, this, [this, portName]() { emit portRemarkRequested(portName); });

    menu.addSeparator();

    QAction *deleteAction = menu.addAction(QIcon(":/icons/delete.png"), tr("Delete"));
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, this, [this, portName]() { emit deletePortRequested(portName); });

    menu.exec(pos);
}

void FriendListWidget::showGroupContextMenu(const QString &groupId, const QPoint &pos) {
    QMenu menu(this);

    QAction *settingsAction = menu.addAction(QIcon(":/icons/settings.png"), tr("Group Settings..."));
    connect(settingsAction, &QAction::triggered, this, [this, groupId]() { emit groupSettingsRequested(groupId); });

    menu.addSeparator();

    QAction *deleteAction = menu.addAction(QIcon(":/icons/delete.png"), tr("Delete Group"));
    connect(deleteAction, &QAction::triggered, this, [this, groupId]() { emit deleteGroupRequested(groupId); });

    menu.exec(pos);
}

void FriendListWidget::updateLastMessage(const QString &portName, const QString &message) {
    if (m_friendItems.contains(portName)) {
        m_friendItems[portName]->setLastMessage(message);
    }
}

void FriendListWidget::incrementUnread(const QString &portName) {
    if (m_friendItems.contains(portName)) {
        // Only increment if not currently selected
        if (m_selectedPort != portName) {
            m_friendItems[portName]->incrementUnreadCount();
        }
    }
}

void FriendListWidget::clearUnread(const QString &portName) {
    if (m_friendItems.contains(portName)) {
        m_friendItems[portName]->clearUnread();
    }
}

QWidget *FriendListWidget::createGroupItem(const ChatGroupInfo &group) {
    QWidget *item = new QWidget(this);
    item->setFixedHeight(60);
    item->setCursor(Qt::PointingHandCursor);
    item->setProperty("groupId", group.id());

    QHBoxLayout *layout = new QHBoxLayout(item);
    layout->setContentsMargins(15, 10, 15, 10);

    QLabel *iconLabel = new QLabel(item);
    iconLabel->setFixedSize(40, 40);
    iconLabel->setPixmap(QPixmap(":/icons/group.png").scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("background-color: #1976D2; border-radius: 8px;");

    QVBoxLayout *infoLayout = new QVBoxLayout();

    QLabel *nameLabel = new QLabel(group.name(), item);
    QFont nameFont = nameLabel->font();
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);

    QLabel *membersLabel = new QLabel(tr("%1 members").arg(group.memberCount()), item);
    membersLabel->setStyleSheet("color: #666666;");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(membersLabel);

    layout->addWidget(iconLabel);
    layout->addLayout(infoLayout, 1);

    // Make clickable
    item->installEventFilter(this);

    return item;
}
