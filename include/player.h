#pragma once
#include "enemy.h"
#include "pillar.h"
#include "utils/array.h"
#include <stdbool.h>

// Forward declaration to break circular dependency with enemy.h
typedef __DynArray Enemies;

typedef struct Player
{
  float x;
  float y;
  float lastX;
  float lastY;
  int   width;
  int   height;
  float acceleration;
  float friction;
  float velocityX;
  float velocityY;
  float maxSpeed;
  float jumpStrength;
  bool  isGrounded;
  bool  wantsToShoot;
  enum
  {
    AR              = 0,
    SHOTGUN         = 1,
    ROCKET_LAUNCHER = 2
  } weapon;
  float reloadTimer;
  float reloadSpeed;
  int   hp;
} Player;

typedef struct PlayerARBullet
{
  float x;
  float y;
  float velocityX;
  float velocityY;
  int   targetX;
  int   targetY;
  int   damage;
} PlayerARBullet;

typedef struct PlayerShotgunPellet
{
  float x;
  float y;
  float velocityX;
  float velocityY;
  int   targetX;
  int   targetY;
  int   damage;
} PlayerShotgunPellet;

typedef struct PlayerRocket
{
  float x;
  float y;
  float velocityX;
  float velocityY;
  int   targetX;
  int   targetY;
  int   damage;
  int   explosionRadius;
} PlayerRocket;

typedef struct PlayerExplosion
{
  float x;
  float y;
  float radius;
  float duration;
  float timer;
  int   damage;
} PlayerExplosion;

typedef dyn_arr(PlayerARBullet) PlayerARBullets;
typedef dyn_arr(PlayerShotgunPellet) PlayerShotgunPellets;
typedef dyn_arr(PlayerRocket) PlayerRockets;
typedef dyn_arr(PlayerExplosion) PlayerExplosions;

PlayerARBullet initPlayerARBullet(float x, float y, float velocityX, float velocityY, int damage);
PlayerShotgunPellet initPlayerShotgunPellet(float x, float y, float velocityX, float velocityY,
                                            int damage);
PlayerRocket        initPlayerRocket(float x, float y, float velocityX, float velocityY, int damage,
                                     int explosionRadius);
PlayerExplosion     initPlayerExplosion(float x, float y, float radius, int damage);

void initPlayerARBullets(PlayerARBullets* bullets);
void initPlayerShotgunPellets(PlayerShotgunPellets* pellets);
void initPlayerRockets(PlayerRockets* rockets);
void initPlayerExplosions(PlayerExplosions* explosions);

Player initPlayer(void);
void   displayPlayer(Player player);
void   takeDamage(Player* player, MeleeEnemyAttacks* attacks, RangedEnemyBullets* bullets);
void   diePlayer(void);

void playerShoot(Player* player, float targetX, float targetY, PlayerARBullets* arBullets,
                 PlayerShotgunPellets* shotgunPellets, PlayerRockets* rockets);
void updatePlayerARBullets(Player* player, PlayerARBullets* bullets, Pillars* pillars);
void displayPlayerARBullets(PlayerARBullets* bullets);
void freePlayerARBullets(PlayerARBullets* bullets);
void arDealDamageToEnemies(PlayerARBullets* bullets, Enemies* enemies);

void updatePlayerShotgunPellets(Player* player, PlayerShotgunPellets* pellets, Pillars* pillars);
void displayPlayerShotgunPellets(PlayerShotgunPellets* pellets);
void freePlayerShotgunPellets(PlayerShotgunPellets* pellets);
void shotgunDealDamageToEnemies(PlayerShotgunPellets* pellets, Enemies* enemies);

void updatePlayerRockets(Player* player, PlayerRockets* rockets, Pillars* pillars, Enemies* enemies, PlayerExplosions* explosions);
void displayPlayerRockets(PlayerRockets* rockets);
void freePlayerRockets(PlayerRockets* rockets);

void updatePlayerExplosions(PlayerExplosions* explosions, Enemies* enemies);
void displayPlayerExplosions(PlayerExplosions* explosions);
void freePlayerExplosions(PlayerExplosions* explosions);
void triggerPlayerExplosion(PlayerExplosions* explosions, float x, float y, float radius, int damage);

void handleMovement(Player* player);
void handlePlayerCollisions(Player* player, Pillars* pillars);
void handleJump(Player* player);
void handlePlayerGravity(Player* player);
void updatePlayer(Player* player, Pillars* pillars);
