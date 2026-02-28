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

    if (width <= 0) width = 1920;
    if (height <= 0) height = 1080;

    InitWindow(width, height, "SIGSEGV");
    ToggleFullscreen();
}

void displayWindow(void) {
    Player player = initPlayer();
    
    Pillars pillars;
    initPillars(&pillars);
    Enemies enemies;
    initEnemies(&enemies);
    RangedEnemyBullets bullets;
    initRangedEnemyBullets(&bullets);
    MeleeEnemyAttacks attacks;
    initMeleeEnemyAttacks(&attacks);

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
        updateEnemies(&enemies, &pillars, &player, &bullets, &attacks);
        updateBullets(&bullets);
        updateMeleeEnemyAttacks(&attacks);

        camera.target.x = player.x + player.width / 2.0f;
        camera.target.y = player.y + player.height / 2.0f;
         
        displayEnemies(&enemies);
        displayPillars(&pillars);
        displayMeleeEnemyAttacks(&attacks);
        
        EndMode2D();
        if (player.weapon == AR) {
            DrawText("Weapon: Assault Rifle", 10, 10, 20, BLACK);
        } else if (player.weapon == SHOTGUN) {
            DrawText("Weapon: Shotgun", 10, 10, 20, BLACK);
        } else if (player.weapon == ROCKET_LAUNCHER) {
            DrawText("Weapon: Rocket Launcher", 10, 10, 20, BLACK);
        }
        EndDrawing();
    } 
    freeEnemies(&enemies);
    freePillars(&pillars);
    freeRangedEnemyBullets(&bullets);
    freeMeleeEnemyAttacks(&attacks);
}
