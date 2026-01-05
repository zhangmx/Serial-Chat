#ifndef SERIAL_PORT_MANAGER_H
#define SERIAL_PORT_MANAGER_H

#include "SerialPortInfo.h"
#include "SerialPortUser.h"
#include <QMap>
#include <QObject>
#include <QSerialPortInfo>
#include <QTimer>

/**
 * @brief Manages all serial port users in the application
 *
 * This class is responsible for:
 * - Discovering available serial ports
 * - Managing serial port user instances
 * - Tracking online/offline status
 * - Maintaining the "friend list" of used ports
 */
class SerialPortManager : public QObject {
    Q_OBJECT

  public:
    explicit SerialPortManager(QObject *parent = nullptr);
    ~SerialPortManager() override;

    // Port discovery
    QStringList availablePorts() const;
    void refreshAvailablePorts();

    // Port user management
    SerialPortUser *getUser(const QString &portName);
    SerialPortUser *createUser(const QString &portName);
    SerialPortUser *createUser(const SerialPortInfo &info);
    bool removeUser(const QString &portName);

    // Friend list (used ports)
    QList<SerialPortInfo> friendList() const;
    QList<SerialPortInfo> onlineFriends() const;
    QList<SerialPortInfo> offlineFriends() const;
    bool hasFriend(const QString &portName) const;
    void addToFriendList(const QString &portName);
    void addToFriendList(const SerialPortInfo &info);

    // Port settings
    void setPortRemark(const QString &portName, const QString &remark);
    void updatePortSettings(const SerialPortInfo &info);

    // Connection management
    bool connectPort(const QString &portName);
    void disconnectPort(const QString &portName);
    void disconnectAll();

    // Status checking
    int onlineCount() const;
    int totalCount() const { return m_users.size(); }

    // Auto-refresh settings
    void setAutoRefresh(bool enabled);
    void setRefreshInterval(int msec);

  signals:
    void availablePortsChanged(const QStringList &ports);
    void userCreated(const QString &portName);
    void userRemoved(const QString &portName);
    void userStatusChanged(const QString &portName, PortStatus status);
    void userMessageReceived(const QString &portName, const Message &message);
    void userMessageSent(const QString &portName, const Message &message);
    void friendListChanged();

  private slots:
    void onRefreshTimer();
    void onUserStatusChanged(PortStatus status);
    void onUserMessageReceived(const Message &message);
    void onUserMessageSent(const Message &message);

  private:
    QMap<QString, SerialPortUser *> m_users;
    QMap<QString, SerialPortInfo> m_friendList;
    QStringList m_availablePorts;
    QTimer *m_refreshTimer;

    void updateAvailablePorts();
};

#endif // SERIAL_PORT_MANAGER_H
