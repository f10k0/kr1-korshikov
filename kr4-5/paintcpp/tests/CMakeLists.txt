cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Для Windows: не переопределять настройки CRT
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Unit-тесты Canvas
add_executable(test_canvas test_canvas.cpp)
target_link_libraries(test_canvas paint_lib GTest::gtest_main)
add_test(NAME CanvasTests COMMAND test_canvas)

# Unit-тесты PaintContext
add_executable(test_paint_context test_paint_context.cpp)
target_link_libraries(test_paint_context paint_lib GTest::gtest_main)
add_test(NAME PaintContextTests COMMAND test_paint_context)

# Тестовые сценарии
add_executable(test_scenarios test_scenarios.cpp)
target_link_libraries(test_scenarios paint_lib GTest::gtest_main)
add_test(NAME ScenarioTests COMMAND test_scenarios)
