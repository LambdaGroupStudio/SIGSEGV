#pragma once
#include <stdbool.h>
#include "pillar.h"

typedef struct Player {
    float x;
    float y;
    float lastX;
    float lastY;
    int width;
    int height;
    float acceleration;
    float friction;
    float velocityX;
    float velocityY;
    float maxSpeed;
    float jumpStrength;
    bool isGrounded;
    enum { AR = 0, SHOTGUN = 1, ROCKET_LAUNCHER = 2 } weapon;
} Player;

typedef struct PlayerARBullet {
    float x;
    float y;
    float velocityX;
    float velocityY;
} PlayerARBullet;

typedef struct PlayerShotgunPellet {
    float x;
    float y;
    float velocityX;
    float velocityY;
} PlayerShotgunPellet;

typedef struct PlayerRocket {
    float x;
    float y;
    float velocityX;
    float velocityY;
} PlayerRocket;

typedef dyn_arr(PlayerARBullet) PlayerARBullets;
typedef dyn_arr(PlayerShotgunPellet) PlayerShotgunPellets;
typedef dyn_arr(PlayerRocket) PlayerRockets;

PlayerARBullet initPlayerARBullet(float x, float y, float velocityX, float velocityY);
PlayerShotgunPellet initPlayerShotgunPellet(float x, float y, float velocityX, float velocityY);
PlayerRocket initPlayerRocket(float x, float y, float velocityX, float velocityY);

void initPlayerARBullets(PlayerARBullets* bullets);
void initPlayerShotgunPellets(PlayerShotgunPellets* pellets);
void initPlayerRockets(PlayerRockets* rockets);

Player initPlayer(void);
void displayPlayer(Player player);
void handleMovement(Player* player);
void handlePlayerCollisions(Player* player, Pillars* pillars);
void handleJump(Player* player);
void handlePlayerGravity(Player* player);
void updatePlayer(Player* player, Pillars* pillars);
