#pragma once

typedef struct Player {
    float x;
    float y;
    int width;
    int height;
    float acceleration;
    float friction;
    float velocityX;
    float velocityY;
    float maxSpeed;
    float jumpStrength;
} Player;

Player initPlayer(void);
void displayPlayer(Player player);
void handleInput(Player* player);
void handleMovement(Player* player);
void handleJump(Player* player);
void handleGravity(Player* player);
void updatePlayer(Player* player);
