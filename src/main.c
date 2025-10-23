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

    BoundingBox bb = bounding_box_init_screen(WINDOW_WIDTH, WINDOW_HEIGHT);

    RainMachine* rm = rainmachine_init(100000);
    if (!rm) return 1;
    bool running = true;
    SDL_Event e;
    uint32_t lastTime = SDL_GetTicks();
    uint32_t x = 1500;
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
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
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
                    w += 1;
                    break;
                case SDLK_LEFT:
                    w -= 1;
                    break;
                }
                printf("second rain create: %u - wind: %d\n", x, w);
            }
        }
        clear_screen_with_color(window.renderer, COLOR[WHITE]);

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        rain_spwan(rm, &bb, x, deltaTime);
        rain_update(rm, &bb, deltaTime, w);

        rain_render(rm, window.renderer);

        SDL_RenderPresent(window.renderer);
    }

    rainmachine_destroy(rm);
    free_SDL2(&window);

    return 0;
}
