#ifndef ADAPTIVETHEMELIB_THEMEMANAGER_H
#define ADAPTIVETHEMELIB_THEMEMANAGER_H

#include "palettemanager.h"
#include "qssmanager.h"

class ThemeManager : public QObject{
    Q_OBJECT
public:
    static ThemeManager& instance();

    void setPaletteFile(const QString& paletteFilePath);
    void setPalettePreset(PaletteManager::PresetPalette preset);

    void setStyleSheetFile(const QString& stylesheetFilePath);
    void setStyleSheetPreset(QssManager::PresetQss preset);

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

    PaletteManager::PresetPalette m_currentPalettePreset = PaletteManager::PresetPalette::System;
    QssManager::PresetQss m_currentQssPreset = QssManager::PresetQss::System;
};


#endif //ADAPTIVETHEMELIB_THEMEMANAGER_H