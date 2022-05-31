cmake_minimum_required(VERSION 3.5)

project(audio_chat_room LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#utf-8
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /utf-8")
endif()

include(${CMAKE_SOURCE_DIR}/build/conanbuildinfo.cmake)
conan_basic_setup()

if(WIN32)
    set(CMAKE_PREFIX_PATH $ENV{QT_HOME})
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Multimedia REQUIRED)

include(${CMAKE_SOURCE_DIR}/build/conanbuildinfo.cmake)
include_directories(src)
conan_basic_setup()

aux_source_directory(src/audio SRC)
aux_source_directory(src/audio_codec SRC)
aux_source_directory(src/example SRC)
add_executable(example ${SRC} src/Config.h)
target_link_libraries(example PRIVATE
    Qt5::Widgets
    Qt5::Core
    Qt5::Network
    Qt5::Multimedia
    ${CONAN_LIBS}
    ws2_32
)#target_link_libraries
