#ifndef RAIN_H
#define RAIN_H
#include "../../lib/SDL2/SDL2lib.h"
#include "../../lib/arena_memory/arena_memory.h"

#include <SDL2/SDL.h>



/* Rain Machine */
typedef struct
{
    float x, y;
    float vY;       //changes to deathCount then dropDeath is true and when < 0 gets replaced with the newest dot
    SDL_Color color;
    uint8_t size;   //1 - 5 where the droplets get a litte bigger and faster each increment
    bool dropDeath;
} Droplet;

typedef struct
{
    Arena* arena;
    Droplet** drops;
    size_t count;
    size_t maxCount;
} RainMachine;


//init RainMachine
RainMachine* rainmachine_init(size_t maxCount);

//Spwans new rain
void rain_spwan(RainMachine* rm, BoundingBox* rainBox, uint32_t count, float deltaTime);

//updated rain possition
//-value for wind goes left | + right  Value equated to pper second movenemt (Basicly xMaxVel)
void rain_update(RainMachine* rm, BoundingBox* rainBox, float deltaTime, int wind, CollisionObjectList* collObjects);
//render
void rain_render(RainMachine* rm, SDL_Renderer* renderer);

//destory
void rainmachine_destroy(RainMachine* rm);

/* Wind Machine */


#endif

