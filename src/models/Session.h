#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QList>
#include "Message.h"

class Session : public QObject
{
    Q_OBJECT

public:
    explicit Session(QObject *parent = nullptr);
    ~Session() override = default;

    void addMessage(Message *message);
    void clearMessages();
    
    QList<Message*> messages() const;
    int messageCount() const;

    QString sessionName() const;
    void setSessionName(const QString &name);

signals:
    void messageAdded(Message *message);
    void messagesCleared();
    void sessionNameChanged();

private:
    QString m_sessionName;
    QList<Message*> m_messages;
};

#endif // SESSION_H
