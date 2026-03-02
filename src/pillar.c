#include "pillar.h"
#include "globals.h"
#include "helpers.h"
#include "raylib.h"
#include <stdlib.h>

Pillar initPillar(float width, float height, float x, float y)
{
  Pillar pillar;
  pillar.width  = (int)width;
  pillar.height = (int)height;
  pillar.x      = x;
  pillar.y      = y;
  return pillar;
}

void initPillars(Pillars* pillars) { *pillars = dyn_arr_create(sizeof(Pillar)); }

void addPillar(Pillars* pillars, Pillar* pillar) { dyn_arr_push_back(pillars, pillar); }

static int comparePillars(const void* a, const void* b)
{
  const Pillar* p1 = (const Pillar*)a;
  const Pillar* p2 = (const Pillar*)b;
  if (p1->x < p2->x)
    return -1;
  if (p1->x > p2->x)
    return 1;
  return 0;
}

void generatePillars(Pillars* pillars, int count)
{
  for (int i = 0; i < count; i++)
  {
    int attempts = 0;

    while (attempts < PILLAR_GEN_ATTEMPTS)
    {
      float width  = (float)(GetRandomValue(PILLAR_MIN_WIDTH, PILLAR_MAX_WIDTH));
      float height = (float)(GetRandomValue(PILLAR_MIN_HEIGHT, PILLAR_MAX_HEIGHT));
      // Increase min x to 2000 to ensure a clear starting area
      float x      = (float)GetRandomValue(PILLAR_MIN_X, PILLAR_MAX_X);
      float y      = (float)GetScreenHeight() - height;

      Pillar newPillar = initPillar(width, height, x, y);

      bool collision = false;
      for (size_t j = 0; j < pillars->size; j++)
      {
        const Pillar* p = dyn_arr_get(pillars, j);
        if (isColliding(newPillar.x, newPillar.y, (float)newPillar.width, (float)newPillar.height, 
                         p->x, p->y, (float)p->width, (float)p->height))
        {
          collision = true;
          break;
        }
      }

      if (!collision)
      {
        addPillar(pillars, &newPillar);
        break;
      }
      attempts++;
    }
  }

  // Sort by X coordinate so that pillars[size-1] is truly the physical final pillar
  if (pillars->size > 0)
  {
    qsort(pillars->arr_data, pillars->size, pillars->elem_size, comparePillars);
  }
}

void displayPillars(Pillars* pillars)
{
  for (size_t i = 0; i < pillars->size; i++)
  {
    const Pillar* p = dyn_arr_get(pillars, i);
    if (i == pillars->size - 1)
    {
      // Final pillar glows with random colors
      Color glowColor = (Color){GetRandomValue(0, 255), GetRandomValue(0, 255),
                               GetRandomValue(0, 255), 255};
      DrawRectangle((int)p->x, (int)p->y, p->width, p->height, glowColor);
    }
    else
    {
      DrawRectangle((int)p->x, (int)p->y, p->width, p->height, GRAY);
    }
  }
}

void freePillars(Pillars* pillars) { dyn_arr_free(pillars); }
