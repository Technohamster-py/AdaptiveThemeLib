#include <QtTest/QtTest>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include "palettemanager.h"


class TestPaletteManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "=== Starting PaletteManager tests ===";
        int fakeArgc = 1;
        char fakeArgv[] = {"test"};
        char* fakeArgvPtr = fakeArgv;
        m_app = new QApplication(fakeArgc, &fakeArgvPtr);
    }

    void cleanupTestCase() {
        qDebug() << "=== PaletteManager tests finished ===";
        delete m_app;
    }

    void init() {
        m_manager = &PaletteManager::instance();
        m_manager->resetToSystemPalette();
    }

    void cleanup() {

    }

    void testSingleton() {
        PaletteManager& instance1 = PaletteManager::instance();
        PaletteManager& instance2 = PaletteManager::instance();
        QCOMPARE(&instance1, &instance2);
    }

    void testApplyLightPreset() {
        m_manager->applyPreset(PaletteManager::PresetPalette::Light);
        QPalette palette = m_manager->currentPalette();

        QCOMPARE(palette.color(QPalette::Window), QColor("#f5f5f5"));
        QCOMPARE(palette.color(QPalette::WindowText), Qt::black);
        QCOMPARE(palette.color(QPalette::Base), Qt::white);
        QCOMPARE(palette.color(QPalette::Highlight), QColor("#2196f3"));

        QCOMPARE(qApp->palette().color(QPalette::Window), QColor("#f5f5f5"));
    }

    void testApplyDarkPreset() {
        m_manager->applyPreset(PaletteManager::PresetPalette::Dark);
        QPalette palette = m_manager->currentPalette();

        QCOMPARE(palette.color(QPalette::Window), QColor("#1e1f22"));
        QCOMPARE(palette.color(QPalette::WindowText), Qt::white);
        QCOMPARE(palette.color(QPalette::Text), QColor("#e0e0e0"));
        QCOMPARE(palette.color(QPalette::Highlight), QColor("#3f51b5"));
    }

    void testResetToSystem() {
        m_manager->applyPreset(PaletteManager::PresetPalette::Dark);
        QPalette darkPalette = m_manager->currentPalette();

        m_manager->resetToSystemPalette();
        QPalette systemPalette = m_manager->currentPalette();

        QVERIFY(systemPalette != darkPalette);

        QVERIFY(qApp->palette() != darkPalette);
    }

    void testLoadFromValidXml() {
        QString xmlContent =
            "<?xml version=\"1.0\"?>"
            "<palette>"
            "  <active>"
            "    <colorrole role=\"Window\">"
            "      <color><red>255</red><green>0</green><blue>0</blue></color>"
            "    </colorrole>"
            "    <colorrole role=\"Text\">"
            "      <color><red>0</red><green>255</green><blue>0</blue></color>"
            "    </colorrole>"
            "  </active>"
            "</palette>";

        QTemporaryFile tempFile;
        tempFile.open();
        tempFile.write(xmlContent.toUtf8());
        tempFile.close();

        bool result = m_manager->loadFromXml(tempFile.fileName());
        QVERIFY(result == true);

        QPalette palette = m_manager->currentPalette();
        QCOMPARE(palette.color(QPalette::Active, QPalette::Window), QColor(255, 0, 0));
        QCOMPARE(palette.color(QPalette::Active, QPalette::Text), QColor(0, 255, 0));
    }

    void testLoadFromInvalidXml() {
        QString invalidXml = "<?xml version=\"1.0\"?><broken>tag";

        QTemporaryFile tempFile;
        tempFile.open();
        tempFile.write(invalidXml.toUtf8());
        tempFile.close();

        bool result = m_manager->loadFromXml(tempFile.fileName());
        QVERIFY(result == false);

        QPalette original = m_manager->currentPalette();
        QCOMPARE(qApp->palette(), original);
    }

    void testLoadFromNonExistentFile() {
        bool result = m_manager->loadFromXml("/path/to/nonexistent/file.xml");
        QVERIFY(result == false);
    }

    void testSignalEmission() {
        QSignalSpy spy(m_manager, &PaletteManager::paletteChanged);

        m_manager->applyPreset(PaletteManager::PresetPalette::Light);

        QCOMPARE(spy.count(), 1);

        QList<QVariant> arguments = spy.takeFirst();
        const QPalette emittedPalette = arguments.at(0).value<QPalette>();
        QCOMPARE(emittedPalette.color(QPalette::Window), QColor("#f5f5f5"));
    }

    void testCurrentPalette() {
        QPalette before = m_manager->currentPalette();
        m_manager->applyPreset(PaletteManager::PresetPalette::Dark);
        QPalette after = m_manager->currentPalette();

        QVERIFY(before != after);
        QCOMPARE(after.color(QPalette::Window), QColor("#1e1f22"));
    }

private:
    PaletteManager* m_manager;
    QApplication* m_app;
};

int testPaletteManager() {
    TestPaletteManager test;
    return QTest::qExec(&test);
}