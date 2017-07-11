#include "defs.h"

typedef struct Game
{
	int score;
	SDL_Surface *screen;
	TTF_Font *font;
} Game;

typedef struct Sprite
{
	SDL_Surface *image;
} Sprite;

typedef struct Sound
{
	Mix_Chunk *effect;
} Sound;

typedef struct Entity
{
	int active, type;
	int x, y, thinkTime;
	SDL_Surface *sprite;
	void (*action)(int,int);
	void (*draw)(void);
} Entity;

typedef struct Control
{
	int up1, down1,up2, down2, left, right, fire;
} Control;
