#include "MessageBubbleWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>

MessageBubbleWidget::MessageBubbleWidget(Message *message, QWidget *parent)
    : QWidget(parent)
    , m_message(message)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(10, 8, 10, 8);
    m_layout->setSpacing(4);

    m_contentLabel = new QLabel(this);
    m_contentLabel->setWordWrap(true);
    m_contentLabel->setTextFormat(Qt::PlainText);

    m_timestampLabel = new QLabel(this);
    m_timestampLabel->setStyleSheet("color: gray; font-size: 10px;");

    m_layout->addWidget(m_contentLabel);
    m_layout->addWidget(m_timestampLabel);

    setLayout(m_layout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setMaximumWidth(400);

    updateUI();
}

void MessageBubbleWidget::setMessage(Message *message)
{
    m_message = message;
    updateUI();
}

Message *MessageBubbleWidget::message() const
{
    return m_message;
}

void MessageBubbleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);

    if (m_message && m_message->direction() == Message::Outgoing) {
        painter.fillPath(path, QColor(220, 248, 198));
    } else {
        painter.fillPath(path, QColor(255, 255, 255));
    }

    painter.setPen(QColor(200, 200, 200));
    painter.drawPath(path);
}

void MessageBubbleWidget::updateUI()
{
    if (m_message) {
        m_contentLabel->setText(m_message->content());
        m_timestampLabel->setText(m_message->timestamp().toString("hh:mm:ss"));

        if (m_message->direction() == Message::Outgoing) {
            m_contentLabel->setAlignment(Qt::AlignRight);
            m_timestampLabel->setAlignment(Qt::AlignRight);
        } else {
            m_contentLabel->setAlignment(Qt::AlignLeft);
            m_timestampLabel->setAlignment(Qt::AlignLeft);
        }
    }
}
