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

//Spawns new rain
void rain_spwan(RainMachine* rm, BoundingBox* rainBox, uint32_t count, float deltaTime);

//updated rain possition
//-value for wind goes left | + right  Value equated to pper second movenemt (Basicly xMaxVel)
void rain_update(RainMachine* rm, BoundingBox* rainBox, float deltaTime, int wind, CollisionObjectList* collObjects);
//render
void rain_render(RainMachine* rm, SDL_Renderer* renderer);

//destory
void rainmachine_destroy(RainMachine* rm);

/* Lightning !!! */

typedef struct
{
    float x, y;
} Lightning;

typedef struct
{
    float x, y;  //start possition
    uint8_t count;
    uint8_t maxCount;
    Lightning** lightningPoints;
    uint8_t intensity; // 1-5 offshoots && 6-10 main branch
} LightningStrand;

typedef struct
{
    Arena* arena;
    LightningStrand** strands;
    uint8_t strandCount;
    uint8_t strandMaxCount;
    uint32_t frequence;   //cooldown between potential lightning strike
    float coolDownTimer;
    bool ready;
    uint8_t serverity;  //likelyhood of a lightning and how likely the intensity of the Strand is higher (1 - 10)
    float intervalTime;
    float intervalCooldownTimer; //for deltaTime in updating lightning per second
} LightningMachine;


LightningMachine* lightning_machine_init(uint8_t maxStrands, uint32_t frequence, uint8_t serverity);

//spawns a new lightning strand if the coolDown is up and the roll is successful
void lightning_machine_update(LightningMachine* lm, BoundingBox* weatherBox, float deltaTime);

//Controles the grow of the strands when lightning is active
void lightning_strand_grow(LightningMachine* lm, BoundingBox* weatherBox, float deltaTime);

//lightning Render
void lightning_render(LightningMachine* lm, BoundingBox* weatherBox, SDL_Renderer* renderer);

void lightning_machine_destroy(LightningMachine* lm);



typedef struct
{
    BoundingBox* weatherBox;
    RainMachine* rainMachine;
    LightningMachine* lightningMachine;
    bool lightningAfterBoost;  //true just as lightning has ended to fade lighting effect
    uint8_t fadeLevel;          //level of fade for after lightning effect
    float lightningAfterBoostTimer; //timer for the after boost effect

    CollisionObjectList* environmentCollision;
} WeatherMachine;

WeatherMachine* weather_machine_init(size_t rainMaxCount, uint8_t lightningMaxStrands, uint32_t lightningFrequence, uint8_t lightningServerity, BoundingBox* weatherBox, CollisionObjectList* environmentCollision);
void weather_machine_render(WeatherMachine* wm, SDL_Renderer* renderer, float deltaTime);
void weather_machine_destroy(WeatherMachine* wm);
#endif

