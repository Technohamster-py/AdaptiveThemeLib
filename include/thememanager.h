#ifndef ADAPTIVETHEMELIB_THEMEMANAGER_H
#define ADAPTIVETHEMELIB_THEMEMANAGER_H

#include <QObject>
#include <QString>

class ThemeManager : public QObject{
    Q_OBJECT
public:
    enum class PresetTheme {Light, Dark, System};
    Q_ENUM(PresetTheme);

    static ThemeManager& instance();

    void applyPreset(PresetTheme preset);
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
    ThemeManager() = default;
    ~ThemeManager() = default;
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    void setupConnections();
    void applyCurrentPalette();
    void applyCurrenStyleSheet();

    QString m_paletteFile;
    QString m_qssFile;
    PresetTheme m_currentPreset = PresetTheme::System;
    bool m_usingCustomFiles = false;
};


#endif //ADAPTIVETHEMELIB_THEMEMANAGER_H