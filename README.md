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
- **🎭 QssManager** — работа с QSS стилями (встроенные пресеты + внешние файлы + нативные стили)
- **🖼️ ThemedIconManager** — динамическая перекраска SVG иконок под текущую тему
- **🎯 ThemeManager** — единый фасад для управления всеми компонентами
- **💾 Поддержка кастомных тем** — загрузка из XML и QSS файлов
- **🔄 Автоматическое обновление** — все компоненты синхронизируются через сигналы
- **📦 Встроенные ресурсы** — Material, Modern, Classic пресеты
- **🎨 Переменные в QSS** — использование `$variable` для динамической подстановки цветов
- **⚡ Нативные стили** — поддержка QStyleFactory (Windows, Fusion, etc.)

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
│ • XML палитры       │ │ • Нативные стили│ │   перекраска     │
│ • Только сигналы    │ │ • Переменные    │ │ • currentColor   │
│                     │ │   ($window etc) │ │   замена         │
└─────────────────────┘ └─────────────────┘ └──────────────────┘
```

### Компоненты

| Компонент             | Ответственность                           | Основные методы                                              |
|-----------------------|-------------------------------------------|--------------------------------------------------------------|
| **PaletteManager**    | Управление QPalette                       | `applyPreset()`, `loadFromXml()`, `resetToSystemPalette()`   |
| **QssManager**        | Управление QSS и нативными стилями        | `applyPreset()`, `applyQssFromFile()`, `applyNativeStyle()`  |
| **ThemedIconManager** | Перекраска SVG иконок в реальном времени  | `addIconTarget()`, `addPixmapTarget()`, `renderIconInline()` |
| **ThemeManager**      | Фасад, координация                        | `applyStyle()`, `applyPalette()`, `resetToSystemTheme()`     |

## 📦 Установка

### Через CMake (рекомендуется)

```cmake
# В вашем CMakeLists.txt
add_subdirectory(path/to/AdaptiveThemeLib)

target_link_libraries(your_app PRIVATE
    AdaptiveThemeLib
)
```

### Ручная установка

```bash
git clone https://github.com/yourusername/AdaptiveThemeLib.git
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
    
    // Применяем тёмную палитру
    PaletteManager::instance().applyPreset(PaletteManager::PresetPalette::Dark);
    
    // Применяем Material QSS стиль
    QssManager::instance().applyPreset(QssManager::PresetQss::Material);
    
    // Или через фасад ThemeManager
    ThemeManager::instance().applyPalette("Dark");
    ThemeManager::instance().applyStyle("Material");
    
    return app.exec();
}
```

### Использование с кастомными файлами

```c++
// Устанавливаем директорию для пользовательских тем
PaletteManager::instance().setUserPaletteDir("/path/to/palettes/");
QssManager::instance().setUserQssDirectory("/path/to/styles/");

// Применяем кастомную палитру из XML
PaletteManager::instance().applyPalette("my_custom_theme");

// Применяем кастомный QSS стиль
QssManager::instance().applyStyle("my_style");
```

### Работа с иконками

```c++
#include <themediconmanager.h>

// В вашем виджете
ThemedIconManager::instance().addIconTarget<QPushButton>(
    ":/icons/play.svg", 
    playButton, 
    &QPushButton::setIcon,
    QSize(24, 24)  // размер иконки
);

// При смене палитры иконки автоматически перекрасятся
```

## 📚 API Reference

### PaletteManager

```c++
// Singleton доступ
static PaletteManager& instance();

// Пресеты
enum class PresetPalette { System, Light, Dark, Undefined };
bool applyPreset(PresetPalette preset);
QString presetName(PresetPalette preset);
PresetPalette presetFromName(const QString &name);

// Кастомные палитры
bool applyPalette(const QString &name);
bool loadFromXml(const QString &path);
void scanCustomPalettes();
QStringList availablePalettes();

// Управление директориями
void setUserPaletteDir(const QString& dir);
QString userPaletteDir() const;

// Текущее состояние
QPalette currentPalette() const;
void resetToSystemPalette();

// Сигналы
void paletteChanged(const QPalette& palette);
void userDirectoryChanged(QString path);
```

### QssManager

```c++
// Singleton доступ
static QssManager& instance();

// Пресеты QSS
enum class PresetQss { System, Material, Classic, Modern, Undefined };
void applyPreset(const PresetQss& preset);
static QString presetName(PresetQss preset);
static PresetQss stringToPreset(const QString& name);

// Нативные стили
bool applyNativeStyle(const QString& styleName);
bool applyQssStyle(const QString& styleName);
bool applyStyle(const QString& styleName);

// Работа с QSS файлами
void applyQssFromFile(const QString& fileName);
void dropStyleSheet();
void applyCurrentStyleSheet();

// Переменные в QSS
void setVariable(const QString& varName, const QString& value);
void setVariable(const QString& varName, const QColor& color);
void setVariable(const QString& varName, int value);

// Получение доступных стилей
QList<StyleInfo> availableStyles();
QList<StyleInfo> nativeStyles() const;
QList<StyleInfo> qssStyles() const;

// Управление директориями
void setUserQssDirectory(const QString& dir);
QString userQssDirectory() const;

// Обновление из палитры
void refreshFromPalette(const QPalette& palette);

// Сигналы
void styleSheetUpdated();
void nativeStyleUpdated(const QString& styleName);
void qssStyleUpdated(const QString& styleName);
void styleChanged(const QString& styleName, StyleType type);
void userDirectoryChanged(const QString& dir);
```

### ThemedIconManager

```c++
// Singleton доступ
static ThemedIconManager& instance();

// Добавление целей для иконок
template<typename T>
void addIconTarget(const QString &svgPath, T *object, 
                   void(T::*setIconMethod)(const QIcon &),
                   QSize size = QSize(24, 24));

void addPixmapTarget(const QString &svgPath, QObject *receiver,
                     std::function<void(const QPixmap &)> applyPixmap,
                     bool override = true, QSize size = QSize(24, 24));

// Статические методы для композитных иконок
static QPixmap renderIconInline(const QStringList& svgPaths, 
                                 QSize iconSize = QSize(16, 16), 
                                 int spacing = 2);
static QPixmap renderIconGrid(const QStringList& svgPaths, 
                               QSize iconSize = QSize(16, 16), 
                               int spacing = 2, 
                               int maxIconsPerRow = 3);

// Сигналы
void themeChanged();
```

### ThemeManager (Фасад)

```c++
// Singleton доступ
static ThemeManager& instance();

// Добавление кастомных файлов
bool addCustomPalette(const QString& fileName);
bool addCustomStyle(const QString& fileName);

// Применение тем
bool applyStyle(const QString& styleName);
bool applyPalette(const QString& paletteName);
void resetToSystemTheme();

// Получение доступных тем
QList<QssManager::StyleInfo> availableStyles() const;
QStringList availablePalettes() const;

// Сигналы
void paletteChanged(const QString& fileName);
void stylesheetChanged(const QString &fileName);
void themeChanged();
```

## 💡 Примеры

### Пример 1: Переключение между светлой и тёмной темой

```c++
static bool isDark = false;

void MainWindow::toggleTheme() {
    isDark = !isDark;
    
    if (isDark) {
        PaletteManager::instance().applyPreset(PaletteManager::PresetPalette::Dark);
        QssManager::instance().applyPreset(QssManager::PresetQss::Material);
    } else {
        PaletteManager::instance().applyPreset(PaletteManager::PresetPalette::Light);
        QssManager::instance().applyPreset(QssManager::PresetQss::Material);
    }
}
```

### Пример 2: Загрузка пользовательской темы из файлов

```c++
void MainWindow::loadUserTheme() {
    QString paletteFile = QFileDialog::getOpenFileName(this, 
        "Select palette XML", "", "XML files (*.xml)");
    
    if (!paletteFile.isEmpty()) {
        // Копируем в пользовательскую директорию
        ThemeManager::instance().addCustomPalette(paletteFile);
        
        // Применяем по имени (без расширения)
        QString paletteName = QFileInfo(paletteFile).baseName();
        ThemeManager::instance().applyPalette(paletteName);
    }
    
    QString qssFile = QFileDialog::getOpenFileName(this, 
        "Select QSS file", "", "QSS files (*.qss)");
    
    if (!qssFile.isEmpty()) {
        ThemeManager::instance().addCustomStyle(qssFile);
        QString styleName = QFileInfo(qssFile).baseName();
        ThemeManager::instance().applyStyle(styleName);
    }
}
```

### Пример 3: Использование переменных в QSS

```c++
// В вашем коде
QssManager::instance().setVariable("myColor", QColor("#ff0000"));
QssManager::instance().setVariable("myPadding", "16px");
QssManager::instance().setVariable("myRadius", 8);

// После установки переменных нужно перезагрузить текущий QSS
// (если используется кастомный файл)
QssManager::instance().applyQssFromFile("path/to/style.qss");

// В QSS файле
QPushButton {
    background-color: $myColor;
    padding: $myPadding;
    border-radius: $myRadius;
}
```

### Пример 4: Предопределённые переменные в QSS

QssManager автоматически устанавливает эти переменные из палитры:
- `$window`
- `$windowText`
- `$base`
- `$alternateBase`
- `$text`
- `$button`
- `$buttonText`
- `$highlight`
- `$highlightedText`
- `$mid`
- `$dark`

Пример QSS с использованием предопределённых переменных"
```css
QMainWindow {
    background-color: $window;
}

QPushButton {
    background-color: $button;
    color: $buttonText;
}

QPushButton:hover {
    background-color: $highlight;
    color: $highlightedText;
}
```

### Пример 5: Адаптивные иконки

```c++
class MyWidget : public QWidget {
    QPushButton* m_settingsButton;
    QLabel* m_logoLabel;
    
public:
    MyWidget() {
        m_settingsButton = new QPushButton(this);
        m_logoLabel = new QLabel(this);
        
        // Регистрируем иконку для кнопки
        ThemedIconManager::instance().addIconTarget<QPushButton>(
            ":/icons/settings.svg", 
            m_settingsButton, 
            &QPushButton::setIcon,
            QSize(24, 24)
        );
        
        // Регистрируем иконку для QLabel (через pixmap)
        ThemedIconManager::instance().addPixmapTarget(
            ":/icons/logo.svg",
            m_logoLabel,
            [this](const QPixmap& px) {
                m_logoLabel->setPixmap(px.scaled(32, 32, Qt::KeepAspectRatio));
            },
            true,
            QSize(32, 32)
        );
        
        // Композитная иконка из нескольких SVG
        QStringList icons = {":/icons/icon1.svg", ":/icons/icon2.svg"};
        QPixmap composite = ThemedIconManager::renderIconInline(icons, QSize(16, 16), 4);
        m_compositeLabel->setPixmap(composite);
    }
};
```

## 🎨 Создание кастомных тем

### Формат файла палитры (XML)

```xml
<?xml version="1.0" encoding="UTF-8"?>
<palette>
    <!-- Цвета для активного состояния -->
    <Active>
        <colorrole role="Window">
            <color alpha="255">
                <red>43</red><green>43</green><blue>43</blue>
            </color>
        </colorrole>
        <colorrole role="WindowText">
            <color><red>255</red><green>255</green><blue>255</blue></color>
        </colorrole>
        <colorrole role="Base">
            <color><red>30</red><green>30</green><blue>30</blue></color>
        </colorrole>
        <colorrole role="Text">
            <color><red>255</red><green>255</green><blue>255</blue></color>
        </colorrole>
        <colorrole role="Button">
            <color><red>60</red><green>60</green><blue>60</blue></color>
        </colorrole>
        <colorrole role="ButtonText">
            <color><red>255</red><green>255</green><blue>255</blue></color>
        </colorrole>
        <colorrole role="Highlight">
            <color><red>14</red><green>99</green><blue>156</blue></color>
        </colorrole>
        <colorrole role="HighlightedText">
            <color><red>255</red><green>255</green><blue>255</blue></color>
        </colorrole>
    </Active>
    
    <!-- Необязательно: Inactive и Disabled состояния -->
    <Inactive>
        <!-- ... -->
    </Inactive>
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
    color: $highlightedText;
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

QMenuBar {
    background-color: $window;
    color: $windowText;
}

QMenuBar::item:selected {
    background-color: $highlight;
    color: $highlightedText;
}
```

### Предустановленные переменные в QSS

| Переменная         | Описание                  | Источник                      |
|--------------------|---------------------------|-------------------------------|
| `$window`          | Фоновый цвет окна         | `QPalette::Window`            |
| `$windowText`      | Цвет текста окна          | `QPalette::WindowText`        |
| `$base`            | Базовый цвет (поля ввода) | `QPalette::Base`              |
| `$alternateBase`   | Альтернативный базовый    | `QPalette::AlternateBase`     |
| `$text`            | Цвет текста               | `QPalette::Text`              |
| `$button`          | Цвет кнопки               | `QPalette::Button`            |
| `$buttonText`      | Цвет текста кнопки        | `QPalette::ButtonText`        |
| `$highlight`       | Цвет выделения            | `QPalette::Highlight`         |
| `$highlightedText` | Цвет текста выделения     | `QPalette::HighlightedText`   |
| `$mid`             | Средний цвет              | `QPalette::Mid`               |
| `$dark`            | Тёмный цвет               | `QPalette::Dark`              |

### Создание SVG иконок с поддержкой темы

```svg
<!-- icon.svg -->
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24">
    <path fill="currentColor" d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2z"/>
    <!-- currentColor будет заменён на themeColor() -->
</svg>
```

## 🔧 Требования

- Qt 6.0 или выше
- C++17 совместимый компилятор
- CMake 3.28 или выше
- Модули Qt: Core, Gui, Widgets, Svg

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
```
