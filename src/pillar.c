#include "pillar.h"
#include "raylib.h"
#include "helpers.h"
#include <stdlib.h>

Pillar initPillar(float width, float height, float x, float y) {
    Pillar pillar;
    pillar.width = width;
    pillar.height = height;
    pillar.x = x;
    pillar.y = y;
    return pillar;
}

void initPillars(Pillars* pillars) {
    pillars->count = 0;
    pillars->capacity = 10;
    pillars->items = malloc(sizeof(Pillar) * pillars->capacity);
}

void addPillar(Pillars* pillars, Pillar pillar) {
    if (pillars->count >= pillars->capacity) {
        pillars->capacity *= 2;
        pillars->items = realloc(pillars->items, sizeof(Pillar) * pillars->capacity);
    }
    pillars->items[pillars->count++] = pillar;
}

void generatePillars(Pillars* pillars, int count) {
    for (int i = 0; i < count; i++) {
        float width = (float)GetRandomValue(100, 300);
        float height = (float)GetRandomValue(100, 500);
        float x = (float)GetRandomValue(400, 10000);
        float y = (float)GetScreenHeight() - (float)GetRandomValue(100, 700);
        
        Pillar newPillar = initPillar(width, height, x, y);
        
        bool collision = false;
        for (size_t j = 0; j < pillars->count; j++) {
            if (isColliding(newPillar.x, newPillar.y, newPillar.width, newPillar.height,
                            pillars->items[j].x, pillars->items[j].y, 
                            pillars->items[j].width, pillars->items[j].height)) {
                collision = true;
                break;
            }
        }
        
        if (!collision) {
            addPillar(pillars, newPillar);
        } else {
            i--; // Retry
        }
    }
}

void displayPillars(Pillars* pillars) {
    for (size_t i = 0; i < pillars->count; i++) {
        DrawRectangle(pillars->items[i].x, pillars->items[i].y, 
                      pillars->items[i].width, pillars->items[i].height, GRAY);
    }
}

void freePillars(Pillars* pillars) {
    if (pillars->items) {
        free(pillars->items);
        pillars->items = NULL;
    }
    pillars->count = 0;
    pillars->capacity = 0;
}