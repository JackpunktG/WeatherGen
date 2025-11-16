// SPDX-FileCopyrightText: 2025 Jack.B - jack.goldsbrough@outlook.com
// SPDX-License-Identifier: MIT

#ifndef COLLISIONOBJECTLIST_SDL2_H
#define COLLISIONOBJECTLIST_SDL2_H
#include "../arena_memory/arena_memory.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>





// ######## Collision and Objects for collision
//for return collision funtions were you want to know the point of collision
enum COLLISION_RETURN
{
    COLLISION_RETURN_NONE,
    COLLISION_RETURN_FLOOR,
    COLLISION_RETURN_CEILING
};

typedef enum OBJ_TYPE
{
    OBJ_CIRCLE,
    OBJ_BOX
} OBJ_TYPE;
//OBJ to be able to move around and handle collision


typedef enum COLLISION_TYPE
{
    COLLISION_CIRCLE,
    COLLISION_BOX,
    COLLISION_BOUNDING_BOX,  //i.e screen of other square enclosing spaces
    COLLISION_ENVIRONMENT_RECT,
    COLLISION_ENVIRONMENT_CIRCLE
} COLLISION_TYPE;
//Different type of collision to interact with

typedef struct
{
    void** obj;
    COLLISION_TYPE* type;
    uint32_t totalObjects;
} CollisionObjectList;

CollisionObjectList* collision_object_list_init();
void free_collision_object_list(CollisionObjectList* colList);
void collision_object_add(CollisionObjectList* colList, void* object, COLLISION_TYPE type);
bool box_detect_collision(SDL_Rect* box, CollisionObjectList* colList, short *sendBack, enum COLLISION_RETURN sendBackType);
//for environment collision objects
void draw_collision_environment(CollisionObjectList* environmentList, SDL_FRect* camera, SDL_Renderer* renderer);




/* Environment Collision, add straight into the environment list */
// Bounding box
typedef struct
{
    short x, y, width, height;
} BoundingBox;

//init boundingBox
BoundingBox* bounding_box_init_screen(short screenWidth, short screenHeight, CollisionObjectList* environmentList);
BoundingBox* bounding_box_init(short x, short y, short width, short height, CollisionObjectList* environmentList);

typedef struct
{
    SDL_Rect rect;
} CollisionRect;
CollisionRect* collision_rect_init(short x, short y, short width, short height, CollisionObjectList* environmentList);

typedef struct
{
    float x, y;  //center of circle
    short radius;
} CollisionCircle;
CollisionCircle* collision_circle_init(float x, float y, short radius, CollisionObjectList* environmentList);
/* ------ */

//Colours
typedef enum
{
    BLACK,
    WHITE,
    RED,
    GREEN,
    BLUE,
    YELLOW,
    MAGENTA,
    CYAN,
    GRAY,
    DARK_GRAY,
    LIGHT_GRAY,
    BROWN,
    ORANGE,
    PINK,
    PURPLE,
    LIME,
    NAVY,
    TEAL,
    OLIVE,
    MAROON,
    TOTAL
} ColorEnum;
// Can input COLOR[*] to get SDL_Color
extern const SDL_Color COLOR[TOTAL];


// Weather Effect drawing functions
void draw_point(SDL_Renderer* renderer, int x, int y, SDL_Color colour);
void draw_filled_rect(SDL_Renderer* renderer, SDL_Rect* rect, SDL_FRect* fRect, SDL_Color colour);
void draw_line_float(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, SDL_Color colour);

#endif
