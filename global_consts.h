#ifndef GLOBAL_CONSTS_H
#define GLOBAL_CONSTS_H

const int RELOADING_TIME = 1000;

const int MAP_SIZE_X_FULL = 1500;
const int MAP_SIZE_X_10 = MAP_SIZE_X_FULL/10 - 1;
const int MAP_SIZE_Y_FULL = 1500;
const int MAP_SIZE_Y_10 = MAP_SIZE_Y_FULL/10 - 1;

const int WINDOW_HEIGHT = 720;
const int WINDOW_WIDTH = 1024;

enum guns {
    PISTOL,
    RIFLE,
    SHOTGUN
};

#endif // GLOBAL_CONSTS_H
