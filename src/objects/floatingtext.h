#ifndef FLOATINGTEXT_H
#define FLOATINGTEXT_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../lib/SDL2/SDL2lib.h"
#include "../../lib/arena_memory/arena_memory.h"


/* Floating text pop ups */
typedef struct
{
    Texture texture;
    float x, y;
    float velY; //pixels per second
    uint32_t lifeTime; //in ms
} FloatingText;


/* Control Structure for floating texts */

typedef struct
{
    Arena* arena;
    FloatingText** texts;
    size_t maxTexts;
    size_t currentTextCount;
    TTF_Font* font;
} FloatingTextController;

// Initialize floating text controller
FloatingTextController* floating_text_controller_init(size_t maxTexts, TTF_Font* font);
//add floating text
void floating_text_add(FloatingTextController* c, WindowConstSize* window, const char* text, SDL_Color textColor);
//update floating texts
void floating_text_controller_update(FloatingTextController* c, float deltaTime);
//render floating texts
void floating_text_controller_render(FloatingTextController* c, SDL_Renderer* renderer);
// Free floating text controller
void floating_text_controller_free(FloatingTextController* c);

#endif
