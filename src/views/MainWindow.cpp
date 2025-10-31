#include "MainWindow.h"
#include "MessageBubbleWidget.h"
#include <QSplitter>
#include <QGroupBox>
#include <QSpacerItem>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_serialPort(new SerialPort(this))
    , m_session(new Session(this))
{
    setupUI();
    setupConnections();
    refreshPortList();
}

void MainWindow::setupUI()
{
    setWindowTitle("Serial Chat");
    resize(900, 600);

    // Create central widget with horizontal layout
    m_centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create splitter for resizable sidebar
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // === SIDEBAR ===
    m_sidebarWidget = new QWidget(this);
    m_sidebarWidget->setMinimumWidth(200);
    m_sidebarWidget->setMaximumWidth(300);
    QVBoxLayout *sidebarLayout = new QVBoxLayout(m_sidebarWidget);

    // Connection settings group
    QGroupBox *connectionGroup = new QGroupBox("Connection Settings", this);
    QVBoxLayout *connectionLayout = new QVBoxLayout(connectionGroup);

    // Port selection
    QLabel *portLabel = new QLabel("Serial Port:", this);
    m_portComboBox = new QComboBox(this);
    connectionLayout->addWidget(portLabel);
    connectionLayout->addWidget(m_portComboBox);

    // Baud rate selection
    QLabel *baudLabel = new QLabel("Baud Rate:", this);
    m_baudRateComboBox = new QComboBox(this);
    m_baudRateComboBox->addItems({"9600", "19200", "38400", "57600", "115200"});
    m_baudRateComboBox->setCurrentText("9600");
    connectionLayout->addWidget(baudLabel);
    connectionLayout->addWidget(m_baudRateComboBox);

    // Buttons
    m_refreshButton = new QPushButton("Refresh Ports", this);
    m_connectButton = new QPushButton("Connect", this);
    m_connectButton->setCheckable(true);
    connectionLayout->addWidget(m_refreshButton);
    connectionLayout->addWidget(m_connectButton);

    connectionGroup->setLayout(connectionLayout);
    sidebarLayout->addWidget(connectionGroup);

    // Status label
    m_statusLabel = new QLabel("Disconnected", this);
    m_statusLabel->setStyleSheet("padding: 10px; background-color: #f0f0f0; border-radius: 5px;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(m_statusLabel);

    sidebarLayout->addStretch();
    m_sidebarWidget->setLayout(sidebarLayout);

    // === CHAT AREA ===
    m_chatWidget = new QWidget(this);
    QVBoxLayout *chatMainLayout = new QVBoxLayout(m_chatWidget);
    chatMainLayout->setContentsMargins(10, 10, 10, 10);

    // Chat scroll area
    m_chatScrollArea = new QScrollArea(this);
    m_chatScrollArea->setWidgetResizable(true);
    m_chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_chatScrollArea->setStyleSheet("QScrollArea { border: 1px solid #ddd; background-color: #f5f5f5; }");

    m_chatContentWidget = new QWidget(this);
    m_chatLayout = new QVBoxLayout(m_chatContentWidget);
    m_chatLayout->setAlignment(Qt::AlignTop);
    m_chatLayout->setSpacing(10);
    m_chatLayout->setContentsMargins(10, 10, 10, 10);

    m_chatContentWidget->setLayout(m_chatLayout);
    m_chatScrollArea->setWidget(m_chatContentWidget);

    chatMainLayout->addWidget(m_chatScrollArea);

    // Message input area
    QHBoxLayout *inputLayout = new QHBoxLayout();
    m_messageInput = new QLineEdit(this);
    m_messageInput->setPlaceholderText("Type a message...");
    m_messageInput->setEnabled(false);

    m_sendButton = new QPushButton("Send", this);
    m_sendButton->setEnabled(false);

    inputLayout->addWidget(m_messageInput);
    inputLayout->addWidget(m_sendButton);
    chatMainLayout->addLayout(inputLayout);

    m_chatWidget->setLayout(chatMainLayout);

    // Add widgets to splitter
    splitter->addWidget(m_sidebarWidget);
    splitter->addWidget(m_chatWidget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);
    m_centralWidget->setLayout(mainLayout);
    setCentralWidget(m_centralWidget);
}

void MainWindow::setupConnections()
{
    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::onSendButtonClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::refreshPortList);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &MainWindow::onSendButtonClicked);

    connect(m_serialPort, &SerialPort::dataReceived, this, &MainWindow::onDataReceived);
    connect(m_session, &Session::messageAdded, this, &MainWindow::onMessageAdded);
}

void MainWindow::refreshPortList()
{
    m_portComboBox->clear();
    QList<QSerialPortInfo> ports = SerialPort::availablePorts();
    
    for (const QSerialPortInfo &info : ports) {
        m_portComboBox->addItem(info.portName());
    }

    if (m_portComboBox->count() == 0) {
        m_portComboBox->addItem("No ports available");
        m_connectButton->setEnabled(false);
    } else {
        m_connectButton->setEnabled(true);
    }
}

void MainWindow::onConnectButtonClicked()
{
    if (m_connectButton->isChecked()) {
        QString portName = m_portComboBox->currentText();
        qint32 baudRate = m_baudRateComboBox->currentText().toInt();

        if (m_serialPort->openPort(portName, baudRate)) {
            m_connectButton->setText("Disconnect");
            m_statusLabel->setText("Connected to " + portName);
            m_statusLabel->setStyleSheet("padding: 10px; background-color: #c8f0c6; border-radius: 5px;");
            m_messageInput->setEnabled(true);
            m_sendButton->setEnabled(true);
            m_portComboBox->setEnabled(false);
            m_baudRateComboBox->setEnabled(false);
        } else {
            m_connectButton->setChecked(false);
            m_statusLabel->setText("Failed to connect");
            m_statusLabel->setStyleSheet("padding: 10px; background-color: #f0c6c6; border-radius: 5px;");
        }
    } else {
        m_serialPort->closePort();
        m_connectButton->setText("Connect");
        m_statusLabel->setText("Disconnected");
        m_statusLabel->setStyleSheet("padding: 10px; background-color: #f0f0f0; border-radius: 5px;");
        m_messageInput->setEnabled(false);
        m_sendButton->setEnabled(false);
        m_portComboBox->setEnabled(true);
        m_baudRateComboBox->setEnabled(true);
    }
}

void MainWindow::onSendButtonClicked()
{
    QString text = m_messageInput->text().trimmed();
    if (text.isEmpty() || !m_serialPort->isOpen()) {
        return;
    }

    QByteArray data = text.toUtf8();
    if (m_serialPort->writeData(data)) {
        Message *message = new Message(text, Message::Outgoing);
        m_session->addMessage(message);
        m_messageInput->clear();
    }
}

void MainWindow::onDataReceived(const QByteArray &data)
{
    QString text = QString::fromUtf8(data);
    Message *message = new Message(text, Message::Incoming);
    m_session->addMessage(message);
}

void MainWindow::onMessageAdded(Message *message)
{
    addMessageToChat(message);
}

void MainWindow::addMessageToChat(Message *message)
{
    MessageBubbleWidget *bubble = new MessageBubbleWidget(message, this);
    
    QHBoxLayout *bubbleLayout = new QHBoxLayout();
    bubbleLayout->setContentsMargins(0, 0, 0, 0);

    if (message->direction() == Message::Outgoing) {
        bubbleLayout->addStretch();
        bubbleLayout->addWidget(bubble);
    } else {
        bubbleLayout->addWidget(bubble);
        bubbleLayout->addStretch();
    }

    m_chatLayout->addLayout(bubbleLayout);

    // Scroll to bottom
    QScrollBar *scrollBar = m_chatScrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
