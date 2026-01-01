#include "TimeUtils.h"
#include <QCoreApplication>

QString TimeUtils::formatChatTime(const QDateTime& timestamp)
{
    QDateTime now = QDateTime::currentDateTime();
    QDate today = now.date();
    QDate msgDate = timestamp.date();
    
    if (msgDate == today) {
        return timestamp.toString("hh:mm:ss");
    } else if (msgDate == today.addDays(-1)) {
        return QCoreApplication::translate("TimeUtils", "Yesterday") + " " + timestamp.toString("hh:mm");
    } else if (msgDate.year() == today.year()) {
        return timestamp.toString("MM-dd hh:mm");
    } else {
        return timestamp.toString("yyyy-MM-dd hh:mm");
    }
}

QString TimeUtils::formatBubbleTime(const QDateTime& timestamp)
{
    return timestamp.toString("hh:mm");
}

QString TimeUtils::formatFullTime(const QDateTime& timestamp)
{
    return timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz");
}

QString TimeUtils::relativeTime(const QDateTime& timestamp)
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 secs = timestamp.secsTo(now);
    
    if (secs < 0) {
        return QCoreApplication::translate("TimeUtils", "just now");
    } else if (secs < 60) {
        return QCoreApplication::translate("TimeUtils", "%1 seconds ago").arg(secs);
    } else if (secs < 3600) {
        int mins = secs / 60;
        return QCoreApplication::translate("TimeUtils", "%1 minutes ago").arg(mins);
    } else if (secs < 86400) {
        int hours = secs / 3600;
        return QCoreApplication::translate("TimeUtils", "%1 hours ago").arg(hours);
    } else {
        int days = secs / 86400;
        return QCoreApplication::translate("TimeUtils", "%1 days ago").arg(days);
    }
}
