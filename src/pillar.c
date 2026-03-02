#include "pillar.h"
#include "helpers.h"
#include "raylib.h"
#include <stdlib.h>

Pillar initPillar(float width, float height, float x, float y)
{
  Pillar pillar;
  pillar.width  = width;
  pillar.height = height;
  pillar.x      = x;
  pillar.y      = y;
  return pillar;
}

void initPillars(Pillars* pillars) { *pillars = dyn_arr_create(sizeof(Pillar)); }

void addPillar(Pillars* pillars, Pillar* pillar) { dyn_arr_push_back(pillars, pillar); }

void generatePillars(Pillars* pillars, int count)
{
  for (int i = 0; i < count; i++)
  {
    int attempts = 0;

    while (attempts < 100)
    {
      float width  = (float)(GetRandomValue(600, 900));
      float height = (float)(GetRandomValue(600, 800));
      float x      = (float)GetRandomValue(400, 50000);
      float y      = (float)GetScreenHeight() - height;

      Pillar newPillar = initPillar(width, height, x, y);

      bool collision = false;
      for (size_t j = 0; j < pillars->size; j++)
      {
        const Pillar* p = dyn_arr_get(pillars, j);
        if (isColliding(newPillar.x, newPillar.y, newPillar.width, newPillar.height, p->x, p->y,
                        p->width, p->height))
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
}

void displayPillars(Pillars* pillars)
{
  for (size_t i = 0; i < pillars->size; i++)
  {
    const Pillar* p = dyn_arr_get(pillars, i);
    DrawRectangle(p->x, p->y, p->width, p->height, GRAY);
  }
}

void freePillars(Pillars* pillars) { dyn_arr_free(pillars); }
