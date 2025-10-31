#include "session.h"
#include "serialport.h"
#include "message.h"

Session::Session(QObject *parent)
    : QObject(parent)
    , m_name("Default Session")
{
}

Session::Session(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
}

Session::~Session()
{
    // Clean up messages (they are owned by this session)
    qDeleteAll(m_messages);
    m_messages.clear();
}

void Session::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

void Session::addSerialPort(SerialPort *port)
{
    if (port && !m_serialPorts.contains(port)) {
        m_serialPorts.append(port);
        // Connect to receive data from this port
        connect(port, &SerialPort::dataReceived, this, &Session::handleDataReceived);
        emit serialPortAdded(port);
    }
}

void Session::removeSerialPort(SerialPort *port)
{
    if (port && m_serialPorts.contains(port)) {
        disconnect(port, &SerialPort::dataReceived, this, &Session::handleDataReceived);
        m_serialPorts.removeOne(port);
        emit serialPortRemoved(port);
    }
}

void Session::addMessage(Message *message)
{
    if (message) {
        m_messages.append(message);
        emit messageAdded(message);
    }
}

void Session::clearMessages()
{
    qDeleteAll(m_messages);
    m_messages.clear();
    emit messagesCleared();
}

void Session::handleDataReceived(const QByteArray &data)
{
    SerialPort *port = qobject_cast<SerialPort*>(sender());
    if (port) {
        Message *message = new Message(data, Message::Incoming, port, this);
        addMessage(message);
    }
}
