#pragma once

#include "utils/array.h"
#include <stddef.h>

typedef struct Pillar
{
  int   width;
  int   height;
  float x;
  float y;
} Pillar;

typedef dyn_arr(Pillar) Pillars;

Pillar initPillar(float width, float height, float x, float y);
void   initPillars(Pillars* pillars);
void   addPillar(Pillars* pillars, Pillar* pillar);
void   generatePillars(Pillars* pillars, int count);
void   displayPillars(Pillars* pillars);
void   freePillars(Pillars* pillars);
