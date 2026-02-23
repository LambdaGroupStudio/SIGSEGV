#pragma once
#include <stdbool.h>

bool isColliding(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);

bool collidingTop(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);

bool collidingBottom(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);

bool collidingLeft(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);

bool collidingRight(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);