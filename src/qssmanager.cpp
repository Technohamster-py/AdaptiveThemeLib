#include "qssmanager.h"
#include <QFile>
#include <QApplication>
#include <QRegularExpression>
#include <QDebug>
#include <QDir>
#include <QStyleFactory>

QString QssManager::presetName(PresetQss preset) {
    switch (preset) {
        case PresetQss::Material: return "Material";
        case PresetQss::Classic: return "Classic";
        case PresetQss::Modern: return "Modern";
        default: return "Unknown";
    }
}

QssManager::PresetQss QssManager::stringToPreset(const QString &name) {
    static QMap<QString, PresetQss> presetMap = {
        {"System", PresetQss::System},
        {"Material", PresetQss::Material},
        {"Classic", PresetQss::Classic},
        {"Modern", PresetQss::Modern},
    };
    return presetMap.value(name, PresetQss::Undefined);
}

QssManager& QssManager::instance() {
    static QssManager instance;
    return instance;
}

QList<QssManager::StyleInfo> QssManager::availableStyles() {
    scanNativeStyles();
    scanQssStyles();
    QList<StyleInfo> result;
    result.append(m_nativeStyles.values());
    result.append(m_qssStyles.values());
    return result;
}

bool QssManager::applyStyle(const QString &styleName) {
    if (m_nativeStyles.contains(styleName)) {
        return applyNativeStyle(styleName);
    }

    if (m_qssStyles.contains(styleName)) {
        return applyQssStyle(styleName);
    }

    qWarning() << "Style not found: " << styleName;
    return false;
}

bool QssManager::applyNativeStyle(const QString &styleName) {
    if (!m_nativeStyles.contains(styleName)) {
        qWarning() << "Native style not found: " << styleName;
        return false;
    }
    dropStyleSheet();

    QStyle *style = QStyleFactory::create(styleName);
    if (style) {
        qApp->setStyle(style);
        m_currentStyleType = StyleType::Native;
        m_currentStyleName = styleName;

        emit nativeStyleUpdated(styleName);
        emit styleChanged(styleName, StyleType::Native);

        qDebug() << "Applying native style" << styleName;
        return true;
    }
    qWarning() << "Error while applying native style: " << styleName;
    return false;
}

bool QssManager::applyQssStyle(const QString &styleName) {
    if (!m_qssStyles.contains(styleName)) {
        qWarning() << "Qss style not found: " << styleName;
        return false;
    }

    if (m_customStylesheets.contains(styleName)) {
        applyQssFromFile(m_customStylesheets[styleName]);
    }
    else {
        applyPreset(stringToPreset(styleName));
    }

    m_currentStyleType = StyleType::Qss;
    m_currentStyleName = styleName;

    emit styleChanged(styleName, StyleType::Qss);
    emit qssStyleUpdated(styleName);

    return true;
}

bool QssManager::loadQssFromFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.exists()) {
        qWarning() << "File" << fileName << "does not exist";
        return false;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open file" << fileName << "because of" << file.errorString();
        return false;
    }

    QString rawQss = QString::fromUtf8(file.readAll());
    file.close();

    m_currentFile = fileName;
    m_currentStyleSheet = processVariables(rawQss);
    return true;
}

QssManager::QssManager() {
    scanNativeStyles();
    scanQssStyles();
}

void QssManager::applyQssFromFile(const QString &fileName) {
    if (loadQssFromFile(fileName))
        applyCurrentStyleSheet();
    else
        qWarning() << "Unable to load stylesheet from file " << fileName;
}

void QssManager::applyPreset(const PresetQss& preset) {
    QString fileName;
    switch (preset) {
        case PresetQss::Material:
            fileName = ":/material.qss";
            break;
        case PresetQss::Modern:
            fileName = ":/modern.qss";
            break;
        case PresetQss::Classic:
            fileName = ":/classic.qss";
            break;
        default:
            dropStyleSheet();
            return;
    }
    applyQssFromFile(fileName);
}

void QssManager::applyCurrentStyleSheet() {
    if (!m_currentStyleSheet.isEmpty()) qApp->setStyleSheet(m_currentStyleSheet);
    emit styleSheetUpdated();
}

void QssManager::dropStyleSheet() {
    m_currentStyleSheet = "";
    m_currentFile = "";
    qApp->setStyleSheet(QString());
    emit styleSheetUpdated();
}

void QssManager::refreshFromPalette(const QPalette &palette) {
    initDefaultVariables(palette);
    if (!m_currentFile.isEmpty()) {
        loadQssFromFile(m_currentFile);
        applyCurrentStyleSheet();
    }
    if (!m_currentStyleSheet.isEmpty())
        applyCurrentStyleSheet();
}

void QssManager::setVariable(const QString &varName, const QString &value) {
    m_variables[varName] = value;
    if (!m_currentStyleSheet.isEmpty()) loadQssFromFile(m_currentFile);
}

void QssManager::setVariable(const QString &varName, const QColor &color) {
    setVariable(varName, color.name());
}

void QssManager::setVariable(const QString &varName, int value) {
    setVariable(varName, QString::number(value));
}

void QssManager::setUserQssDirectory(const QString &dir) {
    m_userQssDirectory = dir;
    emit userDirectoryChanged(dir);
}

void QssManager::scanNativeStyles() {
    m_nativeStyles.clear();
    QStringList nativeStyles = QStyleFactory::keys();

    for (const QString &styleName : nativeStyles) {
        StyleInfo styleInfo;
        styleInfo.name = styleName;
        styleInfo.type = StyleType::Native;

        m_nativeStyles[styleName] = styleInfo;
    }
}

void QssManager::scanQssStyles() {
    m_qssStyles.clear();
    QList<PresetQss> presets = {PresetQss::Material, PresetQss::Classic, PresetQss::Modern};

    for (PresetQss preset : presets) {
        StyleInfo info;
        info.name = presetName(preset);
        info.type = StyleType::Qss;

        m_qssStyles[info.name] = info;
    }

    QDir dir(m_userQssDirectory);
    if (!dir.exists()) {
        qWarning() << "Specified user qss directory"<< m_userQssDirectory << " does not exist";
        return;
    }

    QStringList qssFiles = dir.entryList(QStringList() << "*.qss",QDir::Files);
    for (const QString &fileName : qssFiles) {
        QString themeName = QFileInfo(fileName).baseName();
        if (m_qssStyles.contains(themeName)) continue;

        StyleInfo info;
        info.name = fileName;
        info.type = StyleType::Qss;

        m_qssStyles[info.name] = info;
        m_customStylesheets[themeName] = fileName;
    }
}

QString QssManager::processVariables(const QString &qss) {
    QString result = qss;

    QRegularExpression regExp("\\$([A-Za-z_][A-Za-z0-9_]*)");
    QRegularExpressionMatchIterator it = regExp.globalMatch(qss);

    QSet<QString> variables;
    while (it.hasNext()) {
        variables.insert(it.next().captured(1));
    }

    for (const QString &varName : variables) {
        if (m_variables.contains(varName)) {
            result.replace("$" + varName, m_variables[varName]);
        }
    }
    return result;
}

void QssManager::initDefaultVariables(const QPalette &palette) {
    m_variables["window"] = palette.color(QPalette::Window).name();
    m_variables["windowText"] = palette.color(QPalette::WindowText).name();
    m_variables["base"] = palette.color(QPalette::Base).name();
    m_variables["alternateBase"] = palette.color(QPalette::AlternateBase).name();
    m_variables["text"] = palette.color(QPalette::Text).name();
    m_variables["button"] = palette.color(QPalette::Button).name();
    m_variables["buttonText"] = palette.color(QPalette::ButtonText).name();
    m_variables["highlight"] = palette.color(QPalette::Highlight).name();
    m_variables["highlightedText"] = palette.color(QPalette::HighlightedText).name();
    m_variables["mid"] = palette.color(QPalette::Mid).name();
    m_variables["dark"] = palette.color(QPalette::Dark).name();
}

