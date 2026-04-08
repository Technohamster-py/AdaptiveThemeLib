set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)


add_executable(tests)


target_link_libraries(tests PRIVATE
        AdaptiveThemeLib::AdaptiveThemeLib
        Qt6::Core
        Qt6::Test
        Qt6::Widgets
        Qt6::Gui
        Qt6::Svg
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME AdaptiveThemeLibTests COMMAND tests)

# Включаем вывод в консоль
set_tests_properties(AdaptiveThemeLibTests PROPERTIES
        PASS_REGULAR_EXPRESSION "PASSED"
        FAIL_REGULAR_EXPRESSION "FAILED"
)