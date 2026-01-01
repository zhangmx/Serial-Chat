#ifndef CHAT_BUBBLE_H
#define CHAT_BUBBLE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Message.h"

/**
 * @brief A chat bubble widget for displaying messages
 */
class ChatBubble : public QWidget {
    Q_OBJECT

public:
    explicit ChatBubble(const Message& message, MessageFormat format = MessageFormat::Text, 
                        QWidget* parent = nullptr);
    ~ChatBubble() override;
    
    // Message
    Message message() const { return m_message; }
    
    // Display format
    void setFormat(MessageFormat format);
    MessageFormat format() const { return m_format; }
    
    // Styling
    void setSentStyle();
    void setReceivedStyle();
    
    // Update content
    void updateDisplay();

signals:
    void copyRequested(const QString& text);
    void formatToggled();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    Message m_message;
    MessageFormat m_format;
    
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_bubbleLayout;
    QWidget* m_bubbleWidget;
    QVBoxLayout* m_contentLayout;
    QLabel* m_portLabel;
    QLabel* m_contentLabel;
    QLabel* m_timeLabel;
    
    void setupUi();
    void applyStyle();
    QString bubbleStyleSheet(bool isSent);
};

#endif // CHAT_BUBBLE_H
