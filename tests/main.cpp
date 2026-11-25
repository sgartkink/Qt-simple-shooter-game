#include <QApplication>
#include <QtTest>
#include <QSettings>
#include <QTemporaryDir>

int runTestPathfinding(int argc, char *argv[]);
int runTestWeapons(int argc, char *argv[]);
int runTestFieldOfView(int argc, char *argv[]);
int runTestBot(int argc, char *argv[]);
int runTestGame(int argc, char *argv[]);
int runTestShotMarker(int argc, char *argv[]);
int runTestFlow(int argc, char *argv[]);
int runTestSniper(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    // headless by default, so the tests also run without a display
    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);

    // the options the tests change go to a throwaway folder, not the player's own settings
    QTemporaryDir settings;
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settings.path());

    int failedClasses = 0;
    for (auto run : {runTestPathfinding, runTestWeapons, runTestFieldOfView, runTestBot, runTestGame, runTestShotMarker, runTestFlow, runTestSniper})
        if (run(argc, argv) != 0)
            failedClasses++;
    return failedClasses;
}
