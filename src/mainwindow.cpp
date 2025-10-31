#include "mainwindow.h"
#include "messagebubblewidget.h"
#include "models/session.h"
#include "models/message.h"
#include "models/serialport.h"
#include <QSplitter>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_sessionListModel(new QStringListModel(this))
    , m_currentSession(nullptr)
{
    setupUI();
    createMenuBar();

    // Create a default session for demonstration
    m_currentSession = new Session("Default Session", this);
    connect(m_currentSession, &Session::messageAdded, this, &MainWindow::handleMessageAdded);

    // Add a welcome system message
    Message *welcomeMsg = new Message(
        QByteArray("Welcome to Serial Chat! Connect a serial port to start chatting."),
        Message::System,
        nullptr,
        m_currentSession
    );
    m_currentSession->addMessage(welcomeMsg);

    // Update session list
    QStringList sessions;
    sessions << "Default Session";
    m_sessionListModel->setStringList(sessions);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("Serial Chat");
    resize(1000, 700);

    // Create central widget with horizontal splitter
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(mainSplitter);

    // Left sidebar - Session list
    QWidget *leftPanel = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *sessionLabel = new QLabel("Sessions", leftPanel);
    sessionLabel->setStyleSheet("QLabel { font-weight: bold; padding: 10px; background-color: #f0f0f0; }");
    leftLayout->addWidget(sessionLabel);

    m_sessionListView = new QListView(leftPanel);
    m_sessionListView->setModel(m_sessionListModel);
    leftLayout->addWidget(m_sessionListView);

    mainSplitter->addWidget(leftPanel);

    // Central area - Chat view
    QWidget *centralPanel = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout(centralPanel);
    centralLayout->setContentsMargins(0, 0, 0, 0);

    // Chat scroll area
    m_chatScrollArea = new QScrollArea(centralPanel);
    m_chatScrollArea->setWidgetResizable(true);
    m_chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_chatContainer = new QWidget();
    m_chatLayout = new QVBoxLayout(m_chatContainer);
    m_chatLayout->setAlignment(Qt::AlignTop);
    m_chatLayout->setSpacing(5);
    m_chatScrollArea->setWidget(m_chatContainer);

    centralLayout->addWidget(m_chatScrollArea);

    // Bottom bar - Input area
    QWidget *inputPanel = new QWidget(centralPanel);
    QHBoxLayout *inputLayout = new QHBoxLayout(inputPanel);

    m_messageInput = new QLineEdit(inputPanel);
    m_messageInput->setPlaceholderText("Type a message or hex data...");
    m_sendButton = new QPushButton("Send", inputPanel);
    m_sendButton->setFixedWidth(80);

    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::handleSendButtonClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &MainWindow::handleSendButtonClicked);

    inputLayout->addWidget(m_messageInput);
    inputLayout->addWidget(m_sendButton);

    centralLayout->addWidget(inputPanel);

    mainSplitter->addWidget(centralPanel);

    // Right dock - Serial port status (optional for MVP)
    m_portDock = new QDockWidget("Serial Ports", this);
    QWidget *dockWidget = new QWidget(m_portDock);
    QVBoxLayout *dockLayout = new QVBoxLayout(dockWidget);
    QLabel *portLabel = new QLabel("No ports connected", dockWidget);
    portLabel->setAlignment(Qt::AlignCenter);
    dockLayout->addWidget(portLabel);
    m_portDock->setWidget(dockWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_portDock);

    // Set splitter proportions
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 3);
}

void MainWindow::createMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // File menu
    QMenu *fileMenu = menuBar->addMenu("&File");
    QAction *exitAction = fileMenu->addAction("E&xit");
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    // Session menu
    QMenu *sessionMenu = menuBar->addMenu("&Session");
    QAction *newSessionAction = sessionMenu->addAction("&New Session");
    QAction *clearAction = sessionMenu->addAction("&Clear Messages");
    connect(clearAction, &QAction::triggered, [this]() {
        if (m_currentSession) {
            // Clear layout
            QLayoutItem *item;
            while ((item = m_chatLayout->takeAt(0)) != nullptr) {
                delete item->widget();
                delete item;
            }
            m_currentSession->clearMessages();
        }
    });

    // Port menu
    QMenu *portMenu = menuBar->addMenu("&Port");
    QAction *connectAction = portMenu->addAction("&Connect Port...");
    QAction *disconnectAction = portMenu->addAction("&Disconnect Port");

    // View menu
    QMenu *viewMenu = menuBar->addMenu("&View");
    QAction *togglePortDockAction = viewMenu->addAction("Toggle &Port Panel");
    togglePortDockAction->setCheckable(true);
    togglePortDockAction->setChecked(true);
    connect(togglePortDockAction, &QAction::toggled, m_portDock, &QDockWidget::setVisible);
}

void MainWindow::handleSendButtonClicked()
{
    QString text = m_messageInput->text().trimmed();
    if (text.isEmpty() || !m_currentSession) {
        return;
    }

    // Create outgoing message
    Message *message = new Message(
        text.toUtf8(),
        Message::Outgoing,
        nullptr,
        m_currentSession
    );
    m_currentSession->addMessage(message);

    // Clear input
    m_messageInput->clear();
    m_messageInput->setFocus();
}

void MainWindow::handleMessageAdded(Message *message)
{
    addMessageBubble(message);
}

void MainWindow::addMessageBubble(Message *message)
{
    if (!message) {
        return;
    }

    MessageBubbleWidget *bubble = new MessageBubbleWidget(message, m_chatContainer);
    m_chatLayout->addWidget(bubble);

    // Scroll to bottom
    QScrollBar *scrollBar = m_chatScrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
