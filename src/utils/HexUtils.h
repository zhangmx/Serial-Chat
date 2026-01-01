#ifndef HEX_UTILS_H
#define HEX_UTILS_H

#include <QString>
#include <QByteArray>

/**
 * @brief Utility functions for hex string conversion
 */
class HexUtils {
public:
    /**
     * @brief Convert a hex string to byte array
     * @param hexString Hex string like "48 65 6C 6C 6F" or "48656C6C6F"
     * @return Converted byte array
     */
    static QByteArray hexStringToByteArray(const QString& hexString);
    
    /**
     * @brief Convert byte array to hex string
     * @param data Byte array to convert
     * @param separator Separator between bytes (default: space)
     * @return Hex string representation
     */
    static QString byteArrayToHexString(const QByteArray& data, const QString& separator = " ");
    
    /**
     * @brief Check if a string is a valid hex string
     * @param hexString String to check
     * @return true if valid hex string
     */
    static bool isValidHexString(const QString& hexString);
    
    /**
     * @brief Format hex string with consistent spacing
     * @param hexString Input hex string
     * @return Formatted hex string with spaces between each byte
     */
    static QString formatHexString(const QString& hexString);
    
private:
    HexUtils() = default;
};

#endif // HEX_UTILS_H
