#include <raylib.h>
#include "funcs.h"

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 440;

  InitWindow(screenWidth, screenHeight, "Split screen platformer");

  Rectangle player1 = {10, 200, PLAYER_SIZE, PLAYER_SIZE};
  Rectangle player2 = {LARGEST_PLATFORM_WIDTH - 10 - PLAYER_SIZE, 200, PLAYER_SIZE, PLAYER_SIZE};
  float secondsForP1;
  float secondsForP2;
  float lastStandPos1 = 200;
  float lastStandPos2 = 200;
  bool player1Falling = false;
  bool player2Falling = false;
  int p1Rate = 0;
  int p2Rate = 0;
  int lastDir1 = LEFT;
  int lastDir2 = LEFT;
  int lastShoot1 = 0;
  int lastShoot2 = 0;
  
  for (int i = 0; i < 100; i++){
    bullets[i].framesSurvived = -1;
  }

	EnvItem envItems[] = {
    {{0, 0, LARGEST_PLATFORM_WIDTH, 400}, 0, LIGHTGRAY},
    {{0, 400, LARGEST_PLATFORM_WIDTH, 200}, 1, GRAY},
    {{300, 200, 400, 10}, 1, GRAY},
    {{250, 300, 100, 10}, 1, GRAY},
    {{650, 300, 100, 10}, 1, GRAY},
    {{850, 300, 200, 10}, 1, GRAY},
    {{950, 150, 400, 10}, 1, GRAY},
    {{1150, 200, 350, 10}, 1, GRAY},
  };
	int envItemsLength = sizeof(envItems) / sizeof(envItems[0]);

  Camera2D camera1 = {0};
  camera1.target = (Vector2){player1.x, player1.y};
  camera1.offset = (Vector2){200.0f, 200.0f};
  camera1.rotation = 0.0f;
  camera1.zoom = 1.0f;

  Camera2D camera2 = {0};
  camera2.target = (Vector2){player2.x, player2.y};
  camera2.offset = (Vector2){200.0f, 200.0f};
  camera2.rotation = 0.0f;
  camera2.zoom = 1.0f;

  RenderTexture screenCamera1 = LoadRenderTexture(screenWidth / 2, screenHeight);
  RenderTexture screenCamera2 = LoadRenderTexture(screenWidth / 2, screenHeight);

  // Build a flipped rectangle the size of the split view to use for drawing
  // later
  Rectangle splitScreenRect = {
    0.0f, 
    0.0f, 
    (float)screenCamera1.texture.width,
    (float)-screenCamera1.texture.height
  };

  SetTargetFPS(60);  // Set our game to run at 60 frames-per-second

  // Main game loop
  while (!WindowShouldClose()) {
    frameCounter++;

    movePlayer(&player1, envItems, envItemsLength, (MoveControls){KEY_A, KEY_D , KEY_W}, &secondsForP1, &lastStandPos1, &player1Falling, &p1Rate, &lastDir1);
    movePlayer(&player2, envItems, envItemsLength, (MoveControls){KEY_LEFT, KEY_RIGHT, KEY_UP}, &secondsForP2, &lastStandPos2, &player2Falling, &p2Rate, &lastDir2);

    bulletStuff(&player1, &player2);

    if(frameCounter - lastShoot1 > FIRE_RATE) shootStuff(&player1, KEY_SPACE, lastDir1, &lastShoot1);
    if(frameCounter - lastShoot2 > FIRE_RATE) shootStuff(&player2, KEY_ENTER, lastDir2, &lastShoot2);

    camera1.target = (Vector2){player1.x, player1.y};
    camera2.target = (Vector2){player2.x, player2.y};

    drawStuff(&screenCamera1, camera1, envItemsLength, envItems, &player1, &player2, RED, BLUE, p1Msg);
    drawStuff(&screenCamera2, camera2, envItemsLength, envItems, &player2, &player1, BLUE, RED, p2Msg);

    // Draw both views render textures to the screen side by side
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTextureRec(screenCamera1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
    DrawTextureRec(screenCamera2.texture, splitScreenRect, (Vector2){screenWidth / 2.0f, 0}, WHITE);

    DrawRectangle(GetScreenWidth() / 2 - 2, 0, 4, GetScreenHeight(), GRAY);
    EndDrawing();
  }

  // De-Initialization
  UnloadRenderTexture(screenCamera1);  // Unload render texture
  UnloadRenderTexture(screenCamera2);  // Unload render texture

  CloseWindow();  // Close window and OpenGL context

  return 0;
}
