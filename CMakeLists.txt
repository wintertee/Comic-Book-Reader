cmake_minimum_required(VERSION 3.15)

project(CBR VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
        src/main.cpp
        include/mainwindow.h src/mainwindow.cpp
        include/comicbook.h src/comicbook.cpp
        include/cbfile.h src/cbfile.cpp
        include/smartimage.h src/smartimage.cpp
        include/smartlabel.h src/smartlabel.cpp
        include/extractdialog.h src/extractdialog.cpp
)

set(PROJECT_HEAD)

include_directories(3rdparty/bit7z/include)
include_directories(3rdparty/ImageMagick/include)
include_directories(include)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Gui Widgets REQUIRED)
find_package(Qt6 COMPONENTS Widgets Concurrent REQUIRED)

find_library(BIT7Z bit7z64.lib HINTS 3rdparty/bit7z/lib)
find_library(Magick++ CORE_RL_Magick++_.lib HINTS 3rdparty/ImageMagick/lib)
find_library(MagickCore CORE_RL_MagickCore_.lib HINTS 3rdparty/ImageMagick/lib)
find_library(MagickWand CORE_RL_MagickWand_.lib HINTS 3rdparty/ImageMagick/lib)

qt_add_executable(CBR MANUAL_FINALIZATION ${PROJECT_SOURCES} )
target_link_libraries(CBR PRIVATE optimized Qt6::Widgets Qt6::Concurrent ${BIT7Z} ${Magick++} ${MagickCore} ${MagickWand})

configure_file(3rdparty/7zip/7z.dll 7z.dll COPYONLY)

qt_finalize_executable(CBR)

