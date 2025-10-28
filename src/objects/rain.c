#include "rain.h"
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

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
            d->color.a = 230 + (rand() % 26);
            d->vY = rand_float(125.0f, 130.0f);
        }
        else if (count > 1000 && i % 4 == 0 || (count > 1200 && i % 3 == 0) || count > 1400 && i % 2 == 0 || count > 2000) //med rain
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
        else  //drizzling
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

//don't include rainBox in CollisionObjectList
void rain_update(RainMachine* rm, BoundingBox* rainBox, float deltaTime, int wind, CollisionObjectList* colList)
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

            short sendBack = SHRT_MIN;
            SDL_Rect rect = {d->x, d->y, d->size, d->size *3};
            //checking collision
            if ((d->y > rainBox->height - rainBox->y) || box_detect_collision(&rect, colList, &sendBack, COLLISION_RETURN_FLOOR))
            {
                d->y = (sendBack == SHRT_MIN) ? rainBox->height -rainBox->y : sendBack;

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
                draw_point(renderer, d->x + (d->size * 3) +  1 + (rand() % (j +1)), d->y - (d->size + (rand() % d->size)), d->color);
        }
    }
}


void rainmachine_destroy(RainMachine* rm)
{
    if (!rm) return;

    //need to just free the pointers we created
    free(rm->drops);

    //arena destory handels the acutally Droplet
    arena_destroy(rm->arena);

    free(rm);
}

LightningMachine* lightning_machine_init(uint8_t maxStrands, uint32_t frequence, uint8_t serverity)
{
    if (maxStrands == 0 || frequence == 0) return NULL;

    LightningMachine* lm = malloc(sizeof(LightningMachine));
    if(!lm)
    {
        printf("ERROR - failed to create lighning machine\n");
        return NULL;
    }

    lm->arena = arena_init(ARENA_BLOCK_SIZE, 8);
    if(!lm->arena)
    {
        free(lm);
        return NULL;
    }

    lm->strands = arena_alloc(lm->arena, maxStrands * sizeof(LightningStrand*));
    {
        if(!lm->strands)
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

    lm->intervalTime = 0.02;
    lm->intervalCooldownTimer = 0.02;
    srand((unsigned)time(NULL));

    return lm;
}

void lightning_machine_destroy(LightningMachine* lm)
{
    if (!lm) return;

    arena_destroy(lm->arena);

    free(lm);

}


LightningStrand* spawn_lightning(LightningMachine* lm, float x, float y, uint8_t intensity)
{
    LightningStrand* ls = arena_alloc(lm->arena, sizeof(LightningStrand));

    ls->intensity = intensity;
    ls->maxCount = intensity *3 + (rand() % 20); //slightly random amount of lighting point -- higher intesity more

    ls->lightningPoints = arena_alloc(lm->arena, ls->maxCount * sizeof(Lightning*));
    if (!ls->lightningPoints)
    {
        printf("ERROR - arena_alloc for lightningPoints\n");
        return NULL;
    }

    ls->x = x;
    ls->y = y;
    ls->count = 0;

    return ls;
}

void lightning_machine_reset(LightningMachine* lm)
{
    arena_destroy(lm->arena);
    lm->strands = NULL;
    lm->arena = NULL;

    lm->arena = arena_init(ARENA_BLOCK_SIZE, 8);
    if(!lm->arena)
        printf("ERROR couldn't reinit arena");


    lm->strands = arena_alloc(lm->arena, lm->strandMaxCount * sizeof(LightningStrand*));
    {
        if(!lm->strands)
        {
            arena_destroy(lm->arena);

            printf("ERROR couldn't reinit strand pointers");
        }
    }

    lm->strandCount = 0;
    lm->ready = false;

    lm->intervalTime = 0.02;
    lm->intervalCooldownTimer = 0.02;
    lm->coolDownTimer = 0.05;

}

void lightning_machine_update(LightningMachine* lm, BoundingBox* weatherBox, float deltaTime)
{
    if (!lm || !weatherBox) return;

    if (!lm->ready && lm->coolDownTimer > 0)     //counting down till next strike is possibile and only if the collDownTimer has been reset
    {
        lm->coolDownTimer -= deltaTime;
        if (lm->coolDownTimer < 0)
            lm->ready = true;

        return;
    }

    if (lm->ready)
    {
        uint8_t roll = 1 + (rand() % 30); //dice roll between 1 - 30;
        if (lm->serverity > roll)
        {
            short x = weatherBox->x + (weatherBox->width / 4) + (rand() % (weatherBox->width / 2));  // getting a random starting point at the top of the weatherBox in the middle 3 / 4 of the box
            lm->strands[lm->strandCount++] = spawn_lightning(lm, x, weatherBox->y, lm->serverity > 5 ? lm->serverity : 6); //making this one always a main branch
            lm->ready = false;
        }
        return;
    }

    //check if all the lightning has finished and reset if
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
    len = sqrtf(dx*dx + dy*dy);

    float ux = dx / len;
    float uy = dy / len;

    l->x = l2->x + ux * (5 + rand() % 8);
    l->y = l2->y + uy * (5 +rand() % 8);

}

void start_small_strand(LightningStrand* ls, Lightning* l)
{

    if (ls->lightningPoints[2]->x - ls->lightningPoints[1]->x < 5)      //if the points are in line on the x-axis pick a direction
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
    else if (ls->lightningPoints[2]->y - ls->lightningPoints[0]->y < 5)   //if the points are in line on the y-axis pick a direction
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
    else                                                        //else take the points are work out the next point
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
    if (lm->strandCount < 0 || !lm || !weatherBox) return;

    lm->intervalCooldownTimer -= deltaTime;
    if (lm->intervalCooldownTimer < 0)
        lm->intervalCooldownTimer = lm->intervalTime;
    else
        return;  //return if interval timer has not been meet

    for (int i = 0; i < lm->strandCount; ++i)
    {
        LightningStrand* ls = lm->strands[i];

        if (ls->count > ls->maxCount)
            continue;

        Lightning* l = arena_alloc(lm->arena, sizeof(Lightning));
        if (!l)
        {
            printf("ERROR - alloc lighting\n");
            return;
        }

        if (ls->intensity > 5)
        {
            if (ls->count == ls->maxCount)      //last one hitting the ground for main Strands
            {
                l->x = ls->lightningPoints[ls->count -1]->x;
                l->y = weatherBox->y + weatherBox->height;
            }
            else if (ls->count == 0)        // first one taking the start of the strand
            {
                l->x = ls->x;
                l->y = ls->y;
            }
            else                    // all the others growning out but tending down
            {
                int diceRoll = 10 + rand() % 10;
                if (diceRoll > 25)
                    l->x = rand_float(ls->lightningPoints[ls->count -1]->x - 200, ls->lightningPoints[ls->count -1]->x + 200);
                else
                    l->x = rand_float(ls->lightningPoints[ls->count -1]->x - 10, ls->lightningPoints[ls->count -1]->x + 10);
                l->y = rand_float(ls->lightningPoints[ls->count -1]->y, ls->lightningPoints[ls->count -1]->y + ((float)weatherBox->height / ls->maxCount) * 2);
            }
        }
        else if (ls->intensity > 2)
        {
            if (ls->count == 0)        // first one taking the start of the strand
            {
                l->x = ls->x;
                l->y = ls->y;
            }
            else                    // all the others growning out but tending down
            {
                int diceRoll = 5 + rand() % 10;
                if (diceRoll > 12)
                    l->x = rand_float(ls->lightningPoints[ls->count -1]->x - 100, ls->lightningPoints[ls->count -1]->x + 100);
                else
                    l->x = rand_float(ls->lightningPoints[ls->count -1]->x - 10, ls->lightningPoints[ls->count -1]->x + 10);
                l->y = rand_float(ls->lightningPoints[ls->count -1]->y, ls->lightningPoints[ls->count -1]->y + (float)weatherBox->height / ls->maxCount / 4);
            }
        }
        else
        {
            if (ls->count == 0)        // first one taking the start of the strand
            {
                l->x = ls->x;
                l->y = ls->y;
            }
            else if(ls->count == 1)     //to get the point before and saving
            {
                l->x = lm->strands[i-1]->lightningPoints[lm->strands[i-1]->count - 2]->x;
                l->y = lm->strands[i-1]->lightningPoints[lm->strands[i-1]->count - 2]->y;

            }
            else if(ls->count == 2) //to get the point after and build perpendicular
            {
                l->x = lm->strands[i-1]->lightningPoints[lm->strands[i-1]->count - 1]->x;
                l->x = lm->strands[i-1]->lightningPoints[lm->strands[i-1]->count - 1]->y;

            }
            else if(ls->count == 3)
                start_small_strand(ls, l);
            else if (ls->count == 4)
                build_out_small_strand(ls->lightningPoints[0], ls->lightningPoints[3], l);
            else
                build_out_small_strand(ls->lightningPoints[ls->count -2], ls->lightningPoints[ls->count -1], l);

        }
        ls->lightningPoints[ls->count++] = l;

    }

    for (int i = 0; i < lm->strandCount; ++i)
    {
        LightningStrand* ls = lm->strands[i];

        if (ls->count < 10 || ls->intensity < 3 || (ls->intensity > 4 && ls->y > weatherBox->height))
            continue;

        //roll dice to create another strand
        short diceRoll = (ls->intensity * 2) + (rand() % 100);
        if (diceRoll > 50 && lm->strandCount < lm->strandMaxCount)
            lm->strands[lm->strandCount++] = spawn_lightning(lm, ls->lightningPoints[ls->count -1]->x, ls->lightningPoints[ls->count -1]->y, ls->intensity > 6 ? (4 + rand() % 3) : (ls->intensity - 1) + rand() % 1);
        //lm->strands[lm->strandCount++] = spawn_lightning(lm, 20, 20, ls->intensity > 6 ? (4 + rand() % 3) : (ls->intensity - 1) + rand() % 1);

    }
}

void lightning_render(LightningMachine* lm, BoundingBox* wB, SDL_Renderer* renderer)
{
    if (lm->strandCount < 0 || !lm || !renderer) return;

    //printf("--------- LOOP ----------------\n");

    for (int i = 0; i < lm->strandCount; ++i)
    {
        LightningStrand *ls = lm->strands[i];
        for (int k = 1; k < ls->count; ++k)
        {
            if (ls->intensity < 3 && k < 4) continue; //skipping the set up points


            /*  JANKY FIX FOR POINTS BEING FREE OR OVERWRITTEN EARLY --- REQUIRES DEBUGGING*/
            if (ls->lightningPoints[k-1]->x < wB->x ||  ls->lightningPoints[k-1]->y <  wB->x || ls->lightningPoints[k]->x <  wB->x ||  ls->lightningPoints[k]->y <  wB->x )
            {
                //printf("-- PT1 x: %0.2f  y: %0.2f -- ", ls->lightningPoints[k-1]->x, ls->lightningPoints[k-1]->y);
                //printf("PT2 x: %0.2f  y: %0.2f --\n", ls->lightningPoints[k]->x, ls->lightningPoints[k-1]->y);
                continue;
            }
            if (ls->lightningPoints[k-1]->x > (wB->width + wB->x) ||  ls->lightningPoints[k-1]->y > (wB->width + wB->x) || ls->lightningPoints[k]->x >  (wB->width + wB->x) ||  ls->lightningPoints[k]->y >  (wB->width + wB->x) )
            {
                //printf("-- PT1 x: %0.2f  y: %0.2f -- ", ls->lightningPoints[k-1]->x, ls->lightningPoints[k-1]->y);
                //printf("PT2 x: %0.2f  y: %0.2f --\n", ls->lightningPoints[k]->x, ls->lightningPoints[k-1]->y);
                continue;
            }


            if (ls->intensity > 5)
            {
                draw_line_float(renderer, ls->lightningPoints[k-1]->x, ls->lightningPoints[k-1]->y, ls->lightningPoints[k]->x, ls->lightningPoints[k]->y, COLOR[PURPLE]);
                draw_line_float(renderer, ls->lightningPoints[k-1]->x -1, ls->lightningPoints[k-1]->y - 1, ls->lightningPoints[k]->x -1, ls->lightningPoints[k]->y -1, COLOR[PURPLE]);
                draw_line_float(renderer, ls->lightningPoints[k-1]->x +1, ls->lightningPoints[k-1]->y + 1, ls->lightningPoints[k]->x +1, ls->lightningPoints[k]->y + 1, COLOR[PURPLE]);
            }
            else if (ls->intensity > 2)
            {
                draw_line_float(renderer, ls->lightningPoints[k-1]->x, ls->lightningPoints[k-1]->y, ls->lightningPoints[k]->x, ls->lightningPoints[k]->y, COLOR[PURPLE]);

                draw_line_float(renderer, ls->lightningPoints[k-1]->x -1, ls->lightningPoints[k-1]->y - 1, ls->lightningPoints[k]->x -1, ls->lightningPoints[k]->y -1, COLOR[PURPLE]);
            }
            else draw_line_float(renderer, ls->lightningPoints[k-1]->x, ls->lightningPoints[k-1]->y, ls->lightningPoints[k]->x, ls->lightningPoints[k]->y, COLOR[PURPLE]);
            //printf("-- PT1 x: %0.2f  y: %0.2f -- ", ls->lightningPoints[k-1]->x, ls->lightningPoints[k-1]->y);
            //printf("PT2 x: %0.2f  y: %0.2f --\n", ls->lightningPoints[k]->x, ls->lightningPoints[k-1]->y);
        }

    }
}



