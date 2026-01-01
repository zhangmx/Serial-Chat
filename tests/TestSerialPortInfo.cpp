#include <gtest/gtest.h>
#include "SerialPortInfo.h"

class SerialPortInfoTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(SerialPortInfoTest, DefaultConstructor) {
    SerialPortInfo info;
    
    EXPECT_TRUE(info.portName().isEmpty());
    EXPECT_TRUE(info.remark().isEmpty());
    EXPECT_EQ(info.baudRate(), 115200);
    EXPECT_EQ(info.dataBits(), QSerialPort::Data8);
    EXPECT_EQ(info.stopBits(), QSerialPort::OneStop);
    EXPECT_EQ(info.parity(), QSerialPort::NoParity);
    EXPECT_EQ(info.flowControl(), QSerialPort::NoFlowControl);
    EXPECT_EQ(info.status(), PortStatus::Offline);
}

TEST_F(SerialPortInfoTest, NameConstructor) {
    SerialPortInfo info("COM1");
    
    EXPECT_EQ(info.portName(), "COM1");
    EXPECT_EQ(info.status(), PortStatus::Offline);
}

TEST_F(SerialPortInfoTest, DisplayName_NoRemark) {
    SerialPortInfo info("COM1");
    
    EXPECT_EQ(info.displayName(), "COM1");
}

TEST_F(SerialPortInfoTest, DisplayName_WithRemark) {
    SerialPortInfo info("COM1");
    info.setRemark("Arduino");
    
    EXPECT_EQ(info.displayName(), "Arduino (COM1)");
}

TEST_F(SerialPortInfoTest, IsOnline) {
    SerialPortInfo info("COM1");
    
    EXPECT_FALSE(info.isOnline());
    
    info.setStatus(PortStatus::Online);
    EXPECT_TRUE(info.isOnline());
    
    info.setStatus(PortStatus::Error);
    EXPECT_FALSE(info.isOnline());
}

TEST_F(SerialPortInfoTest, Setters) {
    SerialPortInfo info;
    
    info.setPortName("COM2");
    EXPECT_EQ(info.portName(), "COM2");
    
    info.setRemark("GPS Module");
    EXPECT_EQ(info.remark(), "GPS Module");
    
    info.setBaudRate(9600);
    EXPECT_EQ(info.baudRate(), 9600);
    
    info.setDataBits(QSerialPort::Data7);
    EXPECT_EQ(info.dataBits(), QSerialPort::Data7);
    
    info.setStopBits(QSerialPort::TwoStop);
    EXPECT_EQ(info.stopBits(), QSerialPort::TwoStop);
    
    info.setParity(QSerialPort::EvenParity);
    EXPECT_EQ(info.parity(), QSerialPort::EvenParity);
    
    info.setFlowControl(QSerialPort::HardwareControl);
    EXPECT_EQ(info.flowControl(), QSerialPort::HardwareControl);
}

TEST_F(SerialPortInfoTest, JsonSerialization) {
    SerialPortInfo original("COM3");
    original.setRemark("Test Device");
    original.setBaudRate(9600);
    original.setDataBits(QSerialPort::Data7);
    original.setStopBits(QSerialPort::TwoStop);
    original.setParity(QSerialPort::OddParity);
    original.updateLastActiveTime();
    
    QJsonObject json = original.toJson();
    SerialPortInfo restored = SerialPortInfo::fromJson(json);
    
    EXPECT_EQ(restored.portName(), original.portName());
    EXPECT_EQ(restored.remark(), original.remark());
    EXPECT_EQ(restored.baudRate(), original.baudRate());
    EXPECT_EQ(restored.dataBits(), original.dataBits());
    EXPECT_EQ(restored.stopBits(), original.stopBits());
    EXPECT_EQ(restored.parity(), original.parity());
    // Status is not serialized (always offline after load)
    EXPECT_EQ(restored.status(), PortStatus::Offline);
}

TEST_F(SerialPortInfoTest, EqualityOperator) {
    SerialPortInfo info1("COM1");
    SerialPortInfo info2("COM1");
    SerialPortInfo info3("COM2");
    
    EXPECT_EQ(info1, info2);
    EXPECT_NE(info1, info3);
}

TEST_F(SerialPortInfoTest, LastActiveTime) {
    SerialPortInfo info("COM1");
    
    EXPECT_FALSE(info.lastActiveTime().isValid());
    
    info.updateLastActiveTime();
    EXPECT_TRUE(info.lastActiveTime().isValid());
    
    QDateTime before = QDateTime::currentDateTime();
    info.updateLastActiveTime();
    QDateTime after = QDateTime::currentDateTime();
    
    EXPECT_TRUE(info.lastActiveTime() >= before);
    EXPECT_TRUE(info.lastActiveTime() <= after);
}
