#include "Session.h"

Session::Session(QObject *parent)
    : QObject(parent)
    , m_sessionName("Default Session")
{
}

void Session::addMessage(Message *message)
{
    if (message) {
        message->setParent(this);
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

QList<Message*> Session::messages() const
{
    return m_messages;
}

int Session::messageCount() const
{
    return m_messages.count();
}

QString Session::sessionName() const
{
    return m_sessionName;
}

void Session::setSessionName(const QString &name)
{
    if (m_sessionName != name) {
        m_sessionName = name;
        emit sessionNameChanged();
    }
}
