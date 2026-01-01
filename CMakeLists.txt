cmake_minimum_required(VERSION 3.14)
project(SerialChat VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)

# Qt version support (5.12 and 5.15)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Widgets SerialPort Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets SerialPort Network)

message(STATUS "Qt version: ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")

# Source files
set(CORE_SOURCES
    src/core/SerialPortManager.cpp
    src/core/SerialPortUser.cpp
    src/core/ChatGroup.cpp
    src/core/MessageManager.cpp
    src/core/DataPersistence.cpp
)

set(CORE_HEADERS
    src/core/SerialPortManager.h
    src/core/SerialPortUser.h
    src/core/ChatGroup.h
    src/core/MessageManager.h
    src/core/DataPersistence.h
)

set(MODEL_SOURCES
    src/models/Message.cpp
    src/models/SerialPortInfo.cpp
    src/models/ChatGroupInfo.cpp
)

set(MODEL_HEADERS
    src/models/Message.h
    src/models/SerialPortInfo.h
    src/models/ChatGroupInfo.h
)

set(UI_SOURCES
    src/ui/MainWindow.cpp
    src/ui/FriendListWidget.cpp
    src/ui/FriendListItem.cpp
    src/ui/ChatWidget.cpp
    src/ui/ChatBubble.cpp
    src/ui/ChatGroupDialog.cpp
    src/ui/SerialPortSettingsDialog.cpp
    src/ui/SerialPortRemarkDialog.cpp
)

set(UI_HEADERS
    src/ui/MainWindow.h
    src/ui/FriendListWidget.h
    src/ui/FriendListItem.h
    src/ui/ChatWidget.h
    src/ui/ChatBubble.h
    src/ui/ChatGroupDialog.h
    src/ui/SerialPortSettingsDialog.h
    src/ui/SerialPortRemarkDialog.h
)

set(UTIL_SOURCES
    src/utils/HexUtils.cpp
    src/utils/TimeUtils.cpp
)

set(UTIL_HEADERS
    src/utils/HexUtils.h
    src/utils/TimeUtils.h
)

# Resource files
set(RESOURCES
    resources/resources.qrc
)

# Main application
add_executable(${PROJECT_NAME}
    src/main.cpp
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${MODEL_SOURCES}
    ${MODEL_HEADERS}
    ${UI_SOURCES}
    ${UI_HEADERS}
    ${UTIL_SOURCES}
    ${UTIL_HEADERS}
    ${RESOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::Network
)

# Unit tests with GTest
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest QUIET)
    
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    set(TEST_SOURCES
        tests/TestMessage.cpp
        tests/TestSerialPortInfo.cpp
        tests/TestChatGroup.cpp
        tests/TestHexUtils.cpp
        tests/TestMessageManager.cpp
        tests/main_test.cpp
    )

    add_executable(${PROJECT_NAME}_tests
        ${TEST_SOURCES}
        ${CORE_SOURCES}
        ${MODEL_SOURCES}
        ${UTIL_SOURCES}
    )

    target_include_directories(${PROJECT_NAME}_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/models
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    )

    target_link_libraries(${PROJECT_NAME}_tests PRIVATE
        GTest::gtest
        GTest::gtest_main
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::SerialPort
        Qt${QT_VERSION_MAJOR}::Network
    )

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_tests)
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
