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

set(SRC 
    #.ui
    src/login_register/login.ui
    src/login_register/registerwin.ui
    src/mainwindow.ui
    src/meet_room/roomdialog.ui
    src/mychatroom.ui
    src/myqq.ui
    src/videoitem.ui
)
aux_source_directory(src SRC)
aux_source_directory(src/audio SRC)
aux_source_directory(src/audio_codec SRC)
aux_source_directory(src/common SRC)
aux_source_directory(src/login_register SRC)
aux_source_directory(src/meet_room SRC)
aux_source_directory(src/net_api SRC)

set(QRC resource.qrc)
add_executable(${PROJECT_NAME} ${SRC} ${QRC})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::Multimedia
    ${CONAN_LIBS}
    ws2_32
)#target_link_libraries

if (EXAMPLE)
    aux_source_directory(src/audio ESRC)
    aux_source_directory(src/audio_codec ESRC)
    aux_source_directory(src/example ESRC)
    add_executable(example ${ESRC} ${QRC} src/Config.h src/example/mainwindow.ui)
    target_link_libraries(example PRIVATE
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui
        Qt5::Network
        Qt5::Multimedia
        ${CONAN_LIBS}
        ws2_32
    )#target_link_libraries
endif()
