#ifndef FRIEND_LIST_ITEM_H
#define FRIEND_LIST_ITEM_H

#include "SerialPortInfo.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @brief A single item in the friend list representing a serial port
 */
class FriendListItem : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int slideOffset READ slideOffset WRITE setSlideOffset)

  public:
    explicit FriendListItem(const SerialPortInfo &info, QWidget *parent = nullptr);
    ~FriendListItem() override;

    // Port info
    SerialPortInfo info() const { return m_info; }
    QString portName() const { return m_info.portName(); }

    // Update
    void setInfo(const SerialPortInfo &info);
    void setStatus(PortStatus status);
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

    // Last message and unread count
    void setLastMessage(const QString &message);
    void setUnreadCount(int count);
    void incrementUnreadCount();
    void clearUnread();
    int unreadCount() const { return m_unreadCount; }

    // Slide offset for swipe gesture
    int slideOffset() const { return m_slideOffset; }
    void setSlideOffset(int offset);

  signals:
    void clicked(const QString &portName);
    void deleteRequested(const QString &portName);
    void connectRequested(const QString &portName);
    void disconnectRequested(const QString &portName);
    void settingsRequested(const QString &portName);
    void remarkRequested(const QString &portName);
    void contextMenuRequested(const QString &portName, const QPoint &globalPos);

  protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

  private slots:
    void onDeleteClicked();

  private:
    SerialPortInfo m_info;
    bool m_selected;
    bool m_hovered;
    int m_unreadCount;
    QString m_lastMessage;

    // Swipe gesture
    int m_slideOffset;
    QPoint m_pressPos;
    bool m_swiping;
    QPropertyAnimation *m_slideAnimation;

    // UI components
    QWidget *m_contentWidget;
    QHBoxLayout *m_mainLayout;
    QHBoxLayout *m_contentLayout;
    QWidget *m_avatarWidget;
    QLabel *m_statusIndicator;
    QVBoxLayout *m_infoLayout;
    QLabel *m_nameLabel;
    QLabel *m_lastMessageLabel;
    QLabel *m_timeLabel;
    QLabel *m_unreadBadge;

    // Delete button (hidden by default)
    QPushButton *m_deleteButton;

    void setupUi();
    void updateDisplay();
    void updateStyle();
    void animateSlide(int targetOffset);
};

#endif // FRIEND_LIST_ITEM_H
