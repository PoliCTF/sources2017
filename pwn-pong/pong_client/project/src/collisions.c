#include "collisions.h"

int collision(int, int, int, int, int, int, int, int);

void doCollisions()
{
	int i, j;

	/* Check each entity against the rest, skipping over inactive ones */

	for (i=0;i<MAX_ENTITIES;i++)
	{
		if (entity[i].active == 0)
		{
			continue;
		}

		for (j=0;j<MAX_ENTITIES;j++)
		{
			/* Don't collide with yourself, inactive entities or entities of the same type */
			
			if (i == j || entity[j].active == 0 || entity[j].type == entity[i].type)
			{
				continue;
			}
			
			/* Test the collision */

			if (collision(entity[i].x, entity[i].y, entity[i].sprite->w, entity[i].sprite->h, entity[j].x, entity[j].y, entity[j].sprite->w, entity[j].sprite->h) == 1)
			{
				/* If a collision occured, remove both Entities */
				
				entity[j].active = 0;
				
				entity[i].active = 0;
				
				/* Add 100 points to the score */
				
				game.score += 100;
			}
		}
	}
}

/* Very standard 2D collision detection routine */

int collision(int x0, int y0, int w0, int h0, int x2, int y2, int w1, int h1)
{
	int x1 = x0 + w0;
	int y1 = y0 + h0;

	int x3 = x2 + w1;
	int y3 = y2 + h1;

	return !(x1<x2 || x3<x0 || y1<y2 || y3<y0);
}
