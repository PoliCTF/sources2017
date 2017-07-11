#include "draw.h"

extern void drawPlayer1(void);
extern void drawPlayer2(void);
extern void drawEntities(void);
extern void drawString(char *, int, int, TTF_Font *, int, int);

void draw()
{

	/* Blank the screen */
	
	SDL_FillRect(game.screen, NULL, 0);
	
        if (end==1)
    {
        char c[20];
        sprintf(c,"Player %d WIN",winner);
        drawString(c,0,0,game.font,1,1);
    }
        
	/* Draw the player */
	
	drawPlayer1();
        
        drawPlayer2();
	
	/* Draw the entities */
	
	drawEntities();
	
	/* Update the buffer */

	SDL_Flip(game.screen);
	
	/* Sleep briefly */
	
	SDL_Delay(1);
}

void delay(unsigned int frameLimit)
{
	unsigned int ticks = SDL_GetTicks();

	if (frameLimit < ticks)
	{
		return;
	}
	
	if (frameLimit > ticks + 16)
	{
		SDL_Delay(16);
	}
	
	else
	{
		SDL_Delay(frameLimit - ticks);
	}
}
