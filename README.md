# Serial Chat

<p align="center">
  <strong>A Serial Port Communication Tool with Chat-like Interface</strong>
</p>

<p align="center">
  <a href="README_CN.md">中文文档</a>
</p>

## Overview

Serial Chat is an innovative serial port communication tool that transforms the traditional serial debugging experience into a chat-like interface. Each serial port is treated as a "user" that can send and receive messages, just like in a messaging application.

## Features

### 🔌 Serial Port as Users
- Each serial port is represented as a "friend" in your friend list
- Online status indicates connection state
- Preserve offline ports with custom remarks

### 💬 Chat-like Interface
- Messages displayed as chat bubbles
- Sent messages on the right (green)
- Received messages on the left (white)
- Timestamp and port name for each message

### 👥 Chat Groups
- Create custom groups with multiple serial ports
- Message forwarding between group members
- Monitor communication between different devices

### 📝 Additional Features
- Text and Hexadecimal display modes
- Custom remarks for serial ports
- Persistent friend list and message history
- Search functionality
- Data export

## Screenshots

```
+------------------------------------------+
|  Menu Bar                                 |
+------------------------------------------+
|          |                               |
|  Friend  |     Chat Area                 |
|  List    |  +-------------------------+  |
|          |  |  Chat Bubbles           |  |
|  Online  |  |  ...                    |  |
|  Offline |  +-------------------------+  |
|  Groups  |  |  Input Area             |  |
|          |  +-------------------------+  |
+------------------------------------------+
|  Status Bar                              |
+------------------------------------------+
```

## Requirements

- Qt 5.12 or Qt 5.15
- CMake 3.14+
- C++17 compatible compiler
- Google Test (for unit tests)

## Building

### Clone the Repository

```bash
git clone https://github.com/yourusername/Serial-Chat.git
cd Serial-Chat
```

### Build with CMake

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt

# Build
cmake --build .

# Run
./SerialChat
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | ON | Build unit tests |

### Running Tests

```bash
cd build
ctest --output-on-failure

# Or run directly
./SerialChat_tests
```

## Project Structure

```
Serial-Chat/
├── CMakeLists.txt          # CMake configuration
├── README.md               # This file
├── README_CN.md            # Chinese documentation
├── src/
│   ├── main.cpp            # Entry point
│   ├── core/               # Core business logic
│   ├── models/             # Data models
│   ├── ui/                 # User interface components
│   └── utils/              # Utility classes
├── tests/                  # Unit tests
├── resources/              # Resource files
└── docs/                   # Documentation
```

## Usage

### Adding a Serial Port

1. Click "Add Port" button
2. Select the port and configure settings
3. Click OK to add to friend list

### Connecting

1. Click on a port in the friend list
2. The port will automatically connect
3. Green indicator shows online status

### Sending Messages

1. Select a port from the friend list
2. Type your message in the input area
3. Click "Send" or press Enter
4. Toggle "Hex Mode" to send hexadecimal data

### Creating a Chat Group

1. Click "Group" button
2. Enter group name and description
3. Select at least 2 members
4. Enable "Message Forwarding" to relay messages between ports
5. Click "Create"

### Setting Port Remarks

1. Right-click on a port in the friend list
2. Select "Set Remark"
3. Enter a custom name for the port

## Configuration

### Serial Port Settings

| Setting | Options | Default |
|---------|---------|---------|
| Baud Rate | 1200 - 921600 | 115200 |
| Data Bits | 5, 6, 7, 8 | 8 |
| Stop Bits | 1, 1.5, 2 | 1 |
| Parity | None, Even, Odd, Mark, Space | None |
| Flow Control | None, Hardware, Software | None |

### Data Storage

Application data is stored in:
- **Windows**: `%APPDATA%/SerialChat/`
- **macOS**: `~/Library/Application Support/SerialChat/`
- **Linux**: `~/.local/share/SerialChat/`

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| F5 | Refresh port list |
| Ctrl+N | Add new port |
| Ctrl+Q | Quit |
| Enter | Send message |

## Development

See [Development Documentation](docs/development.md) for:
- Architecture overview
- Code conventions
- API documentation
- Testing guidelines

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Qt Framework for the excellent cross-platform GUI toolkit
- Google Test for the testing framework
