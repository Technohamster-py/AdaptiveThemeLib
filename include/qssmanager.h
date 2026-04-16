#ifndef ADAPTIVETHEMELIB_QSSMANAGER_H
#define ADAPTIVETHEMELIB_QSSMANAGER_H

#include <QApplication>
#include <QObject>
#include <QString>
#include <QHash>
#include <QPalette>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qssCategory)

class QssManager : public QObject{
    Q_OBJECT
public:
    enum class PresetQss {System, Material, Classic, Modern, Undefined};
    Q_ENUM(PresetQss)
    enum class StyleType{Native, Qss};
    struct  StyleInfo {
        QString name;
        StyleType type;
    };

    static QString presetName(PresetQss preset);
    static PresetQss stringToPreset(const QString& name);

    static QssManager& instance();

    QList<StyleInfo> availableStyles();

    QList<StyleInfo> nativeStyles() const {return m_nativeStyles.values();}
    QList<StyleInfo> qssStyles() const {return m_qssStyles.values();}

    bool applyStyle(const QString& styleName);
    bool applyNativeStyle(const QString& styleName);
    bool applyQssStyle(const QString& styleName);

    void applyCurrentStyleSheet();
    void dropStyleSheet();

    void setVariable(const QString& varName, const QString& value);
    void setVariable(const QString& varName, const QColor& color);
    void setVariable(const QString& varName, int value);

    void setUserQssDirectory(const QString& dir);
    QString userQssDirectory() const {return m_userQssDirectory;};

signals:
    void styleSheetUpdated();
    void nativeStyleUpdated(const QString& styleName);
    void qssStyleUpdated(const QString& styleName);
    void styleChanged(const QString& styleName, StyleType type);
    void userDirectoryChanged(const QString& dir);

public slots:
    void applyQssFromFile(const QString& fileName);
    void applyPreset(const PresetQss& preset);
    void refreshFromPalette(const QPalette& palette);

protected:
    bool loadQssFromFile(const QString& fileName);

private:
    QssManager();
    ~QssManager() = default;
    QssManager(const QssManager&) = delete;
    QssManager& operator=(const QssManager&) = delete;

    void scanNativeStyles();
    void scanQssStyles();

    QString processVariables(const QString& qss);
    void initDefaultVariables(const QPalette& palette);

    QString m_userQssDirectory = QApplication::applicationDirPath() + "/themes/";

    QString m_currentStyleName;
    QString m_currentStyleSheet;
    QString m_currentFile;
    StyleType m_currentStyleType = StyleType::Native;

    QHash<QString, StyleInfo> m_nativeStyles;
    QHash<QString, StyleInfo> m_qssStyles;
    QHash<QString, QString> m_customStylesheets;
    QHash<QString,QString> m_variables;
};


#endif //ADAPTIVETHEMELIB_QSSMANAGER_H