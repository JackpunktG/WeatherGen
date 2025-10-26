#include "../lib/SDL2/SDL2lib.h"
#include "../lib/arena_memory/arena_memory.h"
#include "objects/rain.h"

#include <stdio.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

int main(int argc, char* argv[])
{
    WindowConstSize window;
    init_SDL2_basic_vsync(&window, "RAINGEN", WINDOW_WIDTH, WINDOW_HEIGHT);

    BoundingBox rainBox = bounding_box_init_screen(WINDOW_WIDTH, WINDOW_HEIGHT - 20);
    BoundingBox screenBox = bounding_box_init_screen(WINDOW_WIDTH, WINDOW_HEIGHT -20);

    Box stickBro = box_init_platformer_movement(500, 500, 75, 125, 0.15f, 400, 450);
    //CollisionObjectList* rainCol = collision_object_list_init();
    CollisionObjectList* manCol = collision_object_list_init();

    //collision_object_add(rainCol, &stickBro, COLLISION_BOX);
    collision_object_add(manCol, &screenBox, COLLISION_BOUNDING_BOX);


    //RainMachine* rm = rainmachine_init(100000);
    //if (!rm) return 1;

    LightningMachine* lm = lightning_machine_init(100, 1, 10);

    bool running = true;
    SDL_Event e;
    uint32_t lastTime = SDL_GetTicks();
    uint32_t x = 30;
    int w = 0;
    while(running)
    {
        // Handle events
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_x)
            {
                running = false;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    x += 30;
                    break;
                case SDLK_DOWN:
                    x -= 30;
                    break;
                case SDLK_RIGHT:
                    w += 50;
                    break;
                case SDLK_LEFT:
                    w -= 50;
                    break;
                }
                x = (x > 10000) ? 0 : x;
            }
            motion_handle_event_wasd(&stickBro, OBJ_BOX, &e, MOTION_PLATFORMER);
        }
        clear_screen_with_color(window.renderer, COLOR[WHITE]);
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        //rain_spwan(rm, &rainBox, x, deltaTime);
        box_move_platformer(&stickBro, manCol, deltaTime, CONTACT_STOP);
        //rain_update(rm, &rainBox, deltaTime, w, rainCol);

        //rain_render(rm, window.renderer);
        lightning_machine_update(lm, &screenBox, deltaTime);

        lightning_strand_grow(lm, &screenBox, deltaTime);

        lightning_render(lm, &screenBox, window.renderer);
        box_filled_draw(&stickBro, window.renderer, COLOR[GREEN]);

        SDL_RenderPresent(window.renderer);
    }

    //rainmachine_destroy(rm);
    lightning_machine_destroy(lm);
    free_SDL2(&window);

    return 0;
}
