#include "player.h"
#include "globals.h"
#include "raylib.h"

void displayPlayer(Player player) {
    DrawRectangle(player.x, player.y, player.width, player.height, RED);
}

void handleMovement(Player* player) {
    if (IsKeyDown(KEY_D)) {
        player->x += player->speed * deltaTime;
    }
    if (IsKeyDown(KEY_A)) {
        player->x -= player->speed * deltaTime;
    }
}

void updatePlayer(Player* player) {
    handleMovement(player);
    displayPlayer(*player);
}

Player initPlayer(void) {
    Player player;
    player.x = 100;
    player.y = 100;
    player.width = 100;
    player.height = 100;
    player.speed = 500.0f;
    return player;
}