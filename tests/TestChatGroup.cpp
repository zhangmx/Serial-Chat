#include <gtest/gtest.h>
#include "ChatGroupInfo.h"

class ChatGroupInfoTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(ChatGroupInfoTest, DefaultConstructor) {
    ChatGroupInfo group;
    
    EXPECT_FALSE(group.id().isEmpty());
    EXPECT_TRUE(group.name().isEmpty());
    EXPECT_TRUE(group.description().isEmpty());
    EXPECT_TRUE(group.members().isEmpty());
    EXPECT_EQ(group.memberCount(), 0);
    EXPECT_TRUE(group.isForwardingEnabled());
    EXPECT_TRUE(group.createdTime().isValid());
}

TEST_F(ChatGroupInfoTest, NameConstructor) {
    ChatGroupInfo group("Test Group");
    
    EXPECT_FALSE(group.id().isEmpty());
    EXPECT_EQ(group.name(), "Test Group");
    EXPECT_TRUE(group.isForwardingEnabled());
}

TEST_F(ChatGroupInfoTest, AddMember) {
    ChatGroupInfo group("Test");
    
    group.addMember("COM1");
    EXPECT_EQ(group.memberCount(), 1);
    EXPECT_TRUE(group.hasMember("COM1"));
    
    group.addMember("COM2");
    EXPECT_EQ(group.memberCount(), 2);
    EXPECT_TRUE(group.hasMember("COM2"));
    
    // Adding same member again should not increase count
    group.addMember("COM1");
    EXPECT_EQ(group.memberCount(), 2);
}

TEST_F(ChatGroupInfoTest, RemoveMember) {
    ChatGroupInfo group("Test");
    group.addMember("COM1");
    group.addMember("COM2");
    group.addMember("COM3");
    
    EXPECT_EQ(group.memberCount(), 3);
    
    group.removeMember("COM2");
    EXPECT_EQ(group.memberCount(), 2);
    EXPECT_FALSE(group.hasMember("COM2"));
    EXPECT_TRUE(group.hasMember("COM1"));
    EXPECT_TRUE(group.hasMember("COM3"));
}

TEST_F(ChatGroupInfoTest, ClearMembers) {
    ChatGroupInfo group("Test");
    group.addMember("COM1");
    group.addMember("COM2");
    
    group.clearMembers();
    
    EXPECT_EQ(group.memberCount(), 0);
    EXPECT_FALSE(group.hasMember("COM1"));
    EXPECT_FALSE(group.hasMember("COM2"));
}

TEST_F(ChatGroupInfoTest, SetMembers) {
    ChatGroupInfo group("Test");
    group.addMember("COM1");
    
    QStringList newMembers = {"COM3", "COM4", "COM5"};
    group.setMembers(newMembers);
    
    EXPECT_EQ(group.memberCount(), 3);
    EXPECT_FALSE(group.hasMember("COM1"));
    EXPECT_TRUE(group.hasMember("COM3"));
    EXPECT_TRUE(group.hasMember("COM4"));
    EXPECT_TRUE(group.hasMember("COM5"));
}

TEST_F(ChatGroupInfoTest, Setters) {
    ChatGroupInfo group;
    
    group.setName("My Group");
    EXPECT_EQ(group.name(), "My Group");
    
    group.setDescription("A test group");
    EXPECT_EQ(group.description(), "A test group");
    
    group.setForwardingEnabled(false);
    EXPECT_FALSE(group.isForwardingEnabled());
    
    group.setForwardingEnabled(true);
    EXPECT_TRUE(group.isForwardingEnabled());
}

TEST_F(ChatGroupInfoTest, JsonSerialization) {
    ChatGroupInfo original("Test Group");
    original.setDescription("Test Description");
    original.setForwardingEnabled(false);
    original.addMember("COM1");
    original.addMember("COM2");
    original.addMember("COM3");
    
    QJsonObject json = original.toJson();
    ChatGroupInfo restored = ChatGroupInfo::fromJson(json);
    
    EXPECT_EQ(restored.id(), original.id());
    EXPECT_EQ(restored.name(), original.name());
    EXPECT_EQ(restored.description(), original.description());
    EXPECT_EQ(restored.isForwardingEnabled(), original.isForwardingEnabled());
    EXPECT_EQ(restored.memberCount(), original.memberCount());
    EXPECT_EQ(restored.members(), original.members());
}

TEST_F(ChatGroupInfoTest, EqualityOperator) {
    ChatGroupInfo group1("Group 1");
    ChatGroupInfo group2("Group 2");
    
    // Different groups have different IDs
    EXPECT_NE(group1, group2);
    
    // Groups with same ID should be equal
    ChatGroupInfo group3 = group1;
    // Note: This copies the ID as well
    EXPECT_EQ(group1.id(), group3.id());
}

TEST_F(ChatGroupInfoTest, HasMember_EmptyGroup) {
    ChatGroupInfo group("Empty");
    
    EXPECT_FALSE(group.hasMember("COM1"));
    EXPECT_FALSE(group.hasMember(""));
}
