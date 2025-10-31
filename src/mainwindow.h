#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QStringListModel>

// Forward declarations
class Session;
class Message;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
    void handleSendButtonClicked();
    void handleMessageAdded(Message *message);

private:
    void setupUI();
    void createMenuBar();
    void addMessageBubble(Message *message);

    // UI Components
    QListView *m_sessionListView;
    QStringListModel *m_sessionListModel;
    QScrollArea *m_chatScrollArea;
    QWidget *m_chatContainer;
    QVBoxLayout *m_chatLayout;
    QLineEdit *m_messageInput;
    QPushButton *m_sendButton;
    QDockWidget *m_portDock;

    // Data
    Session *m_currentSession;
};

#endif // MAINWINDOW_H
