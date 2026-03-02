#include "window.h"
#include "enemy.h"
#include "globals.h"
#include "pillar.h"
#include "player.h"
#include <assert.h>
#include <stdio.h> 

void initWindow(void)
{
  int monitor = GetCurrentMonitor();
  int width   = GetMonitorWidth(monitor);
  int height  = GetMonitorHeight(monitor);

  if (width <= 0)
    width = 1920;
  if (height <= 0)
    height = 1080;

  InitWindow(width, height, "SIGSEGV");
  ToggleFullscreen();
}

void displayWindow(void)
{
  Player player = initPlayer();
  // Spawn player centered above the first pillar
  // First pillar: x=-200, width=850, y=500. Center = -200 + 425 = 225. 
  // Player width=100. Spawn x = 225 - 50 = 175.
  // Spawn y = pillar.y - player.height = 500 - 100 = 400.
  player.x = 175.0f;
  player.y = 400.0f;

  Pillars pillars;
  initPillars(&pillars);
  Enemies enemies;
  initEnemies(&enemies);
  RangedEnemyBullets bullets;
  initRangedEnemyBullets(&bullets);
  MeleeEnemyAttacks attacks;
  initMeleeEnemyAttacks(&attacks);

  PlayerARBullets playerARBullets;
  initPlayerARBullets(&playerARBullets);
  PlayerShotgunPellets playerShotgunPellets;
  initPlayerShotgunPellets(&playerShotgunPellets);
  PlayerRockets playerRockets;
  initPlayerRockets(&playerRockets);
  PlayerExplosions playerExplosions;
  initPlayerExplosions(&playerExplosions);

  Pillar initialPillar = initPillar(850.0f, 850.0f, -200.0f, 500.0f);

  // Add starting pillar
  addPillar(&pillars, &initialPillar);

  generatePillars(&pillars, 60);

  generateEnemies(&enemies, &pillars);

  Camera2D camera       = {0};
  int      monitor      = GetCurrentMonitor();
  int      screenWidth  = GetMonitorWidth(monitor);
  int      screenHeight = GetMonitorHeight(monitor);
  camera.target         = (Vector2){player.x, player.y};
  camera.offset         = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  camera.rotation       = 0.0f;
  camera.zoom           = 0.7f;

  gameTimer = INITIAL_GAME_TIMER;

  GameState gameState = MAIN_MENU;

  while (!WindowShouldClose())
  {
    deltaTime = GetFrameTime();

    switch (gameState)
    {
      case MAIN_MENU:
      {
        if (IsKeyPressed(KEY_ENTER))
        {
          gameState = GAME;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("SIGSEGV", screenWidth / 2 - MeasureText("SIGSEGV", 60) / 2, screenHeight / 2 - 100,
                 60, RED);
        DrawText("PRESS ENTER TO START", screenWidth / 2 - MeasureText("PRESS ENTER TO START", 20) / 2,
                 screenHeight / 2 + 20, 20, DARKGRAY);
        EndDrawing();
        break;
      }

      case GAME:
      {
        if (player.hp <= 0)
        {
          gameState = DEAD;
        }


        player.wantsToShoot = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        updatePlayer(&player, &pillars);
        updateEnemies(&enemies, &pillars, &player, &bullets, &attacks, &playerARBullets,
                      &playerShotgunPellets, &playerRockets);
        takeDamage(&player, &attacks, &bullets);

        gameTimer -= deltaTime;
        if (gameTimer <= 0.0f)
        {
          gameTimer = 0.0f;
          gameState = DEAD;
        }

        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        playerShoot(&player, mouseWorldPos.x, mouseWorldPos.y, &playerARBullets,
                    &playerShotgunPellets, &playerRockets);

        updatePlayerARBullets(&player, &playerARBullets, &pillars);
        updatePlayerShotgunPellets(&player, &playerShotgunPellets, &pillars);
        updatePlayerRockets(&player, &playerRockets, &pillars, &enemies, &playerExplosions);
        updatePlayerExplosions(&playerExplosions, &enemies);

        updateBullets(&bullets, &player);
        updateMeleeEnemyAttacks(&attacks);

        // Update camera target after all calculations
        camera.target = (Vector2){player.x + player.width / 2.0f, player.y + player.height / 2.0f};

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);

        displayPillars(&pillars);
        displayEnemies(&enemies);
        displayBullets(&bullets);
        displayMeleeEnemyAttacks(&attacks);
        displayPlayer(player);

        displayPlayerARBullets(&playerARBullets);
        displayPlayerShotgunPellets(&playerShotgunPellets);
        displayPlayerRockets(&playerRockets);
        displayPlayerExplosions(&playerExplosions);

        EndMode2D();

        if (player.weapon == AR)
        {
          DrawText("Weapon: Assault Rifle", 10, 10, 20, BLACK);
        }
        else if (player.weapon == SHOTGUN)
        {
          DrawText("Weapon: Shotgun", 10, 10, 20, BLACK);
        }
        else if (player.weapon == ROCKET_LAUNCHER)
        {
          DrawText("Weapon: Rocket Launcher", 10, 10, 20, BLACK);
        }

        char hpText[32];
        sprintf(hpText, "HP: %d", player.hp);
        DrawText(hpText, 10, 40, 20, player.hp < 30 ? RED : GREEN);

        char timerText[32];
        sprintf(timerText, "Time: %.1f", gameTimer);
        DrawText(timerText, screenWidth / 2 - MeasureText(timerText, 30) / 2, 20, 30,
                 gameTimer < 10.0f ? RED : BLACK);

        EndDrawing();
        break;
      }

      case DEAD:
      {
        if (IsKeyPressed(KEY_R))
        {
          // Simple reset: re-init everything
          player = initPlayer();
          player.x = 175.0f;
          player.y = 400.0f;
          gameTimer = INITIAL_GAME_TIMER;
          // Ideally we would clear arrays here too, but let's keep it simple for now
          gameState = GAME;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        const char* deathText = "SIGSEGV";
        int         fontSize  = 100;
        int         textWidth = MeasureText(deathText, fontSize);
        DrawText(deathText, screenWidth / 2 - textWidth / 2, screenHeight / 2 - fontSize / 2,
                 fontSize, RED);
        DrawText("PRESS R TO RESTART", screenWidth / 2 - MeasureText("PRESS R TO RESTART", 20) / 2,
                 screenHeight / 2 + 100, 20, RAYWHITE);
        EndDrawing();
        break;
      }
    }
  }
    freeEnemies(&enemies);
    freePillars(&pillars);
    freeRangedEnemyBullets(&bullets);
    freeMeleeEnemyAttacks(&attacks);
    freePlayerARBullets(&playerARBullets);
    freePlayerShotgunPellets(&playerShotgunPellets);
    freePlayerRockets(&playerRockets);
    freePlayerExplosions(&playerExplosions);
}
