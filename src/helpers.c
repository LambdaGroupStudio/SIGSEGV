#include "helpers.h"

bool isColliding(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2) {
    return !(x1 + w1 <= x2 || x1 >= x2 + w2 || y1 + h1 <= y2 || y1 >= y2 + h2);
}

bool collidingTop(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2) {
    return y1 < y2 + h2 && y1 + h1 > y2 + h2 && x1 + w1 > x2 && x1 < x2 + w2;
}

bool collidingBottom(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2) {
    return y1 < y2 && y1 + h1 > y2 && x1 + w1 > x2 && x1 < x2 + w2 && y1 + h1 <= y2 + h2;
}

bool collidingLeft(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2) {
    return x1 < x2 + w2 && x1 + w1 > x2 + w2 && y1 + h1 > y2 && y1 < y2 + h2;
}

bool collidingRight(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2) {
    return x1 < x2 + w2 && x1 + w1 > x2 + w2 && y1 + h1 > y2 && y1 < y2 + h2;
}