#include "messagebubblewidget.h"
#include "models/message.h"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>

MessageBubbleWidget::MessageBubbleWidget(Message *message, QWidget *parent)
    : QWidget(parent)
    , m_message(message)
    , m_displayMode(TextMode)
{
    setMinimumHeight(40);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

void MessageBubbleWidget::setMessage(Message *message)
{
    m_message = message;
    update();
    updateGeometry();
}

void MessageBubbleWidget::setDisplayMode(DisplayMode mode)
{
    if (m_displayMode != mode) {
        m_displayMode = mode;
        update();
        updateGeometry();
    }
}

QString MessageBubbleWidget::formatContent() const
{
    if (!m_message) {
        return QString();
    }

    if (m_displayMode == HexMode) {
        return m_message->content().toHex(' ').toUpper();
    } else {
        return QString::fromUtf8(m_message->content());
    }
}

QRect MessageBubbleWidget::calculateBubbleRect() const
{
    if (!m_message) {
        return QRect();
    }

    QFontMetrics fm(font());
    QString text = formatContent();
    QRect textRect = fm.boundingRect(QRect(0, 0, width() - 80, 1000),
                                     Qt::TextWordWrap, text);

    int bubbleWidth = qMin(textRect.width() + 20, width() - 40);
    int bubbleHeight = textRect.height() + 20;

    int x = 10;
    if (m_message->direction() == Message::Outgoing) {
        x = width() - bubbleWidth - 10;
    }

    return QRect(x, 5, bubbleWidth, bubbleHeight);
}

void MessageBubbleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (!m_message) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect bubbleRect = calculateBubbleRect();

    // Choose color based on message direction
    QColor bubbleColor;
    QColor textColor = Qt::black;

    switch (m_message->direction()) {
    case Message::Incoming:
        bubbleColor = QColor(230, 230, 230); // Light gray
        break;
    case Message::Outgoing:
        bubbleColor = QColor(100, 150, 255); // Blue
        textColor = Qt::white;
        break;
    case Message::System:
        bubbleColor = QColor(255, 250, 200); // Light yellow
        break;
    }

    // Draw bubble with rounded corners
    QPainterPath path;
    path.addRoundedRect(bubbleRect, 10, 10);
    painter.fillPath(path, bubbleColor);

    // Draw text
    painter.setPen(textColor);
    QString text = formatContent();
    QRect textRect = bubbleRect.adjusted(10, 10, -10, -10);
    painter.drawText(textRect, Qt::TextWordWrap, text);

    // Draw timestamp in smaller font
    painter.setFont(QFont(font().family(), font().pointSize() - 2));
    painter.setPen(QColor(100, 100, 100));
    QString timestamp = m_message->timestamp().toString("hh:mm:ss");
    QRect timestampRect = bubbleRect.adjusted(0, bubbleRect.height() + 2, 0, bubbleRect.height() + 15);
    if (m_message->direction() == Message::Outgoing) {
        painter.drawText(timestampRect, Qt::AlignRight, timestamp);
    } else {
        painter.drawText(timestampRect, Qt::AlignLeft, timestamp);
    }
}

QSize MessageBubbleWidget::sizeHint() const
{
    QRect bubbleRect = calculateBubbleRect();
    return QSize(width(), bubbleRect.height() + 20);
}

QSize MessageBubbleWidget::minimumSizeHint() const
{
    return QSize(200, 40);
}
