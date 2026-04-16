#include "palettemanager.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QDir>

Q_LOGGING_CATEGORY(paletteCategory, "theme.palette")

static const QHash<QString, QPalette::ColorRole> &getRoleMap() {
    static const QHash<QString, QPalette::ColorRole> roleMap = {
        {"Window", QPalette::Window},
        {"WindowText", QPalette::WindowText},
        {"Base", QPalette::Base},
        {"AlternateBase", QPalette::AlternateBase},
        {"ToolTipBase", QPalette::ToolTipBase},
        {"ToolTipText", QPalette::ToolTipText},
        {"Text", QPalette::Text},
        {"Button", QPalette::Button},
        {"ButtonText", QPalette::ButtonText},
        {"BrightText", QPalette::BrightText},
        {"Highlight", QPalette::Highlight},
        {"HighlightedText", QPalette::HighlightedText},
        {"Link", QPalette::Link},
        {"LinkVisited", QPalette::LinkVisited},
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        {"PlaceholderText", QPalette::PlaceholderText},
#endif
    };
    return roleMap;
}

static const QHash<QString, QPalette::ColorGroup> &getGroupMap() {
    static const QHash<QString, QPalette::ColorGroup> groupMap = {
        {"active", QPalette::Active},
        {"inactive", QPalette::Inactive},
        {"disabled", QPalette::Disabled}
    };
    return groupMap;
}

QString PaletteManager::presetName(PresetPalette preset) {
    switch (preset) {
        case PresetPalette::Light:
            return "Light";
        case PresetPalette::Dark:
            return "Dark";
        case PresetPalette::System:
            return "System";
        default:
            return "Unknown";
    }
}

PaletteManager::PresetPalette PaletteManager::presetFromName(const QString &name) {
    static QMap<QString, PaletteManager::PresetPalette> presetMap = {
        {"System", PresetPalette::System},
        {"Light", PresetPalette::Light},
        {"Dark", PresetPalette::Dark},
    };
    return presetMap.value(name, PresetPalette::Undefined);
}

PaletteManager &PaletteManager::instance() {
    static PaletteManager instance;
    return instance;
}

void PaletteManager::applyPalette(const QPalette &palette) {
    m_currentPalette = palette;
    QApplication::setPalette(m_currentPalette);
    emit paletteChanged(m_currentPalette);
}

bool PaletteManager::applyPalette(const QString &name) {
    qDebug() << "applyPalette(" << name << ")";
    if (!m_availablePalettes.contains(name)) {
        qWarning() << "palette" << name << "not available";
        return false;
    }

    if (presetFromName(name) != PresetPalette::Undefined) {
        qDebug() << "applying preset palette(" << name << ")";
        return applyPreset(presetFromName(name));
    }

    if (m_customPalettes.contains(name)) {
        return loadFromXml(m_customPalettes[name]);
    }
    qCWarning(paletteCategory) << "Palette " << name << " not found";
    return false;
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
 * @param preset The preset theme to apply. It can be either PresetTheme::Light
 * or PresetTheme::Dark. If an unknown value is provided, the default Light
 * theme is applied.
 */
bool PaletteManager::applyPreset(PaletteManager::PresetPalette preset) {
    QPalette palette;
    switch (preset) {
        case PresetPalette::Light:
            palette.setColor(QPalette::Window, QColor("#f5f5f5"));
            palette.setColor(QPalette::WindowText, Qt::black);
            palette.setColor(QPalette::Base, Qt::white);
            palette.setColor(QPalette::Text, Qt::black);
            palette.setColor(QPalette::Button, QColor("#e0e0e0"));
            palette.setColor(QPalette::ButtonText, Qt::black);
            palette.setColor(QPalette::Highlight, QColor("#2196f3"));
            palette.setColor(QPalette::HighlightedText, Qt::white);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
            palette.setColor(QPalette::PlaceholderText, QColor("#9f9f9f"));
#endif
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
            palette.setColor(QPalette::PlaceholderText, QColor("#efefef"));
#endif
            break;
        default:
            resetToSystemPalette();
            qCWarning(paletteCategory) << "Unknown palette preset:" << static_cast<int>(preset);
            return false;
    }
    applyPalette(palette);
    return true;
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
bool PaletteManager::loadFromXml(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(paletteCategory) << "Cannot open theme file:" << path << "\t\tbecause of:" << file.errorString();
        return false;
    }

    QXmlStreamReader xml(&file);
    xml.setEntityResolver(nullptr);
    if (xml.hasError()) {
        qCWarning(paletteCategory) << "XML parse error:" << xml.errorString();
        return false;
    }

    QPalette palette;
    const auto &groupMap = getGroupMap();
    const auto &roleMap = getRoleMap();

    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        if (!xml.isStartElement()) continue;

        QString tagName = xml.name().toString().toLower();
        if (groupMap.contains(tagName)) {
            QPalette::ColorGroup group = groupMap[xml.name().toString().toLower()];
            QString groupTagName = xml.name().toString().toLower();

            while (!(xml.isEndElement() && xml.name().toString().toLower() == groupTagName) && !xml.atEnd()) {
                xml.readNext();
                if (!xml.isStartElement() || !(xml.name().toString() == "colorrole")) continue;

                QString roleStr = xml.attributes().value("role").toString();
                if (!roleMap.contains(roleStr)) continue;

                QPalette::ColorRole role = roleMap[roleStr];
                QColor color;
                bool colorFound = false;

                // Внутри <colorrole> ищем <color>
                while (!(xml.isEndElement() && xml.name().toString() == "colorrole") && !xml.atEnd()) {
                    xml.readNext();
                    if (!xml.isStartElement() || !(xml.name().toString() == "color")) continue;

                    // Сначала считываем alpha, если есть (это атрибут)
                    QXmlStreamAttributes attrs = xml.attributes();
                    int a = attrs.hasAttribute("alpha") ? attrs.value("alpha").toInt() : 255;
                    int r = 0, g = 0, b = 0;

                    // Чтение вложенных тегов red, green, blue
                    while (!(xml.isEndElement() && xml.name().toString() == "color") && !xml.atEnd()) {
                        xml.readNext();
                        if (!xml.isStartElement()) continue;

                        QString cname = xml.name().toString().toLower();
                        xml.readNext();
                        if (!xml.isCharacters()) continue;

                        bool ok = false;
                        int val = xml.text().toInt(&ok);
                        if (ok) {
                            if (cname == "red") r = val;
                            else if (cname == "green") g = val;
                            else if (cname == "blue") b = val;
                        }
                    }

                    color = QColor(r, g, b, a);
                    colorFound = true;
                }

                if (colorFound) {
                    palette.setColor(group, role, color);
                }
            }
        }
    }
    if (xml.hasError()) {
        qCWarning(paletteCategory) << "Palette load error:" << xml.errorString();
        return false;
    }
    applyPalette(palette);
    return true;
}

PaletteManager::PaletteManager() {
    availablePalettes();
}

void PaletteManager::resetToSystemPalette() {
    applyPalette(QPalette());
}

void PaletteManager::setUserPaletteDir(const QString &dir) {
    m_userPaletteDir = dir;
    availablePalettes();
    emit userDirectoryChanged(dir);
}

void PaletteManager::scanCustomPalettes() {
    m_customPalettes.clear();

    QDir dir(m_userPaletteDir);;
    if (!dir.exists()) {
        qCWarning(paletteCategory) << "Palette directory not found:" << m_userPaletteDir;
        return;
    }

    QStringList paletteFiles = dir.entryList(QStringList() << "*.xml*", QDir::Files);
    for (const QString &fileName: paletteFiles) {
        QString paletteName = QFileInfo(fileName).baseName();
        if (!m_availablePalettes.contains(paletteName)) {
            m_customPalettes[paletteName] = dir.absoluteFilePath(fileName);
            m_availablePalettes.append(paletteName);
        }
    }
}

QStringList PaletteManager::availablePalettes() {
    m_availablePalettes.clear();

    m_availablePalettes.append("System");
    m_availablePalettes.append("Light");
    m_availablePalettes.append("Dark");

    scanCustomPalettes();

    return m_availablePalettes;
}
