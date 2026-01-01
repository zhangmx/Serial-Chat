#include "ChatGroupDialog.h"
#include "SerialPortManager.h"
#include <QMessageBox>

ChatGroupDialog::ChatGroupDialog(SerialPortManager* manager, QWidget* parent)
    : QDialog(parent)
    , m_portManager(manager)
    , m_editMode(false)
{
    setupUi();
    populateMembersList();
}

ChatGroupDialog::ChatGroupDialog(const ChatGroupInfo& info, SerialPortManager* manager, QWidget* parent)
    : QDialog(parent)
    , m_info(info)
    , m_portManager(manager)
    , m_editMode(true)
{
    setupUi();
    populateMembersList();
    loadSettings();
}

ChatGroupDialog::~ChatGroupDialog()
{
}

ChatGroupInfo ChatGroupDialog::groupInfo() const
{
    return m_info;
}

void ChatGroupDialog::setGroupInfo(const ChatGroupInfo& info)
{
    m_info = info;
    m_editMode = true;
    loadSettings();
}

void ChatGroupDialog::onOkClicked()
{
    // Validate
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please enter a group name"));
        m_nameEdit->setFocus();
        return;
    }
    
    // Check at least 2 members selected
    int selectedCount = 0;
    for (int i = 0; i < m_membersList->count(); ++i) {
        if (m_membersList->item(i)->checkState() == Qt::Checked) {
            selectedCount++;
        }
    }
    
    if (selectedCount < 2) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select at least 2 members for the group"));
        return;
    }
    
    saveSettings();
    accept();
}

void ChatGroupDialog::onCancelClicked()
{
    reject();
}

void ChatGroupDialog::onSelectAllClicked()
{
    for (int i = 0; i < m_membersList->count(); ++i) {
        m_membersList->item(i)->setCheckState(Qt::Checked);
    }
}

void ChatGroupDialog::onClearSelectionClicked()
{
    for (int i = 0; i < m_membersList->count(); ++i) {
        m_membersList->item(i)->setCheckState(Qt::Unchecked);
    }
}

void ChatGroupDialog::setupUi()
{
    setWindowTitle(m_editMode ? tr("Edit Chat Group") : tr("Create Chat Group"));
    setMinimumSize(400, 500);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    
    m_formLayout = new QFormLayout();
    m_formLayout->setSpacing(10);
    
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(tr("Enter group name"));
    m_nameEdit->setStyleSheet("QLineEdit { padding: 8px; border: 1px solid #E0E0E0; border-radius: 5px; }");
    
    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setPlaceholderText(tr("Enter group description (optional)"));
    m_descriptionEdit->setMaximumHeight(80);
    m_descriptionEdit->setStyleSheet("QTextEdit { padding: 5px; border: 1px solid #E0E0E0; border-radius: 5px; }");
    
    m_forwardingCheckBox = new QCheckBox(tr("Enable message forwarding between members"), this);
    m_forwardingCheckBox->setChecked(true);
    m_forwardingCheckBox->setToolTip(tr("When enabled, messages from one port will be forwarded to all other ports in the group"));
    
    m_formLayout->addRow(tr("Name:"), m_nameEdit);
    m_formLayout->addRow(tr("Description:"), m_descriptionEdit);
    m_formLayout->addRow("", m_forwardingCheckBox);
    
    m_membersLabel = new QLabel(tr("Select group members:"), this);
    m_membersLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    
    m_membersList = new QListWidget(this);
    m_membersList->setStyleSheet("QListWidget { border: 1px solid #E0E0E0; border-radius: 5px; }");
    
    m_selectionLayout = new QHBoxLayout();
    m_selectionLayout->setSpacing(10);
    
    m_selectAllButton = new QPushButton(tr("Select All"), this);
    m_selectAllButton->setStyleSheet("QPushButton { padding: 5px 15px; }");
    connect(m_selectAllButton, &QPushButton::clicked, this, &ChatGroupDialog::onSelectAllClicked);
    
    m_clearSelectionButton = new QPushButton(tr("Clear"), this);
    m_clearSelectionButton->setStyleSheet("QPushButton { padding: 5px 15px; }");
    connect(m_clearSelectionButton, &QPushButton::clicked, this, &ChatGroupDialog::onClearSelectionClicked);
    
    m_selectionLayout->addWidget(m_selectAllButton);
    m_selectionLayout->addWidget(m_clearSelectionButton);
    m_selectionLayout->addStretch();
    
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(10);
    
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    m_cancelButton->setStyleSheet("QPushButton { padding: 8px 20px; }");
    connect(m_cancelButton, &QPushButton::clicked, this, &ChatGroupDialog::onCancelClicked);
    
    m_okButton = new QPushButton(m_editMode ? tr("Save") : tr("Create"), this);
    m_okButton->setDefault(true);
    m_okButton->setStyleSheet("QPushButton { padding: 8px 30px; background-color: #07C160; color: white; border: none; border-radius: 5px; } QPushButton:hover { background-color: #06AD56; }");
    connect(m_okButton, &QPushButton::clicked, this, &ChatGroupDialog::onOkClicked);
    
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addWidget(m_okButton);
    
    m_mainLayout->addLayout(m_formLayout);
    m_mainLayout->addWidget(m_membersLabel);
    m_mainLayout->addWidget(m_membersList, 1);
    m_mainLayout->addLayout(m_selectionLayout);
    m_mainLayout->addLayout(m_buttonLayout);
}

void ChatGroupDialog::populateMembersList()
{
    m_membersList->clear();
    
    if (!m_portManager) {
        return;
    }
    
    QList<SerialPortInfo> friends = m_portManager->friendList();
    for (const SerialPortInfo& info : friends) {
        QListWidgetItem* item = new QListWidgetItem(info.displayName(), m_membersList);
        item->setData(Qt::UserRole, info.portName());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(m_info.hasMember(info.portName()) ? Qt::Checked : Qt::Unchecked);
        
        // Add status indicator
        if (info.isOnline()) {
            item->setIcon(QIcon()); // Could add online icon
            item->setForeground(QColor("#07C160"));
        }
    }
}

void ChatGroupDialog::loadSettings()
{
    m_nameEdit->setText(m_info.name());
    m_descriptionEdit->setText(m_info.description());
    m_forwardingCheckBox->setChecked(m_info.isForwardingEnabled());
    
    // Update member selection
    for (int i = 0; i < m_membersList->count(); ++i) {
        QListWidgetItem* item = m_membersList->item(i);
        QString portName = item->data(Qt::UserRole).toString();
        item->setCheckState(m_info.hasMember(portName) ? Qt::Checked : Qt::Unchecked);
    }
}

void ChatGroupDialog::saveSettings()
{
    if (!m_editMode) {
        m_info = ChatGroupInfo(m_nameEdit->text().trimmed());
    } else {
        m_info.setName(m_nameEdit->text().trimmed());
    }
    
    m_info.setDescription(m_descriptionEdit->toPlainText().trimmed());
    m_info.setForwardingEnabled(m_forwardingCheckBox->isChecked());
    
    // Update members
    m_info.clearMembers();
    for (int i = 0; i < m_membersList->count(); ++i) {
        QListWidgetItem* item = m_membersList->item(i);
        if (item->checkState() == Qt::Checked) {
            QString portName = item->data(Qt::UserRole).toString();
            m_info.addMember(portName);
        }
    }
}
