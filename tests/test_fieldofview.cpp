#include <QtTest>
#include <QtMath>
#include <QGraphicsScene>

#include <cmath>

#include "world/map.h"
#include "world/mapview.h"
#include "world/fieldofview.h"

// buildings at grid (40,28) and (40,32) form a wall at x 400..440, y 280..360
class TestFieldOfView : public QObject
{
    Q_OBJECT

private slots:
    void coneAndFadeBand();
    void buildingsCastShadows();
    void onlyVisibleWallFacesShow();
    void nearCircle();
    void turningAndWrapAround();
    void heroAndClearPointAheadAlwaysVisible();
    void canSee();
    void fadeIsLinearOnBothSides();
    void cachedDarknessMatchesAFreshDraw();

private:
    static bool visible(const QVector<VisiblePiece> & pieces, QPointF point);
};

bool TestFieldOfView::visible(const QVector<VisiblePiece> &pieces, QPointF point)
{
    for (const VisiblePiece & piece : pieces)
        if (piece.area.containsPoint(point, Qt::OddEvenFill))
            return true;
    return false;
}

void TestFieldOfView::coneAndFadeBand()
{
    QGraphicsScene scene;
    Map map(&scene);
    FieldOfView fov(&scene);
    QVector<VisiblePiece> view = fov.visibleArea(QPointF(200, 300), 0);   // aiming right
    QVERIFY(visible(view, QPointF(300, 300)));
    QVERIFY(visible(view, QPointF(300, 320)));       // 11 deg: clear middle
    QVERIFY(visible(view, QPointF(300, 330)));       // 17 deg: fade band
    QVERIFY(!visible(view, QPointF(300, 351)));      // 27 deg: past the band
    QCOMPARE(view.size(), 42);                       // middle, near circle, 20 fade slices per side
}

void TestFieldOfView::buildingsCastShadows()
{
    QGraphicsScene scene;
    Map map(&scene);
    FieldOfView fov(&scene);
    QVector<VisiblePiece> view = fov.visibleArea(QPointF(200, 300), 0);
    QVERIFY(!visible(view, QPointF(500, 300)));
    QVERIFY(!visible(view, QPointF(1000, 300)));
    // the shadow edge from the wall's corner (400,280) is sharp, even far away
    const qreal yEdge = 300 - (20.0 / 200.0) * (800 - 200);
    QVERIFY(visible(view, QPointF(800, yEdge - 2)));
    QVERIFY(!visible(view, QPointF(800, yEdge + 2)));
}

void TestFieldOfView::onlyVisibleWallFacesShow()
{
    QGraphicsScene scene;
    Map map(&scene);
    FieldOfView fov(&scene);
    QVector<VisiblePiece> view = fov.visibleArea(QPointF(200, 300), 0);
    QVERIFY(visible(view, QPointF(401.5, 300)));     // a strip of the facing wall
    QVERIFY(!visible(view, QPointF(410, 300)));      // deeper in
    QVERIFY(!visible(view, QPointF(420, 300)));      // the inside
    QVERIFY(!visible(view, QPointF(430, 281)));      // the top face, turned away
    QVector<VisiblePiece> down = fov.visibleArea(QPointF(420, 200), 270);   // straight above, looking down
    QVERIFY(visible(down, QPointF(420, 282)));
    QVERIFY(!visible(down, QPointF(420, 290)));
    QVERIFY(!visible(down, QPointF(420, 400)));
}

void TestFieldOfView::nearCircle()
{
    QGraphicsScene scene;
    Map map(&scene);
    FieldOfView fov(&scene);
    QVector<VisiblePiece> view = fov.visibleArea(QPointF(200, 300), 0);
    QVERIFY(visible(view, QPointF(180, 300)));       // 20 px behind
    QVERIFY(!visible(view, QPointF(160, 300)));      // 40 px behind
    QVERIFY(visible(view, QPointF(200, 325)));       // beside
}

void TestFieldOfView::turningAndWrapAround()
{
    QGraphicsScene scene;
    Map map(&scene);
    FieldOfView fov(&scene);
    QVector<VisiblePiece> up = fov.visibleArea(QPointF(200, 300), 90);
    QVERIFY(visible(up, QPointF(200, 100)));
    QVERIFY(!visible(up, QPointF(300, 300)));
    QVector<VisiblePiece> acrossZero = fov.visibleArea(QPointF(600, 600), 355);
    QVERIFY(visible(acrossZero, QPointF(700, 590)));
    QVERIFY(visible(acrossZero, QPointF(700, 615)));
    QVERIFY(!visible(acrossZero, QPointF(600, 500)));
}

void TestFieldOfView::heroAndClearPointAheadAlwaysVisible()
{
    // every whole-degree aim from spots in the open, by walls and in corners (merging the pieces with
    // QPainterPath::united() once lost the whole cone at some aims)
    QGraphicsScene scene;
    Map map(&scene);
    FieldOfView fov(&scene);
    const QPointF spots[] = { {335, 335}, {200, 300}, {745, 745}, {395, 275}, {445, 300}, {50, 50}, {1450, 1450}, {700, 1000}, {1000, 120} };
    for (QPointF spot : spots)
        for (int aim = 0; aim < 360; aim++)
        {
            const QVector<VisiblePiece> view = fov.visibleArea(spot, aim);
            const QPointF ahead = spot + 50 * QPointF(qCos(qDegreesToRadians(double(aim))), -qSin(qDegreesToRadians(double(aim))));
            QVERIFY2(visible(view, spot), qPrintable(QString("hero not visible at (%1,%2) aim %3").arg(spot.x()).arg(spot.y()).arg(aim)));
            if (fov.canSee(spot, aim, ahead))       // nothing in the way
                QVERIFY2(visible(view, ahead), qPrintable(QString("point ahead hidden at (%1,%2) aim %3").arg(spot.x()).arg(spot.y()).arg(aim)));
        }
}

void TestFieldOfView::canSee()
{
    QGraphicsScene scene;
    Map map(&scene);
    FieldOfView fov(&scene);
    const QPointF eye(700, 700);
    auto at = [&](qreal degrees, qreal distance) { return eye + distance * QPointF(qCos(qDegreesToRadians(degrees)), -qSin(qDegreesToRadians(degrees))); };
    QVERIFY(fov.canSee(eye, 0, at(0, 300)));
    QVERIFY(fov.canSee(eye, 0, at(19, 150)));        // within the 20 deg notice angle
    QVERIFY(!fov.canSee(eye, 0, at(21, 150)));
    QVERIFY(fov.canSee(eye, 0, at(180, 25)));        // near circle
    QVERIFY(!fov.canSee(eye, 0, at(180, 35)));
    QVERIFY(fov.canSee(QPointF(350, 300), 0, QPointF(390, 300)));    // before the wall
    QVERIFY(!fov.canSee(QPointF(350, 300), 0, QPointF(470, 300)));   // behind it
}

void TestFieldOfView::fadeIsLinearOnBothSides()
{
    // renders the darkness and compares each sampled pixel with the expected darkness for that pixel's own angle:
    // clear to 15 deg, linear fade to 25 deg, black beyond
    QGraphicsScene scene;
    Map map(&scene);
    FieldOfView fov(&scene);
    MapView view(&scene);
    view.resize(1024, 680);
    view.show();
    QCoreApplication::processEvents();
    const QPointF eye(700, 700);                     // open ground
    view.setSceneRect(eye.x() - 511, eye.y() - 339, view.viewport()->width(), view.viewport()->height());

    for (qreal aim : {0.0, 5.0, 135.0, 270.0})
    {
        view.setVisibleArea(fov.visibleArea(eye, aim));
        QCoreApplication::processEvents();
        const QImage image = view.viewport()->grab().toImage();
        for (qreal relative = -35; relative <= 35; relative += 0.25)
        {
            const qreal angle = qDegreesToRadians(aim + relative);
            const QPoint pixel = view.mapFromScene(eye + QPointF(100 * qCos(angle), -100 * qSin(angle)));
            const qreal actual = qAbs(std::fmod(QLineF(eye, view.mapToScene(pixel) + QPointF(0.5, 0.5)).angle() - aim + 540, 360) - 180);
            const qreal expected = actual <= 15 ? 0 : actual >= 25 ? 1 : (actual - 15) / 10;
            const qreal measured = 1.0 - qGray(image.pixel(pixel)) / 255.0;
            QVERIFY2(qAbs(measured - expected) <= 0.05,
                     qPrintable(QString("aim %1 at %2 deg: darkness %3, expected %4").arg(aim).arg(actual).arg(measured).arg(expected)));
        }
    }
}

void TestFieldOfView::cachedDarknessMatchesAFreshDraw()
{
    // MapView caches the darkness; partial repaints and camera moves must not leave stale or missing darkness
    QGraphicsScene scene;
    Map map(&scene);
    FieldOfView fov(&scene);
    MapView view(&scene);
    view.resize(1024, 680);
    view.show();
    QCoreApplication::processEvents();
    const QRectF camera(700 - 511, 700 - 339, view.viewport()->width(), view.viewport()->height());
    view.setSceneRect(camera);
    view.setVisibleArea(fov.visibleArea(QPointF(700, 700), 30));
    view.viewport()->repaint();
    const QImage reference = view.viewport()->grab().toImage();

    QGraphicsRectItem * mover = scene.addRect(0, 0, 6, 6);
    for (int i = 0; i < 200; i++)
    {
        mover->setPos(500 + i * 3, 600 + (i % 17) * 7);
        QCoreApplication::processEvents();
    }
    delete mover;
    QCoreApplication::processEvents();
    QVERIFY(view.viewport()->grab().toImage() == reference);

    view.setSceneRect(camera.translated(37, -21));
    QCoreApplication::processEvents();
    view.viewport()->repaint();
    view.setSceneRect(camera);
    QCoreApplication::processEvents();
    QVERIFY(view.viewport()->grab().toImage() == reference);

    MapView fresh(&scene);
    fresh.resize(1024, 680);
    fresh.show();
    fresh.setSceneRect(camera);
    fresh.setVisibleArea(fov.visibleArea(QPointF(700, 700), 30));
    QCoreApplication::processEvents();
    QVERIFY(fresh.viewport()->grab().toImage() == reference);
}

int runTestFieldOfView(int argc, char *argv[])
{
    TestFieldOfView test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_fieldofview.moc"
