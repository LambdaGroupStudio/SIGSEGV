#include "player.h"
#include "globals.h"
#include "raylib.h"
#include "helpers.h"

void displayPlayer(Player player) {
    DrawRectangle((int)player.x, (int)player.y, player.width, player.height, RED);
}

void handleMovement(Player* player) {
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

    if (player->velocityX > player->maxSpeed) {
        player->velocityX = player->maxSpeed;
    }
    if (player->velocityX < -player->maxSpeed) {
        player->velocityX = -player->maxSpeed;
    }
}

void handleCollisions(Player* player, Pillar* pillar) {
    player->isGrounded = false;

    player->x += player->velocityX * deltaTime;
    
    // Resolve X obstacles
    if (isColliding(player->x, player->y, player->width, player->height, pillar->x, pillar->y, pillar->width, pillar->height)) {
        if (player->velocityX > 0.0f) {
            player->x = pillar->x - player->width;
        } else if (player->velocityX < 0.0f) {
            player->x = pillar->x + pillar->width;
        }
        player->velocityX = 0.0f;
    }

    // Screen bounds X
    if (player->x < 0.0f) {
        player->x = 0.0f;
        player->velocityX = 0.0f;
    }
    if (player->x + player->width > (float)GetScreenWidth()) {
        player->x = (float)GetScreenWidth() - player->width;
        player->velocityX = 0.0f;
    }

    player->y += player->velocityY * deltaTime;

    // Resolve Y obstacles
    if (isColliding(player->x, player->y, player->width, player->height, pillar->x, pillar->y, pillar->width, pillar->height)) {
        if (player->velocityY > 0.0f) {
            player->y = pillar->y - player->height;
            player->isGrounded = true;
        } else if (player->velocityY < 0.0f) {
            player->y = pillar->y + pillar->height;
        }
        player->velocityY = 0.0f;
    }

    // Screen bounds Y (Ground)
    float groundY = (float)GetScreenHeight() - player->height;
    if (player->y > groundY) {
        player->y = groundY;
        player->velocityY = 0.0f;
        player->isGrounded = true;
    }
    if (player->y < 0.0f) {
        player->y = 0.0f;
        player->velocityY = 0.0f;
    }
}

void handleJump(Player* player) {
    if (IsKeyPressed(KEY_SPACE) && player->isGrounded) {
        player->velocityY = -player->jumpStrength;
        player->isGrounded = false;
    }
}

void handleGravity(Player* player) {
    if (!player->isGrounded) {
        player->velocityY += gravity * deltaTime;
    }
}

void updatePlayer(Player* player, Pillar* pillar) {
    handleCollisions(player, pillar);
    handleMovement(player);
    handleJump(player);
    handleGravity(player);
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