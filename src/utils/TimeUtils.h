#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <QString>
#include <QDateTime>

/**
 * @brief Utility functions for time formatting
 */
class TimeUtils {
public:
    /**
     * @brief Format timestamp for display in chat
     * @param timestamp The timestamp to format
     * @return Formatted time string
     */
    static QString formatChatTime(const QDateTime& timestamp);
    
    /**
     * @brief Format timestamp for message bubble
     * @param timestamp The timestamp to format
     * @return Short time string (e.g., "14:30")
     */
    static QString formatBubbleTime(const QDateTime& timestamp);
    
    /**
     * @brief Format timestamp with date for history
     * @param timestamp The timestamp to format
     * @return Full date and time string
     */
    static QString formatFullTime(const QDateTime& timestamp);
    
    /**
     * @brief Get relative time description
     * @param timestamp The timestamp to describe
     * @return Relative description (e.g., "2 minutes ago")
     */
    static QString relativeTime(const QDateTime& timestamp);
    
private:
    TimeUtils() = default;
};

#endif // TIME_UTILS_H
