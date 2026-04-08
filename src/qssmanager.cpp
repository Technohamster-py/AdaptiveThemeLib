#include "qssmanager.h"
#include <QFile>
#include <QApplication>
#include <QRegularExpression>
#include <QDebug>

QssManager& QssManager::instance() {
    static QssManager instance;
    return instance;
}

bool QssManager::loadQssFromFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QString rawQss = QString::fromUtf8(file.readAll());
    file.close();

    m_currentFile = fileName;
    m_currentStyleSheet = processVariables(rawQss);
    return true;
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
    qApp->setStyleSheet("");
    emit styleSheetUpdated();
}

void QssManager::refreshFromPalette(const QPalette &palette) {
    initDefaultVariables(palette);
    if (!m_currentStyleSheet.isEmpty()) applyCurrentStyleSheet();
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

