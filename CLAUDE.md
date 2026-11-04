# CLAUDE.md

Top-down 2D shooter written in C++17 with Qt5 Widgets + `QGraphicsScene`. One human `Player` vs one AI `Bot`, chests with loot, three guns, grenades, leaderboard.
Flat directory, one class per lowercase `.h/.cpp` pair. Some comments and the TODO list at the bottom of `main.cpp` are in Polish.

## Build / run

qmake project: `shooter.pro`. Build out of source:

```bash
mkdir -p build && cd build && qmake ../shooter.pro && make -j && ./shooter
```

- Builds with both Qt 5.15 (system `qmake`) and Qt 6.11 (`~/Qt/6.11.2/gcc_64/bin/qmake`, the kit Qt Creator uses). Include Qt headers explicitly: Qt 5 is stricter about transitive includes (e.g. `qApp` needs `<QApplication>`).
- There is no Designer form. `MainWindow` is built in code, so keep `FORMS +=` empty unless a `.ui` file is actually committed.
- New class → add both files to `SOURCES` and `HEADERS` in `shooter.pro`.
- No tests, no CI.
- `compile_commands.json`, `*.pro.user` and `.qtcreator/` are local IDE state. They're gitignored and untracked, so don't commit them. The local `compile_commands.json` is stale (it points at `/home/szymon/c++ projects/shooter`).
- Build into `build/` (gitignored), never in-source.

## Architecture

- **Widgets:** `MainWindow` (`QStackedWidget`, fixed 1024×720) → `MainMenu` / `MainWidget` / `OptionsWidget`. `MainMenu` wires its buttons to the parent's slots via old-style `SLOT()`.
- **`MainWidget`** owns `MapWidget` + `PlayerBarWidget` (HUD) and overlay widgets placed with absolute `move()`: `Countdown`, `MessageAtTopOfScreen`, `PowerGrenadeThrowing`, `Leaderboard`. It injects the overlays into `MapWidget` via setters.
- **`MapWidget`** is the game hub. It owns the `QGraphicsScene`, `MapView` (a `QGraphicsView` that emits mouse signals), `Map` and `Player`. It handles the keyboard (`keyPressEvent`), creates the Bot in `start()` after a 3 s countdown, spawns a chest every 30 s, and makes the camera follow the player (`changeSceneRect`).
- **`Map`:** 150×150 grid of `GridElement` (each cell is 10 px, map is 1500×1500 px). Buildings are 4×4 cells at hardcoded positions in `map.cpp`, chests 2×1 cells. The cell `taken` flags drive spawning and pathfinding.
- **Scene items:** `ItemsOnScene` (abstract `QGraphicsRectItem`: `attackItem()`, `height` used by grenade collision) → `Building`, `Chest`, `Hero` (abstract, also `QObject`) → `Player`, `Bot`.
- **Combat:** `Gun` → `Pistol` / `Rifle` / `Shotgun` (stats set in the subclass constructors). `Shooting` runs fire timers and spawns `Bullet`s (the shotgun fires 5 spread bullets). `Grenade` handles its arc, bounces and explosion (`GrenadeEllipseItem` animation, `ShakeScreen`).
- **Bot AI** (`bot.cpp`): each 20 ms tick it scans a 300 px radius, attacks when it has line of sight (no `Building`/`Chest` in between), hides while reloading, otherwise walks to a chest or a random point. When blocked it falls back to `BFS_Algorithm` pathfinding on the grid.
- **Stats:** `HeroStats` (kills/deaths) → `LeaderboardSlot` → `Leaderboard` (shown while X is held).
- **Constants** (`RELOADING_TIME`, map/window sizes) and the `guns` enum live in `global_consts.h`.
- Death → `Hero::resetHero()`: respawn at a random free cell after `TIME_RETURN_TO_LIFE` (3 s).

## Conventions

- Mix of old-style `SIGNAL/SLOT` and lambda `connect`s; follow whatever the surrounding code uses.
- Many members are created with in-class `new` initializers and deleted manually in destructors.
- Hero state changes `emit` signals (`hpChanged`, `ammoChangedNoReloading`, …). Lambdas in the `MapWidget` constructor forward them to `PlayerBarWidget`.
- Movement is 2 px per 20 ms tick. Collision checks call `scene->itemAt()` on the two leading corners of the 10 px hero.
- Coordinates flip between grid units and pixels with ×10 / ÷10, so watch which one a value is in.
- `GridElement`s are shared map state. Never change their coordinates. `BFS_Algorithm::getPath` returns pixel `QPoint` copies.
- Chests: `Chest::open()` only hands out loot. The caller must then call `Map::removeChest()`, which frees the grid cells and deletes the chest.

## Controls

WASD move · mouse aim / hold to shoot · R reload · Q grenade (hold for power) · E open chest · 1/2/3 pistol/rifle/shotgun · X leaderboard
