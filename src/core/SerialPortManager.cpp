#include "SerialPortManager.h"

SerialPortManager::SerialPortManager(QObject *parent) : QObject(parent), m_refreshTimer(new QTimer(this)) {
    QObject::connect(m_refreshTimer, &QTimer::timeout, this, &SerialPortManager::onRefreshTimer);

    updateAvailablePorts();
}

SerialPortManager::~SerialPortManager() {
    disconnectAll();
    qDeleteAll(m_users);
    m_users.clear();
}

QStringList SerialPortManager::availablePorts() const { return m_availablePorts; }

void SerialPortManager::refreshAvailablePorts() { updateAvailablePorts(); }

void SerialPortManager::updateAvailablePorts() {
    QStringList newPorts;
    const auto ports = QSerialPortInfo::availablePorts();
    for (const auto &port : ports) {
        newPorts.append(port.portName());
    }

    if (newPorts != m_availablePorts) {
        m_availablePorts = newPorts;
        emit availablePortsChanged(m_availablePorts);
    }
}

SerialPortUser *SerialPortManager::getUser(const QString &portName) { return m_users.value(portName, nullptr); }

SerialPortUser *SerialPortManager::createUser(const QString &portName) {
    if (m_users.contains(portName)) {
        return m_users.value(portName);
    }

    SerialPortInfo info(portName);
    return createUser(info);
}

SerialPortUser *SerialPortManager::createUser(const SerialPortInfo &info) {
    QString portName = info.portName();

    if (m_users.contains(portName)) {
        SerialPortUser *user = m_users.value(portName);
        user->setInfo(info);
        return user;
    }

    SerialPortUser *user = new SerialPortUser(info, this);
    m_users.insert(portName, user);

    QObject::connect(user, &SerialPortUser::statusChanged, this, &SerialPortManager::onUserStatusChanged);
    QObject::connect(user, &SerialPortUser::messageReceived, this, &SerialPortManager::onUserMessageReceived);
    QObject::connect(user, &SerialPortUser::messageSent, this, &SerialPortManager::onUserMessageSent);

    // Add to friend list
    addToFriendList(info);

    emit userCreated(portName);
    return user;
}

bool SerialPortManager::removeUser(const QString &portName) {
    if (!m_users.contains(portName)) {
        return false;
    }

    SerialPortUser *user = m_users.take(portName);
    user->disconnect();
    delete user;

    emit userRemoved(portName);
    return true;
}

QList<SerialPortInfo> SerialPortManager::friendList() const { return m_friendList.values(); }

QList<SerialPortInfo> SerialPortManager::onlineFriends() const {
    QList<SerialPortInfo> online;
    for (const auto &info : m_friendList) {
        if (m_users.contains(info.portName())) {
            SerialPortUser *user = m_users.value(info.portName());
            if (user->isOnline()) {
                SerialPortInfo updatedInfo = info;
                updatedInfo.setStatus(PortStatus::Online);
                online.append(updatedInfo);
            }
        }
    }
    return online;
}

QList<SerialPortInfo> SerialPortManager::offlineFriends() const {
    QList<SerialPortInfo> offline;
    for (const auto &info : m_friendList) {
        bool isOnline = false;
        if (m_users.contains(info.portName())) {
            SerialPortUser *user = m_users.value(info.portName());
            isOnline = user->isOnline();
        }
        if (!isOnline) {
            SerialPortInfo updatedInfo = info;
            updatedInfo.setStatus(PortStatus::Offline);
            offline.append(updatedInfo);
        }
    }
    return offline;
}

bool SerialPortManager::hasFriend(const QString &portName) const { return m_friendList.contains(portName); }

void SerialPortManager::addToFriendList(const QString &portName) {
    if (!m_friendList.contains(portName)) {
        m_friendList.insert(portName, SerialPortInfo(portName));
        emit friendListChanged();
    }
}

void SerialPortManager::addToFriendList(const SerialPortInfo &info) {
    bool changed = false;
    if (!m_friendList.contains(info.portName())) {
        m_friendList.insert(info.portName(), info);
        changed = true;
    } else {
        // Update existing info while preserving some fields
        SerialPortInfo &existing = m_friendList[info.portName()];
        if (existing.remark().isEmpty() && !info.remark().isEmpty()) {
            existing.setRemark(info.remark());
            changed = true;
        }
    }

    if (changed) {
        emit friendListChanged();
    }
}

void SerialPortManager::setPortRemark(const QString &portName, const QString &remark) {
    if (m_friendList.contains(portName)) {
        m_friendList[portName].setRemark(remark);
        emit friendListChanged();
    }

    if (m_users.contains(portName)) {
        m_users[portName]->setRemark(remark);
    }
}

void SerialPortManager::updatePortSettings(const SerialPortInfo &info) {
    QString portName = info.portName();

    if (m_friendList.contains(portName)) {
        SerialPortInfo &existing = m_friendList[portName];
        existing.setBaudRate(info.baudRate());
        existing.setDataBits(info.dataBits());
        existing.setStopBits(info.stopBits());
        existing.setParity(info.parity());
        existing.setFlowControl(info.flowControl());
        if (!info.remark().isEmpty()) {
            existing.setRemark(info.remark());
        }
        emit friendListChanged();
    }

    if (m_users.contains(portName)) {
        m_users[portName]->setInfo(info);
    }
}

bool SerialPortManager::connectPort(const QString &portName) {
    SerialPortUser *user = getUser(portName);
    if (!user) {
        user = createUser(portName);
    }

    return user->connect();
}

void SerialPortManager::disconnectPort(const QString &portName) {
    SerialPortUser *user = getUser(portName);
    if (user) {
        user->disconnect();
    }
}

void SerialPortManager::disconnectAll() {
    for (auto user : m_users) {
        user->disconnect();
    }
}

int SerialPortManager::onlineCount() const {
    int count = 0;
    for (auto user : m_users) {
        if (user->isOnline()) {
            count++;
        }
    }
    return count;
}

void SerialPortManager::setAutoRefresh(bool enabled) {
    if (enabled && !m_refreshTimer->isActive()) {
        m_refreshTimer->start(1000);
    } else if (!enabled && m_refreshTimer->isActive()) {
        m_refreshTimer->stop();
    }
}

void SerialPortManager::setRefreshInterval(int msec) { m_refreshTimer->setInterval(msec); }

void SerialPortManager::onRefreshTimer() { updateAvailablePorts(); }

void SerialPortManager::onUserStatusChanged(PortStatus status) {
    SerialPortUser *user = qobject_cast<SerialPortUser *>(sender());
    if (user) {
        emit userStatusChanged(user->portName(), status);
    }
}

void SerialPortManager::onUserMessageReceived(const Message &message) {
    SerialPortUser *user = qobject_cast<SerialPortUser *>(sender());
    if (user) {
        emit userMessageReceived(user->portName(), message);
    }
}

void SerialPortManager::onUserMessageSent(const Message &message) {
    SerialPortUser *user = qobject_cast<SerialPortUser *>(sender());
    if (user) {
        emit userMessageSent(user->portName(), message);
    }
}
