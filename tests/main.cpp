#include <QApplication>
#include <QtTest>

int runTestPathfinding(int argc, char *argv[]);
int runTestWeapons(int argc, char *argv[]);
int runTestFieldOfView(int argc, char *argv[]);
int runTestBot(int argc, char *argv[]);
int runTestGame(int argc, char *argv[]);
int runTestShotMarker(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    // headless by default, so the tests also run without a display
    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);

    int failedClasses = 0;
    for (auto run : {runTestPathfinding, runTestWeapons, runTestFieldOfView, runTestBot, runTestGame, runTestShotMarker})
        if (run(argc, argv) != 0)
            failedClasses++;
    return failedClasses;
}
