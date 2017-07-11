#include "player_1.h"

extern int loadSprite(char *);
extern void drawImage(SDL_Surface *, int, int);
extern void addBullet(int, int);
extern SDL_Surface *getSprite(int);

void initPlayer1()
{
	player1.sprite = getSprite(PLAYER_SPRITE);
	
	player1.x = GAP;
	player1.y = (SCREEN_HEIGHT / 2) - (PLAYER_HEIGHT / 2);
}

void doPlayer1(int y)
{
    player1.y=y;
//	player1.thinkTime--;
//	
//	if (player1.thinkTime <= 0)
//	{
//		player1.thinkTime = 0;
//	}
//	
//	if (input.up1 == 1)
//	{
//		player1.y -= PLAYER_SPEED;
//		
//		/* Don't allow the player1 to move off the screen */
//		
//		if (player1.y < 0)
//		{
//			player1.y = 0;
//		}
//	}
//	
//	if (input.down1 == 1)
//	{
//		player1.y += PLAYER_SPEED;
//		
//		/* Don't allow the player1 to move off the screen */
//		
//		if (player1.y + player1.sprite->h >= SCREEN_HEIGHT)
//		{
//			player1.y = SCREEN_HEIGHT - (player1.sprite->h + 1);
//		}
//	}
//	
//	/*
//	if (input.fire == 1)
//	{
//		
//		
//		if (player1.thinkTime <= 0)
//		{
//			addBullet(player1.x + player1.sprite->w, player1.y + (player1.sprite->h / 2));
//			
//			player1.thinkTime = MAX_RELOAD_TIME;
//		}
//	}*/
}

void drawPlayer1()
{
	/* Draw the image in the player1 structure */
	
	drawImage(player1.sprite, player1.x, player1.y);
}

