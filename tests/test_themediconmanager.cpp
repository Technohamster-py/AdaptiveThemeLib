#include <QtTest/QtTest>
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QTemporaryFile>
#include <QPainter>
#include "themediconmanager.h"

class TestIconReceiver : public QObject {
    Q_OBJECT
public:
    QPixmap lastPixmap;
    QIcon lastIcon;

    void setPixmap(const QPixmap& pixmap) {
        lastPixmap = pixmap;
    }

    void setIcon(const QIcon& icon) {
        lastIcon = icon;
    }

    bool hasReceivedPixmap() const {
        return !lastPixmap.isNull();
    }

    bool hasReceivedIcon() const {
        return !lastIcon.isNull();
    }
};

class TestButton : public QPushButton {
    Q_OBJECT
public:
    int iconSetCount = 0;

    void setIcon(const QIcon& icon) {
        QPushButton::setIcon(icon);
        iconSetCount++;
    }
};

class TestThemedIconManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        qDebug() << "=== Starting ThemedIconManager tests ===";

        // Создаем QApplication для тестов
        int argc = 1;
        char* argv[] = {const_cast<char*>("test")};
        m_app = new QApplication(argc, argv);

        // Создаем тестовый SVG файл
        createTestSvgFiles();
    }

    void cleanupTestCase() {
        qDebug() << "=== ThemedIconManager tests finished ===";
        delete m_app;
        cleanupTestSvgFiles();
    }

    void init() {
        m_manager = &ThemedIconManager::instance();
        m_testReceiver = new TestIconReceiver();
        m_testButton = new TestButton();
    }

    void cleanup() {
        delete m_testButton;
        delete m_testReceiver;
    }

    void testSingleton() {
        ThemedIconManager& instance1 = ThemedIconManager::instance();
        ThemedIconManager& instance2 = ThemedIconManager::instance();
        QCOMPARE(&instance1, &instance2);
    }

    void testAddPixmapTarget() {
        QString svgPath = m_testSvgFiles["single"];

        m_manager->addPixmapTarget(
            svgPath,
            m_testReceiver,
            [this](const QPixmap& pixmap) {
                m_testReceiver->setPixmap(pixmap);
            },
            true,
            QSize(32, 32)
        );

        QTest::qWait(10);

        QVERIFY(m_testReceiver->hasReceivedPixmap());
        QVERIFY(!m_testReceiver->lastPixmap.isNull());
        QCOMPARE(m_testReceiver->lastPixmap.size(), QSize(32, 32));
    }

    void testAddPixmapTargetWithoutOverride() {
        QString svgPath = m_testSvgFiles["single"];

        m_manager->addPixmapTarget(
            svgPath, m_testReceiver,
            [this](const QPixmap& pixmap) { m_testReceiver->setPixmap(pixmap); },
            false, QSize(16, 16)
        );

        QTest::qWait(10);
        QSize firstSize = m_testReceiver->lastPixmap.size();

        m_manager->addPixmapTarget(
            svgPath, m_testReceiver,
            [this](const QPixmap& pixmap) { m_testReceiver->setPixmap(pixmap); },
            false, QSize(32, 32)
        );

        QTest::qWait(10);

        QCOMPARE(m_testReceiver->lastPixmap.size(), QSize(32, 32));
    }

    void testAddIconTarget() {
        QString svgPath = m_testSvgFiles["single"];

        m_manager->addIconTarget<QAbstractButton>(
            svgPath,
            m_testButton,
            &QPushButton::setIcon,
            QSize(24, 24)
        );

        QTest::qWait(10);

        QVERIFY(m_testButton->iconSetCount > 0);
        QVERIFY(!m_testButton->icon().isNull());
    }

    void testAddTargetWithInvalidPath() {
        m_manager->addPixmapTarget(
            "/invalid/path.svg",
            m_testReceiver,
            [this](const QPixmap& pixmap) { m_testReceiver->setPixmap(pixmap); }
        );

        QTest::qWait(10);

        QVERIFY(!m_testReceiver->hasReceivedPixmap());
    }

    void testAddTargetWithNullReceiver() {
        QString svgPath = m_testSvgFiles["single"];

        m_manager->addPixmapTarget(
            svgPath,
            nullptr,
            [](const QPixmap&) {}
        );

        QVERIFY(true);
    }

    void testUpdateOnPaletteChange() {
        QString svgPath = m_testSvgFiles["single"];

        m_manager->addPixmapTarget(
            svgPath, m_testReceiver,
            [this](const QPixmap& pixmap) { m_testReceiver->setPixmap(pixmap); },
            true, QSize(32, 32)
        );

        QTest::qWait(10);
        QPixmap originalPixmap = m_testReceiver->lastPixmap;

        QPalette newPalette;
        newPalette.setColor(QPalette::WindowText, Qt::red);
        qApp->setPalette(newPalette);

        QEvent paletteEvent(QEvent::ApplicationPaletteChange);
        QCoreApplication::sendEvent(qApp, &paletteEvent);

        QTest::qWait(10);

        QVERIFY(m_testReceiver->lastPixmap.toImage() != originalPixmap.toImage());
    }

    void testThemeChangedSignal() {
        QSignalSpy spy(m_manager, &ThemedIconManager::themeChanged);

        QString svgPath = m_testSvgFiles["single"];
        m_manager->addPixmapTarget(
            svgPath, m_testReceiver,
            [this](const QPixmap& pixmap) { m_testReceiver->setPixmap(pixmap); }
        );

        QTest::qWait(10);

        QEvent paletteEvent(QEvent::ApplicationPaletteChange);
        QCoreApplication::sendEvent(qApp, &paletteEvent);

        QTest::qWait(10);

        QVERIFY(spy.count() >= 1);
    }

    void testRenderIconInline() {
        QStringList svgPaths;
        svgPaths << m_testSvgFiles["single"]
                 << m_testSvgFiles["single"]
                 << m_testSvgFiles["single"];

        QSize iconSize(24, 24);
        int spacing = 4;

        QPixmap result = ThemedIconManager::renderIconInline(svgPaths, iconSize, spacing);

        int expectedWidth = iconSize.width() * svgPaths.size() + spacing * (svgPaths.size() - 1);
        QCOMPARE(result.width(), expectedWidth);
        QCOMPARE(result.height(), iconSize.height());

        QVERIFY(!result.isNull());
    }

    void testRenderIconInlineWithSingleIcon() {
        QStringList svgPaths;
        svgPaths << m_testSvgFiles["single"];

        QPixmap result = ThemedIconManager::renderIconInline(svgPaths, QSize(32, 32), 0);

        QCOMPARE(result.width(), 32);
        QCOMPARE(result.height(), 32);
        QVERIFY(!result.isNull());
    }

    void testRenderIconInlineWithEmptyList() {
        QStringList emptyList;
        QPixmap result = ThemedIconManager::renderIconInline(emptyList, QSize(16, 16));

        QVERIFY(result.isNull());
    }

    void testRenderIconGrid() {
        QStringList svgPaths;
        for (int i = 0; i < 5; ++i) {
            svgPaths << m_testSvgFiles["single"];
        }

        QSize iconSize(24, 24);
        int spacing = 5;
        int maxIconsPerRow = 3;

        QPixmap result = ThemedIconManager::renderIconGrid(svgPaths, iconSize, spacing, maxIconsPerRow);

        int expectedWidth = 3 * iconSize.width() + 2 * spacing;
        int expectedHeight = 2 * iconSize.height() + 1 * spacing;

        QCOMPARE(result.width(), expectedWidth);
        QCOMPARE(result.height(), expectedHeight);
        QVERIFY(!result.isNull());
    }

    void testRenderIconGridWithExactFit() {
        QStringList svgPaths;
        for (int i = 0; i < 4; ++i) {
            svgPaths << m_testSvgFiles["single"];
        }

        QSize iconSize(16, 16);
        int spacing = 2;
        int maxIconsPerRow = 2;

        QPixmap result = ThemedIconManager::renderIconGrid(svgPaths, iconSize, spacing, maxIconsPerRow);

        int expectedWidth = 2 * iconSize.width() + spacing;
        int expectedHeight = 2 * iconSize.height() + spacing;

        QCOMPARE(result.width(), expectedWidth);
        QCOMPARE(result.height(), expectedHeight);
    }

    void testRenderIconGridWithEmptyList() {
        QStringList emptyList;
        QPixmap result = ThemedIconManager::renderIconGrid(emptyList, QSize(16, 16));

        QVERIFY(result.isNull());
    }

    void testCurrentColorReplacement() {
        QString svgPath = m_testSvgFiles["withCurrentColor"];

        m_manager->addPixmapTarget(
            svgPath, m_testReceiver,
            [this](const QPixmap& pixmap) { m_testReceiver->setPixmap(pixmap); },
            true, QSize(32, 32)
        );

        QTest::qWait(10);

        QVERIFY(m_testReceiver->hasReceivedPixmap());
    }

    void testDeadTargetCleanup() {
        QString svgPath = m_testSvgFiles["single"];
        auto* tempReceiver = new TestIconReceiver();

        m_manager->addPixmapTarget(
            svgPath, tempReceiver,
            [](const QPixmap&) {}
        );

        delete tempReceiver;

        QEvent paletteEvent(QEvent::ApplicationPaletteChange);
        QCoreApplication::sendEvent(qApp, &paletteEvent);

        QVERIFY(true);
    }

    void testPerformanceManyIcons() {
        QStringList svgPaths;
        for (int i = 0; i < 20; ++i) {
            svgPaths << m_testSvgFiles["single"];
        }

        QBENCHMARK {
            QPixmap result = ThemedIconManager::renderIconInline(svgPaths, QSize(16, 16), 2);
            QVERIFY(!result.isNull());
        }
    }

    void testPerformanceGridRendering() {
        QStringList svgPaths;
        for (int i = 0; i < 100; ++i) {
            svgPaths << m_testSvgFiles["single"];
        }

        QBENCHMARK {
            QPixmap result = ThemedIconManager::renderIconGrid(svgPaths, QSize(24, 24), 4, 10);
            QVERIFY(!result.isNull());
        }
    }

private:
    void createTestSvgFiles() {
        QString simpleSvg =
            R"(<svg xmlns="http://www.w3.org/2000/svg" width="100" height="100">
                <rect width="100" height="100" fill="currentColor"/>
                <circle cx="50" cy="50" r="40" fill="none" stroke="black" stroke-width="2"/>
               </svg>)";

        QString withCurrentColorSvg =
            R"(<svg xmlns="http://www.w3.org/2000/svg" width="100" height="100">
                <path d="M10,50 L90,50" stroke="currentColor" stroke-width="4"/>
                <circle cx="50" cy="50" r="30" fill="currentColor"/>
               </svg>)";

        QTemporaryFile* tempFile1 = new QTemporaryFile(this);
        tempFile1->open();
        tempFile1->write(simpleSvg.toUtf8());
        tempFile1->close();
        m_testSvgFiles["single"] = tempFile1->fileName();
        m_tempFiles.append(tempFile1);

        QTemporaryFile* tempFile2 = new QTemporaryFile(this);
        tempFile2->open();
        tempFile2->write(withCurrentColorSvg.toUtf8());
        tempFile2->close();
        m_testSvgFiles["withCurrentColor"] = tempFile2->fileName();
        m_tempFiles.append(tempFile2);
    }

    void cleanupTestSvgFiles() {
        qDeleteAll(m_tempFiles);
        m_tempFiles.clear();
    }

private:
    ThemedIconManager* m_manager;
    QApplication* m_app;
    TestIconReceiver* m_testReceiver;
    TestButton* m_testButton;
    QMap<QString, QString> m_testSvgFiles;
    QList<QTemporaryFile*> m_tempFiles;
};

// Регистрируем тесты
int testThemedIconManager() {
    TestThemedIconManager test;
    return QTest::qExec(&test);
}
