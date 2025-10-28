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

    CollisionObjectList* environmentCollision = collision_object_list_init();

    BoundingBox* screenBox = bounding_box_init_screen(WINDOW_WIDTH, WINDOW_HEIGHT, environmentCollision);
    collision_object_add(environmentCollision, screenBox, COLLISION_BOUNDING_BOX);
    //test environment collision
    CollisionRect* box1 = collision_rect_init(300, 600, 200, 50, NULL, environmentCollision);
    CollisionCircle* circle1 = collision_circle_init(800, 500, 35, NULL, environmentCollision);

    Circle dot = circle_init(100, 100, 5, 500, 500);
    collision_object_add(environmentCollision, &dot, COLLISION_CIRCLE);
    Box stickBro = box_init_platformer_movement(500, 500, 75, 125, 0.15f, 400, 450);
    collision_object_add(environmentCollision, &stickBro, COLLISION_BOX);

    WeatherMachine* wm = weather_machine_init(100000, 4, 2, 8, screenBox, environmentCollision);
    //RainMachine* rm = rainmachine_init(100000);
    //if (!rm) return 1;
    //LightningMachine* lm = lightning_machine_init(18, 2, 8);

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

            motion_handle_event_wasd(&stickBro, OBJ_BOX, &e, MOTION_PLATFORMER);
            motion_handle_event_arrow_keys(&dot, OBJ_CIRCLE, &e, MOTION_FREE);
        }
        x = (x > 10000) ? 0 : x + 1;

        clear_screen_with_color(window.renderer, COLOR[GRAY]);
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        rain_spwan(wm->rainMachine, screenBox, x, deltaTime);
        rain_update(wm->rainMachine, screenBox, deltaTime, w, environmentCollision);

        lightning_machine_update(wm->lightningMachine, screenBox, deltaTime);
        lightning_strand_grow(wm->lightningMachine, screenBox, deltaTime);

        box_move_platformer(&stickBro, environmentCollision, deltaTime, CONTACT_STOP);
        circle_move_free(&dot, environmentCollision, deltaTime, CONTACT_BOUNCE_OFF);

        weather_machine_render(wm, window.renderer, deltaTime);
        draw_collision_environment(environmentCollision, window.renderer);
        box_filled_draw(&stickBro, window.renderer, COLOR[LIGHT_GRAY]);
        circle_filled_draw(&dot, window.renderer, COLOR[TEAL]);

        SDL_RenderPresent(window.renderer);
    }

    weather_machine_destroy(wm);
    free_SDL2(&window);

    return 0;
}
