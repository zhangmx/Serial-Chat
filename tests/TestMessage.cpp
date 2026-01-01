#include <gtest/gtest.h>
#include "Message.h"

class MessageTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(MessageTest, DefaultConstructor) {
    Message msg;
    
    EXPECT_FALSE(msg.id().isEmpty());
    EXPECT_TRUE(msg.portName().isEmpty());
    EXPECT_TRUE(msg.data().isEmpty());
    EXPECT_EQ(msg.direction(), MessageDirection::Received);
    EXPECT_TRUE(msg.timestamp().isValid());
}

TEST_F(MessageTest, ParameterizedConstructor) {
    QString portName = "COM1";
    QByteArray data = "Hello World";
    MessageDirection direction = MessageDirection::Sent;
    
    Message msg(portName, data, direction);
    
    EXPECT_FALSE(msg.id().isEmpty());
    EXPECT_EQ(msg.portName(), portName);
    EXPECT_EQ(msg.data(), data);
    EXPECT_EQ(msg.direction(), direction);
    EXPECT_TRUE(msg.timestamp().isValid());
}

TEST_F(MessageTest, ToText) {
    QByteArray data = "Hello World";
    Message msg("COM1", data, MessageDirection::Received);
    
    EXPECT_EQ(msg.toText(), "Hello World");
}

TEST_F(MessageTest, ToHex) {
    QByteArray data = "Hi";
    Message msg("COM1", data, MessageDirection::Received);
    
    QString hex = msg.toHex();
    EXPECT_EQ(hex, "48 69");
}

TEST_F(MessageTest, DisplayText) {
    QByteArray data = "Test";
    Message msg("COM1", data, MessageDirection::Received);
    
    EXPECT_EQ(msg.displayText(MessageFormat::Text), "Test");
    EXPECT_EQ(msg.displayText(MessageFormat::Hex), "54 65 73 74");
}

TEST_F(MessageTest, FormattedTime) {
    Message msg("COM1", "data", MessageDirection::Sent);
    
    QString time = msg.formattedTime();
    EXPECT_FALSE(time.isEmpty());
    // Format should be "hh:mm:ss"
    EXPECT_EQ(time.length(), 8);
}

TEST_F(MessageTest, JsonSerialization) {
    QString portName = "COM3";
    QByteArray data = "Test Data";
    MessageDirection direction = MessageDirection::Sent;
    
    Message original(portName, data, direction);
    
    QJsonObject json = original.toJson();
    Message restored = Message::fromJson(json);
    
    EXPECT_EQ(restored.id(), original.id());
    EXPECT_EQ(restored.portName(), original.portName());
    EXPECT_EQ(restored.data(), original.data());
    EXPECT_EQ(restored.direction(), original.direction());
}

TEST_F(MessageTest, EqualityOperator) {
    Message msg1("COM1", "data", MessageDirection::Sent);
    Message msg2("COM1", "data", MessageDirection::Sent);
    Message msg3 = msg1;
    
    // Different messages have different IDs
    EXPECT_NE(msg1, msg2);
    
    // Same message should be equal
    EXPECT_EQ(msg1.id(), msg3.id());
}

TEST_F(MessageTest, Setters) {
    Message msg;
    
    msg.setPortName("COM5");
    EXPECT_EQ(msg.portName(), "COM5");
    
    msg.setData("New Data");
    EXPECT_EQ(msg.data(), "New Data");
    
    msg.setDirection(MessageDirection::Sent);
    EXPECT_EQ(msg.direction(), MessageDirection::Sent);
    
    QDateTime newTime = QDateTime::currentDateTime().addSecs(-3600);
    msg.setTimestamp(newTime);
    EXPECT_EQ(msg.timestamp(), newTime);
}
