#include "input.h"

void getInput() {
    SDL_Event event;

    /* Loop through waiting messages and process them */

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
                /*
                 ** Closing the Window or pressing Escape will exit the program
                 ** Pressing the arrow keys will move the player
                 ** Pressing space will fire a bullet
                 */

            case SDL_QUIT:
                exit(0);
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        input_grezzo = 1;
                        input.up1 = 1;
                        break;

                    case SDLK_DOWN:
                        input_grezzo = 0;
                        input.down1 = 1;
                        break;

                        //                                        case SDLK_a:
                        //                                                input_grezzo=1;
                        //                                                input.up1 = 1;
                        //						break;
                        //                                        
                        //                                        case SDLK_z:
                        //                                                input_grezzo=0;
                        //                                                input.down1 = 1;
                        //						break;
                        //					
                        //					case SDLK_SPACE:
                        //						input.fire = 1;
                        //						break;
                        //					
                    case SDLK_ESCAPE:
                        exit(0);
                        break;

                    default:
                        break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        input_grezzo = -2;
                        input.up1 = 0;
                        break;

                    case SDLK_DOWN:
                        input_grezzo = -2;
                        input.down1 = 0;
                        break;

                        //                                        case SDLK_a:
                        //                                                input_grezzo=-2;
                        //                                                input.up1 = 0;
                        //						break;
                        //                                        
                        //                                        case SDLK_z:
                        //                                                input_grezzo=-2;
                        //                                                input.down1 = 0;
                        //						break;
                        //						
                        //					case SDLK_SPACE:
                        //						input.fire = 0;
                        //						break;
                        //					
                    default:
                        break;
                }
                break;
        }
    }
}
