#include <gtest/gtest.h>
#include "MessageManager.h"

class MessageManagerTest : public ::testing::Test {
protected:
    MessageManager* manager;
    
    void SetUp() override {
        manager = new MessageManager();
    }
    
    void TearDown() override {
        delete manager;
    }
};

TEST_F(MessageManagerTest, AddMessage) {
    Message msg("COM1", "Hello", MessageDirection::Received);
    manager->addMessage(msg);
    
    EXPECT_EQ(manager->messageCount("COM1"), 1);
    EXPECT_EQ(manager->totalMessageCount(), 1);
}

TEST_F(MessageManagerTest, AddMessage_DifferentPorts) {
    manager->addMessage("COM1", "Hello", MessageDirection::Received);
    manager->addMessage("COM2", "World", MessageDirection::Sent);
    manager->addMessage("COM1", "Test", MessageDirection::Received);
    
    EXPECT_EQ(manager->messageCount("COM1"), 2);
    EXPECT_EQ(manager->messageCount("COM2"), 1);
    EXPECT_EQ(manager->totalMessageCount(), 3);
}

TEST_F(MessageManagerTest, GetMessages) {
    manager->addMessage("COM1", "Message 1", MessageDirection::Received);
    manager->addMessage("COM1", "Message 2", MessageDirection::Sent);
    manager->addMessage("COM1", "Message 3", MessageDirection::Received);
    
    QList<Message> messages = manager->getMessages("COM1");
    
    EXPECT_EQ(messages.size(), 3);
    EXPECT_EQ(messages[0].toText(), "Message 1");
    EXPECT_EQ(messages[1].toText(), "Message 2");
    EXPECT_EQ(messages[2].toText(), "Message 3");
}

TEST_F(MessageManagerTest, GetMessages_WithLimit) {
    for (int i = 0; i < 10; ++i) {
        manager->addMessage("COM1", QString("Message %1").arg(i).toUtf8(), MessageDirection::Received);
    }
    
    QList<Message> messages = manager->getMessages("COM1", 3);
    
    EXPECT_EQ(messages.size(), 3);
    // Should get the last 3 messages
    EXPECT_EQ(messages[0].toText(), "Message 7");
    EXPECT_EQ(messages[1].toText(), "Message 8");
    EXPECT_EQ(messages[2].toText(), "Message 9");
}

TEST_F(MessageManagerTest, GetMessages_NonexistentPort) {
    QList<Message> messages = manager->getMessages("COM99");
    
    EXPECT_TRUE(messages.isEmpty());
}

TEST_F(MessageManagerTest, GetLastMessage) {
    manager->addMessage("COM1", "First", MessageDirection::Received);
    manager->addMessage("COM1", "Second", MessageDirection::Received);
    manager->addMessage("COM1", "Last", MessageDirection::Received);
    
    Message last = manager->getLastMessage("COM1");
    
    EXPECT_EQ(last.toText(), "Last");
}

TEST_F(MessageManagerTest, GetLastMessage_EmptyPort) {
    Message last = manager->getLastMessage("COM99");
    
    EXPECT_TRUE(last.portName().isEmpty());
}

TEST_F(MessageManagerTest, ClearMessages) {
    manager->addMessage("COM1", "Test", MessageDirection::Received);
    manager->addMessage("COM2", "Test", MessageDirection::Received);
    
    manager->clearMessages("COM1");
    
    EXPECT_EQ(manager->messageCount("COM1"), 0);
    EXPECT_EQ(manager->messageCount("COM2"), 1);
}

TEST_F(MessageManagerTest, ClearAllMessages) {
    manager->addMessage("COM1", "Test", MessageDirection::Received);
    manager->addMessage("COM2", "Test", MessageDirection::Received);
    manager->addMessage("COM3", "Test", MessageDirection::Received);
    
    manager->clearAllMessages();
    
    EXPECT_EQ(manager->totalMessageCount(), 0);
    EXPECT_EQ(manager->messageCount("COM1"), 0);
    EXPECT_EQ(manager->messageCount("COM2"), 0);
    EXPECT_EQ(manager->messageCount("COM3"), 0);
}

TEST_F(MessageManagerTest, GetAllMessages) {
    manager->addMessage("COM1", "A", MessageDirection::Received);
    manager->addMessage("COM2", "B", MessageDirection::Received);
    manager->addMessage("COM3", "C", MessageDirection::Received);
    
    QList<Message> all = manager->getAllMessages();
    
    EXPECT_EQ(all.size(), 3);
}

TEST_F(MessageManagerTest, GroupMessages) {
    QString groupId = "group1";
    Message msg1("COM1", "Hello", MessageDirection::Received);
    Message msg2("COM2", "World", MessageDirection::Sent);
    
    manager->addGroupMessage(groupId, msg1);
    manager->addGroupMessage(groupId, msg2);
    
    QList<Message> groupMessages = manager->getGroupMessages(groupId);
    
    EXPECT_EQ(groupMessages.size(), 2);
}

TEST_F(MessageManagerTest, ClearGroupMessages) {
    QString groupId = "group1";
    manager->addGroupMessage(groupId, Message("COM1", "Test", MessageDirection::Received));
    
    manager->clearGroupMessages(groupId);
    
    QList<Message> groupMessages = manager->getGroupMessages(groupId);
    EXPECT_TRUE(groupMessages.isEmpty());
}

TEST_F(MessageManagerTest, MessageAddedSignal) {
    bool signalReceived = false;
    QString receivedPort;
    
    QObject::connect(manager, &MessageManager::messageAdded,
                     [&](const QString& portName, const Message& message) {
        signalReceived = true;
        receivedPort = portName;
        Q_UNUSED(message)
    });
    
    manager->addMessage("COM5", "Test", MessageDirection::Received);
    
    EXPECT_TRUE(signalReceived);
    EXPECT_EQ(receivedPort, "COM5");
}
