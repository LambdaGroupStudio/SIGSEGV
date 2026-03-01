#pragma once
#include <stdbool.h>
#include "pillar.h"
#include "utils/array.h"

// Forward declaration to break circular dependency with enemy.h
typedef __DynArray Enemies;

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
    float reloadTimer;
    float reloadSpeed;
} Player;

typedef struct PlayerARBullet {
    float x;
    float y;
    float velocityX;
    float velocityY;
    int targetX;
    int targetY;
    int damage;
} PlayerARBullet;

typedef struct PlayerShotgunPellet {
    float x;
    float y;
    float velocityX;
    float velocityY;
    int targetX;
    int targetY;
    int damage;
} PlayerShotgunPellet;

typedef struct PlayerRocket {
    float x;
    float y;
    float velocityX;
    float velocityY;
    int targetX;
    int targetY;
    int damage;
    int explosionRadius;
} PlayerRocket;

typedef dyn_arr(PlayerARBullet) PlayerARBullets;
typedef dyn_arr(PlayerShotgunPellet) PlayerShotgunPellets;
typedef dyn_arr(PlayerRocket) PlayerRockets;

PlayerARBullet initPlayerARBullet(float x, float y, float velocityX, float velocityY, int damage);
PlayerShotgunPellet initPlayerShotgunPellet(float x, float y, float velocityX, float velocityY, int damage);
PlayerRocket initPlayerRocket(float x, float y, float velocityX, float velocityY, int damage, int explosionRadius);

void initPlayerARBullets(PlayerARBullets* bullets);
void initPlayerShotgunPellets(PlayerShotgunPellets* pellets);
void initPlayerRockets(PlayerRockets* rockets);

Player initPlayer(void);
void displayPlayer(Player player);
void playerShoot(Player* player, float targetX, float targetY, PlayerARBullets* arBullets, PlayerShotgunPellets* shotgunPellets, PlayerRockets* rockets);
void updatePlayerARBullets(PlayerARBullets* bullets);
void displayPlayerARBullets(PlayerARBullets* bullets);
void freePlayerARBullets(PlayerARBullets* bullets);
void arDealDamageToEnemies(PlayerARBullets* bullets, Enemies* enemies);

void updatePlayerShotgunPellets(PlayerShotgunPellets* pellets);
void displayPlayerShotgunPellets(PlayerShotgunPellets* pellets);
void freePlayerShotgunPellets(PlayerShotgunPellets* pellets);
void shotgunDealDamageToEnemies(PlayerShotgunPellets* pellets, Enemies* enemies);

void updatePlayerRockets(PlayerRockets* rockets);
void displayPlayerRockets(PlayerRockets* rockets);
void freePlayerRockets(PlayerRockets* rockets);
void rocketDealDamageToEnemies(PlayerRockets* rockets, Enemies* enemies, Pillars* pillars);

void handleMovement(Player* player);
void handlePlayerCollisions(Player* player, Pillars* pillars);
void handleJump(Player* player);
void handlePlayerGravity(Player* player);
void updatePlayer(Player* player, Pillars* pillars);
