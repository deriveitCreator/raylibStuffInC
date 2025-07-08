#include <raylib.h>

// CONSTANTS
#define PLAYER_SIZE 40
#define MAX_BULLETS 150
#define GRAVITY 250
#define LARGEST_PLATFORM_WIDTH 1500
#define FIRE_RATE 50

// TYPES
typedef struct EnvItem {
	Rectangle rect;
	int blocking;
	Color color;
} EnvItem;

typedef struct MoveControls {
  int left_key;
	int right_key;
  int up_key;
} MoveControls;

typedef enum {
  LEFT = -1,
  RIGHT = 1
} Directions;

typedef struct Bullet{
  int framesSurvived;
  float x;
  float y;
  Directions dir;
} Bullet;

// VARIABLES
Bullet bullets[MAX_BULLETS];
int frameCounter = 0;
int p1Score = 0;
int p2Score = 0;
char p1Msg[100] = "W/S/A/D to move and SPACE to shoot";
char p2Msg[100] = "UP/DOWN/LEFT/RIGHT to move and ENTER to shoot";