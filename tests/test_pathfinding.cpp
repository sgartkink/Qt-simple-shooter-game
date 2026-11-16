#include <QtTest>
#include <QGraphicsScene>

#include "world/map.h"
#include "characters/bfs_algorithm.h"
#include "core/global_consts.h"

// buildings at grid (40,28) and (40,32) form a wall at x 400..440, y 280..360
class TestPathfinding : public QObject
{
    Q_OBJECT

private slots:
    void normalPath();
    void nearTheMapEdge();
    void unreachableDestination();
    void sourceIsDestination();
    void gridStaysIntact();
};

void TestPathfinding::normalPath()
{
    QGraphicsScene scene;
    Map map(&scene);
    BFS_Algorithm bfs(&map);
    QVector<QPoint> path = bfs.getPath(QPointF(200, 240), QPoint(600, 600));
    QVERIFY(!path.isEmpty());
    QCOMPARE(path.first(), QPoint(600, 600));   // pixels, destination first
    QCOMPARE(path.last(), QPoint(210, 240));
}

void TestPathfinding::nearTheMapEdge()
{
    // the grid bound used to be hardcoded to 248 (max index is 149) and crashed here
    QGraphicsScene scene;
    Map map(&scene);
    BFS_Algorithm bfs(&map);
    QVector<QPoint> path = bfs.getPath(QPointF(1480, 1480), QPoint(1300, 1300));
    QVERIFY(!path.isEmpty());
    for (const QPoint & p : path)
        QVERIFY(p.x() >= 0 && p.x() <= MAP_SIZE_X_FULL && p.y() >= 0 && p.y() <= MAP_SIZE_Y_FULL);
}

void TestPathfinding::unreachableDestination()
{
    // inside a building: used to crash on path.last() of an empty path
    QGraphicsScene scene;
    Map map(&scene);
    BFS_Algorithm bfs(&map);
    QVERIFY(bfs.getPath(QPointF(200, 240), QPoint(415, 295)).isEmpty());
}

void TestPathfinding::sourceIsDestination()
{
    // used to loop forever
    QGraphicsScene scene;
    Map map(&scene);
    BFS_Algorithm bfs(&map);
    QVector<QPoint> path = bfs.getPath(QPointF(200, 240), QPoint(200, 240));
    QCOMPARE(path.size(), 1);
    QCOMPARE(path.first(), QPoint(200, 240));
}

void TestPathfinding::gridStaysIntact()
{
    // searches used to multiply the shared GridElements' coordinates by 10 in place
    QGraphicsScene scene;
    Map map(&scene);
    BFS_Algorithm bfs(&map);
    QVector<QPoint> first = bfs.getPath(QPointF(200, 240), QPoint(600, 600));
    QVector<QPoint> again = bfs.getPath(QPointF(200, 240), QPoint(600, 600));
    bfs.getPath(QPointF(1480, 1480), QPoint(1300, 1300));
    QCOMPARE(again, first);
    for (int x = 0; x <= MAP_SIZE_X_10; x++)
        for (int y = 0; y <= MAP_SIZE_Y_10; y++)
            QCOMPARE(QPoint(*map.getGridElement(x, y)), QPoint(x, y));
}

int runTestPathfinding(int argc, char *argv[])
{
    TestPathfinding test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_pathfinding.moc"
