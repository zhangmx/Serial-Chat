#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QDateTime>
#include <QObject>

class Message : public QObject
{
    Q_OBJECT

public:
    enum Direction {
        Incoming,
        Outgoing
    };

    explicit Message(QObject *parent = nullptr);
    Message(const QString &content, Direction direction, QObject *parent = nullptr);
    ~Message() override = default;

    QString content() const;
    void setContent(const QString &content);

    Direction direction() const;
    void setDirection(Direction direction);

    QDateTime timestamp() const;
    void setTimestamp(const QDateTime &timestamp);

signals:
    void contentChanged();
    void directionChanged();

private:
    QString m_content;
    Direction m_direction;
    QDateTime m_timestamp;
};

#endif // MESSAGE_H
