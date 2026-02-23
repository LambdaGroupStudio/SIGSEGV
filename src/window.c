#include "window.h"
#include "globals.h"
#include "player.h"
#include "pillar.h"

void initWindow(void) {
    int monitor = GetCurrentMonitor();
    int width   = GetMonitorWidth(monitor);
    int height  = GetMonitorHeight(monitor);

    InitWindow(width, height, "SIGSEGV");
    SetTargetFPS(60);
}

void displayWindow(void) {
    Player player = initPlayer();
    
    Pillars pillars;
    initPillars(&pillars);
    
    // Add starting pillar
    addPillar(&pillars, initPillar(850.0f, 850.0f, -200.0f, 500.0f));
    
    // Generate dynamic pillars
    generatePillars(&pillars, 20);
    
    Camera2D camera = {0};
    int monitor = GetCurrentMonitor();
    int screenWidth = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);
    camera.target = (Vector2){player.x, player.y};
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 0.7f;
    
    while (!WindowShouldClose()) {
        deltaTime = GetFrameTime();
        camera.target = (Vector2){player.x + player.width/2.0f, player.y + player.height/2.0f};
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);
        
        updatePlayer(&player, &pillars);
        displayPillars(&pillars);
        
        EndMode2D();
        EndDrawing();
    }
    
    freePillars(&pillars);
}
