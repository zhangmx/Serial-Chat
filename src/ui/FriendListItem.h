#ifndef FRIEND_LIST_ITEM_H
#define FRIEND_LIST_ITEM_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "SerialPortInfo.h"

/**
 * @brief A single item in the friend list representing a serial port
 */
class FriendListItem : public QWidget {
    Q_OBJECT

public:
    explicit FriendListItem(const SerialPortInfo& info, QWidget* parent = nullptr);
    ~FriendListItem() override;
    
    // Port info
    SerialPortInfo info() const { return m_info; }
    QString portName() const { return m_info.portName(); }
    
    // Update
    void setInfo(const SerialPortInfo& info);
    void setStatus(PortStatus status);
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

signals:
    void clicked(const QString& portName);
    void connectRequested(const QString& portName);
    void disconnectRequested(const QString& portName);
    void settingsRequested(const QString& portName);
    void remarkRequested(const QString& portName);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    SerialPortInfo m_info;
    bool m_selected;
    bool m_hovered;
    
    // UI components
    QHBoxLayout* m_mainLayout;
    QWidget* m_avatarWidget;
    QLabel* m_statusIndicator;
    QVBoxLayout* m_infoLayout;
    QLabel* m_nameLabel;
    QLabel* m_remarkLabel;
    QLabel* m_lastActiveLabel;
    
    void setupUi();
    void updateDisplay();
    void updateStyle();
};

#endif // FRIEND_LIST_ITEM_H
