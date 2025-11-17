// SPDX-FileCopyrightText: 2025 Jack.B - jack.goldsbrough@outlook.com
// SPDX-License-Identifier: MIT

/* USE THE LIB in LIB FOLDER - THIS IS HERE FOR THE PROJECT ONLY */

#include "weathergen.h"
#include <assert.h>

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

static float rand_float(float min, float max)
{
    return min + ((float)rand() / (float)RAND_MAX * (max - min));
}


/* Rain Machine */

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
        arena_destroy(rm->arena);
        free(rm->arena);
        free(rm);
        return NULL;
    }

    rm->maxCount = maxCount;
    rm->count = 0;
    rm->spwanRate = 0;


    return rm;
}

void rain_spwan(RainMachine* rm, BoundingBox* rainBox, float deltaTime)
{
    if (!rm || !rainBox || rm->spwanRate <= 0)
        return;
    // top corners of BB get an even-ish spread
    uint32_t count = rm->spwanRate;
    uint32_t toMake = count * deltaTime;  // create count per second
    // best to make the count go up in increments of 30 starting at 0

    float bbx1 = rainBox->x;
    float bbx2 = rainBox->width + bbx1;
    for (uint32_t i = 0; i < toMake && rm->count < rm->maxCount; i++)
    {
        Droplet* d = arena_alloc(rm->arena, sizeof(Droplet));
        if (!d)
        {
            printf("ERROR - droplet arena_alloc failed\n");
            return;
        }

        d->x = rand_float(bbx1, bbx2);
        d->y = rand_float(rainBox->y, rainBox->y + 1.75f);
        if (bbx1 >= bbx2 - 1)
            bbx1 = rainBox->x;

        if ((count > 2000 && i % 4 == 0) || (count > 2200 && i % 3 == 0))
        {
            d->size = 3 + (rand() % 3);
            d->color.r = 130 + (rand() % 3);
            d->color.g = 158 + (rand() % 8);
            d->color.b = 180 + (rand() % 10);
            d->color.a = 240 + (rand() % 16);
            d->vY = rand_float(125.0f, 135.0f);
        }
        else if ((count > 1600 && i % 6 == 0) || (count > 1850 && i % 4 == 0) || (count > 2200 && i % 2 == 0) || count > 2500)    // heavy rain
        {
            d->size = 2 + (rand() % 4);
            d->color.r = 125 + (rand() % 10);
            d->color.g = 150 + (rand() % 15);
            d->color.b = 208 + (rand() % 20);
            d->color.a = 230 + (rand() % 26);
            d->vY = rand_float(125.0f, 130.0f);
        }
        else if ((count > 1000 && i % 4 == 0) || (count > 1200 && i % 3 == 0) || (count > 1400 && i % 2 == 0) || count > 2000)    // med rain
        {
            d->size = 1 + (rand() % 5);
            d->color.r = 156 + (rand() % 10);
            d->color.g = 176 + (rand() % 10);
            d->color.b = 235 + (rand() % 20);
            d->color.a = 220 + (rand() % 36);
            d->vY = rand_float(122.0f, 127.0f);
        }
        else if ((count > 500 && i % 3 == 0) || (count > 800 && i % 2 == 0) || count > 1200)
        {
            d->size = 1 + (rand() % 4);
            d->color.r = 145 + (rand() % 35);
            d->color.g = 165 + (rand() % 35);
            d->color.b = 201 + (rand() % 35);
            d->color.a = 170 + (rand() % 46);
            d->vY = rand_float(120.0f, 125.0f);
        }
        else    // drizzling
        {
            d->size = 1 + (rand() % 3);
            d->color.r = 150 + (rand() % 35);
            d->color.g = 170 + (rand() % 35);
            d->color.b = 206 + (rand() % 35);
            d->color.a = 150 + (rand() % 20);
            d->vY = 120;
        }
        d->dropDeath = false;

        rm->drops[rm->count++] = d;
    }
}

void droplet_death(Droplet* d)
{
    d->vY = d->size;
    d->dropDeath = true;
}

// don't include rainBox in CollisionObjectList
void rain_update(RainMachine* rm, BoundingBox* rainBox, float deltaTime, int wind, CollisionObjectList* colList)
{
    if (!rm)
        return;

    size_t i = 0;
    while (i < rm->count)
    {
        Droplet* d = rm->drops[i];

        if (!d->dropDeath)
        {
            if (d->size >= 4)
                d->y += (d->vY * deltaTime * 4);
            else if (d->size >= 2)
                d->y += (d->vY * deltaTime * 2);
            else
                d->y += (d->vY * deltaTime);

            d->y += (-3 + rand() % 7);

            short sendBack = SHRT_MIN;
            SDL_Rect rect = {d->x, d->y, d->size, d->size * 3};
            // checking collision
            if ((d->y > rainBox->height - rainBox->y) || box_detect_collision(&rect, colList, &sendBack, COLLISION_RETURN_FLOOR))
            {
                d->y = (sendBack == SHRT_MIN) ? rainBox->height - rainBox->y : sendBack;

                droplet_death(d);

                i++;
                continue;  // goes back to the start to process the drop next drop
            }
            // slight x random movement
            d->x += (-2 + rand() % 5);

            // Wind and screen wrap for when wind is active
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

            // if water particle time is up the spot is swapped with newest drop
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

void rain_render(RainMachine* rm, SDL_FRect* camera, SDL_Renderer* renderer)
{
    if (!rm || !renderer)
        return;

    for (size_t i = 0; i < rm->count; i++)
    {
        Droplet* d = rm->drops[i];
        if (!d->dropDeath)
        {
            SDL_FRect rect = {d->x - camera->x, d->y - camera->y, d->size, d->size * 3};
            draw_filled_rect(renderer, NULL, &rect, d->color);
        }
        else if (d->dropDeath && d->vY > 0)
        {
            for (int j = 0; j < d->size; j++)
                draw_point(renderer, d->x + (d->size * 3) + 1 + (rand() % (j + 1)) - camera->x, d->y - (d->size + (rand() % d->size)) - camera->y, d->color);
        }
    }
}

void lightning_rain_render(RainMachine* rm, SDL_FRect* camera, SDL_Renderer* renderer)
{
    if (!rm || !renderer)
        return;

    for (size_t i = 0; i < rm->count; i++)
    {
        Droplet* d = rm->drops[i];
        uint8_t r = (d->color.r + 55 > 255) ? 255 : d->color.r + 55;
        uint8_t g = (d->color.g + 55 > 255) ? 255 : d->color.g + 55;
        uint8_t b = (d->color.b + 55 > 255) ? 255 : d->color.b + 55;
        SDL_Color brighterPallet = {r, g, b, d->color.a};
        if (!d->dropDeath)
        {
            SDL_FRect rect = {d->x - camera->x, d->y - camera->y, d->size, d->size * 3};
            draw_filled_rect(renderer, NULL, &rect, brighterPallet);
        }
        else if (d->dropDeath && d->vY > 0)
        {
            for (int j = 0; j < d->size; j++)
                draw_point(renderer, d->x + (d->size * 3) + 1 + (rand() % (j + 1)) - camera->x, d->y - (d->size + (rand() % d->size)) - camera->y, brighterPallet);
        }
    }
}

void lightning_fade_rain_render(RainMachine* rm, uint8_t fadeLevel, SDL_FRect* camera, SDL_Renderer* renderer)
{
    if (!rm || !renderer)
        return;

    for (size_t i = 0; i < rm->count; i++)
    {
        Droplet* d = rm->drops[i];
        uint8_t r = (d->color.r + 55 - fadeLevel > 255) ? 255 : d->color.r + 55 - fadeLevel;
        uint8_t g = (d->color.g + 55 - fadeLevel > 255) ? 255 : d->color.g + 55 - fadeLevel;
        uint8_t b = (d->color.b + 55 - fadeLevel > 255) ? 255 : d->color.b + 55 - fadeLevel;
        SDL_Color brighterPallet = {r, g, b, d->color.a};
        if (!d->dropDeath)
        {
            SDL_FRect rect = {d->x - camera->x, d->y - camera->y, d->size, d->size * 3};
            draw_filled_rect(renderer, NULL, &rect, brighterPallet);
        }
        else if (d->dropDeath && d->vY > 0)
        {
            for (int j = 0; j < d->size; j++)
                draw_point(renderer, d->x + (d->size * 3) + 1 + (rand() % (j + 1)) - camera->x, d->y - (d->size + (rand() % d->size)) - camera->y, brighterPallet);
        }
    }
}

void rainmachine_destroy(RainMachine* rm)
{
    if (!rm)
        return;

    // need to just free the pointers we created
    free(rm->drops);

    // arena destory handels the acutally Droplet
    arena_destroy(rm->arena);

    free(rm);
}

/* Lightning Machine */

LightningMachine* lightning_machine_init(uint8_t maxStrands, uint32_t frequence, uint8_t serverity)
{
    LightningMachine* lm = malloc(sizeof(LightningMachine));
    if (!lm)
    {
        printf("ERROR - failed to create lighning machine\n");
        return NULL;
    }

    lm->arena = arena_init(ARENA_BLOCK_SIZE, 8);
    if (!lm->arena)
    {
        free(lm);
        return NULL;
    }

    lm->strands = arena_alloc(lm->arena, maxStrands * sizeof(LightningStrand*));
    {
        if (!lm->strands)
        {
            arena_destroy(lm->arena);

            free(lm);
            return NULL;
        }
    }
    lm->strandMaxCount = maxStrands;
    lm->strandCount = 0;
    lm->frequence = frequence;
    lm->coolDownTimer = frequence;
    lm->ready = false;
    lm->serverity = serverity;
    lm->active = false;

    lm->intervalTime = 0.016;
    lm->intervalCooldownTimer = 0.016;

    return lm;
}

void lightning_machine_destroy(LightningMachine* lm)
{
    if (!lm)
        return;

    arena_destroy(lm->arena);

    free(lm);
}

LightningStrand* spawn_lightning(LightningMachine* lm, float x, float y, uint8_t intensity)
{
    LightningStrand* ls = arena_alloc(lm->arena, sizeof(LightningStrand));

    ls->intensity = intensity;
    ls->maxCount = intensity * 3 + (rand() % 20);  // slightly random amount of lighting point -- higher intesity more

    ls->lightningPoints = arena_alloc(lm->arena, (ls->maxCount +1) * sizeof(Lightning*)); //plus 1 because we are include the maxCount number point
    if (!ls->lightningPoints)
    {
        printf("ERROR - arena_alloc for lightningPoints\n");
        return NULL;
    }


    for(int i = 0; i <= ls->maxCount; ++i)
    {
        ls->lightningPoints[i] = arena_alloc(lm->arena, sizeof(Lightning));
        ls->lightningPoints[i]->x = 0;
        ls->lightningPoints[i]->y = 0;
    }

    ls->x = x;
    ls->y = y;
    ls->count = 0;

    return ls;
}

void lightning_machine_reset(LightningMachine* lm)
{
    if (!lm)
        return;

    arena_destroy(lm->arena);
    lm->strands = NULL;
    lm->arena = NULL;

    lm->arena = arena_init(ARENA_BLOCK_SIZE, 8);
    if (!lm->arena)
        printf("ERROR couldn't reinit arena");

    lm->strands = arena_alloc(lm->arena, lm->strandMaxCount * sizeof(LightningStrand*));
    {
        if (!lm->strands)
        {
            arena_destroy(lm->arena);

            printf("ERROR couldn't reinit strand pointers");
        }
    }

    lm->strandCount = 0;
    lm->ready = false;

    lm->intervalTime = 0.016;
    lm->intervalCooldownTimer = 0.016;
    lm->coolDownTimer = lm->frequence;
}

void lightning_machine_update(LightningMachine* lm, BoundingBox* weatherBox, float deltaTime)
{
    if (!lm->active || !lm || !weatherBox)
        return;

    if (!lm->ready && lm->coolDownTimer > 0)  // counting down till next strike is possibile and only if the collDownTimer has been reset
    {
        lm->coolDownTimer -= deltaTime;
        if (lm->coolDownTimer < 0)
            lm->ready = true;

        return;
    }

    if (lm->ready)
    {
        uint32_t roll = 1 + (rand() % 100000);  // dice roll between 1 - 100000;
        //printf("Lightning Roll: %d vs %d\n", roll, (lm->serverity * lm->serverity * lm->serverity + 30));
        if ((lm->serverity * lm->serverity * lm->serverity + 30) > roll)
        {
            short x = weatherBox->x + (weatherBox->width / 4) + (rand() % (weatherBox->width / 2));                         // getting a random starting point at the top of the weatherBox in the middle 3 / 4 of the box
            lm->strands[lm->strandCount++] = spawn_lightning(lm, x, weatherBox->y, lm->serverity > 5 ? lm->serverity : 6);  // making this one always a main branch
            lm->ready = false;
        }
        return;
    }

    // check if all the lightning has finished and reset if
    for (int i = 0; i < lm->strandCount; ++i)
    {
        if (lm->strands[i]->count <= lm->strands[i]->maxCount)
            return;
    }
    lightning_machine_reset(lm);
}

void build_out_small_strand(Lightning* l1, Lightning* l2, Lightning* l)
{
    float dx, dy, len;
    dx = l1->y - l2->y;
    dy = l2->x - l1->y;
    len = sqrtf(dx * dx + dy * dy);

    // Prevent division by zero
    if (len < 0.01f)
    {
        l->x = l2->x + (5 + rand() % 8);
        l->y = l2->y + (5 + rand() % 8);
        return;
    }

    float ux = dx / len;
    float uy = dy / len;

    l->x = l2->x + ux * (5 + rand() % 8);
    l->y = l2->y + uy * (5 + rand() % 8);
}

void start_small_strand(LightningStrand* ls, Lightning* l)
{
    // Safety check: need at least 3 points
    if (ls->count < 3)
    {
        l->x = ls->x;
        l->y = ls->y;
        return;
    }

    if (ls->lightningPoints[2]->x - ls->lightningPoints[1]->x < 5)  // if the points are in line on the x-axis pick a direction
    {
        int flip = rand() % 2;
        if (flip == 1)
        {
            l->x = ls->lightningPoints[0]->x + 5;
            l->y = ls->lightningPoints[0]->y;
        }
        else
        {
            l->x = ls->lightningPoints[0]->x - 5;
            l->y = ls->lightningPoints[0]->y;
        }

    }
    else if (ls->lightningPoints[2]->y - ls->lightningPoints[0]->y < 5)    // if the points are in line on the y-axis pick a direction
    {
        int flip = rand() % 2;
        if (flip == 1)
        {
            l->x = ls->lightningPoints[0]->x;
            l->y = ls->lightningPoints[0]->y + 5;
        }
        else
        {
            l->x = ls->lightningPoints[0]->x;
            l->y = ls->lightningPoints[0]->y - 5;
        }
    }
    else    // else take the points are work out the next point
    {
        int flip = rand() % 2;
        if (flip == 1)
        {
            l->x = ls->lightningPoints[2]->x;
            l->y = ls->lightningPoints[1]->y + 5;
        }
        else
        {
            l->x = ls->lightningPoints[1]->x;
            l->y = ls->lightningPoints[2]->y;
        }
    }
}

void lightning_strand_grow(LightningMachine* lm, BoundingBox* weatherBox, float deltaTime)
{
    if (lm->strandCount == 0 || !lm || !weatherBox)
        return;

    lm->intervalCooldownTimer -= deltaTime;
    if (lm->intervalCooldownTimer < 0)
        lm->intervalCooldownTimer = lm->intervalTime;
    else
        return;  // return if interval timer has not been meet

    for (int i = 0; i < lm->strandCount; ++i)
    {
        if (lm->strands[i]->count > lm->strands[i]->maxCount)
            continue;

        LightningStrand* ls = lm->strands[i];

        Lightning* l = ls->lightningPoints[ls->count]; //assigning the point to be set

        if (ls->intensity > 5)
        {
            if (ls->count == ls->maxCount)  // last one hitting the ground for main Strands
            {
                l->x = ls->lightningPoints[ls->count - 1]->x;
                l->y = weatherBox->y + weatherBox->height;
            }
            else if (ls->count == 0)    // first one taking the start of the strand
            {
                l->x = ls->x;
                l->y = ls->y;
            }
            else    // all the others growning out but tending down
            {
                int diceRoll = 10 + rand() % 10;
                if (diceRoll > 25)
                    l->x = rand_float(ls->lightningPoints[ls->count - 1]->x - 200, ls->lightningPoints[ls->count - 1]->x + 200);
                else
                    l->x = rand_float(ls->lightningPoints[ls->count - 1]->x - 10, ls->lightningPoints[ls->count - 1]->x + 10);
                l->y = rand_float(ls->lightningPoints[ls->count - 1]->y, ls->lightningPoints[ls->count - 1]->y + ((float)weatherBox->height / ls->maxCount) * 2);
            }
        }
        else if (ls->intensity > 2)
        {
            if (ls->count == 0)  // first one taking the start of the strand
            {
                l->x = ls->x;
                l->y = ls->y;
            }
            else if (ls->count > 0)    // all the others growning out but tending down
            {
                int diceRoll = 5 + rand() % 10;
                if (diceRoll > 12)
                    l->x = rand_float(ls->lightningPoints[ls->count - 1]->x - 100, ls->lightningPoints[ls->count - 1]->x + 100);
                else
                    l->x = rand_float(ls->lightningPoints[ls->count - 1]->x - 10, ls->lightningPoints[ls->count - 1]->x + 10);
                l->y = rand_float(ls->lightningPoints[ls->count - 1]->y, ls->lightningPoints[ls->count - 1]->y + (float)weatherBox->height / ls->maxCount / 4);
            }
        }
        else    // Small strands (intensity <= 2)
        {
            if (ls->count == 0)  // first one taking the start of the strand
            {
                l->x = ls->x;
                l->y = ls->y;
            }
            else if (ls->count == 1 && i > 0 && lm->strands[i - 1]->count >= 2)    // to get the point before and saving
            {
                l->x = lm->strands[i - 1]->lightningPoints[lm->strands[i - 1]->count - 2]->x;
                l->y = lm->strands[i - 1]->lightningPoints[lm->strands[i - 1]->count - 2]->y;
            }
            else if (ls->count == 2 && i > 0 && lm->strands[i - 1]->count >= 1)    // to get the point after and build perpendicular
            {
                l->x = lm->strands[i - 1]->lightningPoints[lm->strands[i - 1]->count - 1]->x;
                l->y = lm->strands[i - 1]->lightningPoints[lm->strands[i - 1]->count - 1]->y;
            }
            else if (ls->count == 3 && ls->count >= 3)
            {
                start_small_strand(ls, l);
            }
            else if (ls->count == 4 && ls->count >= 4)
            {
                build_out_small_strand(ls->lightningPoints[0], ls->lightningPoints[3], l);
            }
            else if (ls->count > 4)
            {
                build_out_small_strand(ls->lightningPoints[ls->count - 2], ls->lightningPoints[ls->count - 1], l);
            }
        }
        ++ls->count;  //incrementing count breaks if done earlier
    }

    if (lm->strandCount > lm->strandMaxCount)
        return;
    for (int i = 0; i < lm->strandCount; ++i)
    {
        LightningStrand* ls = lm->strands[i];

        if (ls->count < 10 || ls->intensity < 3 || (ls->intensity > 4 && ls->y > weatherBox->height))
            continue;

        // roll dice to create another strand
        short diceRoll = (ls->intensity * 2) + (rand() % 100);
        if (diceRoll > 50 && lm->strandCount < lm->strandMaxCount)
            lm->strands[lm->strandCount++] = spawn_lightning(lm, ls->lightningPoints[ls->count - 1]->x, ls->lightningPoints[ls->count - 1]->y, ls->intensity > 6 ? (4 + rand() % 3) : (ls->intensity - 1) + rand() % 1);
    }
}

void lightning_render(LightningMachine* lm, BoundingBox* wB, SDL_FRect* camera, SDL_Renderer* renderer)
{
    if (lm->strandCount < 1 || !lm || !renderer)
        return;

    for (int i = 0; i < lm->strandCount; ++i)
    {
        if(lm->strands[i]->count < 2) continue; //skipping newly spawned strands with less than 2 points

        LightningStrand* ls = lm->strands[i];


        for (int k = 1; k < ls->count; ++k)
        {
            assert(ls->lightningPoints[k]->x > -100 && ls->lightningPoints[k]->y >= 0 && ls->lightningPoints[k -1]->x > -100 && ls->lightningPoints[k - 1]->y >= 0); //sanity check for lightning points (-100 to allow for off screen points if lightning is generated close to edge)
            assert(ls->lightningPoints[k]->x < 10000 && ls->lightningPoints[k]->y < 10000 && ls->lightningPoints[k -1]->x < 10000 && ls->lightningPoints[k - 1]->y < 10000); //sanity check for lightning points to see if and garbage data is being used
            if (ls->intensity > 5)
            {
                draw_line_float(renderer, ls->lightningPoints[k - 1]->x - camera->x, ls->lightningPoints[k - 1]->y - camera->y, ls->lightningPoints[k]->x - camera->x, ls->lightningPoints[k]->y - camera->y, COLOR[PURPLE]);
                draw_line_float(renderer, ls->lightningPoints[k - 1]->x - 1 - camera->x, ls->lightningPoints[k - 1]->y - 1 - camera->y, ls->lightningPoints[k]->x - 1 - camera->x, ls->lightningPoints[k]->y - 1 - camera->y, COLOR[PURPLE]);
                draw_line_float(renderer, ls->lightningPoints[k - 1]->x + 1 - camera->x, ls->lightningPoints[k - 1]->y + 1 - camera->y, ls->lightningPoints[k]->x + 1 - camera->x, ls->lightningPoints[k]->y + 1 - camera->y, COLOR[PURPLE]);
            }
            else if (ls->intensity > 2)
            {
                draw_line_float(renderer, ls->lightningPoints[k - 1]->x - camera->x, ls->lightningPoints[k - 1]->y - camera->y, ls->lightningPoints[k]->x - camera->x, ls->lightningPoints[k]->y - camera->y, COLOR[PURPLE]);

                draw_line_float(renderer, ls->lightningPoints[k - 1]->x - 1 - camera->x, ls->lightningPoints[k - 1]->y - 1 - camera->y, ls->lightningPoints[k]->x - 1 - camera->x, ls->lightningPoints[k]->y - 1 - camera->y, COLOR[PURPLE]);
            }
            else
                draw_line_float(renderer, ls->lightningPoints[k - 1]->x - camera->x, ls->lightningPoints[k - 1]->y - camera->y, ls->lightningPoints[k]->x - camera->x, ls->lightningPoints[k]->y - camera->y, COLOR[PURPLE]);
        }
    }
}

/* Snow machine functions */
SnowMachine* snowmachine_init(size_t maxCount)
{
    SnowMachine* sm = malloc(sizeof(SnowMachine));
    if (!sm)
    {
        printf("ERROR - creating RainMachine\n");
        return NULL;
    }

    sm->arena = arena_init(ARENA_BLOCK_SIZE, 8);
    if (!sm->arena)
    {
        free(sm);
        return NULL;
    }

    sm->snow = arena_alloc(sm->arena, maxCount * sizeof(SnowPartical*));
    if (!sm->snow)
    {
        arena_destroy(sm->arena);
        free(sm->arena);
        free(sm);
        return NULL;
    }

    sm->maxCount = maxCount;
    sm->maxLanded = maxCount / 4;
    sm->count = 0;
    sm->snowLanded = 0;
    sm->spwanRate = 0;


    return sm;
}

void snowmachine_reset(SnowMachine* sm)
{
    if (!sm)
        return;

    arena_destroy(sm->arena);
    sm->snow = NULL;
    sm->arena = NULL;

    sm->arena = arena_init(ARENA_BLOCK_SIZE, 8);
    if (!sm->arena)
        printf("ERROR couldn't reinit arena");

    sm->snow = arena_alloc(sm->arena, sm->maxCount * sizeof(SnowPartical*));
    {
        if (!sm->snow)
        {
            arena_destroy(sm->arena);

            printf("ERROR couldn't reinit snow pointers");
        }
    }

    sm->count = 0;
    sm->snowLanded = 0;
}


void snow_spwan(SnowMachine* sm, BoundingBox* weatherBox, float deltaTime)
{
    if (!sm || !weatherBox || sm->spwanRate <= 0)
        return;

    uint32_t count = sm->spwanRate;
    uint32_t toMake = count * deltaTime;  // create count per second
    // best to make the count go up in increments of 30 starting at 0

    float bbx1 = weatherBox->x;
    float bbx2 = weatherBox->width + bbx1;
    for (uint32_t i = 0; i < toMake && sm->count < sm->maxCount; ++i)
    {
        SnowPartical* s = arena_alloc(sm->arena, sizeof(SnowPartical));
        if (!s)
        {
            printf("ERROR - droplet arena_alloc failed\n");
            return;
        }

        s->x = rand_float(bbx1, bbx2);
        s->y = rand_float(weatherBox->y, weatherBox->y + 1.75f);
        if (bbx1 >= bbx2 - 1)
            bbx1 = weatherBox->x;

        if ((count > 2000 && i % 4 == 0) || (count > 2200 && i % 3 == 0))
        {
            s->size = 3 + (rand() % 3);
            s->color.r = 225 + (rand() % 16);
            s->color.g = 235 + (rand() % 16);
            s->color.b = 250 + (rand() % 6);
            s->color.a = 225 + (rand() % 31);
            s->vY = rand_float(70.0f, 85.0f);
        }
        else if ((count > 1600 && i % 6 == 0) || (count > 1850 && i % 4 == 0) || (count > 2200 && i % 2 == 0) || count > 2500)    // heavy rain
        {
            s->size = 2 + (rand() % 4);
            s->color.r = 250 + (rand() % 6);
            s->color.g = 244 + (rand() % 8);
            s->color.b = 235 + (rand() % 8);
            s->color.a = 220 + (rand() % 36);
            s->vY = rand_float(60.0f, 85.0f);
        }
        else if ((count > 1000 && i % 4 == 0) || (count > 1200 && i % 3 == 0) || (count > 1400 && i % 2 == 0) || count > 2000)    // mes rain
        {
            s->size = 1 + (rand() % 5);
            s->color.r = 225 + (rand() % 16);
            s->color.g = 235 + (rand() % 16);
            s->color.b = 250 + (rand() % 6);
            s->color.a = 200 + (rand() % 36);
            s->vY = rand_float(60, 75);
        }
        else if ((count > 500 && i % 3 == 0) || (count > 800 && i % 2 == 0) || count > 1200)
        {
            s->size = 1 + (rand() % 4);
            s->color.r = 250 + (rand() % 6);
            s->color.g = 246 + (rand() % 3);
            s->color.b = 240 + (rand() % 10);
            s->color.a = 180 + (rand() % 20);
            s->vY = rand_float(60, 70);
        }
        else    // srizzling
        {
            s->size = 1 + (rand() % 3);
            s->color.r = 245 + (rand() % 5);
            s->color.g = 250 + (rand() % 3);
            s->color.b = 253+ (rand() % 3);
            s->color.a = 160 + (rand() % 20);
            s->vY = rand_float(60, 65);
        }
        s->landed = false;
        s->snowDeath = false;

        sm->snow[sm->count++] = s;
    }
}

bool landed_snow_collision(SnowMachine* sm, SnowPartical* s)
{
    for (int i = 0; i < sm->count; ++i)
    {
        if (sm->snow[i]->landed)
            if ((int)sm->snow[i]->x == (int)s->x && (int)sm->snow[i]->y == (int)s->y)
                return true;
    }
    return false;
}

bool laying_snow_collision(SnowPartical* s, CollisionObjectList* colList, float *veloctiy)
{
    for (int i = 0; i < colList->totalObjects; i++)
    {
        if (colList->type[i] == COLLISION_BOUNDING_BOX)
            continue;
        else if (colList->type[i] == COLLISION_ENVIRONMENT_RECT)
        {
            //CollisionRect* cR = (CollisionRect *)colList->obj[i];
            continue;

        }
        else if (colList->type[i] == COLLISION_ENVIRONMENT_CIRCLE)
        {
            //CollisionCircle* cC = (CollisionCircle *)colList->obj[i];
            continue;
        }
        else if (colList->type[i] == COLLISION_CIRCLE)
        {
            //Circle *c = (Circle *)colList->obj[i];
            continue;
        }
        else if (colList->type[i] == COLLISION_BOX)
        {
            //for use in own lib need to convert Box to Rect
            Box *b = (Box *)colList->obj[i];
            if (s->x > b->x && s->x < b->x + b->rect.w && s->y > b->rect.y && s->y < b->rect.h + b->y)
            {
                if (b->x + ((float)b->rect.w / 2) > s->x)
                    *veloctiy = rand_float(20, 30) - 50;
                else
                    *veloctiy = rand_float(20, 30) + 20;

                return true;
            }
        }
    }
    return false;
}


// don't include weatherBox in CollisionObjectList
void snow_update(SnowMachine* sm, BoundingBox* weatherBox, float deltaTime, int wind, CollisionObjectList* colList)
{
    if (!sm)
        return;

    size_t i = 0;
    while (i < sm->count)
    {
        SnowPartical* s = sm->snow[i];

        if (!s->landed)
        {
            if (s->size >= 4)
                s->y += (s->vY * deltaTime * 2.5);
            else if (s->size >= 2)
                s->y += (s->vY * deltaTime * 1.5);
            else
                s->y += (s->vY * deltaTime);

            s->y += (-3 + rand() % 7);

            short sendBack = SHRT_MIN;
            SDL_Rect rect = {s->x, s->y, s->size, s->size * 3};
            // checking collision
            if ((s->y > weatherBox->height - weatherBox->y) || box_detect_collision(&rect, colList, &sendBack, COLLISION_RETURN_FLOOR))
            {
                s->landed = true;
                if (s->y <= weatherBox->y || s->y >= weatherBox->y + weatherBox->height || s->x >= weatherBox->x+weatherBox->width || s->x <= weatherBox->x || sm->snowLanded > sm->maxLanded)
                {
                    s->snowDeath = true;
                    s->vY = 0;
                }
                else
                {
                    s->y = (sendBack == SHRT_MIN) ? weatherBox->height - weatherBox->y : sendBack;
                    s->y -= ((rand() % 25) + 1);
                    ++sm->snowLanded;
                }

                ++i;
                continue;  // goes back to the start to process the next particle
            }
            /* NEEDS WORK UNOPTIMIZED AF */
            /*else if (landed_snow_collision(sm, s))
            {
                s->landed = true;
                ++i;
                continue;
            }*/

            // slight x random movement
            s->x += (-2 + rand() % 5);

            // Wind and screen wrap for when wind is active
            if (wind != 0)
            {
                s->x += (wind * deltaTime);

                if (wind < 0 && s->x < weatherBox->x)
                    s->x = weatherBox->width + weatherBox->x - (weatherBox->x - s->x);
                else if (wind > 0 && s->x > weatherBox->width + weatherBox->x)
                    s->x = weatherBox->x + (s->x - weatherBox->width + weatherBox->x);
            }

        }
        else if (!s->snowDeath)     //checking for collision on laying snow
        {
            float v = 0;
            if (laying_snow_collision(s, colList, &v))
            {
                s->vY = v;
                s->snowDeath = true;
                --sm->snowLanded;
                if (v > 0)
                {
                    s->x += 1 + (rand() % 5);
                }
                else
                {
                    s->x += (rand() % 8) - 30;
                }
            }
            else
            {
                SDL_Rect rect = {s->x, s->y, s->size, s->size * 3};
                if (!box_detect_collision(&rect, colList, NULL, COLLISION_RETURN_FLOOR))
                {
                    --sm->snowLanded;
                    s->landed = false;
                    s->vY = rand_float(35, 55);
                }
            }
        }
        else if (s->vY == 0)
        {
            sm->snow[i] = sm->snow[--sm->count];
            continue;
        }
        else
        {
            if (s->vY > 0)
            {
                s->x += 1 + (rand() % 5);
                s->y += s->vY * deltaTime * 2;
            }
            else
            {
                s->x += (rand() % 5) - 5;
                s->y += s->vY * deltaTime * 2;
            }
            s->vY *= (deltaTime * 35);
            if (s->vY < 0.05 && s->vY > -0.05)
                s->vY = 0;
        }
        i++;
    }

    if (sm->count == 0)
        snowmachine_reset(sm);
}


void snow_render(SnowMachine* sm, BoundingBox* weatherBox, SDL_FRect* camera, SDL_Renderer* renderer)
{
    if (!sm || !renderer)
        return;

    //printf("Landed Snow Particles Active: %u\n", sm->snowLanded);

    for (size_t i = 0; i < sm->count; i++)
    {
        SnowPartical* s = sm->snow[i];
        if (!s->snowDeath)
        {
            SDL_FRect rect = {s->x - camera->x, s->y - camera->y, s->size, s->size * 3};
            draw_filled_rect(renderer, NULL, &rect, s->color);
        }
        else if (s->landed && s->vY != 0)
        {
            if (!s->snowDeath) assert(s->y < weatherBox->y && s->y > weatherBox->y + weatherBox->height && s->x < weatherBox->x+weatherBox->width && s->x > weatherBox->x); //make sure snow does not lay outside of the weather box
            for (int j = 0; j < s->size; j++)
                draw_point(renderer, s->x + (s->size * 3) + 1 + (rand() % (j + 1)) - camera->x, s->y - (s->size + (rand() % s->size)) - camera->y, s->color);

        }
    }
}

/*  Made no difference so no need to have a separate function for bright snow
    Kept just for incase we want to tweak it later but not worth the processing power
void lightning_snow_render(SnowMachine* sm, BoundingBox* weatherBox, SDL_FRect* camera, SDL_Renderer* renderer)
{
    if (!sm || !renderer)
        return;

    //printf("Landed Snow Particles Active: %u\n", sm->snowLanded);

    for (size_t i = 0; i < sm->count; i++)
    {
        SnowPartical* s = sm->snow[i];
        SDL_Color brightColor = {255, 255, 255, s->color.a};
        if (!s->snowDeath)
        {
            SDL_FRect rect = {s->x - camera->x, s->y - camera->y, s->size, s->size * 3};
            draw_filled_rect(renderer, NULL, &rect, brightColor);
        }
        else if (s->landed && s->vY != 0)
        {
            if (!s->snowDeath) assert(s->y < weatherBox->y && s->y > weatherBox->y + weatherBox->height && s->x < weatherBox->x+weatherBox->width && s->x > weatherBox->x); //make sure snow does not lay outside of the weather box
            for (int j = 0; j < s->size; j++)
                draw_point(renderer, s->x + (s->size * 3) + 1 + (rand() % (j + 1)) - camera->x, s->y - (s->size + (rand() % s->size)) - camera->y, brightColor);

        }
    }
}

void lightning_fade_snow_render(SnowMachine* sm, uint8_t fadeLevel, BoundingBox* weatherBox, SDL_FRect* camera, SDL_Renderer* renderer)
{
    if (!sm || !renderer)
        return;

    //printf("Landed Snow Particles Active: %u\n", sm->snowLanded);

    for (size_t i = 0; i < sm->count; i++)
    {
        SnowPartical* s = sm->snow[i];
        uint8_t r = (255 - fadeLevel > s->color.r) ? 255 - fadeLevel : s->color.r;
        uint8_t g = (255 - fadeLevel > s->color.g) ? 255 - fadeLevel : s->color.g;
        uint8_t b = (255 - fadeLevel > s->color.b) ? 255 - fadeLevel : s->color.b;

        SDL_Color brightColor = {r, g, b, s->color.a};
        if (!s->snowDeath)
        {
            SDL_FRect rect = {s->x - camera->x, s->y - camera->y, s->size, s->size * 3};
            draw_filled_rect(renderer, NULL, &rect, brightColor);
        }
        else if (s->landed && s->vY != 0)
        {
            if (!s->snowDeath) assert(s->y < weatherBox->y && s->y > weatherBox->y + weatherBox->height && s->x < weatherBox->x+weatherBox->width && s->x > weatherBox->x); //make sure snow does not lay outside of the weather box
            for (int j = 0; j < s->size; j++)
                draw_point(renderer, s->x + (s->size * 3) + 1 + (rand() % (j + 1)) - camera->x, s->y - (s->size + (rand() % s->size)) - camera->y, brightColor);

        }
    }
}*/

void snowmachine_destroy(SnowMachine* sm)
{
    if (!sm)
        return;

    arena_destroy(sm->arena);

    free(sm);
}

/* WETHER MACHINE CONTROL UNIT */
WeatherMachine* weather_machine_init(size_t rainMaxCount, uint8_t maxStrands, uint32_t lightningFrequence, uint8_t lightningServerity, uint32_t maxSnow, BoundingBox* weatherBox, CollisionObjectList* environmentCollision)
{
    WeatherMachine* wm = malloc(sizeof(WeatherMachine));
    if (!wm)
    {
        printf("ERROR - creating WeatherMachine\n");
        return NULL;
    }

    wm->rainMachine = rainmachine_init(rainMaxCount);
    if (!wm->rainMachine)
    {
        free(wm);
        return NULL;
    }

    wm->lightningMachine = lightning_machine_init(maxStrands, lightningFrequence, lightningServerity);
    if (!wm->lightningMachine)
    {
        rainmachine_destroy(wm->rainMachine);
        free(wm);
        return NULL;
    }

    wm->snowMachine = snowmachine_init(maxSnow);
    if (!wm->snowMachine)
    {
        lightning_machine_destroy(wm->lightningMachine);
        rainmachine_destroy(wm->rainMachine);
        free(wm);
        return NULL;
    }

    wm->weatherBox = weatherBox;
    wm->environmentCollision = environmentCollision;

    wm->lightningAfterBoost = false;
    wm->fadeLevel = 0;
    wm->lightningAfterBoostTimer = 0.4f;
    wm->wind = 0;


    srand((unsigned)time(NULL)); //start rand Seed

    return wm;
}



/* Delete / comment out when being implemented for your own project */
void weather_machine_controls(WeatherMachine* wm, FloatingTextController* c, WindowConstSize* window, SDL_Event* e)
{
    if(e->type != SDL_KEYDOWN || !wm)
        return;

    switch(e->key.keysym.sym)
    {
    case SDLK_l:
        if (!wm->lightningMachine->active)
        {
            wm->lightningMachine->active = true;
            floating_text_add(c, window, "LIGHTNING ACTIVATED", COLOR[BLACK]);
        }
        else
        {
            lightning_machine_reset(wm->lightningMachine);
            wm->lightningMachine->active = false;
            floating_text_add(c, window, "LIGHTNING DEACTIVATED", COLOR[BLACK]);
        }
        break;
    case SDLK_k:
    {
        wm->lightningMachine->serverity += 1;
        if (wm->lightningMachine->serverity > 10)
            wm->lightningMachine->serverity = 1;
        char tmpStr[64];
        snprintf(tmpStr, sizeof(tmpStr), "LIGHTNING SERVERITY SET TO %d", wm->lightningMachine->serverity);
        floating_text_add(c, window, tmpStr, COLOR[BLACK]);
        break;
    }
    case SDLK_j:
    {
        uint8_t tmpCount = wm->lightningMachine->strandMaxCount;
        tmpCount = tmpCount > 80 ? 1 : tmpCount + 1;
        uint32_t tmpFreq = wm->lightningMachine->frequence;
        uint8_t tmpServ = wm->lightningMachine->serverity;
        lightning_machine_destroy(wm->lightningMachine);
        wm->lightningMachine = NULL;
        wm->lightningMachine = lightning_machine_init(tmpCount, tmpFreq, tmpServ);
        wm->lightningMachine->active = true;
        char tmpStr[64];
        snprintf(tmpStr, sizeof(tmpStr), "LIGHTNING STRAND COUNT SET TO %d", tmpCount);
        floating_text_add(c, window, tmpStr, COLOR[BLACK]);
        break;
    }
    case SDLK_h:
    {
        wm->lightningMachine->frequence += 1;
        if (wm->lightningMachine->frequence > 100)
            wm->lightningMachine->frequence = 1;
        char tmpStr[64];
        snprintf(tmpStr, sizeof(tmpStr), "LIGHTNING FREQUENCE SET TO %d", wm->lightningMachine->frequence);
        floating_text_add(c, window, tmpStr, COLOR[BLACK]);
        break;
    }
    case SDLK_p:
        wm->rainMachine->spwanRate = 0;
        floating_text_add(c, window, "RAIN STOPPED", COLOR[BLACK]);
        break;
    case SDLK_o:
    {
        wm->rainMachine->spwanRate += 30;
        char tmpStr[64];
        snprintf(tmpStr, sizeof(tmpStr), "RAIN INCREASED TO %d DROPLETS PER SECOND", wm->rainMachine->spwanRate);
        floating_text_add(c, window, tmpStr, COLOR[BLACK]);
        break;
    }
    case SDLK_i:
    {
        wm->rainMachine->spwanRate -= 30;
        if (wm->rainMachine->spwanRate > UINT32_MAX - 31)
            wm->rainMachine->spwanRate = 0;
        char tmpStr[64];
        snprintf(tmpStr, sizeof(tmpStr), "RAIN DECREASED TO %d DROPLETS PER SECOND", wm->rainMachine->spwanRate);
        floating_text_add(c, window, tmpStr, COLOR[BLACK]);
        break;
    }
    case SDLK_u:
    {
        wm->rainMachine->spwanRate = wm->rainMachine->spwanRate == 0 ? UINT32_MAX : 0;
        char tmpStr[64];
        snprintf(tmpStr, sizeof(tmpStr), "RAIN PULSE MODE ");
        floating_text_add(c, window, tmpStr, COLOR[BLACK]);
        break;
    }
    case SDLK_m:
        wm->snowMachine->spwanRate = 0;
        floating_text_add(c, window, "SNOW STOPPED", COLOR[BLACK]);
        break;
    case SDLK_n:
    {
        wm->snowMachine->spwanRate += 30;
        char tmpStr[64];
        snprintf(tmpStr, sizeof(tmpStr), "SNOW INCREASED TO %d PARTICLES PER SECOND", wm->snowMachine->spwanRate);
        floating_text_add(c, window, tmpStr, COLOR[BLACK]);
        break;
    }
    case SDLK_b:
    {
        wm->snowMachine->spwanRate -= 30;
        if (wm->snowMachine->spwanRate > UINT32_MAX - 31)
            wm->snowMachine->spwanRate = 0;
        char tmpStr[64];
        snprintf(tmpStr, sizeof(tmpStr), "SNOW DECREASED TO %d PARTICLES PER SECOND", wm->snowMachine->spwanRate);
        floating_text_add(c, window, tmpStr, COLOR[BLACK]);
        break;
    }
    case SDLK_v:
        wm->snowMachine->spwanRate = wm->snowMachine->spwanRate == 0 ? UINT32_MAX : 0;
        floating_text_add(c, window, "SNOW PULSE MODE ", COLOR[BLACK]);
        break;
    case SDLK_9:
    {
        wm->wind -= 20;
        if (wm->wind < -1000)
            wm->wind = 0;
        char tmpStr[64];
        snprintf(tmpStr, sizeof(tmpStr), "WIND SET TO %d", wm->wind);
        floating_text_add(c, window, tmpStr, COLOR[BLACK]);
        break;
    }
    case SDLK_0:
    {
        wm->wind += 20;
        if (wm->wind > 1000)
            wm->wind = 0;
        char tmpStr[64];
        snprintf(tmpStr, sizeof(tmpStr), "WIND SET TO %d", wm->wind);
        floating_text_add(c, window, tmpStr, COLOR[BLACK]);
        break;
    }
    }
}

void weather_machine_render(WeatherMachine* wm, SDL_Renderer* renderer, BoundingBox* weatherBox, SDL_FRect* camera, float deltaTime)
{
    if (!wm || !renderer)
        return;

    if (wm->lightningMachine->strandCount > 0)
    {
        // Render the lightning
        lightning_rain_render(wm->rainMachine, camera, renderer);
        lightning_render(wm->lightningMachine, wm->weatherBox, camera, renderer);
        wm->lightningAfterBoost = true;
    }
    else if (wm->lightningAfterBoost)
    {
        wm->lightningAfterBoostTimer -= deltaTime;
        if (wm->fadeLevel < 55)
            wm->fadeLevel = (1 + (rand() % 2) > 1) ? wm->fadeLevel + 3 : wm->fadeLevel + 2;
        lightning_fade_rain_render(wm->rainMachine, wm->fadeLevel, camera, renderer);
        if (wm->lightningAfterBoostTimer <= 0)
        {
            wm->lightningAfterBoost = false;
            wm->lightningAfterBoostTimer = 0.4f;
            wm->fadeLevel = 0;
        }
    }
    else
        rain_render(wm->rainMachine, camera, renderer);

    snow_render(wm->snowMachine, weatherBox, camera, renderer);

}

void weather_machine_destroy(WeatherMachine* wm)
{
    if (!wm)
        return;

    rainmachine_destroy(wm->rainMachine);
    lightning_machine_destroy(wm->lightningMachine);
    snowmachine_destroy(wm->snowMachine);

    free(wm);
}
