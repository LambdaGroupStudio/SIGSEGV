#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include "pillar.h"
#include "player.h"

typedef struct Enemy {
    float x;
    float y;
    float velocityX;
    float velocityY;
    float acceleration;
    float maxSpeed;
    float speed;
    int width;
    int height;
    int id;
    int agroRangeBoxWidth;
    int agroRangeBoxHeight;
    bool isGrounded;
    enum {MELEE = 0, RANGED = 1} type;
} Enemy;

typedef struct Enemies {
    Enemy* items;
    size_t count;
    size_t capacity;
} Enemies;

Enemy initEnemy(float x, float y, float speed, int width, int height, int type, int id, int agroRangeBoxWidth, int agroRangeBoxHeight, bool isGrounded, float acceleration, float maxSpeed);
void initEnemies(Enemies* enemies);
void addEnemy(Enemies* enemies, Enemy enemy);
void generateEnemies(Enemies* enemies, Pillars* pillars);
void displayEnemies(Enemies* enemies);
void freeEnemies(Enemies* enemies);
void freeEnemy(Enemies* enemies, int id);

void updateEnemies(Enemies* enemies, Pillars* pillars, Player* player);

void moveEnemyTowardsPlayer(Enemy* enemy, Player* player);
void enemyJump(Enemy* enemy);
void handleEnemyCollisions(Enemy* enemy, Pillars* pillars);
void handleEnemyGravity(Enemy* enemy);