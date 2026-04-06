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
    m_usingCustomPalette = true;

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
    m_usingCustomStylesheet = true;

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
    m_currentPalettePreset = preset;
    m_usingCustomPalette = false;
    PaletteManager::instance().applyPreset(preset);
    QssManager::instance().refreshFromPalette(qApp->palette());
}

void ThemeManager::applyPresetStyleSheet(QssManager::PresetQss preset) {
    m_currentQssPreset = preset;
    m_usingCustomStylesheet = false;
    QssManager::instance().applyPreset(preset);
    QssManager::instance().refreshFromPalette(qApp->palette());
}

void ThemeManager::resetToSystemTheme() {
    PaletteManager::instance().applyPreset(PaletteManager::PresetPalette::System);
    QssManager::instance().applyPreset(QssManager::PresetQss::System);
}


void ThemeManager::refresh() {
    qDebug() << "Refreshing theme state";

    /// Both Qss and Palette are custom
    if (m_usingCustomStylesheet && m_usingCustomPalette && !m_qssFile.isEmpty() && !m_paletteFile.isEmpty()) {
        qDebug() << "Applying custom theme state with files: \n\t palette:" << m_paletteFile << "\n\t qss:" << m_qssFile;
        applyCustomTheme();
        return;
    }

    /// Palette is custom, Qss from preset
    if (m_usingCustomPalette && !m_paletteFile.isEmpty()) {
        qDebug() << "Applying preset qss with custom palette: \t palette:" << m_paletteFile;
        PaletteManager::instance().loadFromXml(m_paletteFile);
        QssManager::instance().applyPreset(m_currentQssPreset);
        QssManager::instance().refreshFromPalette(qApp->palette());
        emit themeChanged();
        return;
    }

    /// Qss is custom, palette from preset
    if (m_usingCustomStylesheet && !m_qssFile.isEmpty()) {
        qDebug() << "Applying preset palette with custom : \t qss:" << m_paletteFile;
        QssManager::instance().loadQssFromFile(m_qssFile);
        PaletteManager::instance().applyPreset(m_currentPalettePreset);
        QssManager::instance().refreshFromPalette(qApp->palette());
        emit themeChanged();
        return;
    }

    /// Both Qss and Palette are preset
    if (!m_usingCustomPalette && !m_usingCustomStylesheet) {
        qDebug() << "Applying preset palette and qss";
        QssManager::instance().applyPreset(m_currentQssPreset);
        PaletteManager::instance().applyPreset(m_currentPalettePreset);
        QssManager::instance().refreshFromPalette(qApp->palette());
        return;
    }

    qWarning() << "Unknown palette and qss state, falling back to system";
    resetToSystemTheme();
}