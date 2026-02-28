#include "player.h"
#include "globals.h"
#include "raylib.h"
#include "helpers.h"
#include <math.h>

PlayerARBullet initPlayerARBullet(float x, float y, float velocityX, float velocityY) {
    PlayerARBullet bullet;
    bullet.x = x;
    bullet.y = y;
    bullet.velocityX = velocityX;
    bullet.velocityY = velocityY;
    return bullet;
}

PlayerShotgunPellet initPlayerShotgunPellet(float x, float y, float velocityX, float velocityY) {
    PlayerShotgunPellet pellet;
    pellet.x = x;
    pellet.y = y;
    pellet.velocityX = velocityX;
    pellet.velocityY = velocityY;
    return pellet;
}

PlayerRocket initPlayerRocket(float x, float y, float velocityX, float velocityY) {
    PlayerRocket rocket;
    rocket.x = x;
    rocket.y = y;
    rocket.velocityX = velocityX;
    rocket.velocityY = velocityY;
    return rocket;
}

void initPlayerARBullets(PlayerARBullets* bullets) {
    *bullets = dyn_arr_create(sizeof(PlayerARBullet));
}

void initPlayerShotgunPellets(PlayerShotgunPellets* pellets) {
    *pellets = dyn_arr_create(sizeof(PlayerShotgunPellet));
}

void initPlayerRockets(PlayerRockets* rockets) {
    *rockets = dyn_arr_create(sizeof(PlayerRocket));
}

void displayPlayer(Player player) {
    DrawRectangle((int)player.x, (int)player.y, player.width, player.height, RED);
}

void handleMovement(Player* player) {
    if (IsKeyDown(KEY_D)) {
        if (player->velocityX < 0.0f) player->velocityX = 0.0f;
        player->velocityX += player->acceleration * deltaTime;
    }
    else if (IsKeyDown(KEY_A)) {
        if (player->velocityX > 0.0f) player->velocityX = 0.0f;
        player->velocityX -= player->acceleration * deltaTime;
    }
    else {
        if (player->velocityX > 0.0f) {
            player->velocityX -= player->friction * deltaTime;
            if (player->velocityX < 0.0f) player->velocityX = 0.0f;
        }
        else if (player->velocityX < 0.0f) {
            player->velocityX += player->friction * deltaTime;
            if (player->velocityX > 0.0f) player->velocityX = 0.0f;
        }
    }
    if (player->velocityX > player->maxSpeed) player->velocityX = player->maxSpeed;
    if (player->velocityX < -player->maxSpeed) player->velocityX = -player->maxSpeed;
}

void handleGunStateMachine(Player* player) {
    if (IsKeyPressed(KEY_ONE)) {
        player->weapon = AR;
    } else if (IsKeyPressed(KEY_TWO)) {
        player->weapon = SHOTGUN;
    } else if (IsKeyPressed(KEY_THREE)) {
        player->weapon = ROCKET_LAUNCHER;
    }
}

void playerShoot(Player* player, float targetX, float targetY, PlayerARBullets* arBullets, PlayerShotgunPellets* shotgunPellets, PlayerRockets* rockets) {
    if (player->reloadTimer > 0) return;

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        float px = player->x + player->width / 2.0f;
        float py = player->y + player->height / 2.0f;
        
        float dx = targetX - px;
        float dy = targetY - py;
        float dist = sqrtf(dx * dx + dy * dy);
        
        if (dist > 0) {
            float dirX = dx / dist;
            float dirY = dy / dist;

            if (player->weapon == AR) {
                float speed = 1500.0f;
                PlayerARBullet bullet = initPlayerARBullet(px, py, dirX * speed, dirY * speed);
                dyn_arr_push_back(arBullets, &bullet);
                player->reloadTimer = 0.1f; // Fast fire rate
            } else if (player->weapon == SHOTGUN) {
                float speed = 1200.0f;
                int pelletCount = 8;
                float spread = 0.2f;
                for (int i = 0; i < pelletCount; i++) {
                    float angle = atan2f(dirY, dirX) + ((float)rand() / (float)RAND_MAX - 0.5f) * spread;
                    PlayerShotgunPellet pellet = initPlayerShotgunPellet(px, py, cosf(angle) * speed, sinf(angle) * speed);
                    dyn_arr_push_back(shotgunPellets, &pellet);
                }
                player->reloadTimer = 0.8f; // Slow fire rate
            } else if (player->weapon == ROCKET_LAUNCHER) {
                float speed = 800.0f;
                PlayerRocket rocket = initPlayerRocket(px, py, dirX * speed, dirY * speed);
                dyn_arr_push_back(rockets, &rocket);
                player->reloadTimer = 1.5f; // Very slow fire rate
            }
        }
    }
}

void updatePlayerARBullets(PlayerARBullets* bullets) {
    for (size_t i = 0; i < bullets->size; i++) {
        PlayerARBullet* b = dyn_arr_get(bullets, i);
        b->x += b->velocityX * deltaTime;
        b->y += b->velocityY * deltaTime;
        
        if (b->x < -5000 || b->x > 5000 || b->y < -5000 || b->y > 5000) {
            dyn_arr_pop_at(bullets, i);
            i--;
        }
    }
}

void displayPlayerARBullets(PlayerARBullets* bullets) {
    for (size_t i = 0; i < bullets->size; i++) {
        PlayerARBullet* b = dyn_arr_get(bullets, i);
        DrawCircle((int)b->x, (int)b->y, 3, YELLOW);
    }
}

void freePlayerARBullets(PlayerARBullets* bullets) {
    dyn_arr_free(bullets);
}

void updatePlayerShotgunPellets(PlayerShotgunPellets* pellets) {
    for (size_t i = 0; i < pellets->size; i++) {
        PlayerShotgunPellet* p = dyn_arr_get(pellets, i);
        p->x += p->velocityX * deltaTime;
        p->y += p->velocityY * deltaTime;

        if (p->x < -5000 || p->x > 5000 || p->y < -5000 || p->y > 5000) {
            dyn_arr_pop_at(pellets, i);
            i--;
        }
    }
}

void displayPlayerShotgunPellets(PlayerShotgunPellets* pellets) {
    for (size_t i = 0; i < pellets->size; i++) {
        PlayerShotgunPellet* p = dyn_arr_get(pellets, i);
        DrawCircle((int)p->x, (int)p->y, 2, GRAY);
    }
}

void freePlayerShotgunPellets(PlayerShotgunPellets* pellets) {
    dyn_arr_free(pellets);
}

void updatePlayerRockets(PlayerRockets* rockets) {
    for (size_t i = 0; i < rockets->size; i++) {
        PlayerRocket* r = dyn_arr_get(rockets, i);
        r->x += r->velocityX * deltaTime;
        r->y += r->velocityY * deltaTime;

        if (r->x < -5000 || r->x > 5000 || r->y < -5000 || r->y > 5000) {
            dyn_arr_pop_at(rockets, i);
            i--;
        }
    }
}

void displayPlayerRockets(PlayerRockets* rockets) {
    for (size_t i = 0; i < rockets->size; i++) {
        PlayerRocket* r = dyn_arr_get(rockets, i);
        DrawRectangle((int)r->x - 10, (int)r->y - 5, 20, 10, RED);
    }
}

void freePlayerRockets(PlayerRockets* rockets) {
    dyn_arr_free(rockets);
}

void handlePlayerCollisions(Player* player, Pillars* pillars) {
    player->isGrounded = false;
    float buffer = 10.0f;

    player->x += player->velocityX * deltaTime;
    
    for (size_t i = 0; i < pillars->size; i++) {
        const Pillar* p = dyn_arr_get(pillars, i);
        if (isColliding(player->x, player->y, player->width, player->height, p->x, p->y, p->width, p->height)) {
            if (player->velocityX > 0.0f) player->x = p->x - player->width;
            else if (player->velocityX < 0.0f) player->x = p->x + p->width;
            player->velocityX = 0.0f;
        }
    }

    player->y += player->velocityY * deltaTime;

    for (size_t i = 0; i < pillars->size; i++) {
        const Pillar* p = dyn_arr_get(pillars, i);
        if (isColliding(player->x, player->y, player->width, player->height, p->x, p->y, p->width, p->height)) {
            if (player->velocityY >= 0.0f) {
                player->y = p->y - (float)player->height;
                player->isGrounded = true;
                player->velocityY = 0.0f;
            } else {
                player->y = p->y + (float)p->height;
                player->velocityY = 0.0f;
            }
        }
    }

    float groundY = (float)GetScreenHeight() - (float)player->height;
    if (player->y >= groundY - buffer && player->velocityY >= 0.0f) {
        player->y = groundY;
        player->velocityY = 0.0f;
        player->isGrounded = true;
    } else {
        // Also check if we are grounded on top of any pillar due to buffer
        for (size_t i = 0; i < pillars->size; i++) {
            const Pillar* p = dyn_arr_get(pillars, i);
            if (isColliding(player->x, player->y + buffer, player->width, player->height, p->x, p->y, p->width, p->height)) {
                player->isGrounded = true;
                break;
            }
        }
    }

    // Screen bounds top/bottom Y
    if (player->y < -1000.0f) { // Some limit to prevent flying away forever
         player->y = -1000.0f;
         player->velocityY = 0.0f;
    }
}

void handleJump(Player* player) {
    if (IsKeyPressed(KEY_SPACE) && player->isGrounded) {
        player->velocityY = -player->jumpStrength;
        player->isGrounded = false;
    }
}

void handlePlayerGravity(Player* player) {
    if (!player->isGrounded) {
        player->velocityY += gravity * deltaTime;
    }
}

void updatePlayer(Player* player, Pillars* pillars) {
    handlePlayerCollisions(player, pillars);
    handleMovement(player);
    handleJump(player);
    handlePlayerGravity(player);
    handleGunStateMachine(player);
    if (player->reloadTimer > 0) {
        player->reloadTimer -= deltaTime;
    }
    displayPlayer(*player);
}

Player initPlayer(void) {
    Player player = {0};
    player.x = 100.0f;
    player.y = 100.0f;
    player.width = 100.0f;
    player.height = 100.0f;
    player.velocityX = 0.0f;
    player.velocityY = 0.0f;
    player.acceleration = 2000.0f;
    player.friction = 1500.0f;
    player.maxSpeed = 600.0f;
    player.jumpStrength = 600.0f;
    player.isGrounded = false;
    player.reloadTimer = 0.0f;
    player.reloadSpeed = 1.0f;
    player.weapon = AR;
    return player;
}
