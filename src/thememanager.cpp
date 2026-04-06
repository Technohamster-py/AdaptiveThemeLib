#include "thememanager.h"
#include "palettemanager.h"
#include "qssmanager.h"
#include "themediconmanager.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

ThemeManager& ThemeManager::instance() {
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager() : QObject() {
    auto& paletteManager = PaletteManager::instance();
    auto& qssManager = QssManager::instance();
    auto& iconManager = ThemedIconManager::instance();

    connect(&paletteManager, &PaletteManager::paletteChanged, &qssManager, &QssManager::refreshFromPalette);
    connect(&paletteManager, &PaletteManager::paletteChanged, &iconManager, &ThemedIconManager::onPaletteChanged);
    connect(&qssManager, &QssManager::styleSheetUpdated, &qssManager, &QssManager::applyCurrentStyleSheet);
}

void ThemeManager::setPaletteFile(const QString& paletteFilePath) {
    if (!QFile::exists(paletteFilePath)) {
        qWarning() << "Palette file not found:" << paletteFilePath;
        return;
    }
    m_paletteFile = paletteFilePath;
    m_usingCustomFiles = true;

    emit paletteChanged(m_paletteFile);

    if (!m_qssFile.isEmpty()) {
        applyCustomTheme();
    }
}

void ThemeManager::setStyleSheetFile(const QString& qssFilePath) {
    if (!QFile::exists(qssFilePath)) {
        qWarning() << "StyleSheet file not found:" << qssFilePath;
        return;
    }

    m_qssFile = qssFilePath;
    m_usingCustomFiles = true;

    emit stylesheetChanged(m_qssFile);

    if (!m_paletteFile.isEmpty()) {
        applyCustomTheme();
    }
}

void ThemeManager::applyCustomTheme() {
    if (m_paletteFile.isEmpty() || m_qssFile.isEmpty()) {
        qWarning() << "Cannot apply custom theme: both palette and QSS files must be set";
        return;
    }
    PaletteManager::instance().loadFromXml(m_paletteFile);
    QssManager::instance().loadQssFromFile(m_qssFile);
    emit themeChanged();
}

void ThemeManager::applyPresetPalette(PaletteManager::PresetPalette preset) {
    m_currentPreset = preset;
    m_usingCustomFiles = false;
    PaletteManager::instance().applyPreset(preset);
    QssManager::instance().refreshFromPalette(qApp->palette());
}

void ThemeManager::resetToSystemTheme() {
    applyPresetPalette(PaletteManager::PresetPalette::System);
}

void ThemeManager::refresh() {
    if (m_usingCustomFiles && !m_paletteFile.isEmpty() && !m_qssFile.isEmpty()) {
        applyCustomTheme();
    } else {
        applyPresetPalette(m_currentPreset);
    }
}