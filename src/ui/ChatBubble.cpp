#include "ChatBubble.h"
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QApplication>
#include <QContextMenuEvent>
#include <QPainter>
#include <QPainterPath>

ChatBubble::ChatBubble(const Message& message, MessageFormat format, QWidget* parent)
    : QWidget(parent)
    , m_message(message)
    , m_format(format)
{
    setupUi();
    updateDisplay();
}

ChatBubble::~ChatBubble()
{
}

void ChatBubble::setFormat(MessageFormat format)
{
    if (m_format != format) {
        m_format = format;
        updateDisplay();
        emit formatToggled();
    }
}

void ChatBubble::setSentStyle()
{
    applyStyle();
}

void ChatBubble::setReceivedStyle()
{
    applyStyle();
}

void ChatBubble::updateDisplay()
{
    m_portLabel->setText(m_message.portName());
    m_contentLabel->setText(m_message.displayText(m_format));
    m_timeLabel->setText(m_message.formattedTime());
    applyStyle();
}

void ChatBubble::setupUi()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 2, 5, 2);
    m_mainLayout->setSpacing(0);
    
    m_bubbleLayout = new QHBoxLayout();
    m_bubbleLayout->setContentsMargins(0, 0, 0, 0);
    
    m_bubbleWidget = new QWidget(this);
    m_bubbleWidget->setMaximumWidth(400);
    
    m_contentLayout = new QVBoxLayout(m_bubbleWidget);
    m_contentLayout->setContentsMargins(10, 8, 10, 8);
    m_contentLayout->setSpacing(4);
    
    m_portLabel = new QLabel(m_bubbleWidget);
    m_portLabel->setObjectName("portLabel");
    QFont portFont = m_portLabel->font();
    portFont.setPointSize(9);
    portFont.setBold(true);
    m_portLabel->setFont(portFont);
    
    m_contentLabel = new QLabel(m_bubbleWidget);
    m_contentLabel->setObjectName("contentLabel");
    m_contentLabel->setWordWrap(true);
    m_contentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    
    m_timeLabel = new QLabel(m_bubbleWidget);
    m_timeLabel->setObjectName("timeLabel");
    QFont timeFont = m_timeLabel->font();
    timeFont.setPointSize(8);
    m_timeLabel->setFont(timeFont);
    
    m_contentLayout->addWidget(m_portLabel);
    m_contentLayout->addWidget(m_contentLabel);
    m_contentLayout->addWidget(m_timeLabel);
    
    m_bubbleLayout->addWidget(m_bubbleWidget);
    m_mainLayout->addLayout(m_bubbleLayout);
}

void ChatBubble::applyStyle()
{
    bool isSent = m_message.direction() == MessageDirection::Sent;
    
    if (isSent) {
        m_bubbleLayout->setAlignment(m_bubbleWidget, Qt::AlignRight);
        m_timeLabel->setAlignment(Qt::AlignRight);
    } else {
        m_bubbleLayout->setAlignment(m_bubbleWidget, Qt::AlignLeft);
        m_timeLabel->setAlignment(Qt::AlignLeft);
    }
    
    m_bubbleWidget->setStyleSheet(bubbleStyleSheet(isSent));
}

QString ChatBubble::bubbleStyleSheet(bool isSent)
{
    if (isSent) {
        return R"(
            QWidget {
                background-color: #95EC69;
                border-radius: 10px;
            }
            QLabel#portLabel {
                color: #2E7D32;
                background: transparent;
            }
            QLabel#contentLabel {
                color: #000000;
                background: transparent;
            }
            QLabel#timeLabel {
                color: #558B2F;
                background: transparent;
            }
        )";
    } else {
        return R"(
            QWidget {
                background-color: #FFFFFF;
                border-radius: 10px;
            }
            QLabel#portLabel {
                color: #1976D2;
                background: transparent;
            }
            QLabel#contentLabel {
                color: #000000;
                background: transparent;
            }
            QLabel#timeLabel {
                color: #757575;
                background: transparent;
            }
        )";
    }
}

void ChatBubble::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);
    
    QAction* copyAction = menu.addAction(tr("Copy"));
    QAction* copyHexAction = menu.addAction(tr("Copy as Hex"));
    QAction* copyTextAction = menu.addAction(tr("Copy as Text"));
    menu.addSeparator();
    QAction* toggleFormatAction = menu.addAction(
        m_format == MessageFormat::Hex ? tr("Show as Text") : tr("Show as Hex"));
    
    QAction* selected = menu.exec(event->globalPos());
    
    if (selected == copyAction) {
        QApplication::clipboard()->setText(m_contentLabel->text());
    } else if (selected == copyHexAction) {
        QApplication::clipboard()->setText(m_message.toHex());
    } else if (selected == copyTextAction) {
        QApplication::clipboard()->setText(m_message.toText());
    } else if (selected == toggleFormatAction) {
        setFormat(m_format == MessageFormat::Hex ? MessageFormat::Text : MessageFormat::Hex);
    }
}

void ChatBubble::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
}
