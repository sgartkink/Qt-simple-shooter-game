# CLAUDE.md

Top-down 2D shooter written in C++17 with Qt5 Widgets + `QGraphicsScene`. One human `Player` vs one AI `Bot`, chests with loot, three guns, grenades, leaderboard.
One class per lowercase `.h/.cpp` pair, kept side by side in folders by game area. Some comments and the TODO list at the bottom of `main.cpp` are in Polish.

## Layout

- `main.cpp`, `shooter.pro` (the game), `game.pri` (every game source except `main.cpp`; shared by the game and the tests): project root
- `tests/`: Qt Test suite (`tests.pro`, one `test_*.cpp` per area, shared setup in `helpers.h`)
- `core/`: `global_consts.h`
- `ui/`: windows, menus, HUD and overlays (`MainWindow`, `MainMenu`, `MainWidget`, `OptionsWidget`, `PlayerBarWidget`, `Leaderboard`, `LeaderboardSlot`, `Countdown`, `MessageAtTopOfScreen`, `PowerGrenadeThrowing`)
- `world/`: map, view and scene items (`Map`, `MapWidget`, `MapView`, `FieldOfView`, `GridElement`, `ItemsOnScene`, `Building`, `Chest`, `ShakeScreen`)
- `characters/`: heroes and AI (`Hero`, `Player`, `Bot`, `HeroStats`, `BFS_Algorithm`)
- `weapons/`: guns and projectiles (`Gun`, `Pistol`, `Rifle`, `Shotgun`, `Shooting`, `Bullet`, `Grenade`, `GrenadeEllipseItem`)

Project includes are written relative to the root (`#include "characters/hero.h"`). `INCLUDEPATH += $$PWD` in `game.pri` makes that work. Don't use bare names or `../` paths.

## Build / run

qmake project: `shooter.pro`. Build out of source:

```bash
mkdir -p build && cd build && qmake ../shooter.pro && make -j && ./shooter
```

- Builds with both Qt 5.15 (system `qmake`) and Qt 6.11 (`~/Qt/6.11.2/gcc_64/bin/qmake`, the kit Qt Creator uses). Include Qt headers explicitly: Qt 5 is stricter about transitive includes (e.g. `qApp` needs `<QApplication>`).
- There is no Designer form. `MainWindow` is built in code, so keep `FORMS +=` empty unless a `.ui` file is actually committed.
- New class → put both files in the matching folder and add them as `$$PWD/<folder>/<file>` to `SOURCES` / `HEADERS` in `game.pri`, not `shooter.pro`.
- No CI yet.

## Tests

```bash
mkdir -p build/tests && cd build/tests && qmake ../../tests/tests.pro && make -j && make check
```

- Qt Test, one executable (`tests`), running offscreen unless `QT_QPA_PLATFORM` is set. It takes ~15 s, mostly the 8 s real-game smoke test. Run it after every change, with both Qt versions for anything build-related.
- For memory errors, build it with `CONFIG+=debug "QMAKE_CXXFLAGS+=-fsanitize=address -fno-omit-frame-pointer" "QMAKE_LFLAGS+=-fsanitize=address"` and run with `ASAN_OPTIONS=detect_leaks=0` (the game leaks on purpose at exit).
- `World` in `helpers.h` builds a map, the player and the overlays the way the game does, and on teardown deletes bots, bullets and grenades before the map, since those run their own timers.
- Tests that need internals are friends of the class (`Bot` → `TestBot`, `Bullet` → `TestWeapons`). Add a `friend class` line rather than `#define private public`.
- New test class: add a `test_*.cpp` ending with `#include "test_*.moc"` and a `runTest*()` function, list it in `tests.pro`, and call it from `tests/main.cpp`.
- `compile_commands.json`, `*.pro.user` and `.qtcreator/` are local IDE state. They're gitignored and untracked, so don't commit them. The local `compile_commands.json` is stale (it points at `/home/szymon/c++ projects/shooter`).
- Build into `build/` (gitignored), never in-source.

## Architecture

- **Widgets:** `MainWindow` (`QStackedWidget`, fixed 1024×720) → `MainMenu` / `MainWidget` / `OptionsWidget`. `MainMenu` wires its buttons to the parent's slots via old-style `SLOT()`.
- **`MainWidget`** owns `MapWidget` + `PlayerBarWidget` (HUD) and overlay widgets placed with absolute `move()`: `Countdown`, `MessageAtTopOfScreen`, `PowerGrenadeThrowing`, `Leaderboard`. It injects the overlays into `MapWidget` via setters.
- **`MapWidget`** is the game hub. It owns the `QGraphicsScene`, `MapView` (a `QGraphicsView` that emits mouse signals), `Map` and `Player`. It handles the keyboard (`keyPressEvent`), creates the Bot in `start()` after a 3 s countdown, spawns a chest every 30 s, and makes the camera follow the player (`changeSceneRect`).
- **`Map`:** 150×150 grid of `GridElement` (each cell is 10 px, map is 1500×1500 px). Buildings are 4×4 cells at hardcoded positions in `map.cpp`, chests 2×1 cells. The cell `taken` flags drive spawning and pathfinding.
- **Scene items:** `ItemsOnScene` (abstract `QGraphicsRectItem`: `attackItem()`, `height` used by grenade collision) → `Building`, `Chest`, `Hero` (abstract, also `QObject`) → `Player`, `Bot`.
- **Combat:** `Gun` → `Pistol` / `Rifle` / `Shotgun` (stats set in the subclass constructors). `Shooting` runs fire timers and spawns `Bullet`s (the shotgun fires 5 spread bullets). `Grenade` handles its arc, bounces and explosion (`GrenadeEllipseItem` animation, `ShakeScreen`).
- **Bot AI** (`bot.cpp`): each 20 ms tick it looks around with the player's view: a screen-sized area around it, of which `FieldOfView::canSee` keeps what is within 20° of its facing (`NOTICE_ANGLE`) or 30 px, with no building in between. It faces where it walks, or the target while attacking. It attacks when it has line of sight (no `Building`/`Chest` in between), hides while reloading, otherwise walks to a chest or a random point. When it loses sight of the player it walks to the last seen spot, turns a full circle there (`LOOK_AROUND_TICKS`) and only then gives up; chests are ignored meanwhile. When blocked it falls back to `BFS_Algorithm` pathfinding on the grid.
- **Player's view:** `FieldOfView` raycasts against the buildings from the hero's centre. It gives a 30° fully visible cone along the aim, reaching to the screen edge, then a further 10° on each side (`PERIPHERAL_ANGLE`) that fades linearly to dark. The fade is 0.5° solid slices drawn without antialiasing; don't use `QConicalGradient`, which costs ~6 ms a frame. Add to that a 30 px circle around the hero. Rays that hit a wall continue `WALL_DEPTH` (3 px) into it, so only the wall faces in line of sight show up; the rest of the building stays dark. `MapView::drawForeground` paints black over everything and erases each `VisiblePiece` with `DestinationOut`, as strongly as its brush's alpha. `MapWidget::updateFieldOfView` refreshes it on `Hero::viewChanged` (emitted by `Player::updateLineHeroMouse`) and `posChanged`, at most once per 16 ms frame: the first change is drawn at once and the latest one when the frame ends, so a fast mouse can't force hundreds of redraws a second. `MapView` keeps the darkness for the whole screen in `fogCache` and rebuilds it only when the view or the camera changes. Small repaints (bullets, the bot, the hero turning) just copy from it. The overlay isn't a scene item, so it never affects collision or bot checks. Don't merge the pieces with `QPainterPath::united()`: it breaks on the edges that rays share with buildings.
- **Stats:** `HeroStats` (kills/deaths) → `LeaderboardSlot` → `Leaderboard` (shown while X is held).
- **Constants** (`RELOADING_TIME`, map/window sizes) and the `guns` enum live in `global_consts.h`.
- Death → `Hero::resetHero()`: respawn at a random free cell after `TIME_RETURN_TO_LIFE` (3 s).

## Conventions

- Mix of old-style `SIGNAL/SLOT` and lambda `connect`s; follow whatever the surrounding code uses.
- `QTimer::singleShot` with a lambda always gets a context object (`singleShot(ms, this, [this]{...})`), so Qt cancels it if the object is deleted first.
- Many members are created with in-class `new` initializers and deleted manually in destructors.
- Hero state changes `emit` signals (`hpChanged`, `ammoChangedNoReloading`, …). Lambdas in the `MapWidget` constructor forward them to `PlayerBarWidget`.
- Movement is 2 px per 20 ms tick. Collision checks call `Hero::isBlockedAt()` on the two leading corners of the 10 px hero. It checks every item at the point. Don't use `scene->itemAt()` for collision: it only returns the topmost item, so explosions and bullets hide walls.
- Bullets hit via `collidingItems()` (their whole rotated 4×2 px shape), skipping their owner and anything that isn't `ItemsOnScene`.
- Bot chest routing: chest cells are taken, so the bot targets the nearest free cell beside the chest (`Bot::freeCellNextToChest`) and plans the route once. The loose 25 px arrival tolerance applies only to that final stop, never to waypoints.
- Coordinates flip between grid units and pixels with ×10 / ÷10, so watch which one a value is in.
- `GridElement`s are shared map state. Never change their coordinates. `BFS_Algorithm::getPath` returns pixel `QPoint` copies.
- Chests: `Chest::open()` only hands out loot. The caller must then call `Map::removeChest()`, which frees the grid cells and deletes the chest.

## Controls

WASD move · mouse aim / hold to shoot · R reload · Q grenade (hold for power) · E open chest · 1/2/3 pistol/rifle/shotgun · X leaderboard
