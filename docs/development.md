# Serial Chat 开发文档

## 项目概述

Serial Chat 是一个基于 Qt 的串口通信工具，采用类似社交软件的界面设计。每个串口被视为一个"用户"，可以发送和接收消息，就像在聊天应用中一样。

## 技术栈

- **编程语言**: C++17
- **GUI 框架**: Qt 5.12 / Qt 5.15
- **构建系统**: CMake 3.14+
- **单元测试**: Google Test (GTest)

## 项目结构

```
Serial-Chat/
├── CMakeLists.txt              # 主 CMake 配置文件
├── README.md                   # 英文说明文档
├── README_CN.md                # 中文说明文档
├── src/
│   ├── main.cpp                # 程序入口
│   ├── core/                   # 核心业务逻辑
│   │   ├── SerialPortManager.h/cpp    # 串口管理器
│   │   ├── SerialPortUser.h/cpp       # 串口用户封装
│   │   ├── ChatGroup.h/cpp            # 聊天组管理
│   │   ├── MessageManager.h/cpp       # 消息管理器
│   │   └── DataPersistence.h/cpp      # 数据持久化
│   ├── models/                 # 数据模型
│   │   ├── Message.h/cpp              # 消息模型
│   │   ├── SerialPortInfo.h/cpp       # 串口信息模型
│   │   └── ChatGroupInfo.h/cpp        # 聊天组信息模型
│   ├── ui/                     # 用户界面
│   │   ├── MainWindow.h/cpp           # 主窗口
│   │   ├── FriendListWidget.h/cpp     # 好友列表组件
│   │   ├── FriendListItem.h/cpp       # 好友列表项
│   │   ├── ChatWidget.h/cpp           # 聊天窗口组件
│   │   ├── ChatBubble.h/cpp           # 聊天气泡
│   │   ├── ChatGroupDialog.h/cpp      # 创建/编辑群组对话框
│   │   ├── SerialPortSettingsDialog.h/cpp  # 串口设置对话框
│   │   └── SerialPortRemarkDialog.h/cpp    # 串口备注对话框
│   └── utils/                  # 工具类
│       ├── HexUtils.h/cpp             # 十六进制转换工具
│       └── TimeUtils.h/cpp            # 时间格式化工具
├── tests/                      # 单元测试
│   ├── main_test.cpp                  # 测试入口
│   ├── TestMessage.cpp                # 消息测试
│   ├── TestSerialPortInfo.cpp         # 串口信息测试
│   ├── TestChatGroup.cpp              # 聊天组测试
│   ├── TestHexUtils.cpp               # 十六进制工具测试
│   └── TestMessageManager.cpp         # 消息管理器测试
├── resources/                  # 资源文件
│   ├── resources.qrc                  # Qt 资源文件
│   └── icons/                         # 图标资源
└── docs/                       # 文档
    ├── readme.md                      # 文档索引
    ├── development.md                 # 开发文档
    └── features.md                    # 功能文档
```

## 架构设计

### 核心类

#### SerialPortManager
串口管理器，负责管理所有串口用户实例。

主要职责：
- 发现可用串口
- 创建和管理 `SerialPortUser` 实例
- 维护"好友列表"（已使用的串口）
- 跟踪串口在线/离线状态

#### SerialPortUser
串口用户封装类，将 `QSerialPort` 包装成类似聊天用户的接口。

主要职责：
- 封装 `QSerialPort` 功能
- 管理连接状态
- 发送和接收数据
- 发出消息信号

#### ChatGroup
聊天组管理类，允许多个串口之间互通消息。

主要职责：
- 管理组成员
- 消息转发
- 组消息历史

#### MessageManager
消息管理器，管理所有串口会话的消息历史。

主要职责：
- 存储消息历史
- 提供消息查询接口
- 管理组消息

#### DataPersistence
数据持久化类，负责保存和加载应用数据。

主要职责：
- 保存/加载好友列表
- 保存/加载聊天组
- 保存/加载消息历史

### 数据模型

#### Message
消息模型，表示聊天中的一条消息。

属性：
- `id`: 消息唯一标识
- `portName`: 串口名称
- `data`: 消息数据
- `direction`: 消息方向（发送/接收）
- `timestamp`: 时间戳

#### SerialPortInfo
串口信息模型，包含串口的配置和状态信息。

属性：
- `portName`: 串口名称
- `remark`: 备注名称
- `baudRate`: 波特率
- `dataBits`: 数据位
- `stopBits`: 停止位
- `parity`: 校验位
- `flowControl`: 流控制
- `status`: 连接状态

#### ChatGroupInfo
聊天组信息模型。

属性：
- `id`: 组唯一标识
- `name`: 组名称
- `description`: 组描述
- `members`: 成员列表
- `forwardingEnabled`: 是否启用消息转发

### UI 组件

#### MainWindow
主窗口，包含菜单栏、状态栏和主界面布局。

#### FriendListWidget
好友列表组件，显示所有串口"好友"和聊天组。

功能：
- 在线/离线串口分组显示
- 搜索功能
- 添加串口/创建群组按钮

#### ChatWidget
聊天窗口组件，显示消息并提供发送功能。

功能：
- 消息气泡显示
- 文本/十六进制格式切换
- 消息发送输入框

#### ChatBubble
聊天气泡组件，显示单条消息。

功能：
- 发送/接收消息不同样式
- 右键菜单（复制、格式切换）
- 时间戳显示

## 开发指南

### 环境准备

1. 安装 Qt 5.12 或 5.15
2. 安装 CMake 3.14+
3. 安装 C++17 兼容的编译器

### 构建项目

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt

# 构建
cmake --build .

# 运行测试
ctest --output-on-failure
```

### 代码规范

1. **文件命名**: 使用驼峰命名法，文件名与类名一致
2. **类命名**: 使用大驼峰命名法（PascalCase）
3. **方法命名**: 使用小驼峰命名法（camelCase）
4. **成员变量**: 使用 `m_` 前缀
5. **常量**: 使用全大写下划线分隔

### 添加新功能

1. 在相应目录创建头文件和源文件
2. 更新 `CMakeLists.txt` 添加新文件
3. 编写单元测试
4. 更新文档

## 信号与槽连接

### SerialPortManager 信号

| 信号 | 参数 | 说明 |
|------|------|------|
| `availablePortsChanged` | `QStringList ports` | 可用串口列表变化 |
| `userCreated` | `QString portName` | 新用户创建 |
| `userRemoved` | `QString portName` | 用户移除 |
| `userStatusChanged` | `QString portName, PortStatus status` | 用户状态变化 |
| `userMessageReceived` | `QString portName, Message message` | 收到用户消息 |
| `friendListChanged` | - | 好友列表变化 |

### SerialPortUser 信号

| 信号 | 参数 | 说明 |
|------|------|------|
| `connected` | - | 连接成功 |
| `disconnected` | - | 断开连接 |
| `dataReceived` | `QByteArray data` | 收到数据 |
| `messageSent` | `Message message` | 消息发送 |
| `messageReceived` | `Message message` | 消息接收 |
| `statusChanged` | `PortStatus status` | 状态变化 |
| `errorOccurred` | `QString error` | 发生错误 |

## 数据存储

应用数据存储在用户数据目录：
- Windows: `%APPDATA%/SerialChat/`
- macOS: `~/Library/Application Support/SerialChat/`
- Linux: `~/.local/share/SerialChat/`

存储格式：
- `friends.json`: 好友列表
- `groups.json`: 聊天组列表
- `messages/`: 消息历史目录
- `group_messages/`: 组消息历史目录

## 测试

### 运行所有测试

```bash
cd build
ctest --output-on-failure
```

### 运行特定测试

```bash
./SerialChat_tests --gtest_filter=MessageTest.*
```

### 测试覆盖

- `TestMessage`: 消息模型测试
- `TestSerialPortInfo`: 串口信息模型测试
- `TestChatGroup`: 聊天组信息测试
- `TestHexUtils`: 十六进制工具测试
- `TestMessageManager`: 消息管理器测试
