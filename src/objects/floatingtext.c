#include "floatingtext.h"


FloatingText* floating_text_basic_init(Arena* arena, WindowConstSize* window, const char* text, TTF_Font* font, SDL_Color textColor)
{
    FloatingText* fText = arena_alloc(arena, sizeof(FloatingText));
    if (!fText)
        return NULL;

    if (!load_texture_from_rendered_text(&fText->texture, text, font, textColor, window->renderer))
        return NULL;

    fText->x = (window->camera.w- fText->texture.width) / 2.0f;
    fText->y = (window->camera.h - fText->texture.height) / 2.0f;
    fText->velY = -50.0f;
    fText->lifeTime = 800;
    return fText;
}


FloatingTextController* floating_text_controller_init(size_t maxTexts, TTF_Font* font)
{
    FloatingTextController* c = malloc(sizeof(FloatingTextController));
    if (!c)
        return NULL;

    c->arena = arena_init(ARENA_BLOCK_SIZE / 4, 8);
    c->texts = arena_alloc(c->arena, sizeof(FloatingText*) * maxTexts);
    if (!c->texts)
    {
        arena_destroy(c->arena);
        free(c);
        return NULL;
    }
    c->font = font;
    c->maxTexts = maxTexts;
    c->currentTextCount = 0;
    return c;
}


void floating_text_add(FloatingTextController* c, WindowConstSize* window, const char* text, SDL_Color textColor)
{
    if (c->currentTextCount >= c->maxTexts)
    {
        printf("WARNING floating text controller full, cannot add more texts!\n");
        return;
    }

    FloatingText* newText = floating_text_basic_init(c->arena, window, text, c->font, textColor);
    if (!newText)
        return;

    c->texts[c->currentTextCount++] = newText;
}


void floating_text_controller_update(FloatingTextController* c, float deltaTime)
{
    if (c->currentTextCount == 0)
        return;

    for (int i = 0; i < c->currentTextCount; ++i)
    {
        FloatingText* text = c->texts[i];
        if (text->lifeTime > 0)
        {
            text->y += (text->velY * deltaTime * 3);
            text->lifeTime -= (deltaTime * 1000.0f); //convert to ms
            //printf("Text lifeTime: %d\n", text->lifeTime);
            //printf("Text deltaTime: %f\n", deltaTime);
        }
        else
        {
            //remove text
            free_texture(&text->texture);
            c->texts[i--] = c->texts[c->currentTextCount - 1];
            c->currentTextCount--;  //recycle last text into current slot

        }
    }
}


void floating_text_controller_render(FloatingTextController* c, SDL_Renderer* renderer)
{
    if (c->currentTextCount == 0)
        return;

    for (int i = 0; i < c->currentTextCount; ++i)
    {
        FloatingText* text = c->texts[i];
        set_texture_alpha(&text->texture, (uint8_t)(255 * ((float)text->lifeTime / 800.0f))); //fade out based on lifetime
        render_texture(&text->texture, renderer, (int)text->x, (int)text->y);

    }
}


void floating_text_controller_free(FloatingTextController* c)
{
    for (int i = 0; i < c->currentTextCount; ++i)
    {
        free_texture(&c->texts[i]->texture);
    }

    arena_destroy(c->arena);
    free(c);
}
