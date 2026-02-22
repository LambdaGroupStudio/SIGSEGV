#include "window.h"
#include "globals.h"
#include "player.h"

void initWindow(void) {
    int monitor = GetCurrentMonitor();
    int width   = GetMonitorWidth(monitor);
    int height  = GetMonitorHeight(monitor);

    InitWindow(width, height, "SIGSEGV");
    ToggleFullscreen(); // For testing purposes, we can remove this later if we want to allow windowed mode
}

void displayWindow(void) {
    Player player = initPlayer();
    while (!WindowShouldClose()) {
        deltaTime = GetFrameTime();
        BeginDrawing();
        ClearBackground(RAYWHITE);
        updatePlayer(&player);
        EndDrawing();
    }
}
