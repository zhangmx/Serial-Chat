#ifndef CHAT_GROUP_DIALOG_H
#define CHAT_GROUP_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include "ChatGroupInfo.h"

class SerialPortManager;

/**
 * @brief Dialog for creating or editing a chat group
 */
class ChatGroupDialog : public QDialog {
    Q_OBJECT

public:
    explicit ChatGroupDialog(SerialPortManager* manager, QWidget* parent = nullptr);
    ChatGroupDialog(const ChatGroupInfo& info, SerialPortManager* manager, QWidget* parent = nullptr);
    ~ChatGroupDialog() override;
    
    // Get the configured group info
    ChatGroupInfo groupInfo() const;
    
    // Set initial values
    void setGroupInfo(const ChatGroupInfo& info);

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onSelectAllClicked();
    void onClearSelectionClicked();

private:
    ChatGroupInfo m_info;
    SerialPortManager* m_portManager;
    bool m_editMode;
    
    // UI Components
    QVBoxLayout* m_mainLayout;
    QFormLayout* m_formLayout;
    
    QLineEdit* m_nameEdit;
    QTextEdit* m_descriptionEdit;
    QCheckBox* m_forwardingCheckBox;
    
    QLabel* m_membersLabel;
    QListWidget* m_membersList;
    QHBoxLayout* m_selectionLayout;
    QPushButton* m_selectAllButton;
    QPushButton* m_clearSelectionButton;
    
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    
    void setupUi();
    void populateMembersList();
    void loadSettings();
    void saveSettings();
};

#endif // CHAT_GROUP_DIALOG_H
