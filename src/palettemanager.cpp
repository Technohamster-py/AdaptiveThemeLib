#include "palettemanager.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

PaletteManager& PaletteManager::instance() {
    static PaletteManager instance;
    return instance;
}

void PaletteManager::applyPalette(const QPalette &palette) {
    m_currentPalette = palette;
    QApplication::setPalette(m_currentPalette);
    emit paletteChanged(m_currentPalette);
    emit paletteRequested(m_currentPalette);
}

/**
 * @brief Applies the selected preset theme to the application.
 *
 * This function updates the application palette based on the specified
 * theme preset. It modifies various palette roles (like Window, WindowText,
 * Base, Text, Button, ButtonText, Highlight, and HighlightedText) to
 * correspond to the selected visual style. The new palette is then applied to
 * the application instance.
 *
 * @param theme The preset theme to apply. It can be either PresetTheme::Light
 * or PresetTheme::Dark. If an unknown value is provided, the default Light
 * theme is applied.
 */
void PaletteManager::applyPreset(PaletteManager::PresetPalette theme) {
    QPalette palette;
    switch (theme) {
        case PresetPalette::Light:
            palette.setColor(QPalette::Window, QColor("#f5f5f5"));
            palette.setColor(QPalette::WindowText, Qt::black);
            palette.setColor(QPalette::Base, Qt::white);
            palette.setColor(QPalette::Text, Qt::black);
            palette.setColor(QPalette::Button, QColor("#e0e0e0"));
            palette.setColor(QPalette::ButtonText, Qt::black);
            palette.setColor(QPalette::Highlight, QColor("#2196f3"));
            palette.setColor(QPalette::HighlightedText, Qt::white);
            palette.setColor(QPalette::PlaceholderText, QColor("#9f9f9f"));
            break;
        case PresetPalette::Dark:
            palette.setColor(QPalette::Window, QColor("#1e1f22"));
            palette.setColor(QPalette::WindowText, Qt::white);
            palette.setColor(QPalette::Base, QColor("#2b2d30"));
            palette.setColor(QPalette::Text, QColor("#e0e0e0"));
            palette.setColor(QPalette::Button, QColor("#2d2d2d"));
            palette.setColor(QPalette::ButtonText, Qt::white);
            palette.setColor(QPalette::Highlight, QColor("#3f51b5"));
            palette.setColor(QPalette::HighlightedText, Qt::white);
            palette.setColor(QPalette::Link, QColor("#55aaff"));
            palette.setColor(QPalette::PlaceholderText, QColor("#efefef"));
            break;
    }
    applyPalette(palette);
}

/**
 * Loads a theme configuration from an XML file and applies it to the application palette.
 *
 * The method parses an XML file containing theme configuration data,
 * including color roles and groups, and sets the corresponding colors
 * in the application palette. The XML structure is expected to contain
 * color groups (e.g., active, inactive, disabled) and color roles for
 * these groups (e.g., Window, Button, Text). The colors are defined
 * within the corresponding groups and roles.
 *
 * The method supports an extended color role "PlaceholderText" if the
 * application is built with Qt version 5.12 or higher.
 *
 * If the XML file cannot be loaded or parsed, a warning message is
 * logged, and the method returns false. Otherwise, the application's
 * palette is updated with the loaded theme, and the method returns true.
 *
 * @param path The file path to the XML theme configuration file.
 * @return Returns true if the theme is successfully loaded and applied,
 *         false if there is an error in loading or parsing the XML file.
 */
void PaletteManager::loadFromXml(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open theme file:" << path << "\t\tbecause of:" << file.errorString();
        return;
    }

    QXmlStreamReader xml(&file);
    if (xml.hasError()) {
        qWarning() << "XML parse error:" << xml.errorString();
        return;
    }

    QPalette palette;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement() && groupMap.contains(xml.name().toString().toLower())) {
            QPalette::ColorGroup group = groupMap[xml.name().toString().toLower()];
            QString groupTagName = xml.name().toString().toLower();

            while (!(xml.isEndElement() && xml.name().toString().toLower() == groupTagName) && !xml.atEnd()) {
                xml.readNext();

                if (xml.isStartElement() && xml.name().toString() == "colorrole") {
                    QString roleStr = xml.attributes().value("role").toString();
                    if (!roleMap.contains(roleStr)) continue;

                    QPalette::ColorRole role = roleMap[roleStr];
                    QColor color;
                    bool colorFound = false;

                    // Внутри <colorrole> ищем <color>
                    while (!(xml.isEndElement() && xml.name().toString() == "colorrole") && !xml.atEnd()) {
                        xml.readNext();

                        if (xml.isStartElement() && xml.name().toString() == "color") {
                            // Сначала считываем alpha, если есть (это атрибут)
                            QXmlStreamAttributes attrs = xml.attributes();
                            int a = attrs.hasAttribute("alpha") ? attrs.value("alpha").toInt() : 255;

                            int r = 0, g = 0, b = 0;

                            // Чтение вложенных тегов red, green, blue
                            while (!(xml.isEndElement() && xml.name().toString() == "color") && !xml.atEnd()) {
                                xml.readNext();

                                if (xml.isStartElement()) {
                                    QString cname = xml.name().toString().toLower();
                                    xml.readNext();

                                    if (xml.isCharacters()) {
                                        bool ok = false;
                                        int val = xml.text().toInt(&ok);
                                        if (ok) {
                                            if (cname == "red") r = val;
                                            else if (cname == "green") g = val;
                                            else if (cname == "blue") b = val;
                                        }
                                    }
                                }
                            }

                            color = QColor(r, g, b, a);
                            colorFound = true;
                        }
                    }

                    if (colorFound) {
                        palette.setColor(group, role, color);
                    }
                }
            }
        }
    }
    applyPalette(palette);
}

void PaletteManager::resetToSystemPalette() {
    applyPalette(QPalette());
}

void PaletteManager::refreshStyleSheet() {
    QPalette palette = QApplication::palette();
    QString stylesheet = generateStyleSheet(palette);
    qApp->setStyleSheet(stylesheet);
}

QString PaletteManager::generateStyleSheet(const QPalette &palette) {
    QColor window = palette.color(QPalette::Window);
    QColor windowText = palette.color(QPalette::WindowText);
    QColor base = palette.color(QPalette::Base);
    QColor alternateBase = palette.color(QPalette::AlternateBase);
    QColor text = palette.color(QPalette::Text);
    QColor button = palette.color(QPalette::Button);
    QColor buttonText = palette.color(QPalette::ButtonText);
    QColor highlight = palette.color(QPalette::Highlight);
    QColor highlightedText = palette.color(QPalette::HighlightedText);
    QColor mid = palette.color(QPalette::Mid);
    QColor dark = palette.color(QPalette::Dark);

    return QString(R"(
        /* Глобальные настройки */
        * {
            transition: all 0.2s ease-in-out;
        }

        QMainWindow, QDialog {
            background-color: %1;
        }

        /* Современные вкладки как в VS Code */
        QTabWidget::pane {
            border: 1px solid %10;
            background-color: %1;
            border-radius: 8px;
            top: -1px;
        }

        QTabBar::tab {
            background-color: %8;
            color: %2;
            padding: 8px 16px;
            margin-right: 4px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
        }

        QTabBar::tab:selected {
            background-color: %1;
            color: %7;
            border-bottom: 2px solid %7;
        }

        QTabBar::tab:hover:!selected {
            background-color: %9;
        }

        /* Современные кнопки */
        QPushButton {
            background-color: %5;
            color: %6;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: 500;
        }

        QPushButton:hover {
            background-color: %7;
            color: %12;
        }

        QPushButton:pressed {
            background-color: %11;
        }

        QPushButton:checked {
            background-color: %7;
            color: %12;
        }

        /* Поля ввода с плавной подсветкой */
        QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QComboBox {
            background-color: %3;
            color: %4;
            border: 1px solid %10;
            border-radius: 6px;
            padding: 6px 10px;
            selection-background-color: %7;
        }

        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border: 1px solid %7;
            background-color: %1;
        }

        /* Сплиттеры с современным дизайном */
        QSplitter::handle {
            background-color: %10;
            border-radius: 2px;
            margin: 2px;
        }

        QSplitter::handle:hover {
            background-color: %7;
        }

        /* Полосы прокрутки как в macOS */
        QScrollBar:vertical {
            background: %1;
            width: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:vertical {
            background: %10;
            border-radius: 6px;
            min-height: 30px;
        }

        QScrollBar::handle:vertical:hover {
            background: %7;
        }

        QScrollBar:horizontal {
            background: %1;
            height: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:horizontal {
            background: %10;
            border-radius: 6px;
            min-width: 30px;
        }

        QScrollBar::handle:horizontal:hover {
            background: %7;
        }

        /* Меню и выпадающие списки */
        QMenu {
            background-color: %3;
            border: 1px solid %10;
            border-radius: 8px;
            padding: 4px;
        }

        QMenu::item {
            padding: 6px 24px;
            border-radius: 4px;
        }

        QMenu::item:selected {
            background-color: %7;
            color: %12;
        }

        /* ToolBar с прозрачностью */
        QToolBar {
            background-color: transparent;
            border: none;
            spacing: 4px;
            padding: 4px;
        }

        QToolButton {
            background-color: transparent;
            border-radius: 6px;
            padding: 6px;
            icon-size: 20px;
        }

        QToolButton:hover {
            background-color: %9;
        }

        QToolButton:checked {
            background-color: %7;
        }

        /* ProgressBar с анимацией */
        QProgressBar {
            border: none;
            border-radius: 4px;
            background-color: %8;
            text-align: center;
            color: %2;
        }

        QProgressBar::chunk {
            background-color: %7;
            border-radius: 4px;
        }

        /* Групповые боксы */
        QGroupBox {
            border: 1px solid %10;
            border-radius: 8px;
            margin-top: 12px;
            padding-top: 8px;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 8px;
            color: %2;
        }

        /* Слайдеры */
        QSlider::groove:horizontal {
            height: 4px;
            background: %10;
            border-radius: 2px;
        }

        QSlider::handle:horizontal {
            background: %7;
            width: 16px;
            height: 16px;
            margin: -6px 0;
            border-radius: 8px;
        }

        QSlider::handle:horizontal:hover {
            background: %11;
            transform: scale(1.2);
        }

        /* Статус бар */
        QStatusBar {
            background-color: %1;
            border-top: 1px solid %10;
        }

        /* Ваш кастомный виджет инициативы */
        InitiativeTrackerWidget {
            background-color: %3;
            border-radius: 8px;
            padding: 4px;
        }

        /* Дерево кампании */
        QTreeWidget {
            background-color: %3;
            border: none;
            border-radius: 8px;
            outline: 0;
        }

        QTreeWidget::item {
            padding: 4px;
            border-radius: 4px;
        }

        QTreeWidget::item:selected {
            background-color: %7;
            color: %12;
        }

        QTreeWidget::item:hover {
            background-color: %9;
        }

        /* Таблица инициативы */
        QTableWidget {
            background-color: %3;
            border: none;
            border-radius: 8px;
            gridline-color: %10;
        }

        QTableWidget::item {
            padding: 8px;
        }

        QTableWidget::item:selected {
            background-color: %7;
            color: %12;
        }

        QHeaderView::section {
            background-color: %8;
            padding: 8px;
            border: none;
            border-right: 1px solid %10;
            border-bottom: 1px solid %10;
        }
    )").arg(
        window.name(),           // %1
        windowText.name(),       // %2
        base.name(),             // %3
        text.name(),             // %4
        button.name(),           // %5
        buttonText.name(),       // %6
        highlight.name(),        // %7
        highlightedText.name(),  // %8 - для тёмных тем
        mid.name(),              // %9 - для hover
        dark.name(),             // %10 - для границ
        highlight.darker(120).name(), // %11 - pressed
        highlightedText.name()   // %12 - для текста на highlight
    );
}

void PaletteManager::updateIconManager() {
}
