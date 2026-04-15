#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QPalette>
#include <QShortcut>
#include <QApplication>

/**
 * @class PaletteManager
 * @brief Manages application themes through preset options or custom XML-based themes.
 *
 * The ThemeManager class allows users to apply predefined "Light" or "Dark" themes
 * and also enables dynamic theme loading from custom XML files to customize the application's appearance.
 */
class PaletteManager : public QObject {
    Q_OBJECT
public:
    enum class PresetPalette{System, Light, Dark, Undefined};
    QString presetName(PresetPalette preset);
    PresetPalette presetFromName(const QString &name);

    static PaletteManager& instance();

    void resetToSystemPalette();
    QPalette currentPalette() const {return m_currentPalette;};

    void setUserPaletteDir(const QString& dir);
    QString userPaletteDir() const {return m_userPaletteDir;};

signals:
    void userDirectoryChanged(QString path);
    void paletteChanged(const QPalette& palette);

public slots:
    void applyPalette(const QPalette& palette);
    void applyPreset(PresetPalette preset);
    bool loadFromXml(const QString &path);

private:
    PaletteManager() = default;
    ~PaletteManager() = default;
    PaletteManager(const PaletteManager&) = delete;
    PaletteManager& operator=(const PaletteManager&) = delete;

    QString m_userPaletteDir = QApplication::applicationDirPath() + "/themes/";

    QPalette m_currentPalette;
    PresetPalette m_currentPreset = PresetPalette::System;
};


#endif //THEMEMANAGER_H
