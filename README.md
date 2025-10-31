# Serial-Chat
**Serial Chat**: Reimagining serial port debugging as an intuitive chat conversation. Visualize data flows between devices and applications in a familiar, collaborative interface.

## Features
- Qt5-based C++ application (compatible with Qt 5.12+)
- Clean MVC architecture with separation between models and views
- Real-time serial port communication
- Chat-like interface for visualizing incoming/outgoing data
- Support for multiple baud rates and serial port configurations

## Architecture

The project follows a clean Model-View separation:

### Models (`src/models/`)
- **SerialPort**: Wrapper around QSerialPort for managing serial port connections
  - Signals: `dataReceived()`, `errorOccurred()`, `connectionStatusChanged()`
  - Methods: `openPort()`, `closePort()`, `writeData()`, `readData()`
  
- **Message**: Represents a single message with content, direction (incoming/outgoing), and timestamp
  - Properties: `content`, `direction`, `timestamp`
  - Signals: `contentChanged()`, `directionChanged()`
  
- **Session**: Manages a collection of messages
  - Methods: `addMessage()`, `clearMessages()`, `messages()`
  - Signals: `messageAdded()`, `messagesCleared()`

### Views (`src/views/`)
- **MainWindow**: Main application window with sidebar and chat area
  - Sidebar: Port selection, baud rate configuration, connection controls
  - Chat Area: QScrollArea displaying message bubbles
  - Input: Text field and send button for outgoing messages
  
- **MessageBubbleWidget**: Visual representation of a message as a chat bubble
  - Different styling for incoming (white) vs outgoing (green) messages
  - Displays message content and timestamp

## Building the Project

### Prerequisites
- CMake 3.5 or higher
- Qt5 5.12 or higher (Core, Gui, Widgets, SerialPort modules)
- C++11 compatible compiler

### Linux (Ubuntu/Debian)
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y qtbase5-dev libqt5serialport5-dev cmake build-essential

# Build
mkdir build
cd build
cmake ..
cmake --build .

# Run
./SerialChat
```

### Future Qt6 Compatibility
The code is structured to be portable to Qt6 with minimal changes:
- Uses `find_package(Qt5 ...)` which can be changed to `find_package(Qt6 ...)`
- Uses Qt5 signal/slot syntax compatible with Qt6
- No deprecated Qt5 APIs used
- Clean separation of concerns makes migration easier

## Usage
1. Launch the application
2. Select a serial port from the dropdown (click "Refresh Ports" to update the list)
3. Choose the appropriate baud rate
4. Click "Connect" to establish the connection
5. Type messages in the input field and press "Send" or Enter
6. Incoming data appears as white bubbles on the left
7. Outgoing data appears as green bubbles on the right

## License
See LICENSE file for details.
