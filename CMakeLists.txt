cmake_minimum_required(VERSION 3.5)

project(SerialChat VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically handle Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt5 packages (minimum version 5.12)
find_package(Qt5 5.12 REQUIRED COMPONENTS Core Gui Widgets SerialPort)

# Source files
set(SOURCES
    src/main.cpp
    src/models/SerialPort.cpp
    src/models/Message.cpp
    src/models/Session.cpp
    src/views/MainWindow.cpp
    src/views/MessageBubbleWidget.cpp
)

# Header files
set(HEADERS
    src/models/SerialPort.h
    src/models/Message.h
    src/models/Session.h
    src/views/MainWindow.h
    src/views/MessageBubbleWidget.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt5 libraries
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::SerialPort
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/views
)

# Platform-specific settings for Qt6 compatibility
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
