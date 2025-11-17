// SPDX-FileCopyrightText: 2025 Jack.B - jack.goldsbrough@outlook.com
// SPDX-License-Identifier: MIT

/* USE THE LIB in LIB FOLDER - THIS IS HERE FOR THE PROJECT ONLY */

#ifndef WEATHERGEN_H
#define WEATHERGEN_H
#include "../../lib/arena_memory/arena_memory.h"
#include "../../lib/SDL2/SDL2lib.h"
#include "floatingtext.h"
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
    uint32_t spwanRate; //drops per second
    size_t count;
    size_t maxCount;
} RainMachine;


//init RainMachine
RainMachine* rainmachine_init(size_t maxCount);

//Spawns new rain
void rain_spwan(RainMachine* rm, BoundingBox* rainBox, float deltaTime);

//updated rain possition
//-value for wind goes left | + right  Value equated to pper second movenemt (Basicly xMaxVel)
void rain_update(RainMachine* rm, BoundingBox* rainBox, float deltaTime, int wind, CollisionObjectList* collObjects);
//render
void rain_render(RainMachine* rm, SDL_FRect* camera, SDL_Renderer* renderer);

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
    uint8_t strandMaxCount;  //for each lightning strike
    uint32_t frequence;   //cooldown between potential lightning strike
    float coolDownTimer;
    bool ready;
    uint8_t serverity;  //likelyhood of a lightning and how likely the intensity of the Strand is higher (1 - 10)
    float intervalTime;
    float intervalCooldownTimer; //for deltaTime in updating lightning per second
    bool active;
} LightningMachine;


LightningMachine* lightning_machine_init(uint8_t maxStrands, uint32_t frequence, uint8_t serverity);

//spawns a new lightning strand if the coolDown is up and the roll is successful
void lightning_machine_update(LightningMachine* lm, BoundingBox* weatherBox, float deltaTime);

//Controles the grow of the strands when lightning is active
void lightning_strand_grow(LightningMachine* lm, BoundingBox* weatherBox, float deltaTime);

//lightning Render
void lightning_render(LightningMachine* lm, BoundingBox* weatherBox, SDL_FRect* camera, SDL_Renderer* renderer);

void lightning_machine_destroy(LightningMachine* lm);


/* Snow Machine */
typedef struct
{
    float x, y;
    float vY;
    uint8_t size;
    SDL_Color color;
    bool landed;
    bool snowDeath;
} SnowPartical;

typedef struct
{
    Arena* arena;
    SnowPartical** snow;
    uint32_t snowLanded;
    uint32_t maxLanded;
    uint32_t spwanRate; //snow per second
    size_t count;
    size_t maxCount;
} SnowMachine;

//init SnowMachine
SnowMachine* snowmachine_init(size_t maxCount);

//Spawns new rain
void snow_spwan(SnowMachine* rm, BoundingBox* rainBox, float deltaTime);

//updated rain possition
//-value for wind goes left | + right  Value equated to pper second movenemt (Basicly xMaxVel)
void snow_update(SnowMachine* rm, BoundingBox* rainBox, float deltaTime, int wind, CollisionObjectList* collObjects);
//render
void snow_render(SnowMachine* rm, BoundingBox* weatherBox, SDL_FRect* camera, SDL_Renderer* renderer);

//destory
void snowmachine_destroy(SnowMachine* rm);




/* Weather Machine */

typedef struct
{
    BoundingBox* weatherBox;
    RainMachine* rainMachine;
    LightningMachine* lightningMachine;
    SnowMachine* snowMachine;
    bool lightningAfterBoost;  //true just as lightning has ended to fade lighting effect
    uint8_t fadeLevel;          //level of fade for after lightning effect
    float lightningAfterBoostTimer; //timer for the after boost effect
    int wind;
    CollisionObjectList* environmentCollision;
} WeatherMachine;

WeatherMachine* weather_machine_init(size_t rainMaxCount, uint8_t maxStrands, uint32_t lightningFrequence, uint8_t lightningServerity, uint32_t snowMax, BoundingBox* weatherBox, CollisionObjectList* environmentCollision);

void weather_machine_render(WeatherMachine* wm, SDL_Renderer* renderer, BoundingBox* weatherBox, SDL_FRect* camera, float deltaTime);
void weather_machine_destroy(WeatherMachine* wm);

void weather_machine_controls(WeatherMachine* wm, FloatingTextController* c, WindowConstSize* window, SDL_Event* e);
#endif

