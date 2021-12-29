cmake_minimum_required(VERSION 3.5)

project(CBR VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        src/main.cpp

        src/mainwindow.cpp
        src/mainwindow.h
        #src/mainwindow.ui

        src/comicbook.cpp
        src/comicbook.h
)

#file(GLOB PROJECT_SOURCES "src/*.cpp")
#file(GLOB PROJECT_SOURCES "src/*.h")
#file(GLOB PROJECT_SOURCES "src/*.ui")

message(STATUS ${GLOB})

set(PROJECT_HEAD)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CBR
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CBR APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(CBR SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CBR
            ${PROJECT_SOURCES}
        )
    endif()
endif()

include_directories(external/bit7z/include)
find_library(BIT7Z bit7z64.lib HINTS external/bit7z/lib)
find_library(BIT7Z_D bit7z64_d.lib  HINTS external/bit7z/lib)

# message(STATUS ${PROJECT_SOURCES})

target_link_libraries(CBR PRIVATE debug ${BIT7Z_D})
# target_link_libraries(CBR PRIVATE optimized ${BIT7Z})
target_link_libraries(CBR PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CBR)
endif()
