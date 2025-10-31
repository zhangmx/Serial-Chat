#ifndef SESSION_H
#define SESSION_H

#include <QObject>
#include <QList>
#include <QString>

// Forward declarations
class SerialPort;
class Message;

class Session : public QObject
{
    Q_OBJECT

public:
    explicit Session(QObject *parent = nullptr);
    explicit Session(const QString &name, QObject *parent = nullptr);
    ~Session();

    QString name() const { return m_name; }
    void setName(const QString &name);

    void addSerialPort(SerialPort *port);
    void removeSerialPort(SerialPort *port);
    QList<SerialPort*> serialPorts() const { return m_serialPorts; }

    void addMessage(Message *message);
    QList<Message*> messages() const { return m_messages; }
    void clearMessages();

Q_SIGNALS:
    void nameChanged();
    void serialPortAdded(SerialPort *port);
    void serialPortRemoved(SerialPort *port);
    void messageAdded(Message *message);
    void messagesCleared();

private Q_SLOTS:
    void handleDataReceived(const QByteArray &data);

private:
    QString m_name;
    QList<SerialPort*> m_serialPorts;
    QList<Message*> m_messages;
};

#endif // SESSION_H
