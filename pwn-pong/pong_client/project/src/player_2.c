#include "player_2.h"

extern int loadSprite(char *);
extern void drawImage(SDL_Surface *, int, int);
extern void addBullet(int, int);
extern SDL_Surface *getSprite(int);

void initPlayer2()
{
	player2.sprite = getSprite(PLAYER_SPRITE);
	
	player2.x = SCREEN_WIDTH - PLAYER_WIDTH - GAP;
	player2.y = (SCREEN_HEIGHT / 2) - (PLAYER_HEIGHT / 2);
}

void doPlayer2(int y)
{
    player2.y=y;
//	player2.thinkTime--;
//	
//	if (player2.thinkTime <= 0)
//	{
//		player2.thinkTime = 0;
//	}
//	
//	if (input.up2 == 1)
//	{
//		player2.y -= PLAYER_SPEED;
//		
//		/* Don't allow the player2 to move off the screen */
//		
//		if (player2.y < 0)
//		{
//			player2.y = 0;
//		}
//	}
//	
//	if (input.down2 == 1)
//	{
//		player2.y += PLAYER_SPEED;
//		
//		/* Don't allow the player2 to move off the screen */
//		
//		if (player2.y + player2.sprite->h >= SCREEN_HEIGHT)
//		{
//			player2.y = SCREEN_HEIGHT - (player2.sprite->h + 1);
//		}
//	}
//	
//	/*
//	
//	if (input.fire == 1)
//	{
//		
//		if (player2.thinkTime <= 0)
//		{
//			addBullet(player2.x + player2.sprite->w, player2.y + (player2.sprite->h / 2));
//			
//			player2.thinkTime = MAX_RELOAD_TIME;
//		}
//	}*/
}

void drawPlayer2()
{
	/* Draw the image in the player2 structure */
	
	drawImage(player2.sprite, player2.x, player2.y);
}

