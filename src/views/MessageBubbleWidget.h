#ifndef MESSAGEBUBBLEWIDGET_H
#define MESSAGEBUBBLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "Message.h"

class MessageBubbleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageBubbleWidget(Message *message, QWidget *parent = nullptr);
    ~MessageBubbleWidget() override = default;

    void setMessage(Message *message);
    Message *message() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void updateUI();

    Message *m_message;
    QLabel *m_contentLabel;
    QLabel *m_timestampLabel;
    QVBoxLayout *m_layout;
};

#endif // MESSAGEBUBBLEWIDGET_H
