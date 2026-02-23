#include "player.h"
#include "globals.h"
#include "raylib.h"
#include "helpers.h"

void displayPlayer(Player player) {
    DrawRectangle((int)player.x, (int)player.y, player.width, player.height, RED);
}

void handleInput(Player* player) {

    if (IsKeyDown(KEY_D)) {

        if (player->velocityX < 0.0f) {
            player->velocityX = 0.0f;
        }

        player->velocityX += player->acceleration * deltaTime;
    }

    else if (IsKeyDown(KEY_A)) {

        if (player->velocityX > 0.0f) {
            player->velocityX = 0.0f;
        }

        player->velocityX -= player->acceleration * deltaTime;
    }

    else {

        if (player->velocityX > 0.0f) {
            player->velocityX -= player->friction * deltaTime;
            if (player->velocityX < 0.0f) {
                player->velocityX = 0.0f;
            }
                
        }
        else if (player->velocityX < 0.0f) {
            player->velocityX += player->friction * deltaTime;
            if (player->velocityX > 0.0f) {
                player->velocityX = 0.0f;
            }
        }
    }

    // Clamp max speed
    if (player->velocityX > player->maxSpeed) {
        player->velocityX = player->maxSpeed;
    }
        

    if (player->velocityX < -player->maxSpeed) {
        player->velocityX = -player->maxSpeed;
    }
}

void handleMovement(Player* player, Pillar* pillar) {
    player->lastX = player->x;
    player->lastY = player->y;

    float newX = player->x + player->velocityX * deltaTime;
    float newY = player->y + player->velocityY * deltaTime;

    player->x = newX;
    if (isColliding(player->x, player->y, player->width, player->height, pillar->x, pillar->y, pillar->width, pillar->height)) {
        if (player->velocityX > 0) {
            player->x = pillar->x - player->width;
        } else if (player->velocityX < 0) {
            player->x = pillar->x + pillar->width;
        }
        player->velocityX = 0.0f;
    }

    player->y = newY;
    if (isColliding(player->x, player->y, player->width, player->height, pillar->x, pillar->y, pillar->width, pillar->height)) {
        if (player->velocityY > 0) {
            player->y = pillar->y - player->height;
        } else if (player->velocityY < 0) {
            player->y = pillar->y + pillar->height;
        }
        player->velocityY = 0.0f;
    }

    if (player->x < 0.0f) {
        player->x = 0.0f;
        player->velocityX = 0.0f;
    }
    if (player->x + player->width > (float)GetScreenWidth()) {
        player->x = (float)GetScreenWidth() - player->width;
        player->velocityX = 0.0f;
    }
    if (player->y < 0.0f) {
        player->y = 0.0f;
        player->velocityY = 0.0f;
    }
    if (player->y + player->height > (float)GetScreenHeight()) {
        player->y = (float)GetScreenHeight() - player->height;
        player->velocityY = 0.0f;
    }
}

void handleJump(Player* player) {
    float groundY = (float)GetScreenHeight() - player->height;

    if (IsKeyPressed(KEY_SPACE) && player->y >= groundY) {
        player->velocityY = -player->jumpStrength;
    }
}

void handleGravity(Player* player) {
    float groundY = (float)GetScreenHeight() - player->height;

    if (player->y < groundY) {
        player->velocityY += gravity * deltaTime;
    } else {
        player->y = groundY;
        // Only reset velocity if we are falling into the ground
        if (player->velocityY > 0) {
            player->velocityY = 0.0f;
        }
    }
}

void updatePlayer(Player* player, Pillar* pillar) {
    handleInput(player);
    handleJump(player);
    handleGravity(player);
    handleMovement(player, pillar);
    displayPlayer(*player);
}

Player initPlayer(void) {
    Player player = {0};
    player.x = 100.0f;
    player.y = 100.0f;
    player.lastX = player.x;
    player.lastY = player.y;
    player.width = 100.0f;
    player.height = 100.0f;
    player.velocityX = 0.0f;
    player.velocityY = 0.0f;
    player.acceleration = 2000.0f;
    player.friction = 1500.0f;
    player.maxSpeed = 600.0f;
    player.jumpStrength = 600.0f;
    return player;
}