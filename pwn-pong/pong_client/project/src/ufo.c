#include "ufo.h"

extern int getFreeEntity(void);
extern void drawStandardEntity(void);
extern SDL_Surface *getSprite(int);

static void moveUFO(int,int);

void addUFO(int x, int y)
{
	int i = getFreeEntity();
	
	if (i == -1)
	{
		printf("Couldn't get a free slot for a UFO!\n");
		
		return;
	}
	
	entity[i].x = x;
	entity[i].y = y;
	entity[i].action = &moveUFO;
	entity[i].draw = &drawStandardEntity;
	entity[i].sprite = getSprite(UFO_SPRITE);
	entity[i].type = TYPE_ENEMY;
}

static void moveUFO(int x,int y)
{
	/* Do nothing, just sit there */
}
