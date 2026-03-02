#include "window.h"
#include "enemy.h"
#include "globals.h"
#include "pillar.h"
#include "player.h"
#include "helpers.h"
#include <assert.h>
#include <stdio.h> 
#include <time.h>

static void rebuildWorld(Player* player, Pillars* pillars, Enemies* enemies,
                         RangedEnemyBullets* bullets, MeleeEnemyAttacks* attacks,
                         PlayerARBullets* playerARBullets, PlayerShotgunPellets* playerShotgunPellets,
                         PlayerRockets* playerRockets, PlayerExplosions* playerExplosions)
{
  // First free any existing dynamic arrays
  freePillars(pillars);
  freeEnemies(enemies);
  freeRangedEnemyBullets(bullets);
  freeMeleeEnemyAttacks(attacks);
  freePlayerARBullets(playerARBullets);
  freePlayerShotgunPellets(playerShotgunPellets);
  freePlayerRockets(playerRockets);
  freePlayerExplosions(playerExplosions);

  // Initialize new dynamic arrays
  initPillars(pillars);
  initEnemies(enemies);
  initRangedEnemyBullets(bullets);
  initMeleeEnemyAttacks(attacks);
  initPlayerARBullets(playerARBullets);
  initPlayerShotgunPellets(playerShotgunPellets);
  initPlayerRockets(playerRockets);
  initPlayerExplosions(playerExplosions);

  // Set the random seed so that each generation is unique
  SetRandomSeed((unsigned int)time(NULL));

  // Initialize player
  *player = initPlayer();
  // Spawn player centered above the first pillar
  player->x = PLAYER_SPAWN_X;
  player->y = PLAYER_SPAWN_Y;

  Pillar initialPillar = initPillar(STARTING_PILLAR_WIDTH, STARTING_PILLAR_HEIGHT, 
                                   STARTING_PILLAR_X, STARTING_PILLAR_Y);
  addPillar(pillars, &initialPillar);

  // Generation
  generatePillars(pillars, PILLAR_COUNT);
  generateEnemies(enemies, pillars);

  // Reset clock
  gameTimer = INITIAL_GAME_TIMER;
}


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
  SetRandomSeed((unsigned int)time(NULL));
}

void displayWindow(void)
{
  Player player;
  Pillars pillars;
  Enemies enemies;
  RangedEnemyBullets bullets;
  MeleeEnemyAttacks attacks;
  PlayerARBullets playerARBullets;
  PlayerShotgunPellets playerShotgunPellets;
  PlayerRockets playerRockets;
  PlayerExplosions playerExplosions;

  // Initialize variables to a safe, empty state before the loop
  // This ensures that the first call to rebuildWorld (which calls free) is safe
  initPillars(&pillars);
  initEnemies(&enemies);
  initRangedEnemyBullets(&bullets);
  initMeleeEnemyAttacks(&attacks);
  initPlayerARBullets(&playerARBullets);
  initPlayerShotgunPellets(&playerShotgunPellets);
  initPlayerRockets(&playerRockets);
  initPlayerExplosions(&playerExplosions);

  // Generate initial world
  rebuildWorld(&player, &pillars, &enemies, &bullets, &attacks, &playerARBullets, 
               &playerShotgunPellets, &playerRockets, &playerExplosions);

  Camera2D camera       = {0};
  int      monitor      = GetCurrentMonitor();
  int      screenWidth  = GetMonitorWidth(monitor);
  int      screenHeight = GetMonitorHeight(monitor);
  camera.target         = (Vector2){player.x, player.y};
  camera.offset         = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  camera.rotation       = 0.0f;
  camera.zoom           = 0.7f;

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
          rebuildWorld(&player, &pillars, &enemies, &bullets, &attacks, &playerARBullets, 
                       &playerShotgunPellets, &playerRockets, &playerExplosions);
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

        if (player.wantsToShoot)
        {
          Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
          playerShoot(&player, mousePos.x, mousePos.y, &playerARBullets, &playerShotgunPellets,
                      &playerRockets);
        }

        updatePlayerARBullets(&player, &playerARBullets, &pillars);
        updatePlayerShotgunPellets(&player, &playerShotgunPellets, &pillars);
        updatePlayerRockets(&player, &playerRockets, &pillars, &enemies, &playerExplosions);
        updatePlayerExplosions(&playerExplosions, &enemies);

        updateBullets(&bullets, &player);
        updateMeleeEnemyAttacks(&attacks);

        updateEnemies(&enemies, &pillars, &player, &bullets, &attacks, &playerARBullets,
                      &playerShotgunPellets, &playerRockets);
        takeDamage(&player, &attacks, &bullets);

        // Check for win condition: touching the final (right-most) pillar 
        // Tolerant check for standing on it: use a 1 pixel buffer specifically here
        if (pillars.size > 0 && gameState == GAME)
        {
          Pillar* finalPillar = dyn_arr_get(&pillars, pillars.size - 1);
          if (player.x < finalPillar->x + finalPillar->width && 
              player.x + player.width > finalPillar->x && 
              player.y + player.height > finalPillar->y - 1.0f && // Slightly above top is ok
              player.y < finalPillar->y + finalPillar->height)
          {
            gameState = WON;
            exitTimer = 3.9f;
          }
        }

        // Update camera target after all calculations
        camera.target = (Vector2){player.x + player.width / 2.0f, player.y + player.height / 2.0f};

        gameTimer -= deltaTime;
        if (gameTimer <= 0.0f)
        {
          gameTimer = 0.0f;
          gameState = DEAD;
        }

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
          rebuildWorld(&player, &pillars, &enemies, &bullets, &attacks, &playerARBullets, 
                       &playerShotgunPellets, &playerRockets, &playerExplosions);
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

      case WON:
      {
        exitTimer -= deltaTime;
        if (exitTimer <= 0.0f)
        {
          goto cleanup; // Exit the game loop
        }

        BeginDrawing();
        ClearBackground(BLACK);
        
        const char* winText = "VIRUS REPELLED";
        int fontSize = 60;
        DrawText(winText, screenWidth / 2 - MeasureText(winText, fontSize) / 2, screenHeight / 2 - 50, fontSize, GREEN);
        
        char exitText[64];
        sprintf(exitText, "EXITING IN %d...", (int)exitTimer);
        DrawText(exitText, screenWidth / 2 - MeasureText(exitText, 30) / 2, screenHeight / 2 + 50, 30, RAYWHITE);
        
        EndDrawing();
        break;
      }
    }
  }

cleanup:
    freeEnemies(&enemies);
    freePillars(&pillars);
    freeRangedEnemyBullets(&bullets);
    freeMeleeEnemyAttacks(&attacks);
    freePlayerARBullets(&playerARBullets);
    freePlayerShotgunPellets(&playerShotgunPellets);
    freePlayerRockets(&playerRockets);
    freePlayerExplosions(&playerExplosions);
    CloseWindow();
}
