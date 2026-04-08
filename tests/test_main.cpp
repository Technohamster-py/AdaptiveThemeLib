#include <QCoreApplication>
#include <QtTest/QtTest>

int testPaletteManager();
int testQssManager();
int testThemedIconManager();
int testThemeManager();

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    int status = 0;

    status |= testPaletteManager();
    status |= testQssManager();
    status |= testThemedIconManager();
    status |= testThemeManager();

    return status;
}