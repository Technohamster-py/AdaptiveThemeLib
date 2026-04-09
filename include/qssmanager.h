#ifndef ADAPTIVETHEMELIB_QSSMANAGER_H
#define ADAPTIVETHEMELIB_QSSMANAGER_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QPalette>

class QssManager : public QObject{
    Q_OBJECT
public:
    enum class PresetQss {System, Material, Classic, Modern, LiquidGlass};
    static QString presetName(PresetQss preset);
    static QssManager& instance();

    void applyCurrentStyleSheet();
    void dropStyleSheet();

    void setVariable(const QString& varName, const QColor& color);
    void setVariable(const QString& varName, const QString& value);
    void setVariable(const QString& varName, int value);
    void setPalette(const QPalette& palette);

    QHash<QString,QString> variables() const {return m_variables;};
    QString currentQss() const {return m_currentStyleSheet;};
signals:
    void styleSheetUpdated();

public slots:
    void applyQssFromFile(const QString& fileName);
    void applyPreset(const PresetQss& preset);
    void applyQss(const QString& qss);
    void refreshFromPalette(const QPalette& palette);

protected:
    bool loadQssFromFile(const QString& fileName);

private:
    QssManager() = default;
    ~QssManager() = default;
    QssManager(const QssManager&) = delete;
    QssManager& operator=(const QssManager&) = delete;

    QString processVariables(const QString& qss);
    void initDefaultVariables(const QPalette& palette);

    QString m_currentStyleSheet;
    QString m_raw;
    QHash<QString,QString> m_variables;
};


#endif //ADAPTIVETHEMELIB_QSSMANAGER_H