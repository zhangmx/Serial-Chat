#include "HexUtils.h"
#include <QRegularExpression>

QByteArray HexUtils::hexStringToByteArray(const QString& hexString)
{
    // Remove all whitespace and common separators
    QString cleanHex = hexString;
    cleanHex.remove(QRegularExpression("[\\s,;:-]"));
    
    // Check for valid hex characters
    if (!QRegularExpression("^[0-9A-Fa-f]*$").match(cleanHex).hasMatch()) {
        return QByteArray();
    }
    
    // Ensure even number of characters
    if (cleanHex.length() % 2 != 0) {
        cleanHex.prepend('0');
    }
    
    return QByteArray::fromHex(cleanHex.toUtf8());
}

QString HexUtils::byteArrayToHexString(const QByteArray& data, const QString& separator)
{
    if (data.isEmpty()) {
        return QString();
    }
    
    QStringList hexParts;
    for (int i = 0; i < data.size(); ++i) {
        hexParts.append(QString("%1").arg(static_cast<unsigned char>(data.at(i)), 2, 16, QChar('0')).toUpper());
    }
    
    return hexParts.join(separator);
}

bool HexUtils::isValidHexString(const QString& hexString)
{
    QString cleanHex = hexString;
    cleanHex.remove(QRegularExpression("[\\s,;:-]"));
    
    if (cleanHex.isEmpty()) {
        return false;
    }
    
    return QRegularExpression("^[0-9A-Fa-f]+$").match(cleanHex).hasMatch();
}

QString HexUtils::formatHexString(const QString& hexString)
{
    QByteArray data = hexStringToByteArray(hexString);
    return byteArrayToHexString(data, " ");
}
