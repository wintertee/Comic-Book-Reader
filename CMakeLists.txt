cmake_minimum_required(VERSION 3.5)

project(CBR VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.h src/mainwindow.cpp
        src/comicbook.h src/comicbook.cpp
        src/cbfile.h src/cbfile.cpp
        src/smartimage.h src/smartimage.cpp
)

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

add_subdirectory(external/bit7z)

include_directories(external/bit7z/include)
include_directories(external/ImageMagick/include)
find_library(BIT7Z bit7z.lib HINTS external/bit7z)
find_library(BIT7Z_DLIB bit7z_d.lib  HINTS external/bit7z/bin/)
find_library(Magick++ CORE_RL_Magick++_.lib HINTS external/ImageMagick/lib)
find_library(MagickCore CORE_RL_MagickCore_.lib HINTS external/ImageMagick/lib)
find_library(MagickWand CORE_RL_MagickWand_.lib HINTS external/ImageMagick/lib)

target_link_libraries(CBR PRIVATE debug ${BIT7Z_DLIB} ${Magick++} ${MagickCore} ${MagickWand})
# target_link_libraries(CBR PRIVATE optimized ${BIT7Z})
target_link_libraries(CBR PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CBR)
endif()
