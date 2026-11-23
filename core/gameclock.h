#ifndef GAMECLOCK_H
#define GAMECLOCK_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QPointer>
#include <QList>
#include <QAbstractAnimation>

#include <functional>

class GameTimer;

// The game's own time. It stands still while the game is paused (or the round is over), and every gameplay timer,
// delayed callback and animation goes through it, so pausing freezes all of them where they are and resuming
// carries on from there. Use GameTimer instead of QTimer, GameClock::singleShot instead of QTimer::singleShot,
// GameClock::now() instead of wall-clock time, and GameClock::track() for animations.
class GameClock : public QObject
{
    Q_OBJECT
public:
    static GameClock & instance();

    static qint64 now();                    // ms of game time
    static bool isPaused();
    static void setPaused(bool paused);
    static void reset();                    // running again, for a new game

    // calls f after ms of game time, unless context has been deleted by then
    static void singleShot(int ms, QObject * context, std::function<void()> f);
    // paused and resumed with the clock, from now until it finishes
    static void track(QAbstractAnimation * animation);

signals:
    void pausedChanged(bool paused);

private:
    GameClock();

    struct Pending
    {
        qint64 due;
        QPointer<QObject> context;
        std::function<void()> f;
    };

    QElapsedTimer real;
    qint64 pausedTotal = 0;
    qint64 pausedSince = -1;                // -1 while running
    QList<GameTimer *> timers;
    QList<QPointer<QAbstractAnimation>> animations;
    QList<QPointer<QAbstractAnimation>> pausedAnimations;
    QList<Pending> pending;
    QTimer pendingTimer;                    // real time, until the next pending callback is due

    void schedulePending();
    void firePending();

    friend class GameTimer;
};

// A QTimer that keeps game time: it stops while the GameClock is paused and then resumes with the time it had left.
// isActive() stays true while it is only paused.
class GameTimer : public QObject
{
    Q_OBJECT
public:
    explicit GameTimer(QObject * parent = nullptr);
    ~GameTimer();

    void start(int ms);
    void start() { start(intervalMs); }
    void stop();
    void setInterval(int ms) { intervalMs = ms; }
    int interval() const { return intervalMs; }
    void setSingleShot(bool singleShot) { single = singleShot; }
    bool isSingleShot() const { return single; }
    bool isActive() const { return active; }
    int remainingTime() const;

signals:
    void timeout();

private:
    QTimer timer;
    int intervalMs = 0;
    int remainingWhenPaused = -1;
    bool single = false;
    bool active = false;
    bool resuming = false;                  // running out the time left from before a pause

    void pause();
    void resume();
    void fired();

    friend class GameClock;
};

#endif // GAMECLOCK_H
