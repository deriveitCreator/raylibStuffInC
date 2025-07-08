#include <stdio.h>
#include <raylib.h>
#include "globals.h"

bool platformBelow(Rectangle *player, float prevY, EnvItem envItem){
  return ((player->x + player->width) > envItem.rect.x) &&
    (player->x < (envItem.rect.x + envItem.rect.width)) &&
    ((prevY + player->height) <= envItem.rect.y) &&
    ((player->y + player->height) >= envItem.rect.y);
}

bool playerFalls(Rectangle *player, float *secondsForP, EnvItem *envItems, int envItemsLength, float lastStandPos, int yRate){
  bool playerFalling = true;
  float prevY = player->y;
  (*secondsForP) += 1.0/60.0;
  player->y = ((yRate*(*secondsForP)) + (0.5*GRAVITY*(*secondsForP)*(*secondsForP))) + lastStandPos;
  if(prevY < player->y){
    for (int i = 0; i < envItemsLength; i++){
      if (platformBelow(player, prevY, envItems[i])){
        player->y = envItems[i].rect.y - player->height;
        playerFalling = false;
        break;
      };
    }
  }
  return playerFalling;
}

void movePlayer(Rectangle *player, EnvItem *envItems, int envItemsLength, MoveControls keys, float *secondsForP, float *lastStandPos, bool *playerFalling, int *yRate, int *lastDir){

  if (IsKeyDown(keys.right_key)){
    if(player->x < LARGEST_PLATFORM_WIDTH - PLAYER_SIZE) player->x += 3.0f;
    *lastDir = RIGHT;
  }
  else if (IsKeyDown(keys.left_key)){
    if(player->x > 0) player->x -= 3.0f;
    *lastDir = LEFT;
  }
  if (IsKeyPressed(keys.up_key) && (!(*playerFalling))){
    (*playerFalling) = true;
    (*lastStandPos) = player->y;
    (*secondsForP) = 0;
    (*yRate) = -250;
  }

  if(*playerFalling)
    (*playerFalling) = playerFalls(player, secondsForP, envItems, envItemsLength, *lastStandPos, *yRate);
  else{
    (*yRate) = 0;
    for (int i = 0; i < envItemsLength; i++){
      if (platformBelow(player, player->y, envItems[i])){
        (*playerFalling) = false;
        break;
      }
      else{
        (*playerFalling) = true;
        (*lastStandPos) = player->y;
        (*secondsForP) = 0;
      }
    }
  }
  
}

void drawStuff(RenderTexture *screenCamera, Camera2D camera, int envItemsLength, EnvItem *envItems, Rectangle *player1, Rectangle *player2, Color color1, Color color2, char *topMessage){
  BeginTextureMode(*screenCamera);
  ClearBackground(LIGHTGRAY);

  BeginMode2D(camera);
  
  for (int i = 0; i < envItemsLength; i++)
    DrawRectangleRec(envItems[i].rect, envItems[i].color);

  DrawRectangleRec(*player2, color2);
  DrawRectangleRec(*player1, color1);

  for (int i = 0; i < MAX_BULLETS; i++)
    if(bullets[i].framesSurvived != -1)
      DrawRectangleRec((Rectangle){bullets[i].x, bullets[i].y, 10, 10}, YELLOW);

  EndMode2D();

  DrawRectangle(0, 0, GetScreenWidth() / 2, 30, Fade(RAYWHITE, 0.6f));
  DrawText(topMessage, 10, 10, 10, BLACK);

  EndTextureMode();
}

bool checkPlayerHit(Rectangle *player, int i){
  return (
    bullets[i].x > player->x &&
    bullets[i].x < (player->x + player->width) &&
    bullets[i].y > player->y &&
    bullets[i].y < (player->y + player->height)
  );
}

void bulletStuff(Rectangle *player1, Rectangle *player2){
  for (int i = 0; i < MAX_BULLETS; i++){
    if(bullets[i].framesSurvived != -1){
      bullets[i].framesSurvived += 1;
      bullets[i].x += (float) (bullets[i].dir*8);
      if (checkPlayerHit(player1, i)) {
        p2Score += 1;
        sprintf(p2Msg, "P2 Score: %d", p2Score);
        bullets[i].framesSurvived = -1;
      }
      else if (checkPlayerHit(player2, i)){
        p1Score += 1;
        sprintf(p1Msg, "P1 Score: %d", p1Score);
        bullets[i].framesSurvived = -1;
      }
      else if (bullets[i].x < -100 || bullets[i].x > LARGEST_PLATFORM_WIDTH + 100)
        bullets[i].framesSurvived = -1;
    }
  }
}

void shootStuff(Rectangle *player, int shoot_key, Directions dir, int *lastShoot){
  if(IsKeyPressed(shoot_key)){
    (*lastShoot) = frameCounter;
    for (int i = 0; i < MAX_BULLETS; i++){
      if(bullets[i].framesSurvived == -1){
        if(dir > 0) bullets[i].x = player->x + player->width;
        else bullets[i].x = player->x - 10;
        bullets[i].y = player->y + (player->height/2);
        bullets[i].dir = dir;
        bullets[i].framesSurvived = 0;
        break;
      }
    }
  }
}