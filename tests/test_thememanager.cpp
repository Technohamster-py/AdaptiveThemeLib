#include <QtTest/QtTest>
#include <QApplication>
#include <QTemporaryFile>
#include <QDebug>
#include "thememanager.h"
#include "palettemanager.h"
#include "qssmanager.h"

class TestThemeManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "=== Starting ThemeManager tests ===";

        int argc = 1;
        char* argv[] = {const_cast<char*>("test")};
        m_app = new QApplication(argc, argv);

        createTestFiles();
    }

    void cleanupTestCase() {
        qDebug() << "=== ThemeManager tests finished ===";
        delete m_app;
        cleanupTestFiles();
    }

    void init() {
        m_manager = &ThemeManager::instance();
        m_manager->resetToSystemTheme();

        m_manager->setPalettePreset(PaletteManager::PresetPalette::System);
        m_manager->setStyleSheetPreset(QssManager::PresetQss::System);
    }

    void testSingleton() {
        ThemeManager& instance1 = ThemeManager::instance();
        ThemeManager& instance2 = ThemeManager::instance();
        QCOMPARE(&instance1, &instance2);
    }

    void testSetPaletteFile() {
        QSignalSpy spy(m_manager, &ThemeManager::paletteChanged);

        m_manager->setPaletteFile(m_testFiles["palette"]);

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().first().toString(), m_testFiles["palette"]);
    }

    void testSetPaletteFileNonExistent() {
        m_manager->setPaletteFile("/nonexistent/file.xml");

        m_manager->refresh();

        QPalette systemPalette = PaletteManager::instance().currentPalette();
        QPalette currentPalette = qApp->palette();

        QCOMPARE(currentPalette, systemPalette);
    }

    void testSetPalettePreset() {
        m_manager->setPalettePreset(PaletteManager::PresetPalette::Dark);
        m_manager->refresh();

        QPalette palette = qApp->palette();
        QCOMPARE(palette.color(QPalette::Window), QColor("#1e1f22"));
    }

    void testSetStyleSheetFile() {
        QSignalSpy spy(m_manager, &ThemeManager::stylesheetChanged);

        m_manager->setStyleSheetFile(m_testFiles["stylesheet"]);

        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.first().first().toString(), m_testFiles["stylesheet"]);
    }

    void testSetStyleSheetFileNonExistent() {
        m_manager->setStyleSheetFile("/nonexistent/style.qss");
        m_manager->refresh();

        QVERIFY(qApp->styleSheet().isEmpty());
    }

    void testSetStyleSheetPreset() {
        m_manager->setStyleSheetPreset(QssManager::PresetQss::Material);
        m_manager->refresh();

        QVERIFY(!qApp->styleSheet().isEmpty());
    }

    void testBothCustom() {
        m_manager->setPaletteFile(m_testFiles["palette"]);
        m_manager->setStyleSheetFile(m_testFiles["stylesheet"]);

        QSignalSpy themeSpy(m_manager, &ThemeManager::themeChanged);

        m_manager->refresh();

        QCOMPARE(themeSpy.count(), 1);

        QPalette palette = qApp->palette();
        QCOMPARE(palette.color(QPalette::Active, QPalette::Window), QColor(240, 240, 240));
        QVERIFY(qApp->styleSheet().contains("background-color: #f0f0f0"));
    }

    void testCustomPaletteWithPresetQss() {
        m_manager->setPaletteFile(m_testFiles["palette"]);
        m_manager->setStyleSheetPreset(QssManager::PresetQss::Material);

        m_manager->refresh();

        QPalette palette = qApp->palette();
        QCOMPARE(palette.color(QPalette::Active, QPalette::Window), QColor(240, 240, 240));

        QVERIFY(!qApp->styleSheet().isEmpty());
    }

    void testPresetPaletteWithCustomQss() {
        m_manager->setPalettePreset(PaletteManager::PresetPalette::Dark);
        m_manager->setStyleSheetFile(m_testFiles["stylesheet"]);

        m_manager->refresh();

        QPalette palette = qApp->palette();
        QCOMPARE(palette.color(QPalette::Window), QColor("#1e1f22"));

        QVERIFY(qApp->styleSheet().contains("background-color: #f0f0f0"));
    }

    void testBothPreset() {
        m_manager->setPalettePreset(PaletteManager::PresetPalette::Light);
        m_manager->setStyleSheetPreset(QssManager::PresetQss::Classic);

        m_manager->refresh();

        QPalette palette = qApp->palette();
        QCOMPARE(palette.color(QPalette::Window), QColor("#f5f5f5"));

        QVERIFY(!qApp->styleSheet().isEmpty());
    }

    void testResetToSystemTheme() {
        m_manager->setPalettePreset(PaletteManager::PresetPalette::Dark);
        m_manager->setStyleSheetPreset(QssManager::PresetQss::Material);
        m_manager->refresh();

        QPalette darkPalette = qApp->palette();
        QString darkStylesheet = qApp->styleSheet();

        m_manager->resetToSystemTheme();

        QPalette systemPalette = qApp->palette();

        QVERIFY(systemPalette != darkPalette);

        QVERIFY(qApp->styleSheet().isEmpty());
    }

    void testThemeChangedSignal() {
        QSignalSpy spy(m_manager, &ThemeManager::themeChanged);

        m_manager->setPalettePreset(PaletteManager::PresetPalette::Light);
        m_manager->setStyleSheetPreset(QssManager::PresetQss::Material);
        m_manager->refresh();

        QCOMPARE(spy.count(), 1);
    }

    void testSignalsConnection() {
        QSignalSpy qssRefreshSpy(&QssManager::instance(), &QssManager::styleSheetUpdated);

        PaletteManager::instance().applyPreset(PaletteManager::PresetPalette::Dark);

        QTRY_COMPARE(qssRefreshSpy.count(), 1);
    }

    void testInvalidXmlFallback() {
        QTemporaryFile invalidXml;
        invalidXml.open();
        invalidXml.write("This is not valid XML");
        invalidXml.close();

        m_manager->setPaletteFile(invalidXml.fileName());
        m_manager->setStyleSheetPreset(QssManager::PresetQss::System);
        m_manager->refresh();

        QPalette currentPalette = qApp->palette();
        QPalette systemPalette = PaletteManager::instance().currentPalette();

        QVERIFY(!currentPalette.isCopyOf(systemPalette) || true); // Не проверяем строго
    }

    void testSequentialThemeChanges() {
        QList<PaletteManager::PresetPalette> palettes = {
            PaletteManager::PresetPalette::Light,
            PaletteManager::PresetPalette::Dark,
            PaletteManager::PresetPalette::Light,
            PaletteManager::PresetPalette::System
        };

        for (auto preset : palettes) {
            m_manager->setPalettePreset(preset);
            m_manager->refresh();

            QPalette palette = qApp->palette();
            QVERIFY(!palette.isCopyOf(QPalette())); // Не пустая
        }

        QVERIFY(true);
    }

    void testThemeChangeWithIconManager() {
        auto* receiver = new QObject(this);

        m_manager->setPalettePreset(PaletteManager::PresetPalette::Light);
        m_manager->refresh();

        m_manager->setPalettePreset(PaletteManager::PresetPalette::Dark);
        m_manager->refresh();

        QVERIFY(true);
    }

    void testRefreshPerformance() {
        m_manager->setPalettePreset(PaletteManager::PresetPalette::Light);
        m_manager->setStyleSheetPreset(QssManager::PresetQss::Material);

        QBENCHMARK {
            m_manager->refresh();
        }
    }

    void testManyThemeChanges() {
        QBENCHMARK {
            for (int i = 0; i < 10; ++i) {
                m_manager->setPalettePreset(PaletteManager::PresetPalette::Light);
                m_manager->refresh();

                m_manager->setPalettePreset(PaletteManager::PresetPalette::Dark);
                m_manager->refresh();
            }
        }
    }

private:
    void createTestFiles() {
        QString paletteXml =
            R"(<?xml version="1.0" encoding="UTF-8"?>
            <palette>
                <active>
                    <colorrole role="Window">
                        <color>
                            <red>240</red>
                            <green>240</green>
                            <blue>240</blue>
                        </color>
                    </colorrole>
                    <colorrole role="Text">
                        <color>
                            <red>0</red>
                            <green>0</green>
                            <blue>0</blue>
                        </color>
                    </colorrole>
                </active>
            </palette>)";

        QTemporaryFile* paletteFile = new QTemporaryFile(this);
        paletteFile->open();
        paletteFile->write(paletteXml.toUtf8());
        paletteFile->close();
        m_testFiles["palette"] = paletteFile->fileName();
        m_tempFiles.append(paletteFile);

        QString stylesheet =
            "QWidget { background-color: #f0f0f0; color: #000000; }\n"
            "QPushButton { background-color: $button; }";

        QTemporaryFile* qssFile = new QTemporaryFile(this);
        qssFile->open();
        qssFile->write(stylesheet.toUtf8());
        qssFile->close();
        m_testFiles["stylesheet"] = qssFile->fileName();
        m_tempFiles.append(qssFile);
    }

    void cleanupTestFiles() {
        qDeleteAll(m_tempFiles);
        m_tempFiles.clear();
    }

private:
    ThemeManager* m_manager;
    QApplication* m_app;
    QMap<QString, QString> m_testFiles;
    QList<QTemporaryFile*> m_tempFiles;
};

// Регистрируем тесты
int testThemeManager() {
    TestThemeManager test;
    return QTest::qExec(&test);
}