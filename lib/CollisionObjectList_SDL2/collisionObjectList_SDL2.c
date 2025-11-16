// SPDX-FileCopyrightText: 2025 Jack.B - jack.goldsbrough@outlook.com
// SPDX-License-Identifier: MIT

#include "collisionObjectList_SDL2.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

const SDL_Color COLOR[TOTAL] =
{
    {0, 0, 0, 255},       /* BLACK */
    {255, 255, 255, 255}, /* WHITE */
    {255, 0, 0, 255},     /* RED */
    {0, 255, 0, 255},     /* GREEN */
    {0, 0, 255, 255},     /* BLUE */
    {255, 255, 0, 255},   /* YELLOW */
    {255, 0, 255, 255},   /* MAGENTA */
    {0, 255, 255, 255},   /* CYAN */
    {128, 128, 128, 255}, /* GRAY */
    {64, 64, 64, 255},    /* DARK_GRAY */
    {192, 192, 192, 255}, /* LIGHT_GRAY */
    {165, 42, 42, 255},   /* BROWN */
    {255, 165, 0, 255},   /* ORANGE */
    {255, 192, 203, 255}, /* PINK */
    {128, 0, 128, 255},   /* PURPLE */
    {50, 205, 50, 255},   /* LIME */
    {0, 0, 128, 255},     /* NAVY */
    {0, 128, 128, 255},   /* TEAL */
    {128, 128, 0, 255},   /* OLIVE */
    {128, 0, 0, 255}      /* MAROON */
};

//Collision detection functions
//*************************************************************
int distanced_squared(int x1, int y1, int x2, int y2) //for speed optimization rather than using sqrt
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    return dx * dx + dy * dy;
}

bool radii_collision(int x1, int y1, int r1, int x2, int y2, int r2)
{
    int radiusSum = r1 + r2;

    return distanced_squared(x1, y1, x2, y2) <= radiusSum * radiusSum;
}

bool box_box_collision(SDL_Rect* a, SDL_Rect* b)
{
    // Calculate the sides of rect A
    int leftA = a->x;
    int rightA = a->x + a->w;
    int topA = a->y;
    int bottomA = a->y + a->h;

    // Calculate the sides of rect B
    int leftB = b->x;
    int rightB = b->x + b->w;
    int topB = b->y;
    int bottomB = b->y + b->h;

    // Check for no overlap
    if (bottomA <= topB)
    {
        return false;
    }
    if (topA >= bottomB)
    {
        return false;
    }
    if (rightA <= leftB)
    {
        return false;
    }
    if (leftA >= rightB)
    {
        return false;
    }

    // If none of the sides from A are outside B
    return true;
}

bool circle_box_collision(int circleX, int circleY, int radius, SDL_Rect* box)
{
    // Find the closest point to the circle
    int closestX = (circleX < box->x) ? box->x : (circleX > box->x + box->w) ? box->x + box->w : circleX;
    int closestY = (circleY < box->y) ? box->y : (circleY > box->y + box->h) ? box->y + box->h : circleY;

    // Calculate the distance between the circle's center and this closest point
    int distanceX = circleX - closestX;
    int distanceY = circleY - closestY;

    int distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
    return distanceSquared < (radius * radius);
}

bool box_detect_collision(SDL_Rect* box, CollisionObjectList* colList, short *sendBack, enum COLLISION_RETURN sendBackType)
{
    for (int i = 0; i < colList->totalObjects; i++)
    {
        if (colList->type[i] == COLLISION_BOUNDING_BOX)
        {
            BoundingBox *bb = (BoundingBox *)colList->obj[i];
            if ((box->x < bb->x) || (box->y < bb->y) || (box->x + box->w > bb->width) || (box->y + box->h > bb->height))
            {
                if (sendBack != NULL && sendBackType == COLLISION_RETURN_FLOOR ) *sendBack = bb->height + bb->y;
                else if(sendBack != NULL && sendBackType == COLLISION_RETURN_CEILING) *sendBack = bb->y;
                return true;
            }
        }
        else if (colList->type[i] == COLLISION_ENVIRONMENT_RECT)
        {
            CollisionRect* cR = (CollisionRect *)colList->obj[i];
            if (box_box_collision(box, &cR->rect))
            {
                if (sendBack != NULL && sendBackType == COLLISION_RETURN_FLOOR) *sendBack = cR->rect.y;
                else if(sendBack != NULL && sendBackType == COLLISION_RETURN_CEILING) *sendBack = cR->rect.y + cR->rect.h;
                return true;
            }
        }
        else if (colList->type[i] == COLLISION_ENVIRONMENT_CIRCLE)
        {
            CollisionCircle* cC = (CollisionCircle *)colList->obj[i];
            if (circle_box_collision(cC->x, cC->y, cC->radius, box))
            {
                if (sendBack != NULL && sendBackType == COLLISION_RETURN_FLOOR)  *sendBack = cC->y - cC->radius;
                else if(sendBack != NULL && sendBackType == COLLISION_RETURN_CEILING) *sendBack = cC->y + cC->radius;
                return true;
            }
        }
    }
    return false;
}

CollisionObjectList* collision_object_list_init()
{
    CollisionObjectList* colList = malloc(sizeof(CollisionObjectList));
    colList->obj = NULL;
    colList->type = NULL;
    colList->totalObjects = 0;
    return colList;
}

void free_collision_object_list(CollisionObjectList* colList)
{
    if (colList != NULL)
    {
        if (colList->obj != NULL)
        {
            free(colList->obj);
            colList->obj = NULL;
        }
        if (colList->type != NULL)
        {
            free(colList->type);
            colList->type = NULL;
        }
        free(colList);
    }
}

void collision_object_add(CollisionObjectList* colList, void* object, COLLISION_TYPE type)
{
    colList->obj = realloc(colList->obj, sizeof(void*) * (colList->totalObjects + 1));
    colList->type = realloc(colList->type, sizeof(COLLISION_TYPE) * (colList->totalObjects + 1));

    colList->obj[colList->totalObjects] = object;
    colList->type[colList->totalObjects] = type;

    colList->totalObjects++;
}

void draw_basic_collision_rect(CollisionRect* cRect, SDL_FRect* camera, SDL_Renderer* renderer);
void draw_basic_collsion_circle(CollisionCircle* cCircle, SDL_FRect* camera, SDL_Renderer* renderer);
void draw_textured_collision_rect(CollisionRect* cRect, SDL_FRect* camera, SDL_Renderer* renderer);
void draw_textured_collision_circle(CollisionCircle* cCircle, SDL_FRect* camera, SDL_Renderer* renderer);
void draw_collision_environment(CollisionObjectList* environmentList, SDL_FRect* camera, SDL_Renderer* renderer)
{
    for (int i = 0; i < environmentList->totalObjects; i++)
    {
        if (environmentList->type[i] == COLLISION_BOUNDING_BOX)
            continue;

        switch (environmentList->type[i])
        {
        case COLLISION_ENVIRONMENT_RECT:
        {
            CollisionRect* cR = (CollisionRect *)environmentList->obj[i];
            draw_textured_collision_rect(cR, camera, renderer);
            break;
        }
        case COLLISION_ENVIRONMENT_CIRCLE:
        {
            CollisionCircle* cC = (CollisionCircle *)environmentList->obj[i];
            draw_textured_collision_circle(cC, camera, renderer);
            break;
        }
        default:
            break;
        }
    }
}


//*************************************************************


//Bounding Box
//*************************************************************
BoundingBox* bounding_box_init_screen(short screenWidth, short screenHeight, CollisionObjectList* environmentList)
{
    BoundingBox* bb = malloc(sizeof(BoundingBox));
    bb->x = 0;
    bb->y = 0;
    bb->width = screenWidth;
    bb->height = screenHeight;

    collision_object_add(environmentList, bb, COLLISION_BOUNDING_BOX);

    return bb;
}

BoundingBox* bounding_box_init(short x, short y, short width, short height, CollisionObjectList* environmentList)
{
    BoundingBox* bb = malloc(sizeof(BoundingBox));
    bb->x = x;
    bb->y = y;
    bb->width = width;
    bb->height = height;

    collision_object_add(environmentList, bb, COLLISION_BOUNDING_BOX);

    return bb;
}

/* CollisionRect & CollisionCircle */
//************************************************************

CollisionRect* collision_rect_init(short x, short y, short width, short height,  CollisionObjectList* environmentList)
{
    CollisionRect* cRect = malloc(sizeof(CollisionRect));
    cRect->rect.x = x;
    cRect->rect.y = y;
    cRect->rect.w = width;
    cRect->rect.h = height;

    collision_object_add(environmentList, cRect, COLLISION_ENVIRONMENT_RECT);

    return cRect;
}


CollisionCircle* collision_circle_init(float x, float y, short radius, CollisionObjectList* environmentList)
{
    CollisionCircle* cCircle = malloc(sizeof(CollisionCircle));
    cCircle->x = x;
    cCircle->y = y;
    cCircle->radius = radius;

    collision_object_add(environmentList, cCircle, COLLISION_ENVIRONMENT_CIRCLE);

    return cCircle;
}

//*************************************************************


// Weather Effect drawing functions
//*************************************************************
void draw_filled_rect(SDL_Renderer* renderer, SDL_Rect* rect, SDL_FRect* fRect, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    if (rect != NULL)
        SDL_RenderFillRect(renderer, rect);
    else if (fRect != NULL)
        SDL_RenderFillRectF(renderer, fRect);
    else
        printf("ERROR - both rect and fRect were null when trying to draw\n");
}

void draw_point(SDL_Renderer* renderer, int x, int y, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderDrawPoint(renderer, x, y);
}

void draw_line_float(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
}
