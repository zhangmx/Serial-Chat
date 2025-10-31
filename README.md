# Serial-Chat
**Serial Chat**: Reimagining serial port debugging as an intuitive chat conversation. Visualize data flows between devices and applications in a familiar, collaborative interface.

## Project Status
This is the initial MVP boilerplate - a clean, well-architected Qt Widgets application skeleton that serves as the foundation for the Serial Chat vision.

## Features
- **Chat-style interface** inspired by modern messaging apps (WeChat/Telegram)
- **Model-View architecture** with clear separation of concerns
- **Data models** for SerialPort, Message, and Session management
- **Custom message bubble widgets** with direction-based styling
- **Session management** with sidebar and message history
- **Serial port support** using Qt SerialPort module

## Architecture

### Data Models (Model Layer)
- **SerialPort**: Represents a physical/virtual serial port with properties like `portName`, `baudRate`, and signals like `dataReceived(QByteArray)`
- **Message**: Represents a single communication unit with `content`, `timestamp`, `direction` (Incoming/Outgoing/System), and `sourcePort`
- **Session**: Manages collections of SerialPort objects and message logs

### UI Components (View Layer)
- **MainWindow**: Main window with left sidebar for session list, central scrollable message area, and optional right dock for port status
- **MessageBubbleWidget**: Custom widget that renders chat bubbles with different styles based on message direction

## Building

### Prerequisites
- Qt 5.x (Core, GUI, Widgets, SerialPort modules)
- C++17 compatible compiler
- qmake

### Ubuntu/Debian
```bash
sudo apt-get install qtbase5-dev qt5-qmake libqt5serialport5-dev
```

### Build Instructions
```bash
qmake SerialChat.pro
make
```

### Run
```bash
./SerialChat
```

## Project Structure
```
Serial-Chat/
├── SerialChat.pro          # Qt project file
├── src/
│   ├── main.cpp            # Application entry point
│   ├── mainwindow.h/cpp    # Main window with chat interface
│   ├── messagebubblewidget.h/cpp  # Message bubble rendering
│   └── models/
│       ├── message.h/cpp   # Message data model
│       ├── serialport.h/cpp # Serial port wrapper
│       └── session.h/cpp   # Session management
```

## Future Development
This boilerplate provides the foundation for:
- Actual serial port connection and communication
- Multiple session support
- Message filtering and search
- Hex/text view switching
- Data export and logging
- Port configuration UI
- And more...

## License
See LICENSE file for details.
