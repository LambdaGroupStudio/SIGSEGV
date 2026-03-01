#include "player.h"
#include "enemy.h"
#include "globals.h"
#include "helpers.h"
#include "raylib.h"
#include <math.h>

PlayerARBullet initPlayerARBullet(float x, float y, float velocityX, float velocityY, int damage)
{
  PlayerARBullet bullet;
  bullet.x         = x;
  bullet.y         = y;
  bullet.velocityX = velocityX;
  bullet.velocityY = velocityY;
  bullet.damage    = damage;
  return bullet;
}

PlayerShotgunPellet initPlayerShotgunPellet(float x, float y, float velocityX, float velocityY,
                                            int damage)
{
  PlayerShotgunPellet pellet;
  pellet.x         = x;
  pellet.y         = y;
  pellet.velocityX = velocityX;
  pellet.velocityY = velocityY;
  pellet.damage    = damage;
  return pellet;
}

PlayerRocket initPlayerRocket(float x, float y, float velocityX, float velocityY, int damage,
                              int explosionRadius)
{
  PlayerRocket rocket;
  rocket.x               = x;
  rocket.y               = y;
  rocket.velocityX       = velocityX;
  rocket.velocityY       = velocityY;
  rocket.damage          = damage;
  rocket.explosionRadius = explosionRadius;
  return rocket;
}

void diePlayer(void)
{
  // For now we just print SIGSEGV on the screen and stop accepting input
  DrawText("SIGSEGV", GetScreenWidth() / 2 - MeasureText("SIGSEGV", 50) / 2,
           GetScreenHeight() / 2 - 25, 50, RED);
}

void takeDamage(Player* player, MeleeEnemyAttacks* attacks, RangedEnemyBullets* bullets)
{
  for (size_t i = 0; i < attacks->size; i++)
  {
    MeleeEnemyAttack* a = dyn_arr_get(attacks, i);
    if (!a->hasDealtDamage && isColliding(player->x, player->y, player->width, player->height, a->x,
                                          a->y, a->width, a->height))
    {
      player->hp -= a->damage;
      a->hasDealtDamage = true;
    }
  }

  for (size_t i = 0; i < bullets->size; i++)
  {
    RangedEnemyBullet* b = dyn_arr_get(bullets, i);
    if (isColliding(player->x, player->y, player->width, player->height, b->x, b->y,
                    ENEMY_BULLET_SIZE, ENEMY_BULLET_SIZE))
    {
      player->hp -= b->damage;
      dyn_arr_pop_at(bullets, i);
      i--;
    }
  }
}

bool isPlayerDead(Player* player) { return player->hp <= 0; }

void initPlayerARBullets(PlayerARBullets* bullets)
{
  *bullets = dyn_arr_create(sizeof(PlayerARBullet));
}

void initPlayerShotgunPellets(PlayerShotgunPellets* pellets)
{
  *pellets = dyn_arr_create(sizeof(PlayerShotgunPellet));
}

void initPlayerRockets(PlayerRockets* rockets) { *rockets = dyn_arr_create(sizeof(PlayerRocket)); }

void displayPlayer(Player player)
{
  DrawRectangle((int)player.x, (int)player.y, player.width, player.height, RED);
}

void handleMovement(Player* player)
{
  if (IsKeyDown(KEY_D))
  {
    if (player->velocityX < 0.0f)
      player->velocityX = 0.0f;
    player->velocityX += player->acceleration * deltaTime;
  }
  else if (IsKeyDown(KEY_A))
  {
    if (player->velocityX > 0.0f)
      player->velocityX = 0.0f;
    player->velocityX -= player->acceleration * deltaTime;
  }
  else
  {
    if (player->velocityX > 0.0f)
    {
      player->velocityX -= player->friction * deltaTime;
      if (player->velocityX < 0.0f)
        player->velocityX = 0.0f;
    }
    else if (player->velocityX < 0.0f)
    {
      player->velocityX += player->friction * deltaTime;
      if (player->velocityX > 0.0f)
        player->velocityX = 0.0f;
    }
  }
  if (player->velocityX > player->maxSpeed)
    player->velocityX = player->maxSpeed;
  if (player->velocityX < -player->maxSpeed)
    player->velocityX = -player->maxSpeed;
}

void handleGunStateMachine(Player* player)
{
  if (IsKeyPressed(KEY_ONE))
  {
    player->weapon = AR;
  }
  else if (IsKeyPressed(KEY_TWO))
  {
    player->weapon = SHOTGUN;
  }
  else if (IsKeyPressed(KEY_THREE))
  {
    player->weapon = ROCKET_LAUNCHER;
  }
}

void playerShoot(Player* player, float targetX, float targetY, PlayerARBullets* arBullets,
                 PlayerShotgunPellets* shotgunPellets, PlayerRockets* rockets)
{
  if (!player->wantsToShoot || player->reloadTimer > 0.0f)
    return;

  float px = player->x + player->width * 0.5f;
  float py = player->y + player->height * 0.5f;

  float dx     = targetX - px;
  float dy     = targetY - py;
  float distSq = dx * dx + dy * dy;

  if (distSq <= 0.0001f)
    return;

  float invDist = 1.0f / sqrtf(distSq);
  float dirX    = dx * invDist;
  float dirY    = dy * invDist;

  switch (player->weapon)
  {
    case AR:
    {
      PlayerARBullet bullet = initPlayerARBullet(px, py, dirX * PLAYER_AR_SPEED,
                                                 dirY * PLAYER_AR_SPEED, PLAYER_AR_DAMAGE);
      dyn_arr_push_back(arBullets, &bullet);
      player->reloadTimer = PLAYER_AR_RELOAD;
      break;
    }

    case SHOTGUN:
    {
      float baseAngle = atan2f(dirY, dirX);
      for (int i = 0; i < PLAYER_SHOTGUN_PELLET_COUNT; i++)
      {
        float jitter = ((float)rand() / (float)RAND_MAX - 0.5f) * PLAYER_SHOTGUN_SPREAD;
        float angle  = baseAngle + jitter;

        PlayerShotgunPellet pellet =
          initPlayerShotgunPellet(px, py, cosf(angle) * PLAYER_SHOTGUN_SPEED,
                                  sinf(angle) * PLAYER_SHOTGUN_SPEED, PLAYER_SHOTGUN_DAMAGE);
        dyn_arr_push_back(shotgunPellets, &pellet);
      }
      player->reloadTimer = PLAYER_SHOTGUN_RELOAD;
      break;
    }

    case ROCKET_LAUNCHER:
    {
      PlayerRocket rocket = initPlayerRocket(px, py, dirX * PLAYER_ROCKET_SPEED,
                                             dirY * PLAYER_ROCKET_SPEED, PLAYER_ROCKET_DAMAGE, 100);
      dyn_arr_push_back(rockets, &rocket);
      player->reloadTimer = PLAYER_ROCKET_RELOAD;
      break;
    }

    default:
      break;
  }
}

void updatePlayerARBullets(Player* player, PlayerARBullets* bullets)
{
  for (size_t i = 0; i < bullets->size; i++)
  {
    PlayerARBullet* b = dyn_arr_get(bullets, i);
    b->x += b->velocityX * deltaTime;
    b->y += b->velocityY * deltaTime;

    // we update wrt player space (with max bounds based on world)
    //
    // calculate dx and dy and solve
    // the distance eqn (if too far) we
    // pop the projectile
    //
    // same logic applies for pellets and rockets
    const float dx = b->x - player->x;
    const float dy = b->y - player->y;

    if (dx * dx + dy * dy > PROJECTILE_CLEANUP_THRESHOLD_SQUARED)
    {
      dyn_arr_pop_at(bullets, i);
      i--;
    }
  }
}

void displayPlayerARBullets(PlayerARBullets* bullets)
{
  for (size_t i = 0; i < bullets->size; i++)
  {
    PlayerARBullet* b = dyn_arr_get(bullets, i);
    DrawCircle((int)b->x, (int)b->y, (int)PLAYER_AR_BULLET_SIZE, YELLOW);
  }
}

void freePlayerARBullets(PlayerARBullets* bullets) { dyn_arr_free(bullets); }

void updatePlayerShotgunPellets(Player* player, PlayerShotgunPellets* pellets)
{
  for (size_t i = 0; i < pellets->size; i++)
  {
    PlayerShotgunPellet* p = dyn_arr_get(pellets, i);
    p->x += p->velocityX * deltaTime;
    p->y += p->velocityY * deltaTime;

    const float dx = p->x - player->x;
    const float dy = p->y - player->y;

    if (dx * dx + dy * dy > PROJECTILE_CLEANUP_THRESHOLD_SQUARED)
    {
      dyn_arr_pop_at(pellets, i);
      i--;
    }
  }
}

void displayPlayerShotgunPellets(PlayerShotgunPellets* pellets)
{
  for (size_t i = 0; i < pellets->size; i++)
  {
    PlayerShotgunPellet* p = dyn_arr_get(pellets, i);
    DrawCircle((int)p->x, (int)p->y, (int)PLAYER_SHOTGUN_PELLET_SIZE, GRAY);
  }
}

void freePlayerShotgunPellets(PlayerShotgunPellets* pellets) { dyn_arr_free(pellets); }

void updatePlayerRockets(Player* player, PlayerRockets* rockets)
{
  for (size_t i = 0; i < rockets->size; i++)
  {
    PlayerRocket* r = dyn_arr_get(rockets, i);
    r->x += r->velocityX * deltaTime;
    r->y += r->velocityY * deltaTime;

    float dx = r->x - player->x;
    float dy = r->y - player->y;

    if (dx * dx + dy * dy > PROJECTILE_CLEANUP_THRESHOLD_SQUARED)
    {
      dyn_arr_pop_at(rockets, i);
      i--;
    }
  }
}

void displayPlayerRockets(PlayerRockets* rockets)
{
  for (size_t i = 0; i < rockets->size; i++)
  {
    PlayerRocket* r = dyn_arr_get(rockets, i);
    DrawRectangle((int)r->x - (int)(PLAYER_ROCKET_SIZE / 2.0f),
                  (int)r->y - (int)(PLAYER_ROCKET_SIZE / 2.0f), (int)PLAYER_ROCKET_SIZE,
                  (int)PLAYER_ROCKET_SIZE, RED);
  }
}

void freePlayerRockets(PlayerRockets* rockets) { dyn_arr_free(rockets); }

void handlePlayerCollisions(Player* player, Pillars* pillars)
{
  player->isGrounded = false;
  float buffer       = 10.0f;

  player->x += player->velocityX * deltaTime;

  for (size_t i = 0; i < pillars->size; i++)
  {
    const Pillar* p = dyn_arr_get(pillars, i);
    if (isColliding(player->x, player->y, player->width, player->height, p->x, p->y, p->width,
                    p->height))
    {
      if (player->velocityX > 0.0f)
        player->x = p->x - player->width;
      else if (player->velocityX < 0.0f)
        player->x = p->x + p->width;
      player->velocityX = 0.0f;
    }
  }

  player->y += player->velocityY * deltaTime;

  for (size_t i = 0; i < pillars->size; i++)
  {
    const Pillar* p = dyn_arr_get(pillars, i);
    if (isColliding(player->x, player->y, player->width, player->height, p->x, p->y, p->width,
                    p->height))
    {
      if (player->velocityY >= 0.0f)
      {
        player->y          = p->y - (float)player->height;
        player->isGrounded = true;
        player->velocityY  = 0.0f;
      }
      else
      {
        player->y         = p->y + (float)p->height;
        player->velocityY = 0.0f;
      }
    }
  }

  float groundY = (float)GetScreenHeight() - (float)player->height;
  if (player->y >= groundY - buffer && player->velocityY >= 0.0f)
  {
    player->y          = groundY;
    player->velocityY  = 0.0f;
    player->isGrounded = true;
  }
  else
  {
    // Also check if we are grounded on top of any pillar due to buffer
    for (size_t i = 0; i < pillars->size; i++)
    {
      const Pillar* p = dyn_arr_get(pillars, i);
      if (isColliding(player->x, player->y + buffer, player->width, player->height, p->x, p->y,
                      p->width, p->height))
      {
        player->isGrounded = true;
        break;
      }
    }
  }

  // Screen bounds top/bottom Y
  if (player->y < -1000.0f)
  { // Some limit to prevent flying away forever
    player->y         = -1000.0f;
    player->velocityY = 0.0f;
  }
}

void handleJump(Player* player)
{
  if (IsKeyPressed(KEY_SPACE) && player->isGrounded)
  {
    player->velocityY  = -player->jumpStrength;
    player->isGrounded = false;
  }
}

void handlePlayerGravity(Player* player)
{
  if (!player->isGrounded)
  {
    player->velocityY += gravity * deltaTime;
  }
}

void updatePlayer(Player* player, Pillars* pillars)
{
  if (isPlayerDead(player))
  {
    player->velocityX = 0;
    player->velocityY = 0;
    diePlayer();
    return;
  }
  handlePlayerCollisions(player, pillars);
  handleMovement(player);
  handleJump(player);
  handlePlayerGravity(player);
  handleGunStateMachine(player);
  player->reloadTimer -= deltaTime;
  if (player->reloadTimer < 0.0f)
    player->reloadTimer = 0.0f;
  displayPlayer(*player);
}

Player initPlayer(void)
{
  Player player       = {0};
  player.x            = 100.0f;
  player.y            = 100.0f;
  player.width        = 100.0f;
  player.height       = 100.0f;
  player.velocityX    = 0.0f;
  player.velocityY    = 0.0f;
  player.acceleration = 2000.0f;
  player.friction     = 1500.0f;
  player.maxSpeed     = 600.0f;
  player.jumpStrength = 600.0f;
  player.isGrounded   = false;
  player.wantsToShoot = false;
  player.reloadTimer  = 0.0f;
  player.reloadSpeed  = 1.0f;
  player.weapon       = AR;
  player.hp           = 100;
  return player;
}
