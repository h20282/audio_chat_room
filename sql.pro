TEMPLATE = app
TARGET = audio-chat-room

QT       += core gui network

RC_ICONS = ./images/message.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 resources_big

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
LIBS += -lWs2_32


# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../shared/shared.pri)
include($$PWD/login_register/login_register.pri)
include($$PWD/meet_room/meet_room.pri)
include($$PWD/common/common.pri)
include($$PWD/net_api/net_api.pri)
include($$PWD/audio/audio.pri)
include($$PWD/audio_codec/audio_codec.pri)
include($$PWD/structs/structs.pri)


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mychatroom.cpp \

HEADERS += \
    mainwindow.h \
    mychatroom.h \
    useritem.h \

FORMS += \
    mainwindow.ui \
    mychatroom.ui \

#new:

win32:CONFIG(release, debug|release): LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lavcodec
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lavcodec
else:unix: LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lavcodec

INCLUDEPATH += D:/ffmpeg/ffmpeg-4.0.2-win64-dev/include
DEPENDPATH += D:/ffmpeg/ffmpeg-4.0.2-win64-dev/include

win32:CONFIG(release, debug|release): LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lswresample
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lswresample
else:unix: LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lswresample

INCLUDEPATH += D:/ffmpeg/ffmpeg-4.0.2-win64-dev/include
DEPENDPATH += D:/ffmpeg/ffmpeg-4.0.2-win64-dev/include

win32:CONFIG(release, debug|release): LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lavutil
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lavutil
else:unix: LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lavutil

INCLUDEPATH += D:/ffmpeg/ffmpeg-4.0.2-win64-dev/include
DEPENDPATH += D:/ffmpeg/ffmpeg-4.0.2-win64-dev/include

win32:CONFIG(release, debug|release): LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lavformat
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lavformat
else:unix: LIBS += -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib/ -lavformat

INCLUDEPATH += D:/ffmpeg/ffmpeg-4.0.2-win64-dev/include
DEPENDPATH += D:/ffmpeg/ffmpeg-4.0.2-win64-dev/include



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

target.path = $$PWD/audio

RESOURCES += \
    resource.qrc


