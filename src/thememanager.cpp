#include "thememanager.h"
#include "palettemanager.h"
#include "qssmanager.h"
#include "themediconmanager.h"
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <qfileinfo.h>

ThemeManager& ThemeManager::instance() {
    static ThemeManager instance;
    return instance;
}

bool ThemeManager::addCustomPalette(const QString &fileName) {
    QFile file(fileName);
    if (!file.exists()) {
        qWarning() <<"ThemeManager::addCustomPalette - file does not exist" << fileName;
        return false;
    }
    QFileInfo fileInfo(file);
    if (!(fileInfo.suffix() == "xml")) {
        qWarning() << "Invalid file extension: " << fileName;
        return false;
    }
    QString dir = PaletteManager::instance().userPaletteDir();
    QString dest = dir + fileInfo.fileName();

    QDir destDir(dir);
    if (!destDir.exists()) destDir.mkpath(".");

    return QFile::copy(fileName, dest);
}

bool ThemeManager::addCustomStyle(const QString &fileName) {
    QFile file(fileName);
    if (!file.exists()) {
        qWarning() <<"ThemeManager::addCustomStyle - file does not exist" << fileName;
        return false;
    }
    QFileInfo fileInfo(file);
    if (!(fileInfo.suffix() == "qss")) {
        qWarning() << "Invalid file extension: " << fileName;
        return false;
    }
    QString dir = QssManager::instance().userQssDirectory();
    QString dest = dir + fileInfo.fileName();

    QDir destDir(dir);
    if (!destDir.exists()) destDir.mkpath(".");

    return QFile::copy(fileName, dest);
}

bool ThemeManager::applyStyle(const QString &styleName) {
    return QssManager::instance().applyStyle(styleName);
}

bool ThemeManager::applyPalette(const QString &paletteName) {
    return PaletteManager::instance().applyPalette(paletteName);
}

ThemeManager::ThemeManager() : QObject() {
    auto& paletteManager = PaletteManager::instance();
    auto& qssManager = QssManager::instance();

    connect(&paletteManager, &PaletteManager::paletteChanged, &qssManager, &QssManager::refreshFromPalette);
}

void ThemeManager::resetToSystemTheme() {
    PaletteManager::instance().resetToSystemPalette();
    QssManager::instance().dropStyleSheet();
}