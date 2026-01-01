#include <gtest/gtest.h>
#include "HexUtils.h"

class HexUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(HexUtilsTest, HexStringToByteArray_Simple) {
    QString hexString = "48656C6C6F";  // "Hello"
    QByteArray result = HexUtils::hexStringToByteArray(hexString);
    
    EXPECT_EQ(result, QByteArray("Hello"));
}

TEST_F(HexUtilsTest, HexStringToByteArray_WithSpaces) {
    QString hexString = "48 65 6C 6C 6F";  // "Hello"
    QByteArray result = HexUtils::hexStringToByteArray(hexString);
    
    EXPECT_EQ(result, QByteArray("Hello"));
}

TEST_F(HexUtilsTest, HexStringToByteArray_Lowercase) {
    QString hexString = "48 65 6c 6c 6f";  // "Hello"
    QByteArray result = HexUtils::hexStringToByteArray(hexString);
    
    EXPECT_EQ(result, QByteArray("Hello"));
}

TEST_F(HexUtilsTest, HexStringToByteArray_MixedCase) {
    QString hexString = "48 65 6C 6c 6F";
    QByteArray result = HexUtils::hexStringToByteArray(hexString);
    
    EXPECT_EQ(result, QByteArray("Hello"));
}

TEST_F(HexUtilsTest, HexStringToByteArray_OddLength) {
    QString hexString = "1";  // Should be treated as "01"
    QByteArray result = HexUtils::hexStringToByteArray(hexString);
    
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(static_cast<unsigned char>(result.at(0)), 0x01);
}

TEST_F(HexUtilsTest, HexStringToByteArray_Empty) {
    QString hexString = "";
    QByteArray result = HexUtils::hexStringToByteArray(hexString);
    
    EXPECT_TRUE(result.isEmpty());
}

TEST_F(HexUtilsTest, HexStringToByteArray_Invalid) {
    QString hexString = "GHIJ";  // Invalid hex characters
    QByteArray result = HexUtils::hexStringToByteArray(hexString);
    
    EXPECT_TRUE(result.isEmpty());
}

TEST_F(HexUtilsTest, HexStringToByteArray_WithColons) {
    QString hexString = "48:65:6C:6C:6F";
    QByteArray result = HexUtils::hexStringToByteArray(hexString);
    
    EXPECT_EQ(result, QByteArray("Hello"));
}

TEST_F(HexUtilsTest, ByteArrayToHexString_Simple) {
    QByteArray data = "Hi";
    QString result = HexUtils::byteArrayToHexString(data);
    
    EXPECT_EQ(result, "48 69");
}

TEST_F(HexUtilsTest, ByteArrayToHexString_Empty) {
    QByteArray data;
    QString result = HexUtils::byteArrayToHexString(data);
    
    EXPECT_TRUE(result.isEmpty());
}

TEST_F(HexUtilsTest, ByteArrayToHexString_CustomSeparator) {
    QByteArray data = "Hi";
    QString result = HexUtils::byteArrayToHexString(data, ":");
    
    EXPECT_EQ(result, "48:69");
}

TEST_F(HexUtilsTest, ByteArrayToHexString_NoSeparator) {
    QByteArray data = "Hi";
    QString result = HexUtils::byteArrayToHexString(data, "");
    
    EXPECT_EQ(result, "4869");
}

TEST_F(HexUtilsTest, ByteArrayToHexString_BinaryData) {
    QByteArray data;
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0xFF));
    data.append(static_cast<char>(0x0A));
    
    QString result = HexUtils::byteArrayToHexString(data);
    
    EXPECT_EQ(result, "00 FF 0A");
}

TEST_F(HexUtilsTest, IsValidHexString_Valid) {
    EXPECT_TRUE(HexUtils::isValidHexString("48656C6C6F"));
    EXPECT_TRUE(HexUtils::isValidHexString("48 65 6C 6C 6F"));
    EXPECT_TRUE(HexUtils::isValidHexString("AABBCCDD"));
    EXPECT_TRUE(HexUtils::isValidHexString("aabbccdd"));
    EXPECT_TRUE(HexUtils::isValidHexString("0123456789ABCDEFabcdef"));
}

TEST_F(HexUtilsTest, IsValidHexString_Invalid) {
    EXPECT_FALSE(HexUtils::isValidHexString(""));
    EXPECT_FALSE(HexUtils::isValidHexString("GHIJ"));
    EXPECT_FALSE(HexUtils::isValidHexString("Hello"));
    EXPECT_FALSE(HexUtils::isValidHexString("0x48"));  // 'x' is invalid
}

TEST_F(HexUtilsTest, FormatHexString) {
    EXPECT_EQ(HexUtils::formatHexString("48656C6C6F"), "48 65 6C 6C 6F");
    EXPECT_EQ(HexUtils::formatHexString("48:65:6C:6C:6F"), "48 65 6C 6C 6F");
    EXPECT_EQ(HexUtils::formatHexString("  48  65  "), "48 65");
}

TEST_F(HexUtilsTest, RoundTrip) {
    QByteArray original = "Test Data 123!@#";
    QString hex = HexUtils::byteArrayToHexString(original);
    QByteArray restored = HexUtils::hexStringToByteArray(hex);
    
    EXPECT_EQ(restored, original);
}
