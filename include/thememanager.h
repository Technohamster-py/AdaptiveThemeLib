#ifndef ADAPTIVETHEMELIB_THEMEMANAGER_H
#define ADAPTIVETHEMELIB_THEMEMANAGER_H

#include "palettemanager.h"
#include "qssmanager.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(themeCategory)

class ThemeManager : public QObject{
    Q_OBJECT
public:
    static ThemeManager& instance();

    bool addCustomPalette(const QString& fileName);
    bool addCustomStyle(const QString& fileName);

    bool applyStyle(const QString& styleName);
    bool applyPalette(const QString& paletteName);

    QList<QssManager::StyleInfo> availableStyles() const {return QssManager::instance().availableStyles();};
    QStringList availablePalettes() const {return PaletteManager::instance().availablePalettes();};
    void resetToSystemTheme();

signals:
    void paletteChanged(const QString& fileName);
    void stylesheetChanged(const QString &fileName);
    void themeChanged();

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