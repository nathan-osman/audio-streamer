set(SRC
    client.h
    client.cpp
    log.h
    main.cpp
    mainwindow.h
    mainwindow.cpp
    recorder.h
    recorder.cpp
    resource.qrc
    protocol.h
    protocol.cpp
)

add_executable(audio-streamer WIN32 ${SRC})

set_target_properties(audio-streamer PROPERTIES
    CXX_STANDARD          14
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(audio-streamer PRIVATE)
target_link_libraries(audio-streamer Qt5::Multimedia Qt5::Network Qt5::Widgets)

install(TARGETS audio-streamer RUNTIME DESTINATION bin)

set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
include(InstallRequiredSystemLibraries)
