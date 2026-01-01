#include "FriendListItem.h"
#include "TimeUtils.h"
#include <QMouseEvent>
#include <QPainter>

FriendListItem::FriendListItem(const SerialPortInfo& info, QWidget* parent)
    : QWidget(parent)
    , m_info(info)
    , m_selected(false)
    , m_hovered(false)
{
    setupUi();
    updateDisplay();
}

FriendListItem::~FriendListItem()
{
}

void FriendListItem::setInfo(const SerialPortInfo& info)
{
    m_info = info;
    updateDisplay();
}

void FriendListItem::setStatus(PortStatus status)
{
    m_info.setStatus(status);
    updateDisplay();
}

void FriendListItem::setSelected(bool selected)
{
    if (m_selected != selected) {
        m_selected = selected;
        updateStyle();
    }
}

void FriendListItem::setupUi()
{
    setFixedHeight(70);
    setCursor(Qt::PointingHandCursor);
    
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(10, 8, 10, 8);
    m_mainLayout->setSpacing(10);
    
    // Avatar with status indicator
    m_avatarWidget = new QWidget(this);
    m_avatarWidget->setFixedSize(48, 48);
    m_avatarWidget->setStyleSheet("background-color: #4A90D9; border-radius: 24px;");
    
    m_statusIndicator = new QLabel(m_avatarWidget);
    m_statusIndicator->setFixedSize(12, 12);
    m_statusIndicator->move(36, 36);
    
    // Info layout
    m_infoLayout = new QVBoxLayout();
    m_infoLayout->setContentsMargins(0, 0, 0, 0);
    m_infoLayout->setSpacing(2);
    
    m_nameLabel = new QLabel(this);
    QFont nameFont = m_nameLabel->font();
    nameFont.setPointSize(12);
    nameFont.setBold(true);
    m_nameLabel->setFont(nameFont);
    
    m_remarkLabel = new QLabel(this);
    m_remarkLabel->setStyleSheet("color: #666666;");
    QFont remarkFont = m_remarkLabel->font();
    remarkFont.setPointSize(10);
    m_remarkLabel->setFont(remarkFont);
    
    m_lastActiveLabel = new QLabel(this);
    m_lastActiveLabel->setStyleSheet("color: #999999;");
    QFont activeFont = m_lastActiveLabel->font();
    activeFont.setPointSize(9);
    m_lastActiveLabel->setFont(activeFont);
    
    m_infoLayout->addWidget(m_nameLabel);
    m_infoLayout->addWidget(m_remarkLabel);
    m_infoLayout->addWidget(m_lastActiveLabel);
    
    m_mainLayout->addWidget(m_avatarWidget);
    m_mainLayout->addLayout(m_infoLayout, 1);
}

void FriendListItem::updateDisplay()
{
    m_nameLabel->setText(m_info.portName());
    
    if (m_info.remark().isEmpty()) {
        m_remarkLabel->hide();
    } else {
        m_remarkLabel->setText(m_info.remark());
        m_remarkLabel->show();
    }
    
    if (m_info.lastActiveTime().isValid()) {
        m_lastActiveLabel->setText(TimeUtils::relativeTime(m_info.lastActiveTime()));
        m_lastActiveLabel->show();
    } else {
        m_lastActiveLabel->hide();
    }
    
    // Update status indicator
    switch (m_info.status()) {
        case PortStatus::Online:
            m_statusIndicator->setStyleSheet("background-color: #07C160; border-radius: 6px; border: 2px solid white;");
            break;
        case PortStatus::Error:
            m_statusIndicator->setStyleSheet("background-color: #FA5151; border-radius: 6px; border: 2px solid white;");
            break;
        case PortStatus::Offline:
        default:
            m_statusIndicator->setStyleSheet("background-color: #CCCCCC; border-radius: 6px; border: 2px solid white;");
            break;
    }
    
    updateStyle();
}

void FriendListItem::updateStyle()
{
    QString bgColor;
    if (m_selected) {
        bgColor = "#D0E8FF";
    } else if (m_hovered) {
        bgColor = "#F0F0F0";
    } else {
        bgColor = "transparent";
    }
    
    setStyleSheet(QString("FriendListItem { background-color: %1; }").arg(bgColor));
}

void FriendListItem::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_info.portName());
    }
    QWidget::mousePressEvent(event);
}

void FriendListItem::enterEvent(QEvent* event)
{
    m_hovered = true;
    updateStyle();
    QWidget::enterEvent(event);
}

void FriendListItem::leaveEvent(QEvent* event)
{
    m_hovered = false;
    updateStyle();
    QWidget::leaveEvent(event);
}
