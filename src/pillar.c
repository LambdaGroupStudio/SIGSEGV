#include "pillar.h"
#include "raylib.h"

Pillar initPillar(void) {
    Pillar pillar;
    pillar.width = 200;
    pillar.height = 200;
    pillar.x = 500.0f;
    pillar.y = 900.0f;
    return pillar;
}

void displayPillar(Pillar *pillar) {
    DrawRectangle(pillar->x, pillar->y, pillar->width, pillar->height, GRAY);
}