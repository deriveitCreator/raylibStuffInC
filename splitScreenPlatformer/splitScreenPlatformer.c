#include <raylib.h>
#include "funcs.h"

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 440;

  InitWindow(screenWidth, screenHeight, "Split screen platformer");
  InitAudioDevice();

	Platform platforms[] = {
    //{Rect, jumpThrough, Color}
    {{0, LARGEST_PLATFORM_Y, LARGEST_PLATFORM_WIDTH, 200}, false},
    {{100, 100, 200, DEFAULT_PLATFORM_WIDTH}, true},
    {{200, 300, 100, DEFAULT_PLATFORM_WIDTH}, true},
    {{300, 200, 400, DEFAULT_PLATFORM_WIDTH}, true},
    {{320, 0, 150, DEFAULT_PLATFORM_WIDTH}, true},
    {{420, 90, 100, DEFAULT_PLATFORM_WIDTH}, true},
    {{600, 30, 300, DEFAULT_PLATFORM_WIDTH}, true},
    {{650, 300, 100, DEFAULT_PLATFORM_WIDTH}, true},
    {{750, 190, 150, DEFAULT_PLATFORM_WIDTH}, true},
    {{850, 300, 200, DEFAULT_PLATFORM_WIDTH}, true},
    {{950, 100, 400, DEFAULT_PLATFORM_WIDTH}, true},
    {{1150, 200, 350, DEFAULT_PLATFORM_WIDTH}, true},
  };
	const int totalPlatforms = sizeof(platforms) / sizeof(platforms[0]);

  PlayerInfo player1;
  PlayerInfo player2;
  player1.rect = player2.rect = (Rectangle) { INITIAL_P1_X_POS, INITIAL_Y_POS, PLAYER_SIZE, PLAYER_SIZE };
  player2.rect.x = INITIAL_P2_X_POS;
  player1.standingOn = player2.standingOn = NULL;
  player1.lastShootTime = player2.lastShootTime = GetTime();
  player1.isFalling = player2.isFalling = true;
  player1.ySpeed = player2.ySpeed = 0;
  player1.facingRight = true;
  player2.facingRight = false;
  player1.color = RED;
  player2.color = BLUE;
  player1.controls = (Controls) {KEY_A, KEY_D, KEY_W, KEY_S, KEY_SPACE};
  player2.controls = (Controls) {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_ENTER};
  
  for (int i = 0; i < MAX_BULLETS; i++) g_bullets[i].onScreen = false;

  Camera2D camera1 = {0};
  camera1.target = (Vector2){player1.rect.x, player1.rect.y};
  camera1.offset = (Vector2){200.0f, 200.0f};
  camera1.rotation = 0.0f;
  camera1.zoom = 1.0f;

  Camera2D camera2 = {0};
  camera2.target = (Vector2){player2.rect.x, player2.rect.y};
  camera2.offset = (Vector2){200.0f, 200.0f};
  camera2.rotation = 0.0f;
  camera2.zoom = 1.0f;

  RenderTexture screenCamera1 = LoadRenderTexture(screenWidth / 2, screenHeight);
  RenderTexture screenCamera2 = LoadRenderTexture(screenWidth / 2, screenHeight);

  g_jumpSound = LoadSound("resources/jump.mp3");
  g_shootSound = LoadSound("resources/shoot.mp3");
  g_hurtSound = LoadSound("resources/hurt.mp3");

  Rectangle splitScreenRect = {
    0.0f, 
    0.0f, 
    (float) screenCamera1.texture.width,
    (float) -screenCamera1.texture.height
  };

  SetTargetFPS(60); 

  while (!WindowShouldClose()) {
    movePlayer(&player1, platforms, totalPlatforms);
    movePlayer(&player2, platforms, totalPlatforms);

    bulletStuff(&player1, &player2);
    if((GetTime() - player1.lastShootTime > MIN_FIRE_INTERVAL) && IsKeyPressed(player1.controls.shoot_key)) 
      shootStuff(&player1);
    if((GetTime() - player2.lastShootTime > MIN_FIRE_INTERVAL) && IsKeyPressed(player2.controls.shoot_key)) 
      shootStuff(&player2);

    camera1.target = (Vector2) {player1.rect.x, player1.rect.y};
    camera2.target = (Vector2) {player2.rect.x, player2.rect.y};

    BeginDrawing();

    drawStuff(&screenCamera1, camera1, platforms, totalPlatforms, &player1, &player2, g_p1Msg);
    drawStuff(&screenCamera2, camera2, platforms, totalPlatforms, &player2, &player1, g_p2Msg);

    DrawTextureRec(screenCamera1.texture, splitScreenRect, (Vector2) {0, 0}, WHITE);
    DrawTextureRec(screenCamera2.texture, splitScreenRect, (Vector2) {screenWidth / 2.0f, 0}, WHITE);

    //line in middle
    DrawRectangle(GetScreenWidth() / 2 - 2, 0, 4, GetScreenHeight(), PLATFORM_COLOR);
    EndDrawing();
  }

  UnloadRenderTexture(screenCamera1); 
  UnloadRenderTexture(screenCamera2); 
  UnloadSound(g_jumpSound);
  UnloadSound(g_shootSound);
  UnloadSound(g_hurtSound);
  
  CloseAudioDevice();
  CloseWindow();

  return 0;
}
