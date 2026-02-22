#include <stdbool.h>

bool isColliding(float x1, float y1, int width1, int height1, float x2, float y2, int width2, int height2) {
    if (x1 < x2 + width2 &&
        x1 + width1 > x2 &&
        y1 < y2 + height2 &&
        y1 + height1 > y2) {
        return true;
    }
    return false;
}