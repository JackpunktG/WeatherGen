#include "rain.h"
#include <stdlib.h>
#include <time.h>

RainMachine* rainmachine_init(size_t maxCount)
{
    RainMachine* rm = malloc(sizeof(RainMachine));
    if (!rm)
    {
        printf("ERROR - creating RainMachine\n");
        return NULL;
    }

    rm->arena = arena_init(ARENA_BLOCK_SIZE, 8);
    if (!rm->arena)
    {
        free(rm);
        return NULL;
    }

    rm->drops = malloc(maxCount * sizeof(Droplet*));
    if (!rm->drops)
    {
        free(rm->arena);
        free(rm);
        return NULL;
    }

    rm->maxCount = maxCount;
    rm->count = 0;

    srand((unsigned)time(NULL));

    return rm;
}

static float rand_float(float min, float max)
{
    return min + ((float)rand() / (float)RAND_MAX * (max -min));
}

void rain_spwan(RainMachine* rm, BoundingBox* rainBox, uint32_t count, float deltaTime)
{
    if (!rm || !rainBox || count <= 0) return;
    //top corners of BB get an even-ish spread

    uint32_t toMake = count * deltaTime; //create count per second
    // best to make the count go up in increments of 30 starting at 0

    float bbx1 = rainBox->x;
    float bbx2 = rainBox->width + bbx1;
    for (int i = 0; i < toMake && rm->count < rm->maxCount; i++)
    {
        Droplet* d = arena_alloc(rm->arena, sizeof(Droplet));
        if (!d)
        {
            printf("ERROR - droplet arena_alloc failed\n");
            return;
        }

        d->x = rand_float(bbx1, bbx2);
        d->y = rand_float(rainBox->y, rainBox->y + 1.75f);
        if (bbx1 >= bbx2 - 1) bbx1 = rainBox->x;

        if ((count > 2000 && i % 4 == 0) || (count > 2200 && i % 3 == 0))
        {
            d->size = 3 + (rand() % 3);
            d->color.r = 130 + (rand() % 3);
            d->color.g = 158 + (rand() % 8);
            d->color.b = 180 + (rand() % 10);
            d->color.a = 240 + (rand() % 16);
            d->vY = rand_float(125.0f, 135.0f);
        }
        else if ((count > 1600 && i % 6 == 0) || (count > 1850 && i % 4 == 0) || (count > 2200 && i % 2 == 0) || count > 2500)  //heavy rain
        {
            d->size = 2 + (rand() % 4);
            d->color.r = 125 + (rand() % 10);
            d->color.g = 150 + (rand() % 15);
            d->color.b = 208 + (rand() % 20);
            d->color.a = 180 + (rand() % 50);
            d->vY = rand_float(125.0f, 130.0f);
        }
        else if (count > 1000 && i % 4 == 0 || (count > 1200 && i % 3 == 0) || count > 1400 && i % 2 == 0 || count > 2000) //med rain
        {
            d->size = 1 + (rand() % 5);
            d->color.r = 156 + (rand() % 10);
            d->color.g = 176 + (rand() % 10);
            d->color.b = 235 + (rand() % 20);
            d->color.a = 210 + (rand() % 26);
            d->vY = rand_float(122.0f, 127.0f);
        }
        else if ((count > 500 && i % 3 == 0) || (count > 800 && i % 2 == 0) || count > 1200)
        {
            d->size = 1 + (rand() % 4);
            d->color.r = 155 + (rand() % 35);
            d->color.g = 175 + (rand() % 35);
            d->color.b = 201 + (rand() % 35);
            d->color.a = 190 + (rand() % 46);
            d->vY = rand_float(120.0f, 125.0f);
        }
        else  //drizzling
        {
            d->size = 1 + (rand() % 3);
            d->color.r = 170 + (rand() % 35);
            d->color.g = 190 + (rand() % 35);
            d->color.b = 206 + (rand() % 35);
            d->color.a = 185 + (rand() % 56);
            d->vY = 120;
        }
        d->dropDeath = false;

        rm->drops[rm->count++] = d;

    }
}

bool rain_collision_check(Droplet* d, CollisionObjectList* colList)
{
    for (int i = 0; i < colList->totalObjects; i++)
    {
        if (colList->type[i] == COLLISION_BOUNDING_BOX)
        {
            BoundingBox *bb = (BoundingBox *)colList->obj[i];
            if ((d->x < bb->x) || (d->y < bb->y) || (d->x + d->size > bb->width) || (d->y + (d->size * 3) > bb->height))
                return true;
        }
        else if (colList->type[i] == COLLISION_CIRCLE)
        {
            SDL_Rect rect = {d->x, d->y, d->size, d->size * 3};
            Circle *c = (Circle *)colList->obj[i];
            if (circle_box_collision(c->x, c->y, c->radius, &rect))
                return true;

        }
        else if (colList->type[i] == COLLISION_BOX)
        {
            SDL_Rect rect = {d->x, d->y, d->size, d->size * 3};
            Box *b = (Box *)colList->obj[i];
            if (box_box_collision(&rect, &b->rect))
                return true;
        }
    }
    return false;

}

void droplet_death(Droplet* d)
{
    d->vY = d->size;
    d->dropDeath = true;
}

//don't include rainBox in CollisionObjectList
void rain_update(RainMachine* rm, BoundingBox* rainBox, float deltaTime, int wind, CollisionObjectList* collObjects)
{
    if (!rm) return;

    size_t i = 0;
    while(i < rm->count)
    {
        Droplet* d = rm->drops[i];

        if(!d->dropDeath)
        {
            if (d->size >= 4)
                d->y += (d->vY * deltaTime * 4);
            else if (d->size >= 2)
                d->y += (d->vY * deltaTime* 2);
            else
                d->y += (d->vY * deltaTime);

            d->y += (-3 + rand() % 7);

            //checking collision
            if (d->y > rainBox->height - rainBox->y) //|| rain_collision_check(d, collObjects))
            {
                d->y = rainBox->height -rainBox->y;

                droplet_death(d);

                i++;
                continue; // goes back to the start to process the drop next drop
            }
            //slight x random movement
            d->x += (-2 + rand() % 5);

            //Wind and screen wrap for when wind is active
            if (wind != 0)
            {
                d->x += (wind * deltaTime);

                if (wind < 0 && d->x < rainBox->x)
                    d->x = rainBox->width + rainBox->x - (rainBox->x - d->x);
                else if (wind > 0 && d->x > rainBox->width + rainBox->x)
                    d->x = rainBox->x + (d->x - rainBox->width + rainBox->x);
            }

        }
        else if (d->dropDeath && d->vY > 0)
        {
            d->vY -= (deltaTime * 5);

            //if water particle time is up the spot is swapped with newest drop
            if (d->vY < 0)
            {
                rm->drops[i] = rm->drops[--rm->count];
                continue;
            }
        }
        i++;
    }

    if (rm->count == 0)
        arena_reset(rm->arena);
}

void rain_render(RainMachine* rm, SDL_Renderer* renderer)
{
    if (!rm || !renderer) return;


    for (size_t i = 0; i < rm->count; i++)
    {
        Droplet* d = rm->drops[i];
        if (!d->dropDeath)
        {
            SDL_FRect rect = {d->x, d->y, d->size, d->size * 3};
            draw_filled_rect(renderer, NULL, &rect, d->color);
        }
        else if (d->dropDeath && d->vY > 0)
        {
            for (int j = 0; j < d->size; j++)
                draw_point(renderer, d->x + (d->size * 3) +  1 + (rand() % (j +1)), d->y + (d->size + (rand() % d->size)), d->color);
        }
    }
}


void rainmachine_destroy(RainMachine* rm)
{
    if (!rm) return;

    //need to just free the pointers we created
    free(rm->drops);

    //arena destory handels the acutall Droplet
    arena_destroy(rm->arena);

    free(rm);
}
