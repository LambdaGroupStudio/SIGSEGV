#include "player.h"
#include "globals.h"
#include "raylib.h"
#include "helpers.h"

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
    return player;
}
