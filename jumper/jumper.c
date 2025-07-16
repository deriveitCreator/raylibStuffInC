#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define FLOOR_Y 320
#define GRAVITY 250 //px/sec^2

#define PLAYER_WIDTH 20
#define PLAYER_HEIGHT 30
#define INITIAL_Y_VELOCITY -220 // px/sec
#define PLAYER_INITIAL_Y_POS ( FLOOR_Y - PLAYER_HEIGHT )

#define MAX_ENEMIES 6
#define ENEMY_HEIGHT 20
#define ENEMY_INITIAL_X ( SCREEN_WIDTH + 10 )
#define ENEMY_WIDTH_RANGE 10
#define ENEMY_SPEED_RANGE 4
#define EMENY_Y_POS ( FLOOR_Y - ENEMY_HEIGHT )

typedef enum { TITLE, GAMEPLAY } GameScreen;

typedef enum { SINGLE_PLAYER, TWO_PLAYER } PlayerMode;

typedef struct {
  Color color;
  struct Rectangle rec;
  bool isjumping;
  float secondsInAir;
  bool won;
} PlayerRectangle;

typedef struct {
  float x;                // Rectangle top-left corner position x
  float y;                // Rectangle top-left corner position y
  float width;            // Rectangle width
  float height;           // Rectangle height
  float speed;
} EnemyRectangle;


void doJumpLogic(PlayerRectangle *player, Sound *jumpSound, bool jumpKeyPressed){   
  if (jumpKeyPressed && !player->isjumping){
    player->secondsInAir = 0;
    player->isjumping = true;
    PlaySound(*jumpSound);
  }
  
  if(player->isjumping){
    float sec = player->secondsInAir;
    player->rec.y = ((INITIAL_Y_VELOCITY*sec) + (0.5*GRAVITY*sec*sec)) + PLAYER_INITIAL_Y_POS;
    if(player->rec.y > PLAYER_INITIAL_Y_POS){
      player->rec.y = PLAYER_INITIAL_Y_POS;
      player->isjumping = false;
    }
    else player->secondsInAir += (1.0/60.0);
  }
}

void maybeGoLeft(PlayerRectangle *player){
  if (player->rec.x > 10)
    player->rec.x -= 2;
}

void maybeGoRight(PlayerRectangle *player){
  if (player->rec.x < (SCREEN_WIDTH - player->rec.width) -10)
    player->rec.x += 2;
}

void setEnemyObject(EnemyRectangle **enemy, float minimumWidth, float minimumSpeed){
  *enemy = malloc(sizeof(EnemyRectangle));
  (**enemy).x = ENEMY_INITIAL_X;
  (**enemy).y = EMENY_Y_POS;
  (**enemy).width = (( ((float) rand()) / (float) RAND_MAX) * ENEMY_WIDTH_RANGE) + minimumWidth;
  (**enemy).height = ENEMY_HEIGHT;
  (**enemy).speed = (( ((float) rand()) / (float) RAND_MAX) * ENEMY_SPEED_RANGE) + minimumSpeed;
}

bool playerEnemyOverlap(PlayerRectangle *player, EnemyRectangle **enemy){
  if(player->rec.x < (**enemy).x){
    if(
      ((**enemy).x - player->rec.x < player->rec.width) &&
      (player->rec.y + player->rec.height >= (**enemy).y)
    ) return true;
  }
  else if (
    (player->rec.x - (**enemy).x < (**enemy).width) &&
    (player->rec.y + player->rec.height >= (**enemy).y)
  ) return true;
  return false;
}

void updateScoreText(float score, int x, int y, Color c){
  static float highestScore = 0;
  char scoreStr[15];
  if (score > highestScore) highestScore = score;
  sprintf(scoreStr, "Score: %d", (int) (score*100));
  DrawText(scoreStr, x, y, 20, c);
  sprintf(scoreStr, "Highest: %d", (int) (highestScore*100));
  DrawText(scoreStr, x, y+20, 20, c);
}

void drawEnemyRectangles(EnemyRectangle **enemies){
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if(enemies[i]){
      Rectangle enemyRectangle = {
        enemies[i]->x, 
        enemies[i]->y, 
        enemies[i]->width, 
        enemies[i]->height
      };
      DrawRectangleRec(enemyRectangle, DARKGREEN);
    }
  }
}

int main(void) {
  PlayerMode playerMode = SINGLE_PLAYER;
  GameScreen currentScreen = TITLE;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jumpy man");
  InitAudioDevice();
  SetTargetFPS(60);
  
  const Rectangle floorRec = {0, FLOOR_Y, SCREEN_WIDTH, SCREEN_HEIGHT-FLOOR_Y};
  const int defaultXPos = (SCREEN_WIDTH-PLAYER_WIDTH)/2;
  const PlayerRectangle player1Default = { RED, { defaultXPos, PLAYER_INITIAL_Y_POS, PLAYER_WIDTH, PLAYER_HEIGHT }, false, 0.0, false};
  const PlayerRectangle player2Default = { BLUE, { defaultXPos, PLAYER_INITIAL_Y_POS, PLAYER_WIDTH, PLAYER_HEIGHT }, false, 0.0, false};
  PlayerRectangle player1;
  PlayerRectangle player2;
  EnemyRectangle *enemies[MAX_ENEMIES];
  float minimumEnemyWidth;
  float minimumEnemySpeed;
  float score;
  
  Texture2D backgroundImg = LoadTexture("resources/background.png");
  Sound jumpSound = LoadSound("resources/jump.mp3");
  Sound loseSound = LoadSound("resources/lost.mp3");

  while (!WindowShouldClose()){
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawRectangleRec(floorRec, DARKGRAY);
    DrawTexture(backgroundImg, -15, FLOOR_Y - backgroundImg.height, WHITE);

    switch(currentScreen){
      case GAMEPLAY: {
        doJumpLogic(&player1, &jumpSound, IsKeyDown(KEY_W));
        if (IsKeyDown(KEY_A)) maybeGoLeft(&player1);
        if (IsKeyDown(KEY_D)) maybeGoRight(&player1);

        if(playerMode == TWO_PLAYER){
          doJumpLogic(&player2, &jumpSound, IsKeyDown(KEY_UP));
          if (IsKeyDown(KEY_LEFT)) maybeGoLeft(&player2);
          if (IsKeyDown(KEY_RIGHT)) maybeGoRight(&player2);
        }

        for (int i = 0; i < MAX_ENEMIES; i++) {
          if (!enemies[i]) {
            minimumEnemyWidth += 0.5;
            minimumEnemySpeed += 0.1;
            setEnemyObject(&(enemies[i]), minimumEnemyWidth, minimumEnemySpeed);
            continue;
          }

          enemies[i]->x -= enemies[i]->speed; //enemy moves forward

          if (playerEnemyOverlap(&player1, &(enemies[i]))) {
            PlaySound(loseSound);
            player2.won = true;
            currentScreen = TITLE;
            break;
          }

          if ((playerMode == TWO_PLAYER) && playerEnemyOverlap(&player2, &(enemies[i]))) {
            PlaySound(loseSound);
            player1.won = true;
            currentScreen = TITLE;
            break;
          }
          
          if(enemies[i]->x + enemies[i]->width < 0){
            free(enemies[i]);
            enemies[i] = NULL;
          }
        }

        updateScoreText(score += 0.01, 640, 10, BLACK);
      } break;
      
      case TITLE: {  
        DrawText("Press S to play in Single player mode", 180, 120, 20, DARKGREEN);
        DrawText("Press T to play in Two player mode", 180, 160, 20, DARKGREEN);
        
        if (score) {
          updateScoreText(score, 180, 60, BLACK);
          if (playerMode == TWO_PLAYER) {
            if (player1.won) DrawText("Player 1 wins", 180, 40, 20, player1.color);
            else DrawText("Player 2 wins", 180, 40, 20, player2.color);
          }
        }

        if (IsKeyPressed(KEY_S)){
          playerMode = SINGLE_PLAYER;
          player1 = player1Default;
          score = 0.0;
          for (int i = 0; i < MAX_ENEMIES; i++) enemies[i] = NULL;
          minimumEnemyWidth = 20.0;
          minimumEnemySpeed = 0.5;
          currentScreen = GAMEPLAY;
        }
        else if (IsKeyPressed(KEY_T)){
          playerMode = TWO_PLAYER;
          player1 = player1Default;
          player2 = player2Default;
          score = 0.0;
          for (int i = 0; i < MAX_ENEMIES; i++) enemies[i] = NULL;
          minimumEnemyWidth = 20.0;
          minimumEnemySpeed = 0.5;
          currentScreen = GAMEPLAY;
        }

      } break;
      default: break;
    }
    
    // draw player 2 below player 1
    if (playerMode == TWO_PLAYER) DrawRectangleRec(player2.rec, player2.color);
    DrawRectangleRec(player1.rec, player1.color);
    drawEnemyRectangles(enemies);
    EndDrawing();
  }

  UnloadSound(loseSound);
  UnloadSound(jumpSound);
  UnloadTexture(backgroundImg);

  CloseAudioDevice();
  CloseWindow();

  return 0;
}
