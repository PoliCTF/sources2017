#include "bullet.h"

extern int getFreeEntity(void);
extern void drawStandardEntity(void);
extern SDL_Surface *getSprite(int);
//  extern void playSound(int);
extern void drawString(char *, int, int, TTF_Font *, int, int);

static void moveStandardBullet(int,int);

void addBullet(int x, int y)
{
//	int i = getFreeEntity();
//	
//	if (i == -1)
//	{
//		printf("Couldn't get a free slot for a bullet!\n");
//		
//		return;
//	}
	
//	entity[i].x = x;
//	entity[i].y = y;
//	entity[i].action = &moveStandardBullet;
//	entity[i].draw = &drawStandardEntity;
//	entity[i].sprite = getSprite(BULLET_SPRITE);
//	entity[i].type = TYPE_BULLET;
    memset(&ball,0,sizeof(Entity));
    ball.x = x;
    ball.y = y;
    ball.action = &moveStandardBullet;
    ball.draw = &drawStandardEntity;
    ball.sprite = getSprite(BULLET_SPRITE);
    ball.type = TYPE_BULLET;



//    /* Play a sound when the shot is fired */
//
//    playSound(BULLET_SOUND);
}

static void moveStandardBullet(int x,int y)
{
    self->x=x;
    self->y=y;
}
