#include "enemy.h"
#include "raylib.h"
#include "helpers.h"
#include "pillar.h"
#include "globals.h"
#include <stdio.h>
#include <math.h>

// Legit has to be the worst code i have ever written
// If i would need to write a std::vector implemntation one more time i will snap
// Dont even get me started on typedef structs they are the devil
// Kids dont learn C learn C++

Enemy initEnemy(float x, float y, float speed, int width, int height, int type, int id, int agroRangeBoxWidth, int agroRangeBoxHeight, bool isGrounded, float acceleration, float maxSpeed, float reloadSpeed, bool isFleeing) {
    Enemy enemy;
    enemy.x = x;
    enemy.y = y;
    enemy.velocityX = 0.0f;
    enemy.velocityY = 0.0f;
    enemy.speed = speed;
    enemy.width = width;
    enemy.height = height;
    enemy.id = id;
    enemy.type = type;

    if (type == 0) { // MELEE
        enemy.agroRangeBoxWidth = agroRangeBoxWidth;
        enemy.agroRangeBoxHeight = agroRangeBoxHeight;
    } else if (type == 1) { // RANGED
        enemy.agroRangeBoxWidth = (int)(agroRangeBoxWidth * 1.5f); // Ranged enemies have larger agro range
        enemy.agroRangeBoxHeight = (int)(agroRangeBoxHeight * 1.5f);
    }

    enemy.isGrounded = isGrounded;
    enemy.acceleration = acceleration;
    enemy.maxSpeed = maxSpeed;
    enemy.reloadSpeed = reloadSpeed;
    enemy.reloadTimer = 0.0f;
    enemy.isFleeing = isFleeing;
    enemy.direction = IDLE;
    return enemy;
}

void initEnemies(Enemies* enemies) {
  *enemies = dyn_arr_create(sizeof(Enemy));
}

void addEnemy(Enemies* enemies, Enemy* enemy) {
  dyn_arr_push_back(enemies, enemy);
}

void generateEnemies(Enemies* enemies, Pillars* pillars) {
    int totalSpawns = 0;
    for (size_t i = 0; i < pillars->size; i++) {
        const Pillar* p = dyn_arr_get(pillars, i);
        if (i >= 2) { // Start spawning enemies after the first few pillars to give player some breathing room
            float randomChance = (float)GetRandomValue(0, 100) / 100.0f;
            
            if (randomChance > 0.40f) { // 60% chance to spawn an enemy on this pillar
                continue;
            }

            float x = (float)GetRandomValue(
                (int)p->x,
                (int)(p->x + p->width - 50)
            );

            float y = p->y - 100.0f; 
            int type = GetRandomValue(0, 1);
            
            // Basic stats for AI
            float acceleration = 0.0f;
            float maxSpeed = 0.0f;
            int agroWidth = 0;
            int agroHeight = 0;

            float reloadSpeed = 1.0f; // Default reload speed

            if (type == 0) { // MELEE
                acceleration = 2500.0f;
                maxSpeed = 600.0f;
                agroWidth = 1800; // Wide enough to cover most of the screen horizontally, but not too wide to be unfair
                agroHeight = 1600; // Tall enough to cover vertical space, but not too tall to be unfair
            } else if (type == 1) { // RANGED
                acceleration = 2000.0f;
                maxSpeed = 400.0f;
                agroWidth = 1500; // Ranged enemies have larger agro range to compensate for not chasing as aggressively
                agroHeight = 2000;
                reloadSpeed = 1.5f; // Ranged enemies shoot every 1.5 seconds
            }

            Enemy newEnemy = initEnemy(x, y, 100.0f, 50, 50, type, i, agroWidth, agroHeight, false, acceleration, maxSpeed, reloadSpeed, false);
            addEnemy(enemies, &newEnemy);
            totalSpawns++;
        } else {
            // Debug: Log that we're skipping enemy spawn for this pillar
            printf("DEBUG: Skipping enemy spawn for pillar %zu at x=%.2f due to initial safe zone\n", i, p->x);
        }
    }
    printf("DEBUG: Generated %d enemies on %zu pillars\n", totalSpawns, pillars->size);
}

void displayEnemies(Enemies* enemies) {
    for (size_t i = 0; i < enemies->size; i++) {
        Enemy* e = dyn_arr_get(enemies, i);
        Color color = (e->type == 0) ? BLUE : GREEN;
        DrawRectangle((int)e->x, (int)e->y, e->width, e->height, color);
        
        // Debug agro box
        DrawRectangleLines((int)(e->x - e->agroRangeBoxWidth/2.0f + e->width/2.0f), 
                         (int)(e->y - e->agroRangeBoxHeight/2.0f + e->height/2.0f), 
                         e->agroRangeBoxWidth, e->agroRangeBoxHeight, RED);
    }
}

void freeEnemies(Enemies* enemies) {
  dyn_arr_free(enemies);
}

void freeEnemy(Enemies *enemies, int id) {
    for (size_t i = 0; i < enemies->size; i++) {
        Enemy* e = dyn_arr_get(enemies, i);
        if (e->id == id) {
            dyn_arr_pop_at(enemies, i);
            return;
        }
    }
}

void enemyJump(Enemy *enemy) {
    if (enemy->isGrounded) {
        enemy->velocityY = -800.0f; // Similar jump strength to player
        enemy->isGrounded = false;
    }
}

void moveEnemyTowardsPlayer(Enemy* enemy, Player* player, Pillars* pillars) {
    // Center of enemy and agro box
    float cx = enemy->x + enemy->width / 2.0f;
    float cy = enemy->y + enemy->height / 2.0f;
    float agroX = cx - enemy->agroRangeBoxWidth / 2.0f;
    float agroY = cy - enemy->agroRangeBoxHeight / 2.0f;

    // Check full agro box
    if (isColliding(agroX, agroY, enemy->agroRangeBoxWidth, enemy->agroRangeBoxHeight,
                    player->x, player->y, player->width, player->height)) {
        
        enemy->isFleeing = false;
        
        // Ranged enemies flee if player is in the inner 50% of the agro box
        if (enemy->type == RANGED) {
            float innerWidth = enemy->agroRangeBoxWidth * 0.5f;
            float innerHeight = enemy->agroRangeBoxHeight * 0.5f;
            float innerX = cx - innerWidth / 2.0f;
            float innerY = cy - innerHeight / 2.0f;
            
            if (isColliding(innerX, innerY, (int)innerWidth, (int)innerHeight,
                            player->x, player->y, player->width, player->height)) {
                enemy->isFleeing = true;
            }
        }

        // Apply movement
        if (enemy->type == MELEE) {
            // Melee ALWAYS chases within agro box
            if (player->x < enemy->x) {
                enemy->velocityX -= enemy->acceleration * deltaTime;
                enemy->direction = LEFT;
            } else if (player->x > enemy->x) {
                enemy->velocityX += enemy->acceleration * deltaTime;
                enemy->direction = RIGHT;
            }
        } else if (enemy->type == RANGED) {
            if (enemy->isFleeing) {
                // Ranged flees within inner agro box
                if (player->x < enemy->x) {
                    enemy->velocityX += enemy->acceleration * deltaTime;
                    enemy->direction = RIGHT;
                } else if (player->x > enemy->x) {
                    enemy->velocityX -= enemy->acceleration * deltaTime;
                    enemy->direction = LEFT;
                }
            } else {
                // Ranged stays still / "don't fight"
                enemy->velocityX *= 0.9f;
                // Keep current direction or IDLE? Ranged usually face the player.
                // We'll update it in enemyShoot anyway.
                if (fabsf(enemy->velocityX) < 1.0f) enemy->direction = IDLE;
            }
        }

        // Clamp speed
        if (enemy->velocityX > enemy->maxSpeed) enemy->velocityX = enemy->maxSpeed;
        if (enemy->velocityX < -enemy->maxSpeed) enemy->velocityX = -enemy->maxSpeed;
    } else {
        // Friction when not chased
        enemy->velocityX *= 0.9f;
        if (fabsf(enemy->velocityX) < 1.0f) {
            enemy->velocityX = 0.0f;
            enemy->direction = IDLE;
        }
    }

    // Prevent enemies from getting stuck on a wall by making them jump
    if (enemy->velocityX != 0.0f && enemy->isGrounded) {
        float nextX = enemy->x + enemy->velocityX * deltaTime;
        for (size_t i = 0; i < pillars->size; i++) {
            const Pillar* p = dyn_arr_get(pillars, i);
            if (isColliding(nextX, enemy->y, enemy->width, enemy->height, p->x, p->y, p->width, p->height)) {
                enemyJump(enemy);
                break; 
            }
        }
    }

    // Robust Ledge Avoidance
    // This shit should be trademarked by me (The name)
    if (enemy->isGrounded) {
        float probeDist = 15.0f; // Check 15px ahead
        float dirMultiplier = 0.0f;
        if (enemy->direction == RIGHT) dirMultiplier = 1.0f;
        else if (enemy->direction == LEFT) dirMultiplier = -1.0f;
        
        if (dirMultiplier != 0.0f) {
            float probeX = enemy->x + (enemy->width / 2.0f) + (dirMultiplier * (enemy->width / 2.0f + probeDist));
            float probeY = enemy->y + (float)enemy->height + 5.0f; // Just below feet
            
            bool groundAhead = false;
            for (size_t i = 0; i < pillars->size; i++) {
                const Pillar* p = dyn_arr_get(pillars, i);
                if (probeX >= p->x && probeX <= p->x + p->width &&
                    probeY >= p->y && probeY <= p->y + p->height) {
                    groundAhead = true;
                    break;
                }
            }

            if (!groundAhead) {
                bool targetFound = false;
                float jumpTargetRelativeX = dirMultiplier * 500.0f; // Maximum jump look-ahead
                
                for (size_t i = 0; i < pillars->size; i++) {
                    const Pillar* p = dyn_arr_get(pillars, i);
                    // Check if there's a pillar at our height or slightly lower within jump range
                    if (enemy->x + jumpTargetRelativeX >= p->x && enemy->x + jumpTargetRelativeX <= p->x + p->width &&
                        enemy->y + enemy->height >= p->y - 100.0f && enemy->y + enemy->height <= p->y + 500.0f) {
                        targetFound = true;
                        break;
                    }
                }

                if (targetFound) {
                    enemyJump(enemy);
                } else {
                    // VOID AHEAD. HARD STOP.
                    enemy->velocityX = 0.0f;
                    
                    // Snap to the absolute edge of the current pillar to prevent "hanging"
                    for (size_t i = 0; i < pillars->size; i++) {
                        const Pillar* p = dyn_arr_get(pillars, i);
                        if (isColliding(enemy->x, enemy->y + 1.0f, enemy->width, enemy->height, p->x, p->y, p->width, p->height)) {
                            if (dirMultiplier > 0.0f) {
                                enemy->x = p->x + p->width - (float)enemy->width;
                            } else {
                                enemy->x = p->x;
                            }
                            enemy->direction = IDLE; // Set IDLE when we stop at a ledge
                            break;
                        }
                    }
                }
            }
        }
    }
}
   

void handleEnemyGravity(Enemy* enemy) {
    if (!enemy->isGrounded) {
        enemy->velocityY += gravity * deltaTime;
    }
}

void handleEnemyCollisions(Enemy* enemy, Pillars* pillars) {
    enemy->isGrounded = false;
    float buffer = 10.0f;

    // Horizontal
    enemy->x += enemy->velocityX * deltaTime;
    for (size_t i = 0; i < pillars->size; i++) {
        const Pillar* p = dyn_arr_get(pillars, i);
        if (isColliding(enemy->x, enemy->y, enemy->width, enemy->height, p->x, p->y, p->width, p->height)) {
            if (enemy->velocityX > 0.0f) enemy->x = p->x - enemy->width;
            else if (enemy->velocityX < 0.0f) enemy->x = p->x + p->width;
            enemy->velocityX = 0.0f;
            enemy->direction = IDLE; // Stop moving if we hit a wall
        }
    }

    // Vertical
    enemy->y += enemy->velocityY * deltaTime;
    for (size_t i = 0; i < pillars->size; i++) {
        const Pillar* p = dyn_arr_get(pillars, i);
        if (isColliding(enemy->x, enemy->y, enemy->width, enemy->height, p->x, p->y, p->width, p->height)) {
            if (enemy->velocityY >= 0.0f) {
                enemy->y = p->y - (float)enemy->height;
                enemy->isGrounded = true;
                enemy->velocityY = 0.0f;
            } else {
                enemy->y = p->y + (float)p->height;
                enemy->velocityY = 0.0f;
            }
        }
    }

    // Ground buffer check
    if (!enemy->isGrounded) {
        for (size_t i = 0; i < pillars->size; i++) {
            const Pillar* p = dyn_arr_get(pillars, i);
            if (isColliding(enemy->x, enemy->y + buffer, enemy->width, enemy->height, p->x, p->y, p->width, p->height)) {
                enemy->isGrounded = true;
                break;
            }
        }
    }

    // Screen bounds floor
    float groundY = (float)GetScreenHeight() - (float)enemy->height;
    if (enemy->y >= groundY - buffer && enemy->velocityY >= 0.0f) {
        enemy->y = groundY;
        enemy->velocityY = 0.0f;
        enemy->isGrounded = true;
    }
}

RangedEnemyBullet initEnemyBullet(float x, float y, float velocityX, float velocityY, float targetX, float targetY, float speed) {
    RangedEnemyBullet bullet;
    bullet.x = x;
    bullet.y = y;
    bullet.velocityX = velocityX;
    bullet.velocityY = velocityY;
    bullet.targetX = targetX;
    bullet.targetY = targetY;
    bullet.speed = speed;
    return bullet;
}

void initRangedEnemyBullets(RangedEnemyBullets *bullets) {
    *bullets = dyn_arr_create(sizeof(RangedEnemyBullet));
}

void enemyShoot(Enemy *enemy, RangedEnemyBullets* bullets, Player *player) {
    if (enemy->reloadTimer > 0) return;

    RangedEnemyBullet bullet;
    bullet.x = enemy->x;
    bullet.y = enemy->y;
    bullet.targetX = player->x;
    bullet.targetY = player->y;
    
    // Face the player when shooting
    if (player->x < enemy->x) enemy->direction = LEFT;
    else if (player->x > enemy->x) enemy->direction = RIGHT;

    bullet.speed = 1.0f; // Slow for testing, will be increased later
    
    float dx = player->x - bullet.x;
    float dy = player->y - bullet.y;

    float dist = sqrt(dx * dx + dy * dy);

    if (dist > 0) {
        bullet.velocityX = (dx / dist) * bullet.speed;
        bullet.velocityY = (dy / dist) * bullet.speed;
    } else {
        bullet.velocityX = 0;
        bullet.velocityY = 0;
    }
    
    dyn_arr_push_back(bullets, &bullet);
    enemy->reloadTimer = enemy->reloadSpeed;
}

void updateBullets(RangedEnemyBullets* bullets) {
    for (size_t i = 0; i < bullets->size; i++) {
        RangedEnemyBullet* b = dyn_arr_get(bullets, i);
        b->x += b->velocityX;
        b->y += b->velocityY;
        DrawRectangle((int)b->x, (int)b->y, 20, 20, YELLOW); // Dont worry about the magic number they a re temporary
    }
}

void freeRangedEnemyBullets(RangedEnemyBullets *bullets) {
    dyn_arr_free(bullets);
}

MeleeEnemyAttack initMeleeEnemyAttack(float x, float y, float width, float height) {
    MeleeEnemyAttack attack;
    attack.x = x;
    attack.y = y;
    attack.width = width;
    attack.height = height;
    attack.duration = 0.5f; // Attack hitbox lasts for 0.5 seconds
    attack.timer = 0.0f;
    return attack;
}

void initMeleeEnemyAttacks(MeleeEnemyAttacks *attacks) {
    *attacks = dyn_arr_create(sizeof(MeleeEnemyAttack));
}

void freeMeleeEnemyAttacks(MeleeEnemyAttacks *attacks) {
    dyn_arr_free(attacks);
}

void meleeEnemyAttack(Enemy *enemy, MeleeEnemyAttacks *attacks) {
    if (enemy->reloadTimer > 0) return;
    if (enemy->direction == IDLE) return;
    
    if (enemy->direction == LEFT) {
        MeleeEnemyAttack attack = initMeleeEnemyAttack(enemy->x - 100.0f, enemy->y, 100.0f, (float)enemy->height);
        dyn_arr_push_back(attacks, &attack);
    } else if (enemy->direction == RIGHT) {
        MeleeEnemyAttack attack = initMeleeEnemyAttack(enemy->x + (float)enemy->width, enemy->y, 100.0f, (float)enemy->height);
        dyn_arr_push_back(attacks, &attack);
    }
    enemy->reloadTimer = enemy->reloadSpeed;
}

void updateMeleeEnemyAttacks(MeleeEnemyAttacks *attacks) {
    for (size_t i = 0; i < attacks->size; i++) {
        MeleeEnemyAttack* a = dyn_arr_get(attacks, i);
        a->timer += deltaTime;
        if (a->timer >= a->duration) {
            dyn_arr_pop_at(attacks, i);
            i--; // Adjust index after removal
        }
    }
}

void displayMeleeEnemyAttacks(MeleeEnemyAttacks *attacks) {
    for (size_t i = 0; i < attacks->size; i++) {
        MeleeEnemyAttack* a = dyn_arr_get(attacks, i);
        // Draw a semi-transparent red rectangle for the attack hitbox
        DrawRectangleV((Vector2){a->x, a->y}, (Vector2){a->width, a->height}, Fade(RED, 0.4e0f));
    }
}

void updateEnemies(Enemies *enemies, Pillars *pillars, Player* player, RangedEnemyBullets* bullets, MeleeEnemyAttacks* attacks) {
    for (size_t i = 0; i < enemies->size; i++) {
        Enemy* e = dyn_arr_get(enemies, i);

        moveEnemyTowardsPlayer(e, player, pillars);
        handleEnemyGravity(e);
        handleEnemyCollisions(e, pillars);

        if (e->reloadTimer > 0) {
            e->reloadTimer -= deltaTime;
        }

        if (e->type == RANGED) {
            float cx = e->x + e->width / 2.0f;
            float cy = e->y + e->height / 2.0f;

            float agroX = cx - e->agroRangeBoxWidth / 2.0f;
            float agroY = cy - e->agroRangeBoxHeight / 2.0f;

            if (!e->isFleeing &&
                isColliding(agroX, agroY,
                            e->agroRangeBoxWidth,
                            e->agroRangeBoxHeight,
                            player->x, player->y,
                            player->width, player->height)) {

                enemyShoot(e, bullets, player);
            }
        } else if (e->type == MELEE) {
            float attackRange = 50.0f; // Melee attack range
            if (isColliding(e->x - attackRange, e->y, e->width + 2 * attackRange, e->height,
                            player->x, player->y, player->width, player->height)) {
                meleeEnemyAttack(e, attacks);
            }
        }
    }
}
