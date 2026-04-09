#include <QCoreApplication>
#include <QtTest/QtTest>
#include <QDebug>

// Forward declarations
int testPaletteManager();
int testQssManager();
int testThemedIconManager();
int testThemeManager();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int status = 0;

    qDebug() << "\n========================================";
    qDebug() << "Running AdaptiveThemeLib Unit Tests";
    qDebug() << "========================================\n";

    // Запускаем все тесты
    status |= testPaletteManager();
    status |= testQssManager();
    status |= testThemedIconManager();
    status |= testThemeManager();

    if (status == 0) {
        qDebug() << "\nALL TESTS PASSED!";
    } else {
        qDebug() << "\nSOME TESTS FAILED!";
    }

    return status;
}