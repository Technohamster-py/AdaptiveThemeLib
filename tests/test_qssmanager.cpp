#include <QtTest/QtTest>
#include <QApplication>
#include <QTemporaryFile>
#include <QFile>
#include <QString>
#include "qssmanager.h"


class TestQssManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "=== Starting QssManager tests ===";
        int fakeArgc = 1;
        char fakeArgv[] = {"test"};
        char* fakeArgvPtr = fakeArgv;
        m_app = new QApplication(fakeArgc, &fakeArgvPtr);
    }

    void cleanupTestCase() {
        delete m_app;
    }

    void init() {
        m_manager = &QssManager::instance();
        m_manager->dropStyleSheet();
        m_manager->setPalette(qApp->palette());
    }

    void testSingleton() {
        QssManager& instance1 = QssManager::instance();
        QssManager& instance2 = QssManager::instance();
        QCOMPARE(&instance1, &instance2);
    }

    void testSetAndGetVariable() {
        m_manager->setVariable("testColor", QString("#ff0000"));
        m_manager->setVariable("testSize", 42);

        QString qss = "color: $testColor; font-size: ${testSize}px;";
        m_manager->applyQss(qss);
        QString processed = m_manager->currentQss();

        QCOMPARE(processed, "color: #ff0000; font-size: 42px;");
    }

    void testVariableReplacement() {
        m_manager->setVariable("bg", QString("#ffffff"));
        m_manager->setVariable("fg", QString("#000000"));

        QString input = "background: $bg; color: $fg; border: 1px solid $bg;";
        QString expected = "background: #ffffff; color: #000000; border: 1px solid #ffffff;";

        m_manager->applyQss(input);
        QString result = m_manager->currentQss();
        QCOMPARE(result, expected);
    }

    void testLoadFromValidQss() {
        QTemporaryFile tempFile;
        tempFile.open();
        tempFile.write("QWidget { background: $window; color: $text; }");
        tempFile.close();

        m_manager->setVariable("window", QString("#123456"));
        m_manager->setVariable("text", QString("#654321"));

        m_manager->applyQssFromFile(tempFile.fileName());

        QString expected = "QWidget { background: #123456; color: #654321; }";
        QCOMPARE(qApp->styleSheet(), expected);
    }

    void testLoadFromInvalidFile() {
        m_manager->applyQssFromFile("/invalid/path.qss");
        QVERIFY(m_manager->currentQss().isEmpty() == true);
    }

    void testPresetNames() {
        QCOMPARE(QssManager::presetName(QssManager::PresetQss::System), QString("System"));
        QCOMPARE(QssManager::presetName(QssManager::PresetQss::Material), QString("Material"));
        QCOMPARE(QssManager::presetName(QssManager::PresetQss::Classic), QString("Classic"));
        QCOMPARE(QssManager::presetName(QssManager::PresetQss::Modern), QString("Modern"));
        QCOMPARE(QssManager::presetName(QssManager::PresetQss::LiquidGlass), QString("LiquidGlass"));
    }

    void testDropStyleSheet() {
        m_manager->applyQssFromFile(":/material.qss");
        QVERIFY(!qApp->styleSheet().isEmpty());

        m_manager->dropStyleSheet();
        QVERIFY(qApp->styleSheet().isEmpty());
    }

    void testSignalEmission() {
        QSignalSpy spy(m_manager, &QssManager::styleSheetUpdated);

        m_manager->applyQssFromFile(":/material.qss");
        QCOMPARE(spy.count(), 1);

        m_manager->dropStyleSheet();
        QCOMPARE(spy.count(), 2);
    }

    void testInitDefaultVariables() {
        QPalette testPalette;
        testPalette.setColor(QPalette::Window, QColor("#abcdef"));

        m_manager->setPalette(testPalette);

        // Проверяем через processVariables
        m_manager->applyQss("background: $window;");
        QString result = m_manager->currentQss();
        QCOMPARE(result, "background: #abcdef;");
    }

private:
    QssManager* m_manager;
    QApplication* m_app;
};

int testQssManager() {
    TestQssManager test;
    return QTest::qExec(&test);
}

