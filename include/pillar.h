#pragma once

typedef struct Pillar {
    int width;
    int height;
    float x;
    float y;
} Pillar;

extern Pillar initPillar(void);
extern void displayPillar(Pillar* pillar);

