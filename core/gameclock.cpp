#include "core/gameclock.h"

#include <algorithm>

GameClock::GameClock()
{
    real.start();
    pendingTimer.setSingleShot(true);
    connect(&pendingTimer, &QTimer::timeout, this, &GameClock::firePending);
}

GameClock & GameClock::instance()
{
    static GameClock clock;
    return clock;
}

qint64 GameClock::now()
{
    GameClock & c = instance();
    return (c.pausedSince >= 0 ? c.pausedSince : c.real.elapsed()) - c.pausedTotal;
}

bool GameClock::isPaused()
{
    return instance().pausedSince >= 0;
}

void GameClock::setPaused(bool paused)
{
    GameClock & c = instance();
    if (paused == isPaused())
        return;

    if (paused)
    {
        c.pausedSince = c.real.elapsed();
        for (GameTimer * timer : c.timers)
            timer->pause();
        c.pendingTimer.stop();
        c.pausedAnimations.clear();
        for (const QPointer<QAbstractAnimation> & animation : c.animations)
            if (animation && animation->state() == QAbstractAnimation::Running)
            {
                animation->pause();
                c.pausedAnimations.append(animation);
            }
    }
    else
    {
        c.pausedTotal += c.real.elapsed() - c.pausedSince;
        c.pausedSince = -1;
        for (GameTimer * timer : c.timers)
            timer->resume();
        c.schedulePending();
        for (const QPointer<QAbstractAnimation> & animation : c.pausedAnimations)
            if (animation && animation->state() == QAbstractAnimation::Paused)
                animation->resume();
        c.pausedAnimations.clear();
    }
    emit c.pausedChanged(paused);
}

void GameClock::reset()
{
    setPaused(false);
}

void GameClock::singleShot(int ms, QObject *context, std::function<void()> f)
{
    GameClock & c = instance();
    c.pending.append({now() + ms, context, std::move(f)});
    c.schedulePending();
}

void GameClock::track(QAbstractAnimation *animation)
{
    GameClock & c = instance();
    c.animations.removeAll(nullptr);
    c.animations.append(animation);
    if (isPaused() && animation->state() == QAbstractAnimation::Running)
    {
        animation->pause();
        c.pausedAnimations.append(animation);
    }
}

void GameClock::schedulePending()
{
    if (isPaused() || pending.isEmpty())
        return;
    const qint64 next = std::min_element(pending.begin(), pending.end(),
                                         [](const Pending & a, const Pending & b) { return a.due < b.due; })->due;
    pendingTimer.start(int(qMax<qint64>(0, next - now())));
}

void GameClock::firePending()
{
    // the due ones leave the list before any is called: a callback may add new ones, pause the clock or delete things
    const qint64 time = now();
    QList<Pending> due;
    for (int i = 0; i < pending.size(); )
        if (pending[i].due <= time)
            due.append(pending.takeAt(i));
        else
            i++;
    std::stable_sort(due.begin(), due.end(), [](const Pending & a, const Pending & b) { return a.due < b.due; });

    for (int i = 0; i < due.size(); i++)
    {
        if (isPaused())
        {
            // paused by an earlier callback: the rest wait for the resume
            pending.append(due.mid(i));
            break;
        }
        if (due[i].context)
            due[i].f();
    }
    schedulePending();
}


GameTimer::GameTimer(QObject *parent)
    : QObject(parent)
{
    connect(&timer, &QTimer::timeout, this, &GameTimer::fired);
    GameClock::instance().timers.append(this);
}

GameTimer::~GameTimer()
{
    GameClock::instance().timers.removeOne(this);
}

void GameTimer::start(int ms)
{
    intervalMs = ms;
    active = true;
    resuming = false;
    if (GameClock::isPaused())
    {
        timer.stop();
        remainingWhenPaused = ms;
        return;
    }
    remainingWhenPaused = -1;
    timer.setSingleShot(single);
    timer.start(ms);
}

void GameTimer::stop()
{
    active = false;
    resuming = false;
    remainingWhenPaused = -1;
    timer.stop();
}

int GameTimer::remainingTime() const
{
    if (!active)
        return -1;
    return remainingWhenPaused >= 0 ? remainingWhenPaused : timer.remainingTime();
}

void GameTimer::pause()
{
    if (active && timer.isActive())
    {
        remainingWhenPaused = qMax(0, timer.remainingTime());
        timer.stop();
    }
}

void GameTimer::resume()
{
    if (!active || remainingWhenPaused < 0)
        return;
    // the time left first, then the normal interval again
    resuming = !single;
    timer.setSingleShot(true);
    timer.start(remainingWhenPaused);
    remainingWhenPaused = -1;
}

void GameTimer::fired()
{
    if (single)
        active = false;
    else if (resuming)
    {
        resuming = false;
        timer.setSingleShot(false);
        timer.start(intervalMs);
    }
    emit timeout();     // last: whatever handles it may delete this timer (a bullet deleting itself)
}
