#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"

#include <uv.h>
#include <unistd.h>
#include <assert.h>
#include <arpa/inet.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 60
#define BALL_DIMENSION 10
#define GAP 5

#define MAX_ENTITIES 20
#define MAX_RELOAD_TIME 20

#define PLAYER_SPEED 8

enum
{
	PLAYER_SPRITE,
 	BULLET_SPRITE,
  	UFO_SPRITE,
	MAX_SPRITES
};

enum
{
	BULLET_SOUND,
 	MAX_SOUNDS
};

enum
{
	TYPE_BULLET,
 	TYPE_ENEMY
};
