#include "window.h"
#include "globals.h"
#include "player.h"
#include "pillar.h"
#include "enemy.h"
#include <assert.h>

void initWindow(void) {
    int monitor = GetCurrentMonitor();
    int width   = GetMonitorWidth(monitor);
    int height  = GetMonitorHeight(monitor);

    if (width <= 0) width = 1600;
    if (height <= 0) height = 900;

    InitWindow(width, height, "SIGSEGV");
}

void displayWindow(void) {
    Player player = initPlayer();
    
    Pillars pillars;
    initPillars(&pillars);
    Enemies enemies;
    initEnemies(&enemies);

    Pillar initialPillar = initPillar(850.0f, 850.0f, -200.0f, 500.0f);
    
    // Add starting pillar
    addPillar(&pillars, &initialPillar);
    
    generatePillars(&pillars, 60);
    
    generateEnemies(&enemies, &pillars);
    
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
        updateEnemies(&enemies, &pillars, &player);
         
        displayEnemies(&enemies);
        displayPillars(&pillars);
        
        EndMode2D();
        EndDrawing();
    } 
    freeEnemies(&enemies);
    freePillars(&pillars);
    
}
