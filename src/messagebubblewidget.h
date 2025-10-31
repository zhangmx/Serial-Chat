#ifndef MESSAGEBUBBLEWIDGET_H
#define MESSAGEBUBBLEWIDGET_H

#include <QWidget>

// Forward declaration
class Message;

class MessageBubbleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageBubbleWidget(Message *message, QWidget *parent = nullptr);

    Message* message() const { return m_message; }
    void setMessage(Message *message);

    enum DisplayMode {
        TextMode,
        HexMode
    };

    DisplayMode displayMode() const { return m_displayMode; }
    void setDisplayMode(DisplayMode mode);

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    QString formatContent() const;
    QRect calculateBubbleRect() const;

    Message *m_message;
    DisplayMode m_displayMode;
};

#endif // MESSAGEBUBBLEWIDGET_H
