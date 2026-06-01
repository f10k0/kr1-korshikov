cmake_minimum_required(VERSION 3.10)

project(AsciiPaint VERSION 1.0.0 DESCRIPTION "Console Paint for ASCII art")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/main.cpp
    src/Canvas.cpp
    src/PaintContext.cpp
)

add_executable(ascii_paint ${SOURCES})

target_include_directories(ascii_paint PRIVATE src)

if(WIN32)
    # Убираем проблемные флаги
    set_target_properties(ascii_paint PROPERTIES LINK_FLAGS "")
    # Подключаем необходимые библиотеки
    target_link_libraries(ascii_paint)
endif()