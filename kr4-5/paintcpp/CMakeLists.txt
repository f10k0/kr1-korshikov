cmake_minimum_required(VERSION 3.14)

project(AsciiPaint VERSION 1.0.0 DESCRIPTION "Console Paint for ASCII art")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Библиотека с классами (без main.cpp — переиспользуется в тестах)
add_library(paint_lib
    src/Canvas.cpp
    src/PaintContext.cpp
)
target_include_directories(paint_lib PUBLIC src)

if(WIN32)
    target_link_libraries(paint_lib)
endif()

# Основной исполняемый файл
# На Windows: conio.h + Windows API
# На Linux:   ncurses
add_executable(ascii_paint src/main.cpp)

if(WIN32)
    target_link_libraries(ascii_paint paint_lib)
else()
    find_package(Curses REQUIRED)
    target_include_directories(ascii_paint PRIVATE ${CURSES_INCLUDE_DIR})
    target_link_libraries(ascii_paint paint_lib ${CURSES_LIBRARIES})
endif()

# Тесты
enable_testing()
add_subdirectory(tests)
