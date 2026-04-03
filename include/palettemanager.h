#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QPalette>
#include <QShortcut>
#include <QMap>
#include <QColor>
#include <QApplication>


static QMap<QString, QPalette::ColorRole> roleMap = {
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

static QMap<QString, QPalette::ColorGroup> groupMap = {
        {"active", QPalette::Active},
        {"inactive", QPalette::Inactive},
        {"disabled", QPalette::Disabled}
};

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
    enum class PresetPalette{System, Light, Dark};

    static PaletteManager& instance();

    void applyPreset(PresetPalette theme);
    void applyPalette(const QPalette& palette);

    void loadFromXml(const QString &path);
    void resetToSystemPalette();

    QPalette currentPalette() const {return m_currentPalette;};

    static void refreshStyleSheet();

signals:
    void paletteChanged(const QPalette& palette);
    void paletteRequested(const QPalette& palette);


private:
    PaletteManager() = default;
    ~PaletteManager() = default;
    PaletteManager(const PaletteManager&) = delete;
    PaletteManager& operator=(const PaletteManager&) = delete;

    static QString generateStyleSheet(const QPalette &palette);
    static  void updateIconManager();

    QPalette m_currentPalette;
    PresetPalette m_currentPreset = PresetPalette::System;
};


#endif //THEMEMANAGER_H
