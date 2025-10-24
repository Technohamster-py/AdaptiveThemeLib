# AdaptiveThemeLib

Reusable Qt6 theme and icon manager library.

## Features
- Centralized theme management (ThemeManager)
- Dynamic icon recoloring (ThemedIconManager)
- Easy integration with any Qt6 app

## Usage
```cmake
include(FetchContent)
FetchContent_Declare(
ThemeLib
GIT_REPOSITORY https://github.com/Technohamster-py/AdaptiveThemeLib.git
)
FetchContent_MakeAvailable(ThemeLib)

target_link_libraries(MyApp PRIVATE ThemeLib::ThemeLib)
```