# tests/CMakeLists.txt

cmake_minimum_required(VERSION 3.28)

# Находим Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Test Widgets Gui Svg)

# Включаем AUTOMOC ДО создания цели
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Создаем тестовый исполняемый файл
add_executable(tests
        test_main.cpp
        test_palettemanager.cpp
        test_qssmanager.cpp
        test_themediconmanager.cpp
        test_thememanager.cpp
)

# Подключаем библиотеки
target_link_libraries(tests PRIVATE
        AdaptiveThemeLib::AdaptiveThemeLib
        Qt6::Core
        Qt6::Test
        Qt6::Widgets
        Qt6::Gui
        Qt6::Svg
)

# Явно указываем, что эти файлы нужно обработать MOC
set_source_files_properties(
        test_palettemanager.cpp
        test_qssmanager.cpp
        test_themediconmanager.cpp
        test_thememanager.cpp
        PROPERTIES
        AUTOMOC ON
        SKIP_AUTOMOC OFF
)

# Добавляем директорию с ресурсами
target_include_directories(tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Копируем тестовые ресурсы
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

# Добавляем тест в CTest
enable_testing()
add_test(NAME AdaptiveThemeLibTests COMMAND tests)

# Настройка для безголового запуска
set_tests_properties(AdaptiveThemeLibTests PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)