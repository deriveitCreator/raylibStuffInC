#include <raylib.h>

// CONSTANTS
#define PLAYER_SIZE 40
#define INITIAL_Y_POS 200
#define INITIAL_P1_X_POS 10
#define INITIAL_P2_X_POS ( LARGEST_PLATFORM_WIDTH - INITIAL_P1_X_POS - PLAYER_SIZE )
#define INITIAL_JUMP_SPEED -10 // pixels per frame
#define MAX_BULLETS 50
#define BULLET_SPEED 8
#define BULLET_SIZE 10
#define MIN_FIRE_INTERVAL 0.8
#define GRAVITY 0.4 // pixels per frame per frame
#define LARGEST_PLATFORM_WIDTH 1500
#define LARGEST_PLATFORM_Y 400
#define DEFAULT_PLATFORM_WIDTH 10
#define PLATFORM_COLOR GRAY

// TYPES
typedef struct {
	Rectangle rect;
	bool jumpThrough;
	Color color;
} Platform;

typedef struct {
  int left_key;
	int right_key;
  int up_key;
  int bottom_key;
  int shoot_key;
} Controls;

typedef struct {
	Rectangle rect;
  bool facingRight;
  Platform *standingOn;
  double lastShootTime;
  bool isFalling;
  float ySpeed; // pixels per frame
  Color color;
  Controls controls;
} PlayerInfo;

typedef struct {
  bool onScreen;
  bool facingRight;
  float x;
  float y;
} Bullet;

// VARIABLES
Bullet g_bullets[MAX_BULLETS];
int p1Score = 0;
int p2Score = 0;
char g_p1Msg[50] = "W/S/A/D to move and SPACE to shoot";
char g_p2Msg[50] = "UP/DOWN/LEFT/RIGHT to move and ENTER to shoot";
Sound g_jumpSound;
Sound g_shootSound;
Sound g_hurtSound;