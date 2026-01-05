#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QAction>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QStatusBar>
#include <QTextEdit>
#include <QTimer>

#include "ChatGroup.h"
#include "ChatWidget.h"
#include "DataPersistence.h"
#include "FriendListWidget.h"
#include "MessageManager.h"
#include "SerialPortManager.h"

// Version info
#define APP_VERSION "1.0.0"
#define APP_VERSION_MAJOR 1
#define APP_VERSION_MINOR 0
#define APP_VERSION_PATCH 0

/**
 * @brief Main application window
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // Console logging
    void logMessage(const QString &message);
    void logError(const QString &message);
    void logWarning(const QString &message);

  protected:
    void closeEvent(QCloseEvent *event) override;

  private slots:
    // Port actions
    void onAddPortRequested();
    void onPortSelected(const QString &portName);
    void onGroupSelected(const QString &groupId);
    void onConnectRequested(const QString &portName);
    void onDisconnectRequested(const QString &portName);
    void onPortSettingsRequested(const QString &portName);
    void onPortRemarkRequested(const QString &portName);

    // Group actions
    void onCreateGroupRequested();
    void onGroupSettingsRequested(const QString &groupId);
    void onGroupForwardingToggled(const QString &groupId, bool enabled);
    void onSendGroupDataRequested(const QString &groupId, const QByteArray &data, const QStringList &targetPorts);

    // Message actions
    void onSendDataRequested(const QString &portName, const QByteArray &data);
    void onClearHistoryRequested(const QString &portName);

    // Status updates
    void onUserStatusChanged(const QString &portName, PortStatus status);
    void onUserMessageReceived(const QString &portName, const Message &message);
    void onUserMessageSent(const QString &portName, const Message &message);

    // Port deletion
    void onDeletePortRequested(const QString &portName);

    // Menu actions
    void onRefreshPorts();
    void onConnectAll();
    void onDisconnectAll();
    void onClearAllHistory();
    void onExportHistory();
    void onToggleConsole();
    void onAbout();

    // Status bar update
    void updateStatusBar();

  private:
    // Managers
    SerialPortManager *m_portManager;
    MessageManager *m_messageManager;
    DataPersistence *m_dataPersistence;
    QMap<QString, ChatGroup *> m_chatGroups;

    // UI Components
    QWidget *m_centralWidget;
    QHBoxLayout *m_mainLayout;
    QSplitter *m_splitter;

    FriendListWidget *m_friendListWidget;
    ChatWidget *m_chatWidget;

    // Console dock
    QDockWidget *m_consoleDock;
    QTextEdit *m_consoleOutput;

    // Menu
    QMenu *m_fileMenu;
    QMenu *m_portMenu;
    QMenu *m_viewMenu;
    QMenu *m_helpMenu;

    QAction *m_refreshAction;
    QAction *m_addPortAction;
    QAction *m_connectAllAction;
    QAction *m_disconnectAllAction;
    QAction *m_exitAction;
    QAction *m_clearHistoryAction;
    QAction *m_exportHistoryAction;
    QAction *m_toggleConsoleAction;
    QAction *m_aboutAction;

    // Status bar
    QLabel *m_statusLabel;
    QLabel *m_connectionLabel;
    QTimer *m_statusTimer;

    void setupUi();
    void setupMenuBar();
    void setupStatusBar();
    void setupConsoleDock();
    void setupConnections();
    void loadData();
    void saveData();
    void createChatGroup(const ChatGroupInfo &info);
    ChatGroup *getChatGroup(const QString &groupId);
};

#endif // MAIN_WINDOW_H
