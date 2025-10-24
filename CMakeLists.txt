cmake_minimum_required(VERSION 3.28)
project(AdaptiveThemeLib VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    set(CMAKE_PREFIX_PATH "C:/Qt/Qt6/6.9.0/mingw_64/lib/cmake")
elseif (UNIX AND NOT APPLE)
    set(CMAKE_PREFIX_PATH "/usr/local/qt/Qt-6.6.1")
endif ()

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Svg
)

add_library(AdaptiveThemeLib
        src/thememanager.cpp
        src/themediconmanager.cpp
)

target_include_directories(AdaptiveThemeLib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(AdaptiveThemeLib PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Svg
)

add_library(AdaptiveThemeLib::AdaptiveThemeLib ALIAS AdaptiveThemeLib)

install(TARGETS AdaptiveThemeLib EXPORT AdaptiveThemeLibTargets)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT AdaptiveThemeLibTargets
        FILE AdaptiveThemeLibConfig.cmake
        NAMESPACE AdaptiveThemeLib::
        DESTINATION lib/cmake/AdaptiveThemeLib
)