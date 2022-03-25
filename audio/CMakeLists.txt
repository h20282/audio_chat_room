if(BUILD_AUDIO_TEST)
    add_executable(test test/main.cpp ${SRC} ${QRC})
endif()
