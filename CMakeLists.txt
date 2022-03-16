cmake_minimum_required(VERSION 3.5)

project(cmakePro LANGUAGES CXX)

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
conan_basic_setup()

set(SRC 
    #.cpp
    audio/AudioChat.cpp
    audio/audiocollector.cpp
    audio/audiolevel.cpp
    audio/audioplayer.cpp
    audio/audiosynthesizer.cpp
    audio/UdpConnector.cpp

    audio_codec/Decoder.cpp
    audio_codec/Encoder.cpp

    common/customwidget.cpp
    common/IMToolBox.cpp

    login_register/login.cpp
    login_register/registerwin.cpp

    meet_room/roomdialog.cpp
    meet_room/useritem.cpp
    meet_room/userlist.cpp
    meet_room/userwidget.cpp

    net_api/qmytcp_client.cpp
    net_api/tcp_net.cpp

    main.cpp
    mainwindow.cpp
    mychatroom.cpp
    myqq.cpp
    videoitem.cpp

    #.ui
    login_register/login.ui
    login_register/registerwin.ui
    mainwindow.ui
    meet_room/roomdialog.ui
    mychatroom.ui
    myqq.ui
    videoitem.ui
)
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
    avcodec
    swresample
    avutil
    avformat
)#target_link_libraries
