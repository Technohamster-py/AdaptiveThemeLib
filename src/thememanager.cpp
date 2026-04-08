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

    connect(&paletteManager, &PaletteManager::paletteChanged, &qssManager, &QssManager::refreshFromPalette);
}

void ThemeManager::setPaletteFile(const QString& paletteFilePath) {
    if (!QFile::exists(paletteFilePath)) {
        qWarning() << "Palette file not found:" << paletteFilePath;
        return;
    }
    m_paletteFile = paletteFilePath;
    emit paletteChanged(m_paletteFile);
}

void ThemeManager::setPalettePreset(PaletteManager::PresetPalette preset) {
    m_currentPalettePreset = preset;
    m_paletteFile.clear();
}

void ThemeManager::setStyleSheetFile(const QString& qssFilePath) {
    if (!QFile::exists(qssFilePath)) {
        qWarning() << "StyleSheet file not found:" << qssFilePath;
        return;
    }
    m_qssFile = qssFilePath;
    emit stylesheetChanged(m_qssFile);
}

void ThemeManager::setStyleSheetPreset(QssManager::PresetQss preset) {
    m_currentQssPreset = preset;
    m_qssFile.clear();
}

void ThemeManager::resetToSystemTheme() {
    PaletteManager::instance().applyPreset(PaletteManager::PresetPalette::System);
    QssManager::instance().applyPreset(QssManager::PresetQss::System);
}


void ThemeManager::refresh() {
    qDebug() << "Refreshing theme state";

    /// Both Qss and Palette are custom
    if (!m_qssFile.isEmpty() && !m_paletteFile.isEmpty()) {
        qDebug() << "Applying custom theme state with files: \n\t palette:" << m_paletteFile << "\n\t qss:" << m_qssFile;
        QssManager::instance().applyQssFromFile(m_qssFile);
        PaletteManager::instance().loadFromXml(m_paletteFile);
        emit themeChanged();
        return;
    }

    /// Palette is custom, Qss from preset
    if (m_qssFile.isEmpty() && !m_paletteFile.isEmpty()) {
        qDebug() << "Applying preset qss:" << QssManager::presetName(m_currentQssPreset) << " with custom palette: \t palette:" << m_paletteFile;
        QssManager::instance().applyPreset(m_currentQssPreset);
        PaletteManager::instance().loadFromXml(m_paletteFile);
        emit themeChanged();
        return;
    }

    /// Qss is custom, palette from preset
    if (m_paletteFile.isEmpty() && !m_qssFile.isEmpty()) {
        qDebug() << "Applying preset palette with custom : \t qss:" << m_paletteFile;
        QssManager::instance().applyQssFromFile(m_qssFile);
        PaletteManager::instance().applyPreset(m_currentPalettePreset);
        emit themeChanged();
        return;
    }

    /// Both Qss and Palette are preset
    if (m_qssFile.isEmpty() && m_paletteFile.isEmpty()) {
        qDebug() << "Applying preset palette and qss";
        QssManager::instance().applyPreset(m_currentQssPreset);
        PaletteManager::instance().applyPreset(m_currentPalettePreset);
        emit themeChanged();
        return;
    }

    qWarning() << "Unknown palette and qss state, falling back to system";
    resetToSystemTheme();
}