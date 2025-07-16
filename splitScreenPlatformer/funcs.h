#include <stdio.h>
#include <raylib.h>
#include <time.h>
#include "globals.h"

bool platformBelow(PlayerInfo *player, float prevY, Platform platform){
  return ((player->rect.x + player->rect.width) > platform.rect.x) &&
    (player->rect.x < (platform.rect.x + platform.rect.width)) &&
    ((prevY + player->rect.height) <= platform.rect.y) &&
    ((player->rect.y + player->rect.height) >= platform.rect.y);
}

bool playerFall(PlayerInfo *player, Platform *platforms, int totalPlatforms){
  float prevY = player->rect.y;
  player->rect.y += player->ySpeed;
  player->ySpeed += GRAVITY;
  if (player->ySpeed > 0) {
    for (int i = 0; i < totalPlatforms; i++){
      if (platformBelow(player, prevY, platforms[i])) {
        player->rect.y = platforms[i].rect.y - player->rect.height;
        player->standingOn = &(platforms[i]);
        player->isFalling = false;
      };
    }
  }
}

void movePlayer(PlayerInfo *player, Platform *platforms, const int totalPlatforms){
  if (IsKeyDown(player->controls.right_key)){
    player->facingRight = true;
    if (player->rect.x < LARGEST_PLATFORM_WIDTH - PLAYER_SIZE) player->rect.x += 3.0f;
  }
  else if (IsKeyDown(player->controls.left_key)){
    player->facingRight = false;
    if (player->rect.x > 0) player->rect.x -= 3.0f;
  }
  
  if (!(player->isFalling)){
    if (IsKeyPressed(player->controls.up_key)) {
      PlaySound(g_jumpSound);
      player->isFalling = true;
      player->ySpeed = INITIAL_JUMP_SPEED;
    }
    else if (IsKeyPressed(player->controls.bottom_key) && player->standingOn->jumpThrough) {
      player->rect.y += 1;
      player->isFalling = true;
      player->ySpeed = 0;
    }
  }

  if (player->isFalling) playerFall(player, platforms, totalPlatforms);
  else{
    for (int i = 0; i < totalPlatforms; i++){
      if (platformBelow(player, player->rect.y, platforms[i])){
        player->isFalling = false;
        break;
      }
      else player->isFalling = true;
    }
  }
  
}

void drawStuff(RenderTexture *screenCamera, Camera2D camera, Platform *platforms, const int totalPlatforms, PlayerInfo *player1, PlayerInfo *player2, char *topMessage){
  BeginTextureMode(*screenCamera);
  ClearBackground(LIGHTGRAY);

  BeginMode2D(camera);
  
  for (int i = 0; i < totalPlatforms; i++)
    DrawRectangleRec(platforms[i].rect, PLATFORM_COLOR);

  DrawRectangleRec(player2->rect, player2->color);
  DrawRectangleRec(player1->rect, player1->color);

  for (int i = 0; i < MAX_BULLETS && g_bullets[i].onScreen; i++)
    DrawRectangle(g_bullets[i].x, g_bullets[i].y, BULLET_SIZE, BULLET_SIZE, YELLOW);

  EndMode2D();

  //top text
  DrawRectangle(0, 0, GetScreenWidth() / 2, 30, Fade(RAYWHITE, 0.6f));
  DrawText(topMessage, 10, 10, 10, BLACK);

  EndTextureMode();
}

bool checkPlayerHit(PlayerInfo *player, int i){
  return (
    (g_bullets[i].x + BULLET_SIZE) > player->rect.x &&
    g_bullets[i].x < (player->rect.x + player->rect.width) &&
    (g_bullets[i].y + BULLET_SIZE) > player->rect.y &&
    g_bullets[i].y < (player->rect.y + player->rect.height)
  );
}

void bulletStuff(PlayerInfo *player1, PlayerInfo *player2){
  for (int i = 0; i < MAX_BULLETS; i++){
    if (g_bullets[i].onScreen) {
      if (g_bullets[i].facingRight) g_bullets[i].x += (float) BULLET_SPEED;
      else g_bullets[i].x += (float) -BULLET_SPEED;
      if (checkPlayerHit(player1, i)) {
        PlaySound(g_hurtSound);
        p2Score += 1;
        sprintf(g_p2Msg, "P2 Score: %d", p2Score);
        g_bullets[i].onScreen = false;
      }
      else if (checkPlayerHit(player2, i)){
        PlaySound(g_hurtSound);
        p1Score += 1;
        sprintf(g_p1Msg, "P1 Score: %d", p1Score);
        g_bullets[i].onScreen = false;
      }
      else if (g_bullets[i].x < -100 || g_bullets[i].x > LARGEST_PLATFORM_WIDTH + 100)
        g_bullets[i].onScreen = false;
    }
  }
}

void shootStuff(PlayerInfo *player){
  PlaySound(g_shootSound);
  player->lastShootTime = GetTime();
  
  int i = 0;
  while (g_bullets[i].onScreen && i <= MAX_BULLETS) i++;
  if (i == MAX_BULLETS) return;

  g_bullets[i].facingRight = player->facingRight;
  if (g_bullets[i].facingRight) g_bullets[i].x = player->rect.x + player->rect.width;
  else g_bullets[i].x = player->rect.x - BULLET_SIZE;
  g_bullets[i].y = player->rect.y + (player->rect.height/2);
  g_bullets[i].onScreen = true;
}