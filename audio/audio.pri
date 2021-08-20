
QT += multimedia

HEADERS += \
    $$PWD/AbstractAudioFrameProvider.h \
    $$PWD/audiocollector.h \
    $$PWD/audiodevices.h \
    $$PWD/audiolevel.h \
    $$PWD/audioplayer.h \
    $$PWD/audiosynthesizer.h \

SOURCES += \
    $$PWD/audiocollector.cpp \
    $$PWD/audiolevel.cpp \
    $$PWD/audioplayer.cpp \
    $$PWD/audiosynthesizer.cpp \
    $$PWD/audiodevices.cpp


FORMS += \
    $$PWD/audiodevicesbase.ui
