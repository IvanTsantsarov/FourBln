cmake_minimum_required(VERSION 3.19)
project(FourBln LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(FourBln
    main.cpp
    file.h file.cpp
    includes.h
    includes.cpp
)

target_link_libraries(FourBln
    PRIVATE
        Qt::Core
)

install(TARGETS FourBln
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
