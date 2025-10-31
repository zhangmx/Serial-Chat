#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include "SerialPort.h"
#include "Session.h"
#include "Message.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onConnectButtonClicked();
    void onSendButtonClicked();
    void onDataReceived(const QByteArray &data);
    void onMessageAdded(Message *message);
    void refreshPortList();
    void scrollToBottom();

private:
    void setupUI();
    void setupConnections();
    void addMessageToChat(Message *message);

    // Models
    SerialPort *m_serialPort;
    Session *m_session;

    // UI Components - Sidebar
    QWidget *m_sidebarWidget;
    QComboBox *m_portComboBox;
    QComboBox *m_baudRateComboBox;
    QPushButton *m_connectButton;
    QPushButton *m_refreshButton;
    QLabel *m_statusLabel;

    // UI Components - Chat Area
    QWidget *m_chatWidget;
    QScrollArea *m_chatScrollArea;
    QWidget *m_chatContentWidget;
    QVBoxLayout *m_chatLayout;
    QLineEdit *m_messageInput;
    QPushButton *m_sendButton;

    // Central Widget
    QWidget *m_centralWidget;
};

#endif // MAINWINDOW_H
