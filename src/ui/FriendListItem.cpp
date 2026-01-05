#include "FriendListItem.h"
#include "TimeUtils.h"
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>

FriendListItem::FriendListItem(const SerialPortInfo &info, QWidget *parent)
    : QWidget(parent), m_info(info), m_selected(false), m_hovered(false), m_unreadCount(0), m_slideOffset(0),
      m_swiping(false), m_slideAnimation(nullptr) {
    setupUi();
    updateDisplay();
}

FriendListItem::~FriendListItem() {}

void FriendListItem::setInfo(const SerialPortInfo &info) {
    m_info = info;
    updateDisplay();
}

void FriendListItem::setStatus(PortStatus status) {
    m_info.setStatus(status);
    updateDisplay();
}

void FriendListItem::setSelected(bool selected) {
    if (m_selected != selected) {
        m_selected = selected;
        if (selected) {
            clearUnread();
        }
        updateStyle();
    }
}

void FriendListItem::setLastMessage(const QString &message) {
    m_lastMessage = message;
    if (message.length() > 30) {
        m_lastMessageLabel->setText(message.left(30) + "...");
    } else {
        m_lastMessageLabel->setText(message);
    }
    m_lastMessageLabel->show();
}

void FriendListItem::setUnreadCount(int count) {
    m_unreadCount = count;
    if (count > 0) {
        m_unreadBadge->setText(count > 99 ? "99+" : QString::number(count));
        m_unreadBadge->show();
    } else {
        m_unreadBadge->hide();
    }
}

void FriendListItem::incrementUnreadCount() { setUnreadCount(m_unreadCount + 1); }

void FriendListItem::clearUnread() { setUnreadCount(0); }

void FriendListItem::setSlideOffset(int offset) {
    m_slideOffset = offset;
    m_contentWidget->move(offset, 0);

    // Show/hide delete button based on offset
    if (offset < -20) {
        m_deleteButton->show();
    }
}

void FriendListItem::setupUi() {
    setFixedHeight(70);
    setCursor(Qt::PointingHandCursor);

    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Delete button (hidden behind content)
    m_deleteButton = new QPushButton(tr("Delete"), this);
    m_deleteButton->setFixedSize(80, 70);
    m_deleteButton->setStyleSheet(
        "QPushButton { background-color: #FA5151; color: white; border: none; font-weight: bold; }");
    m_deleteButton->hide();
    connect(m_deleteButton, &QPushButton::clicked, this, &FriendListItem::onDeleteClicked);

    // Content widget (slides to reveal delete)
    m_contentWidget = new QWidget(this);
    m_contentWidget->setFixedHeight(70);

    m_contentLayout = new QHBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(10, 8, 10, 8);
    m_contentLayout->setSpacing(10);

    // Avatar with status indicator
    m_avatarWidget = new QWidget(m_contentWidget);
    m_avatarWidget->setFixedSize(48, 48);
    m_avatarWidget->setStyleSheet("background-color: #4A90D9; border-radius: 24px;");

    m_statusIndicator = new QLabel(m_avatarWidget);
    m_statusIndicator->setFixedSize(14, 14);
    m_statusIndicator->move(34, 34);

    // Info layout
    m_infoLayout = new QVBoxLayout();
    m_infoLayout->setContentsMargins(0, 0, 0, 0);
    m_infoLayout->setSpacing(2);

    // Top row: name and time
    QHBoxLayout *topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);

    m_nameLabel = new QLabel(m_contentWidget);
    QFont nameFont = m_nameLabel->font();
    nameFont.setPointSize(12);
    nameFont.setBold(true);
    m_nameLabel->setFont(nameFont);

    m_timeLabel = new QLabel(m_contentWidget);
    m_timeLabel->setStyleSheet("color: #999999;");
    QFont timeFont = m_timeLabel->font();
    timeFont.setPointSize(9);
    m_timeLabel->setFont(timeFont);

    topRow->addWidget(m_nameLabel);
    topRow->addStretch();
    topRow->addWidget(m_timeLabel);

    // Bottom row: last message and unread badge
    QHBoxLayout *bottomRow = new QHBoxLayout();
    bottomRow->setContentsMargins(0, 0, 0, 0);

    m_lastMessageLabel = new QLabel(m_contentWidget);
    m_lastMessageLabel->setStyleSheet("color: #666666;");
    QFont msgFont = m_lastMessageLabel->font();
    msgFont.setPointSize(10);
    m_lastMessageLabel->setFont(msgFont);
    m_lastMessageLabel->hide();

    m_unreadBadge = new QLabel(m_contentWidget);
    m_unreadBadge->setFixedSize(20, 20);
    m_unreadBadge->setAlignment(Qt::AlignCenter);
    m_unreadBadge->setStyleSheet(
        "background-color: #FA5151; color: white; border-radius: 10px; font-size: 10px; font-weight: bold;");
    m_unreadBadge->hide();

    bottomRow->addWidget(m_lastMessageLabel, 1);
    bottomRow->addWidget(m_unreadBadge);

    m_infoLayout->addLayout(topRow);
    m_infoLayout->addLayout(bottomRow);
    m_infoLayout->addStretch();

    m_contentLayout->addWidget(m_avatarWidget);
    m_contentLayout->addLayout(m_infoLayout, 1);

    // Position delete button on the right
    m_deleteButton->move(width() - 80, 0);
}

void FriendListItem::updateDisplay() {
    m_nameLabel->setText(m_info.displayName());

    if (m_info.lastActiveTime().isValid()) {
        m_timeLabel->setText(TimeUtils::relativeTime(m_info.lastActiveTime()));
        m_timeLabel->show();
    } else {
        m_timeLabel->hide();
    }

    // Update status indicator
    switch (m_info.status()) {
    case PortStatus::Online:
        m_statusIndicator->setStyleSheet("background-color: #07C160; border-radius: 7px; border: 2px solid white;");
        break;
    case PortStatus::Error:
        m_statusIndicator->setStyleSheet("background-color: #FA5151; border-radius: 7px; border: 2px solid white;");
        break;
    case PortStatus::Offline:
    default:
        m_statusIndicator->setStyleSheet("background-color: #CCCCCC; border-radius: 7px; border: 2px solid white;");
        break;
    }

    updateStyle();
}

void FriendListItem::updateStyle() {
    QString bgColor;
    if (m_selected) {
        bgColor = "#D0E8FF";
    } else if (m_hovered) {
        bgColor = "#F0F0F0";
    } else {
        bgColor = "#FAFAFA";
    }

    m_contentWidget->setStyleSheet(QString("background-color: %1;").arg(bgColor));
}

void FriendListItem::animateSlide(int targetOffset) {
    if (!m_slideAnimation) {
        m_slideAnimation = new QPropertyAnimation(this, "slideOffset", this);
        m_slideAnimation->setDuration(200);
        m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    }

    m_slideAnimation->stop();
    m_slideAnimation->setStartValue(m_slideOffset);
    m_slideAnimation->setEndValue(targetOffset);
    m_slideAnimation->start();
}

void FriendListItem::onDeleteClicked() { emit deleteRequested(m_info.portName()); }

void FriendListItem::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_pressPos = event->pos();
        m_swiping = false;
    }
    QWidget::mousePressEvent(event);
}

void FriendListItem::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        int dx = event->pos().x() - m_pressPos.x();

        if (qAbs(dx) > 10) {
            m_swiping = true;
        }

        if (m_swiping) {
            int newOffset = m_slideOffset + dx;
            // Limit the slide
            newOffset = qBound(-80, newOffset, 0);
            setSlideOffset(newOffset);
            m_pressPos = event->pos();
        }
    }
    QWidget::mouseMoveEvent(event);
}

void FriendListItem::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (m_swiping) {
            // Snap to either open or closed
            if (m_slideOffset < -40) {
                animateSlide(-80);
            } else {
                animateSlide(0);
                m_deleteButton->hide();
            }
            m_swiping = false;
        } else {
            // It was a click
            if (m_slideOffset < 0) {
                // Close the slide
                animateSlide(0);
                m_deleteButton->hide();
            } else {
                emit clicked(m_info.portName());
            }
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void FriendListItem::enterEvent(QEvent *event) {
    m_hovered = true;
    updateStyle();
    QWidget::enterEvent(event);
}

void FriendListItem::leaveEvent(QEvent *event) {
    m_hovered = false;
    updateStyle();
    QWidget::leaveEvent(event);
}

void FriendListItem::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    m_contentWidget->setFixedWidth(width());
    m_deleteButton->move(width() - 80, 0);
}
