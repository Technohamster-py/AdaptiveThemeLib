# AdaptiveThemeLib

[![Qt Version](https://img.shields.io/badge/Qt-6.0%2B-brightgreen.svg)](https://www.qt.io/)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Библиотека для управления темами в Qt приложениях с поддержкой динамической смены палитр, QSS стилей и адаптивных SVG иконок.

## 📋 Содержание

- [Возможности](#-возможности)
- [Архитектура](#-архитектура)
- [Установка](#-установка)
- [Быстрый старт](#-быстрый-старт)
- [API Reference](#-api-reference)
- [Примеры](#-примеры)
- [Создание кастомных тем](#-создание-кастомных-тем)
- [Лицензия](#-лицензия)

## ✨ Возможности

- **🎨 PaletteManager** — управление цветовыми схемами (Light/Dark/System + кастомные XML)
- **🎭 QssManager** — работа с QSS стилями (встроенные пресеты + внешние файлы)
- **🖼️ ThemedIconManager** — динамическая перекраска SVG иконок под текущую тему
- **🎯 ThemeManager** — единый фасад для управления всеми компонентами
- **💾 Поддержка кастомных тем** — загрузка из XML и QSS файлов
- **🔄 Автоматическое обновление** — все компоненты синхронизируются через сигналы
- **📦 Встроенные ресурсы** — темы компилируются в бинарный файл
- **⚡ Современный дизайн** — Material Design, Modern пресеты

## 🏗️ Архитектура

```
┌─────────────────────────────────────────────────────────┐
│                   ThemeManager (Фасад)                  │
│         Оркестрирует работу всех компонентов            │
└─────────────────┬───────────────┬──────────────┬────────┘
                  │               │              │
                  ▼               ▼              ▼
┌─────────────────────┐ ┌─────────────────┐ ┌──────────────────┐
│   PaletteManager    │ │   QssManager    │ │ThemedIconManager │
│                     │ │                 │ │                  │
│ • QPalette          │ │ • QSS файлы     │ │ • SVG иконки     │
│ • Light/Dark/System │ │ • Пресеты       │ │ • Динамическая   │
│ • XML палитры       │ │ • Переменные    │ │   перекраска     │
│                     │ │   ($window etc) │ │                  │
└─────────────────────┘ └─────────────────┘ └──────────────────┘
```

### Компоненты

| Компонент             | Ответственность                           | Основные методы                                                                                       |
|-----------------------|-------------------------------------------|-------------------------------------------------------------------------------------------------------|
| **PaletteManager**    | Управление QPalette                       | `applyPreset()`, `loadFromXml()`                                                                      |
| **QssManager**        | Управление QSS стилями                    | `applyPreset()`, `applyQssFromFile()`                                                                 |
| **ThemedIconManager** | Перекраска SVG иконок в реальном времени  | `addIconTarget()`, `addPixmapTarget()`, `updateAllIcons()`                                            |
| **ThemeManager**      | Фасад, координация                        | `setPaletteFile()`, `setPalettePreset()`, `setStyleSheetFile()`, `setStyleSheetPreset()`, `refresh()` |

## 📦 Установка

### Через CMake (рекомендуется)

```cmake
# В вашем CMakeLists.txt
include(FetchContent)

FetchContent_Declare(
    AdaptiveThemeLib
    GIT_REPOSITORY https://github.com/Technohamster-py/AdaptiveThemeLib.git
    GIT_TAG main
)

FetchContent_MakeAvailable(AdaptiveThemeLib)

target_link_libraries(your_app PRIVATE
    AdaptiveThemeLib::AdaptiveThemeLib
)
```

### Ручная установка

```bash
git clone https://github.com/Technohamster-py/AdaptiveThemeLib.git
cd AdaptiveThemeLib
mkdir build && cd build
cmake ..
make install
```

## 🚀 Быстрый старт

### Минимальный пример

```c++
#include <QApplication>
#include <thememanager.h>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    // Применяем тёмную тему
    ThemeManager::instance().setPalettePreset(PaletteManager::PresetPalette::Dark);
    ThemeManager::instance().refresh();
    
    // Применяем material-стиль
    ThemeManager::instance().setStyleSheetPreset(QssManager::PresetQss::Material);
    ThemeManager::instance().refresh();

    // Ваше приложение...
    
    return app.exec();
}
```

### Использование с кастомными файлами

```c++
// Устанавливаем кастомную палитру и QSS
ThemeManager::instance().setPaletteFile("/path/to/my_palette.xml");
ThemeManager::instance().setStyleSheetFile("/path/to/my_style.qss");
ThemeManager::instance().refresh();
```

### Работа с иконками

```c++
#include <themediconmanager.h>

// В вашем виджете
ThemedIconManager::instance().addIconTarget<QAbstractButton>(
    ":/icons/play.svg", 
    playButton, 
    &QAbstractButton::setIcon
);

// При смене темы иконки автоматически перекрасятся
```

## 📚 API Reference

### PaletteManager

```c++
// Пресеты
enum class PresetPalette { Light, Dark, System };

void applyPreset(PresetPalette preset);
void loadFromXml(const QString& filePath);
void applyPalette(const QPalette& palette);
QPalette currentPalette() const;

// Сигналы
void paletteChanged(const QPalette& palette);
```

### QssManager

```c++
// Пресеты QSS
enum class PresetQss {System, Material, Classic, Modern, LiquidGlass};

void applyPreset(PresetQss preset);
bool applyQssFromFile(const QString& filePath);
void refreshFromPalette(const QPalette& palette);

// Переменные в QSS
void setVariable(const QString& varName, const QString& value);
void setVariable(const QString& varName, const QColor& color);
void setVariable(const QString& varName, int value);
```

### ThemedIconManager

```c++
template<typename T>
void addIconTarget(const QString& iconPath, T* widget, void (T::*method)(const QIcon&));

void addPixmapTarget(const QString& iconPath, QWidget* widget, 
                     std::function<void(const QPixmap&)> setter);

void updateAllIcons();
void onPaletteChanged(const QPalette& palette);
```

### ThemeManager (Фасад)

```c++
// Управление палитрой и стилями по отдельности
void setPalettePreset(PaletteManager::PresetPalette preset);
void setStyleSheetPreset(QssManager::PresetQss preset);

// Кастомные файлы
void setPaletteFile(const QString& paletteFilePath);
void setStyleSheetFile(const QString& stylesheetFilePath);

// Общие методы
void resetToSystemTheme();
void refresh();

// Сигналы
void paletteChanged(const QString& fileName);
void stylesheetChanged(const QString& fileName);
void themeChanged();
```

## 💡 Примеры

### Пример 1: Переключение между светлой и тёмной темой

```c++
static bool isDark = false;

void MainWindow::toggleTheme() {
    isDark = !isDark;
    
    if (isDark) {
        ThemeManager::instance().setPalettePreset(PaletteManager::PresetPalette::Dark);
    } else {
        ThemeManager::instance().setPalettePreset(PaletteManager::PresetPalette::Light);
    }
    ThemeManager::instance().refresh();
}
```

### Пример 2: Загрузка пользовательской темы из файлов

```c++
void MainWindow::loadUserTheme() {
    QString paletteFile = QFileDialog::getOpenFileName(this, 
        "Select palette XML", "", "XML files (*.xml)");
    QString qssFile = QFileDialog::getOpenFileName(this, 
        "Select QSS file", "", "QSS files (*.qss)");
    
    if (!paletteFile.isEmpty() && !qssFile.isEmpty()) {
        ThemeManager::instance().setPaletteFile(paletteFile);
        ThemeManager::instance().setStyleSheetFile(qssFile);
        ThemeManager::instance().refresh();
    }
}
```

### Пример 3: Использование переменных в QSS

```c++
// В вашем коде
QssManager::instance().setVariable("myColor", QColor("#ff0000"));
QssManager::instance().setVariable("myPadding", "16px");

// В QSS файле
QPushButton {
    background-color: $myColor;
    padding: $myPadding;
}
```

### Пример 4: Адаптивные иконки

```c++
class MyWidget : public QWidget {
public:
    MyWidget() {
        // Регистрируем иконку для кнопки
        ThemedIconManager::instance().addIconTarget<QAbstractButton>(
            ":/icons/settings.svg", 
            m_settingsButton, 
            &QAbstractButton::setIcon
        );
        
        // Регистрируем иконку для QLabel (через pixmap)
        ThemedIconManager::instance().addPixmapTarget(
            ":/icons/logo.svg",
            m_logoLabel,
            [label = m_logoLabel](const QPixmap& px) {
                label->setPixmap(px.scaled(32, 32, Qt::KeepAspectRatio));
            }
        );
    }
};
```

## 🎨 Создание кастомных тем

### Формат файла палитры (XML)

```xml
<?xml version="1.0" encoding="UTF-8"?>
<palette>
    <!-- Основные цвета -->
    <color role="Window">#2b2b2b</color>
    <color role="WindowText">#ffffff</color>
    <color role="Base">#1e1e1e</color>
    <color role="AlternateBase">#252526</color>
    <color role="Text">#ffffff</color>
    
    <!-- Кнопки -->
    <color role="Button">#3c3c3c</color>
    <color role="ButtonText">#ffffff</color>
    
    <!-- Выделение -->
    <color role="Highlight">#0e639c</color>
    <color role="HighlightedText">#ffffff</color>
    
    <!-- Дополнительные цвета -->
    <color role="Mid">#4a4a4a</color>
    <color role="Dark">#3c3c3c</color>
</palette>
```

### Формат QSS файла с переменными

```css
/* custom_theme.qss */
QMainWindow {
    background-color: $window;
}

QPushButton {
    background-color: $button;
    color: $buttonText;
    border-radius: 8px;
    padding: 8px 16px;
}

QPushButton:hover {
    background-color: $highlight;
}

QLineEdit {
    background-color: $base;
    color: $text;
    border: 2px solid $mid;
    border-radius: 6px;
    padding: 6px 10px;
}

QLineEdit:focus {
    border-color: $highlight;
}
```

### Предустановленные переменные в QSS

| Переменная         | Описание                  | Источник                  |
|--------------------|---------------------------|---------------------------|
| `$window`          | Фоновый цвет окна         | QPalette::Window          |
| `$windowText`      | Цвет текста окна          | QPalette::WindowText      |
| `$base`            | Базовый цвет (поля ввода) | QPalette::Base            |
| `$text`            | Цвет текста               | QPalette::Text            |
| `$button`          | Цвет кнопки               | QPalette::Button          |
| `$buttonText`      | Цвет текста кнопки        | QPalette::ButtonText      |
| `$highlight`       | Цвет выделения            | QPalette::Highlight       |
| `$highlightedText` | Цвет текста выделения     | QPalette::HighlightedText |
| `$mid`             | Средний цвет              | QPalette::Mid             |
| `$dark`            | Тёмный цвет               | QPalette::Dark            |

## 🔧 Требования

- Qt 6.0 или выше
- C++17 совместимый компилятор
- CMake 3.28 или выше

## 📝 Лицензия

MIT License. См. файл [LICENSE](LICENSE.md) для деталей.

## 🤝 Вклад в проект

1. Форкните репозиторий
2. Создайте ветку для ваших изменений (`git checkout -b feature/amazing-feature`)
3. Зафиксируйте изменения (`git commit -m 'Add amazing feature'`)
4. Отправьте изменения в ваш форк (`git push origin feature/amazing-feature`)
5. Откройте Pull Request

## 📧 Контакты

Автор: [Technohamster-py](https://github.com/Technohamster-py)

---

⭐ Если вам понравилась библиотека, поставьте звезду на GitHub!