#include "../lib/SDL2/SDL2lib.h"
#include "objects/floatingtext.h"
#include "objects/rain.h"

#include <stdbool.h>

#define LEVEL_WIDTH 1280
#define LEVEL_HEIGTH 720
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main(int argc, char* argv[])
{
    WindowConstSize window;
    init_SDL2_basic_vsync(&window, "RAINGEN", WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!init_TTF())
    {
        printf(" unable to init TTF!");
        free_SDL2(&window);
        return 1;
    }
    TTF_Font* font = TTF_OpenFont("src/assets/lugrasimo-Regular.ttf", 40);
    if (!font)
    {
        printf("ERROR unable to load font! MSG: %s\n", TTF_GetError());
        free_SDL2(&window);
        return 1;
    }
    FloatingTextController* ftc = floating_text_controller_init(50, font);
    CollisionObjectList* environmentCollision = collision_object_list_init();

    BoundingBox* screenBox = bounding_box_init_screen(LEVEL_WIDTH, LEVEL_HEIGTH, environmentCollision);
    collision_object_add(environmentCollision, screenBox, COLLISION_BOUNDING_BOX);
    //test environment collision
    CollisionRect* box1 = collision_rect_init(300, 600, 200, 50, NULL, environmentCollision);
    CollisionCircle* circle1 = collision_circle_init(800, 500, 35, NULL, environmentCollision);

    //Circle dot = circle_init(100, 100, 5, 500, 500);
    //collision_object_add(environmentCollision, &dot, COLLISION_CIRCLE);
    Box stickBro = box_init_platformer_movement(500, 500, 200, 50, 0.15f, 400, 450);
    collision_object_add(environmentCollision, &stickBro, COLLISION_BOX);

    WeatherMachine* wm = weather_machine_init(100000, 1, 1, 1, 100000, screenBox, environmentCollision);
    //if (!rm) return 1;

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
            weather_machine_controls(wm, ftc, &window, &e);
            window_size_update(&window, &e);
            motion_handle_event_wasd(&stickBro, OBJ_BOX, &e, MOTION_PLATFORMER);
            //motion_handle_event_arrow_keys(&dot, OBJ_CIRCLE, &e, MOTION_FREE);
        }

        clear_screen_with_color(window.renderer, COLOR[GRAY]);
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        floating_text_controller_update(ftc, deltaTime);

        rain_spwan(wm->rainMachine, screenBox, deltaTime);
        rain_update(wm->rainMachine, screenBox, deltaTime, wm->wind, environmentCollision);

        lightning_machine_update(wm->lightningMachine, screenBox, deltaTime);
        lightning_strand_grow(wm->lightningMachine, screenBox, deltaTime);

        snow_spwan(wm->snowMachine, screenBox, deltaTime);
        snow_update(wm->snowMachine, screenBox, deltaTime, wm->wind, environmentCollision);



        box_move_platformer(&stickBro, environmentCollision, deltaTime, CONTACT_STOP);
        //circle_move_free(&dot, environmentCollision, deltaTime, CONTACT_BOUNCE_OFF);
        camera_update(&window, &stickBro, OBJ_BOX, LEVEL_WIDTH, LEVEL_HEIGTH);
        draw_collision_environment(environmentCollision, &window.camera, window.renderer);
        box_filled_draw_camera(&stickBro, &window.camera, window.renderer, COLOR[LIGHT_GRAY]);
        weather_machine_render(wm, window.renderer, &window.camera, deltaTime);
        floating_text_controller_render(ftc, window.renderer);
        //box_filled_draw(&stickBro, window.renderer, COLOR[LIGHT_GRAY]);
        //circle_filled_draw(&dot, &window.camera, window.renderer,  COLOR[TEAL]);

        SDL_RenderPresent(window.renderer);
    }
    free_collision_object_list(environmentCollision);
    free(screenBox);
    free(box1);
    free(circle1);
    weather_machine_destroy(wm);
    free_SDL2(&window);
    free_TTF(font);
    floating_text_controller_free(ftc);
    return 0;
}
