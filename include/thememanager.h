#ifndef ADAPTIVETHEMELIB_THEMEMANAGER_H
#define ADAPTIVETHEMELIB_THEMEMANAGER_H

#include "palettemanager.h"

class ThemeManager : public QObject{
    Q_OBJECT
public:

    static ThemeManager& instance();

    void applyPresetPalette(PaletteManager::PresetPalette preset);
    void setPaletteFile(const QString& paletteFilePath);
    void setStyleSheetFile(const QString& stylesheetFilePath);
    void applyCustomTheme();
    void resetToSystemTheme();
    void refresh();
signals:
    void paletteChanged(const QString& fileName);
    void stylesheetChanged(const QString &fileName);
    void themeChanged();
    void themeRefreshRequested();

private:
    ThemeManager();
    ~ThemeManager() = default;
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    QString m_paletteFile;
    QString m_qssFile;
    PaletteManager::PresetPalette m_currentPreset = PaletteManager::PresetPalette::System;
    bool m_usingCustomFiles = false;
};


#endif //ADAPTIVETHEMELIB_THEMEMANAGER_H