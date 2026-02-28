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
  int width;
  int height;
  int id;
  int agroRangeBoxWidth;
  int agroRangeBoxHeight;
  bool isGrounded;
  bool isFleeing;
  enum { MELEE = 0, RANGED = 1 } type;
  enum { IDLE = 0, LEFT = 1, RIGHT = 2 } direction;
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

typedef struct MeleeEnemyAttack {
    float x;
    float y;
    float width;
    float height;
    float duration; // How long the attack hitbox should last
    float timer; // Timer to track how long the hitbox has been active
} MeleeEnemyAttack;

typedef dyn_arr(Enemy) Enemies;
typedef dyn_arr(RangedEnemyBullet) RangedEnemyBullets;
typedef dyn_arr(MeleeEnemyAttack) MeleeEnemyAttacks;

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

void moveEnemyTowardsPlayer(Enemy *enemy, Player *player, Pillars *pillars);
void enemyJump(Enemy *enemy);
void handleEnemyCollisions(Enemy *enemy, Pillars *pillars);
void handleEnemyGravity(Enemy *enemy);
void enemyShoot(Enemy *enemy, RangedEnemyBullets *bullets, Player *player);

RangedEnemyBullet initEnemyBullet(float x, float y, float velocityX, float velocityY, float targetX, float targetY, float speed);
void initRangedEnemyBullets(RangedEnemyBullets *bullets);
void freeRangedEnemyBullets(RangedEnemyBullets *bullets);

MeleeEnemyAttack initMeleeEnemyAttack(float x, float y, float width, float height);
void initMeleeEnemyAttacks(MeleeEnemyAttacks *attacks);

void updateBullets(RangedEnemyBullets *bullets);
