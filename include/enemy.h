#pragma once

#include "pillar.h"
#include "player.h"
#include "utils/array.h"


typedef struct Enemy {
  float x;
  float y;
  float velocityX;
  float velocityY;
  float acceleration;
  float maxSpeed;
  float speed;
  float reloadSpeed;
  float reloadTimer;
  float groundedTime;
  float jumpCooldown;
  int width;
  int height;
  int id;
  int agroRangeBoxWidth;
  int agroRangeBoxHeight;
  bool isGrounded;
  bool isFleeing;
  enum { MELEE = 0, RANGED = 1 } type;
} Enemy;

typedef struct RangedEnemyBullet {
  float x;
  float y;
  float velocityX;
  float velocityY;
  float targetX;
  float targetY;
  float speed;
} RangedEnemyBullet;

typedef dyn_arr(Enemy) Enemies;
typedef dyn_arr(RangedEnemyBullet) RangedEnemyBullets;

Enemy initEnemy(float x, float y, float speed, int width, int height, int type,
                int id, int agroRangeBoxWidth, int agroRangeBoxHeight,
                bool isGrounded, float acceleration, float maxSpeed, float reloadSpeed, bool isFleeing);
void initEnemies(Enemies *enemies);
void addEnemy(Enemies *enemies, Enemy *enemy);
void generateEnemies(Enemies *enemies, Pillars *pillars);
void displayEnemies(Enemies *enemies);
void freeEnemies(Enemies *enemies);
void freeEnemy(Enemies *enemies, int id);

void updateEnemies(Enemies *enemies, Pillars *pillars, Player* player, RangedEnemyBullets* bullets);

bool canEnemyJumpToPillar(const Enemy* enemy, const Pillar* p, float gravity, float direction);
void moveEnemyTowardsPlayer(Enemy *enemy, Player *player, Pillars *pillars);
void enemyJump(Enemy *enemy);
void handleEnemyCollisions(Enemy *enemy, Pillars *pillars);
void handleEnemyGravity(Enemy *enemy);
void enemyShoot(Enemy *enemy, RangedEnemyBullets *bullets, Player *player);

RangedEnemyBullet initEnemyBullet(float x, float y, float velocityX,
                                  float velocityY, float targetX, float targetY,
                                  float speed);
void initRangedEnemyBullets(RangedEnemyBullets *bullets);
void freeRangedEnemyBullets(RangedEnemyBullets *bullets);
void updateBullets(RangedEnemyBullets *bullets);
