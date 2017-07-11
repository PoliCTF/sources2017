#include "init.h"

extern void freeSprites(void);
//extern void freeSounds(void);
extern void closeFont(TTF_Font *);

void init(char *title)
{
	/* Initialise SDL */
	
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0)
	{
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		
		exit(1);
	}
	
	/* Initialise SDL_TTF */
	
	if (TTF_Init() < 0)
	{
		printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());

		exit(1);
	}
	
	/* Open a screen */
	
	game.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, SDL_HWPALETTE|SDL_DOUBLEBUF);
	
	if (game.screen == NULL)
	{
		printf("Couldn't set screen mode to %d x %d: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());

		exit(1);
	}
	
//	/* Set the audio rate to 22050, 16 bit stereo, 2 channels and a 4096 byte buffer */
//	
//	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) != 0)
//	{
//		printf("Could not open audio: %s\n", Mix_GetError());
//		
//		exit(1);
//	}
	
	/* Set the screen title */
	
	SDL_WM_SetCaption(title, NULL);
}

void cleanup()
{
	/* Free the sprites */
	
	freeSprites();
	
//	/* Free the sounds */
//	
//	freeSounds();
	
	/* Close the font */
	
	closeFont(game.font);
	
//	/* Close the mixer */
//	
//	Mix_CloseAudio();
	
	/* Close SDL_TTF */
	
	TTF_Quit();
	
	/* Shut down SDL */
	
	SDL_Quit();
}
