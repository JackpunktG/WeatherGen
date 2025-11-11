#include "SDL2lib.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

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

// Windows and renderer functions
//*************************************************************
bool init_SDL()
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("ERROR SDL could not init! MSG: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

bool init_resizable_window(SDL_Window** window, const char* title, uint32_t width, uint32_t height)
{
    *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (*window == NULL)
    {
        printf("ERROR init window! MSG: %s\n", SDL_GetError());
        return false;
    }
    return true;
}



bool init_sized_window(SDL_Window** window, const char* title, uint32_t width, uint32_t height)
{
    *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (*window == NULL)
    {
        printf("ERROR init window! MSG: %s\n", SDL_GetError());
        return false;
    }
    return true;
}


bool init_renderer(SDL_Window* window, SDL_Renderer** renderer)
{
    *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL)
    {
        printf("ERROR init renderer! MSG: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            printf("ERROR SDL_image could not init! MSG: %s\n", IMG_GetError());
            return false;
        }
    }

    return true;
}

bool init_renderer_vsync(SDL_Window* window, SDL_Renderer** renderer)
{
    *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL)
    {
        printf("ERROR init VSYNC-renderer! MSG: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            printf("ERROR SDL_image could not init! MSG: %s\n", IMG_GetError());
            return false;
        }
    }
    return true;
}

bool init_SDL2_basic(WindowConstSize* windowStruct, const char* title, uint32_t width, uint32_t height)
{
    if (!init_SDL()) return false;

    if (!init_sized_window(&windowStruct->window, title, width, height)) return false;

    if (!init_renderer(windowStruct->window, &windowStruct->renderer)) return false;

    windowStruct->width = width;
    windowStruct->height = height;
    windowStruct->fullscreen = false;

    windowStruct->camera.x = 0;
    windowStruct->camera.y = 0;
    windowStruct->camera.w = width;
    windowStruct->camera.h = height;

    return true;
}

bool init_SDL2_basic_vsync(WindowConstSize* windowStruct, const char* title, uint32_t width, uint32_t height)
{
    if (!init_SDL()) return false;

    if (!init_sized_window(&windowStruct->window, title, width, height)) return false;

    if (!init_renderer_vsync(windowStruct->window, &windowStruct->renderer)) return false;

    windowStruct->width = width;
    windowStruct->height = height;
    windowStruct->fullscreen = false;

    windowStruct->camera.x = 0;
    windowStruct->camera.y = 0;
    windowStruct->camera.w = width;
    windowStruct->camera.h = height;

    return true;
}

void free_SDL2(WindowConstSize* windowStruct)
{
    if (windowStruct->renderer != NULL)
    {
        SDL_DestroyRenderer(windowStruct->renderer);
        windowStruct->renderer = NULL;
    }

    if (windowStruct->window != NULL)
    {
        SDL_DestroyWindow(windowStruct->window);
        windowStruct->window = NULL;
    }
    IMG_Quit();
    SDL_Quit();
}


bool window_size_update(WindowConstSize* windowStruct, SDL_Event* e)
{
    if (e->type != SDL_KEYDOWN)
        return false;

    if (e->key.keysym.sym == SDLK_ESCAPE)
    {
        if(windowStruct->fullscreen)
        {
            SDL_SetWindowFullscreen(windowStruct->window, 0);
            SDL_RestoreWindow(windowStruct->window);

            SDL_SetWindowSize(windowStruct->window, windowStruct->width, windowStruct->height);
            SDL_SetWindowPosition(windowStruct->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            windowStruct->fullscreen = false;
        }
        else
        {
            SDL_SetWindowFullscreen(windowStruct->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            windowStruct->fullscreen = true;

        }
        return true;

    }
    else if (e->key.keysym.sym == SDLK_1)
    {
        if (windowStruct->width == 1980)
        {
            SDL_SetWindowSize(windowStruct->window, 1280, 720);
            windowStruct->width = 1280;
            windowStruct->camera.w = 1280;
            windowStruct->height = 720;
            windowStruct->camera.h = 720;
        }
        else
        {
            SDL_SetWindowSize(windowStruct->window, 1980, 1080);
            windowStruct->width = 1980;
            windowStruct->camera.w = 1980;
            windowStruct->height = 1080;
            windowStruct->camera.h = 1080;
        }
        return true;
    }
    return false;
}

void camera_update(WindowConstSize* windowStruct, void* mainActor, enum OBJ_TYPE type, const uint32_t levelWidth, const uint32_t levelHeight)
{
    switch (type)
    {
    case OBJ_BOX:
    {
        Box* b = (Box *)mainActor;
        windowStruct->camera.x = (b->x + (float)b->rect.w / 2) - (float)windowStruct->width / 2;
        windowStruct->camera.y = (b->y + (float)b->rect.h / 2) - (float)windowStruct->height / 2;
        break;
    }
    case OBJ_CIRCLE:
    {
        Circle* c = (Circle *)mainActor;
        windowStruct->camera.x = c->x - (float)windowStruct->width / 2;
        windowStruct->camera.y = c->y - (float)windowStruct->height / 2;
        break;
    }
    }

    //checking x-axis
    if (windowStruct->camera.x + windowStruct->camera.w > levelWidth)
        windowStruct->camera.x = levelWidth - windowStruct->camera.w;
    else if (windowStruct->camera.x < 0)
        windowStruct->camera.x = 0;

    //check y-axis
    if (windowStruct->camera.y + windowStruct->camera.h > levelHeight)
        windowStruct->camera.y = levelHeight - windowStruct->camera.h;
    else if (windowStruct->camera.y < 0)
        windowStruct->camera.y = 0;

}
//*************************************************************

// TTF functions
//*************************************************************
bool init_TTF()
{
    if (TTF_Init() == -1)
    {
        printf("ERROR SDL_ttf could not init! MSG: %s\n", TTF_GetError());
        return false;
    }
    return true;
}

void free_TTF(TTF_Font* font)
{
    TTF_CloseFont(font);

    TTF_Quit();
}
//*************************************************************

// Texture functions
//*************************************************************

void clear_screen_with_color(SDL_Renderer* renderer, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderClear(renderer);
}

void init_texture(Texture* texture)
{
    texture->mTexture = NULL;
    texture->width = 0;
    texture->height = 0;
}

void free_texture(Texture* texture)
{
    if (texture->mTexture != NULL)
    {
        SDL_DestroyTexture(texture->mTexture);
        texture->mTexture = NULL;
        texture->width = 0;
        texture->height = 0;
    }
}

bool load_texture_from_file(Texture* texture, const char* path, SDL_Renderer* renderer)
{
    init_texture(texture);

    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL)
    {
        printf("ERROR unable to load image %s! MSG: %s\n", path, IMG_GetError());
        return false;
    }

    texture->mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (texture->mTexture == NULL)
    {
        printf("ERROR unable to create texture from %s! MSG: %s\n", path, SDL_GetError());
        SDL_FreeSurface(loadedSurface);
        return false;
    }

    texture->width = loadedSurface->w;
    texture->height = loadedSurface->h;

    SDL_FreeSurface(loadedSurface);
    return true;
}

bool load_texture_from_file_colourKey(Texture* texture, const char* path, SDL_Renderer* renderer, SDL_Color colour)
{
    init_texture(texture);

    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL)
    {
        printf("ERROR unable to load image %s! MSG: %s\n", path, IMG_GetError());
        return false;
    }

    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, colour.r, colour.g, colour.b));

    texture->mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (texture->mTexture == NULL)
    {
        printf("ERROR unable to create texture from %s! MSG: %s\n", path, SDL_GetError());
        SDL_FreeSurface(loadedSurface);
        return false;
    }

    texture->width = loadedSurface->w;
    texture->height = loadedSurface->h;

    SDL_FreeSurface(loadedSurface);
    return true;
}

bool load_texture_from_rendered_text(Texture* texture, const char* text, TTF_Font* font, SDL_Color textColor, SDL_Renderer* renderer)
{
    init_texture(texture);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColor);
    if (textSurface == NULL)
    {
        printf("ERROR unable to render text surface! MSG: %s\n", TTF_GetError());
        return false;
    }

    texture->mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (texture->mTexture == NULL)
    {
        printf("ERROR unable to create texture from rendered text! MSG: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return false;
    }

    texture->width = textSurface->w;
    texture->height = textSurface->h;

    SDL_FreeSurface(textSurface);
    return true;
}

void render_texture(Texture* texture, SDL_Renderer* renderer, int x, int y)
{
    if (texture->mTexture != NULL)
    {
        SDL_Rect renderQuad = { x, y, texture->width, texture->height };
        SDL_RenderCopy(renderer, texture->mTexture, NULL, &renderQuad);
    }
    else
    {
        printf("ERROR texture is NULL, cannot render!\n");
    }
}

void render_texture_clip(Texture* texture, SDL_Renderer* renderer, SDL_Rect* clip, int x, int y)
{
    if (texture->mTexture != NULL)
    {
        SDL_Rect renderQuad = { x, y, clip->w, clip->h };
        SDL_RenderCopy(renderer, texture->mTexture, clip, &renderQuad);
    }
    else
    {
        printf("ERROR texture is NULL, cannot render clipped!\n");
    }
}

void render_texture_background(Texture* texture, SDL_Renderer* renderer, SDL_FRect* camera, uint32_t levelWidth, uint32_t levelHeight)
{
    if (texture->mTexture != NULL)
    {
        // Get the texture's actual dimensions
        int texWidth = texture->width;
        int texHeight = texture->height;

        // Calculate the scaling factors between texture and level
        float scaleX = (float)texWidth / levelWidth;
        float scaleY = (float)texHeight / levelHeight;

        // Calculate which portion of the texture to show based on camera
        SDL_Rect clip = {(int)(camera->x * scaleX), (int)(camera->y * scaleY), (int)(camera->w * scaleX), (int)(camera->h * scaleY)};

        SDL_FRect dest = {0, 0, camera->w, camera->h};

        SDL_RenderCopyF(renderer, texture->mTexture, &clip, &dest);
    }
    else
    {
        printf("ERROR texture is NULL, cannot render background!\n");
    }
}

void render_texture_scaled(Texture* texture, SDL_Renderer* renderer, int x, int y, int width, int height)
{
    if (texture->mTexture != NULL)
    {
        SDL_Rect renderQuad = { x, y, width, height };
        SDL_RenderCopy(renderer, texture->mTexture, NULL, &renderQuad);
    }
    else
    {
        printf("ERROR texture is NULL, cannot render scaled!\n");
    }
}

void render_texture_rotated_clipped(Texture* texture, SDL_Renderer* renderer, SDL_Rect* clip, int x, int y, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    if (texture->mTexture != NULL)
    {
        if (clip == NULL)
        {
            SDL_Rect renderQuad = { x, y, texture->width, texture->height };
            SDL_RenderCopyEx(renderer, texture->mTexture, NULL, &renderQuad, angle, center, flip);
            return;
        }
        SDL_Rect renderQuad = { x, y, clip->w, clip->h };
        SDL_RenderCopyEx(renderer, texture->mTexture, clip, &renderQuad, angle, center, flip);
    }
    else
    {
        printf("ERROR texture is NULL, cannot render rotated clipped!\n");
    }
}

void render_texture_all_options(Texture* texture, SDL_Renderer* renderer, SDL_Rect* clip, int x, int y, int width, int height, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    if (texture->mTexture != NULL)
    {
        if (clip == NULL)
        {
            SDL_Rect renderQuad = { x, y, width, height };
            SDL_RenderCopyEx(renderer, texture->mTexture, NULL, &renderQuad, angle, center, flip);
            return;
        }
        SDL_Rect renderQuad = { x, y, width, height };
        SDL_RenderCopyEx(renderer, texture->mTexture, clip, &renderQuad, angle, center, flip);
    }
    else
    {
        printf("ERROR texture is NULL, cannot render with all options!\n");
    }
}

void set_texture_colour(Texture* texture, uint8_t r, uint8_t g, uint8_t b)
{
    if (texture->mTexture != NULL)
    {
        SDL_SetTextureColorMod(texture->mTexture, r, g, b);
    }
    else
    {
        printf("ERROR texture is NULL, cannot set colour mod!\n");
    }
}


void set_texture_alpha(Texture* texture, uint8_t alpha)
{
    if (texture->mTexture != NULL)
    {
        SDL_SetTextureAlphaMod(texture->mTexture, alpha);
    }
    else
    {
        printf("ERROR texture is NULL, cannot set alpha mod!\n");
    }
}

void set_texture_blend_mode(Texture* texture, SDL_BlendMode blendMode)
{
    if (texture->mTexture != NULL)
    {
        SDL_SetTextureBlendMode(texture->mTexture, blendMode);
    }
    else
    {
        printf("ERROR texture is NULL, cannot set blend mode!\n");
    }
}

static SDL_FRect world_frect_to_screen_frect(const SDL_FRect *camera, const SDL_FRect *world)
{
    SDL_FRect screen;
    screen.x = world->x - camera->x;
    screen.y = world->y - camera->y;
    screen.w = world->w;
    screen.h = world->h;
    return screen;
}
//*************************************************************


// Geometric rendering functions
//*************************************************************
void draw_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void draw_line_float(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderDrawLineF(renderer, x1, y1, x2, y2);
}

void draw_point(SDL_Renderer* renderer, int x, int y, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderDrawPoint(renderer, x, y);
}

void draw_outlined_rect(SDL_Renderer* renderer, SDL_Rect* rect, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderDrawRect(renderer, rect);
}

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


void draw_outlined_circle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y)
    {
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void draw_filled_circle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void draw_outlined_triangle(SDL_Renderer* renderer, SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Color colour)
{
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    SDL_RenderDrawLine(renderer, p2.x, p2.y, p3.x, p3.y);
    SDL_RenderDrawLine(renderer, p3.x, p3.y, p1.x, p1.y);
}


/* Helper: swap two SDL_Points */
static void swap_point(SDL_Point* a, SDL_Point* b)
{
    SDL_Point tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Draws a triangle where the top is one vertex (v0) and bottom edge is v1-v2 (v1.y == v2.y) */
static void draw_flat_bottom_triangle(SDL_Renderer* renderer, SDL_Point v0, SDL_Point v1, SDL_Point v2)
{
    double invslope1 = (double)(v1.x - v0.x) / (v1.y - v0.y);
    double invslope2 = (double)(v2.x - v0.x) / (v2.y - v0.y);

    double curx1 = v0.x;
    double curx2 = v0.x;

    for (int y = v0.y; y <= v1.y; ++y)
    {
        int x_start = (int)ceil(fmin(curx1, curx2));
        int x_end   = (int)floor(fmax(curx1, curx2));
        if (x_end >= x_start)
            SDL_RenderDrawLine(renderer, x_start, y, x_end, y);

        curx1 += invslope1;
        curx2 += invslope2;
    }
}

/* Draws a triangle where the bottom is one vertex (v2) and top edge is v0-v1 (v0.y == v1.y) */
static void draw_flat_top_triangle(SDL_Renderer* renderer, SDL_Point v0, SDL_Point v1, SDL_Point v2)
{
    double invslope1 = (double)(v2.x - v0.x) / (v2.y - v0.y);
    double invslope2 = (double)(v2.x - v1.x) / (v2.y - v1.y);

    double curx1 = v2.x;
    double curx2 = v2.x;

    for (int y = v2.y; y >= v0.y; --y)
    {
        int x_start = (int)ceil(fmin(curx1, curx2));
        int x_end   = (int)floor(fmax(curx1, curx2));
        if (x_end >= x_start)
            SDL_RenderDrawLine(renderer, x_start, y, x_end, y);

        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

/* Public function: fills a triangle specified by three points */
void draw_filled_triangle(SDL_Renderer* renderer, SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Color colour)
{
    if (!renderer) return;

    /* Set draw color */
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);

    /* Sort points by y ascending: p1.y <= p2.y <= p3.y */
    SDL_Point v0 = p1, v1 = p2, v2 = p3;
    if (v0.y > v1.y) swap_point(&v0, &v1);
    if (v1.y > v2.y) swap_point(&v1, &v2);
    if (v0.y > v1.y) swap_point(&v0, &v1);

    /* Handle degenerate triangles (line or point) */
    if (v0.y == v2.y)
    {
        /* All on same horizontal line: draw a horizontal line between min x and max x */
        int minx = v0.x, maxx = v0.x;
        minx = SDL_min(minx, v1.x);
        maxx = SDL_max(maxx, v1.x);
        minx = SDL_min(minx, v2.x);
        maxx = SDL_max(maxx, v2.x);
        SDL_RenderDrawLine(renderer, minx, v0.y, maxx, v0.y);
        return;
    }

    /* If middle vertex shares same y as top -> flat-top triangle */
    if (v0.y == v1.y)
    {
        /* Ensure v0 is left and v1 is right on the top edge */
        if (v0.x > v1.x) swap_point(&v0, &v1);
        draw_flat_top_triangle(renderer, v0, v1, v2);
    }
    else if (v1.y == v2.y) /* flat-bottom triangle */
    {
        /* Ensure v1 is left and v2 is right on the bottom edge */
        if (v1.x > v2.x) swap_point(&v1, &v2);
        draw_flat_bottom_triangle(renderer, v0, v1, v2);
    }
    else
    {
        /* General triangle -- split into a flat-bottom and flat-top at y = v1.y */
        double alpha = (double)(v1.y - v0.y) / (double)(v2.y - v0.y);
        double split_x = v0.x + alpha * (v2.x - v0.x);
        SDL_Point vi = { (int)round(split_x), v1.y };

        /* Determine which side vi should be on (left or right) by comparing x */
        if (vi.x <= v1.x)
        {
            /* vi is left of v1 -> lower triangle v0,vi,v1 is flat-bottom; upper triangle vi,v1,v2 is flat-top */
            draw_flat_bottom_triangle(renderer, v0, vi, v1);
            /* For the top part we need vi,v1,v2 but top edge must be in order left/right */
            if (vi.x > v1.x) swap_point(&vi, &v1);
            draw_flat_top_triangle(renderer, vi, v1, v2);
        }
        else
        {
            /* vi is right of v1 */
            draw_flat_bottom_triangle(renderer, v0, v1, vi);
            if (v1.x > vi.x) swap_point(&v1, &vi);
            draw_flat_top_triangle(renderer, v1, vi, v2);
        }
    }
}

//*************************************************************


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

bool radii_collision_DEBUGG(SDL_Renderer* renderer, int x1, int y1, int r1, int x2, int y2, int r2)
{
    draw_outlined_circle(renderer, x1, y1, r1, COLOR[RED]);
    draw_outlined_circle(renderer, x2, y2, r2, COLOR[GREEN]);

    return radii_collision(x1, y1, r1, x2, y2, r2);
}
bool box_box_collision_DEBUGG(SDL_Renderer* renderer, SDL_Rect* a, SDL_Rect* b)
{
    draw_outlined_rect(renderer, a, COLOR[RED]);
    draw_outlined_rect(renderer, b, COLOR[GREEN]);

    return box_box_collision(a, b);
}
bool circle_box_collision_DEBUGG(SDL_Renderer* renderer, int circleX, int circleY, int radius, SDL_Rect* box)
{
    draw_outlined_circle(renderer, circleX, circleY, radius, COLOR[RED]);
    draw_outlined_rect(renderer, box, COLOR[GREEN]);

    return circle_box_collision(circleX, circleY, radius, box);
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
            if (cR->texture == NULL) draw_basic_collision_rect(cR, camera, renderer);
            else draw_textured_collision_rect(cR, camera, renderer);
            break;
        }
        case COLLISION_ENVIRONMENT_CIRCLE:
        {
            CollisionCircle* cC = (CollisionCircle *)environmentList->obj[i];
            if (cC->texture == NULL) draw_basic_collsion_circle(cC, camera, renderer);
            else draw_textured_collision_circle(cC, camera, renderer);
            break;
        }
        default:
            break;
        }
    }
}


//*************************************************************


// Motion functions
//*************************************************************

// #### ARROW KEY MOVEMENT ####
void circle_free_movement_arrow_keys(Circle* circle, SDL_Event* e)
{

    //If a key was pressed
    if( e->type == SDL_KEYDOWN && e->key.repeat == 0 ) //only give us the first instance the key was pressed
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_UP:
            circle->velY -= circle->maxVelY;
            if (circle->clipIndex == 1)
                circle->clipIndex = 2;
            else if (circle->clipIndex == 3 || circle->clipIndex == 8)
                circle->clipIndex = 6;
            else if (circle->clipIndex == 4 || circle->clipIndex == 9)
                circle->clipIndex = 7;
            else
                circle->clipIndex = 2;
            break;
        case SDLK_DOWN:
            circle->velY += circle->maxVelY;
            if (circle->clipIndex == 1)
                circle->clipIndex = 5;
            else if (circle->clipIndex == 3 || circle->clipIndex == 6)
                circle->clipIndex = 8;
            else if (circle->clipIndex == 7 || circle->clipIndex == 4)
                circle->clipIndex = 9;
            else
                circle->clipIndex = 5;
            break;
        case SDLK_LEFT:
            if (circle->clipIndex == 1)
                circle->clipIndex = 4;
            else if (circle->clipIndex == 2 || circle->clipIndex == 6)
                circle->clipIndex = 7;
            else if (circle->clipIndex == 5 || circle->clipIndex == 8)
                circle->clipIndex = 9;
            else
                circle->clipIndex = 4;
            circle->velX -= circle->maxVelX;
            break;
        case SDLK_RIGHT:
            if (circle->clipIndex == 1)
                circle->clipIndex = 3;
            else if (circle->clipIndex == 2 || circle->clipIndex == 7)
                circle->clipIndex = 6;
            else if (circle->clipIndex == 5 || circle->clipIndex == 9)
                circle->clipIndex = 8;
            else
                circle->clipIndex = 3;
            circle->velX += circle->maxVelX;
            break;
        }
    }//If a key was released
    else if( e->type == SDL_KEYUP && e->key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_UP:
            circle->velY += circle->maxVelY;
            if (circle->clipIndex == 2)
                circle->clipIndex = 1;
            else if (circle->clipIndex == 6 || circle->clipIndex == 8)
                circle->clipIndex = 3;
            else if (circle->clipIndex == 7 || circle->clipIndex == 9)
                circle->clipIndex = 4;
            else
                circle->clipIndex = 1;
            break;
        case SDLK_DOWN:
            circle->velY -= circle->maxVelY;
            if (circle->clipIndex == 5)
                circle->clipIndex = 1;
            else if (circle->clipIndex == 8 || circle->clipIndex == 6)
                circle->clipIndex = 3;
            else if (circle->clipIndex == 9 || circle->clipIndex == 7)
                circle->clipIndex = 4;
            else
                circle->clipIndex = 1;
            break;
        case SDLK_LEFT:
            circle->velX += circle->maxVelX;
            if (circle->clipIndex == 4)
                circle->clipIndex = 1;
            else if (circle->clipIndex == 7 || circle->clipIndex == 9)
                circle->clipIndex = 5;
            else if (circle->clipIndex == 6 || circle->clipIndex == 8)
                circle->clipIndex = 3;
            else
                circle->clipIndex = 1;
            break;
        case SDLK_RIGHT:
            circle->velX -= circle->maxVelX;
            if (circle->clipIndex == 3)
                circle->clipIndex = 1;
            else if (circle->clipIndex == 6 || circle->clipIndex == 8)
                circle->clipIndex = 5;
            else if (circle->clipIndex == 7 || circle->clipIndex == 9)
                circle->clipIndex = 4;
            else
                circle->clipIndex = 1;
            break;
        }
    }
    //printf("X vel %d, y vel %d \n", circle->velX, circle->velY);
}

void box_free_movement_arrow_key(Box *box, SDL_Event* e)
{
    //If a key was pressed
    if( e->type == SDL_KEYDOWN && e->key.repeat == 0 ) //only give us the first instance the key was pressed
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_UP:
            box->velY -= box->maxVelY;
            break;
        case SDLK_DOWN:
            box->velY += box->maxVelY;
            break;
        case SDLK_LEFT:
            box->velX -= box->maxVelX;
            break;
        case SDLK_RIGHT:
            box->velX += box->maxVelX;
            break;
        }
    }//If a key was released
    else if( e->type == SDL_KEYUP && e->key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_UP:
            box->velY += box->maxVelY;
            break;
        case SDLK_DOWN:
            box->velY -= box->maxVelY;
            break;
        case SDLK_LEFT:
            box->velX += box->maxVelX;
            break;
        case SDLK_RIGHT:
            box->velX -= box->maxVelX;
            break;
        }
    }
}

void box_platformer_movement_arrow_keys(Box *box, SDL_Event* e)
{
    //If a key was pressed
    if( e->type == SDL_KEYDOWN && e->key.repeat == 0 ) //only give us the first instance the key was pressed
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_LEFT:
            box->leftKeyHeld = true;
            break;
        case SDLK_RIGHT:
            box->rightKeyHeld = true;
            break;
        case SDLK_SPACE:
            if (box->velY == 0 && !box->jumpKeyHeld) box->jumpKeyHeld = true; //jump
            break;
        }
    }//If a key was released
    else if( e->type == SDL_KEYUP && e->key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_LEFT:
            box->leftKeyHeld = false;
            break;
        case SDLK_RIGHT:
            box->rightKeyHeld = false;
            break;
        case SDLK_SPACE:
            box->jumpKeyHeld = false;
            if (box->velY < -(box->maxVelY /1.4)) box->velY /= 1.2; //damping the jump if keey is released early
            break;
        }
    }
}

void motion_handle_event_arrow_keys(void* object, enum OBJ_TYPE type, SDL_Event* e, enum MOTION_TYPE motionType)
{
    if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP)
    {
        switch (type)
        {
        case OBJ_CIRCLE:
        {
            Circle *obj = (Circle *)object;
            switch (motionType)
            {
            case MOTION_FREE:
                circle_free_movement_arrow_keys(obj, e);
                return;
            case MOTION_PLATFORMER:
                printf("ERROR - platformer motion for the circle has not yet been impletmented");
                return;
            }
            return;
        }
        case OBJ_BOX:
        {
            Box *obj = (Box *)object;
            {
                switch (motionType)
                {
                case MOTION_FREE:
                    box_free_movement_arrow_key(obj, e);
                    return;
                case MOTION_PLATFORMER:
                    box_platformer_movement_arrow_keys(obj, e);
                    return;
                }
                return;
            }
        }
        }
    }
}

// #### WASD MOVEMENT ####
void circle_free_movement_wasd(Circle* circle, SDL_Event* e)
{

    //If a key was pressed
    if( e->type == SDL_KEYDOWN && e->key.repeat == 0 ) //only give us the first instance the key was pressed
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_w:
            circle->velY -= circle->maxVelY;
            break;
        case SDLK_s:
            circle->velY += circle->maxVelY;
            break;
        case SDLK_a:
            circle->velX -= circle->maxVelX;
            break;
        case SDLK_d:
            circle->velX += circle->maxVelX;
            break;
        }
    }//If a key was released
    else if( e->type == SDL_KEYUP && e->key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_w:
            circle->velY += circle->maxVelY;
            break;
        case SDLK_s:
            circle->velY -= circle->maxVelY;
            break;
        case SDLK_a:
            circle->velX += circle->maxVelX;
            break;
        case SDLK_d:
            circle->velX -= circle->maxVelX;
            break;
        }
    }
    //printf("X vel %d, y vel %d \n", circle->velX, circle->velY);
}

void box_free_movement_wasd(Box *box, SDL_Event* e)
{
    //If a key was pressed
    if( e->type == SDL_KEYDOWN && e->key.repeat == 0 ) //only give us the first instance the key was pressed
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_w:
            box->velY -= box->maxVelY;
            break;
        case SDLK_s:
            box->velY += box->maxVelY;
            break;
        case SDLK_a:
            box->velX -= box->maxVelX;
            break;
        case SDLK_d:
            box->velX += box->maxVelX;
            break;
        }
    }//If a key was released
    else if( e->type == SDL_KEYUP && e->key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_w:
            box->velY += box->maxVelY;
            break;
        case SDLK_s:
            box->velY -= box->maxVelY;
            break;
        case SDLK_a:
            box->velX += box->maxVelX;
            break;
        case SDLK_d:
            box->velX -= box->maxVelX;
            break;
        }
    }
}

void box_platformer_movement_wasd(Box *box, SDL_Event* e)
{
    //If a key was pressed
    if( e->type == SDL_KEYDOWN && e->key.repeat == 0 ) //only give us the first instance the key was pressed
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_a:
            box->leftKeyHeld = true;
            box->clipIndex = 3;
            break;
        case SDLK_d:
            box->rightKeyHeld = true;
            box->clipIndex = 2;
            break;
        case SDLK_SPACE:
            box->clipIndex = 4;
            if (box->velY == 0 && !box->jumpKeyHeld) box->jumpKeyHeld = true; //jump
            break;
        }
    }//If a key was released
    else if( e->type == SDL_KEYUP && e->key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e->key.keysym.sym )
        {
        case SDLK_a:
            box->leftKeyHeld = false;
            box->clipIndex = 1;
            break;
        case SDLK_d:
            box->rightKeyHeld = false;
            box->clipIndex = 1;
            break;
        case SDLK_SPACE:
            box->clipIndex = 1;
            box->jumpKeyHeld = false;
            if (box->velY < -(box->maxVelY /1.4)) box->velY /= 1.2; //damping the jump if keey is released early
            break;
        }
    }
}


void motion_handle_event_wasd(void* object, enum OBJ_TYPE type, SDL_Event* e, enum MOTION_TYPE motionType)
{
    if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP)
    {
        switch (type)
        {
        case OBJ_CIRCLE:
        {
            Circle *obj = (Circle *)object;
            switch (motionType)
            {
            case MOTION_FREE:
                circle_free_movement_wasd(obj, e);
                return;

            case MOTION_PLATFORMER:
                printf("ERROR - platformer motion for circle not implemented yet");
                return;
            }
        }
        case OBJ_BOX:
        {
            Box *obj = (Box *)object;
            {
                switch (motionType)
                {
                case MOTION_FREE:
                    box_free_movement_wasd(obj, e);
                    return;
                case MOTION_PLATFORMER:
                    box_platformer_movement_wasd(obj, e);
                    return;
                }
                return;
            }
        }
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

CollisionRect* collision_rect_init(short x, short y, short width, short height, Texture* texture, CollisionObjectList* environmentList)
{
    CollisionRect* cRect = malloc(sizeof(CollisionRect));
    cRect->rect.x = x;
    cRect->rect.y = y;
    cRect->rect.w = width;
    cRect->rect.h = height;
    cRect->texture = (texture != NULL) ? texture : NULL;

    collision_object_add(environmentList, cRect, COLLISION_ENVIRONMENT_RECT);

    return cRect;
}

void draw_basic_collision_rect(CollisionRect* cRect, SDL_FRect* camera, SDL_Renderer* renderer)
{
    SDL_FRect renderRect = {cRect->rect.x - camera->x, cRect->rect.y - camera->y, cRect->rect.w, cRect->rect.h};
    draw_filled_rect(renderer, NULL, &renderRect, COLOR[GREEN]);
}

void draw_textured_collision_rect(CollisionRect* cRect, SDL_FRect* camera, SDL_Renderer* renderer)
{
    SDL_FRect renderRect = {cRect->rect.x - camera->x, cRect->rect.y - camera->y, cRect->rect.w, cRect->rect.h};
    SDL_RenderCopyF(renderer, cRect->texture->mTexture, NULL, &renderRect);
}


CollisionCircle* collision_circle_init(float x, float y, short radius, Texture* texture, CollisionObjectList* environmentList)
{
    CollisionCircle* cCircle = malloc(sizeof(CollisionCircle));
    cCircle->x = x;
    cCircle->y = y;
    cCircle->radius = radius;
    cCircle->texture = (texture != NULL) ? texture : NULL;

    collision_object_add(environmentList, cCircle, COLLISION_ENVIRONMENT_CIRCLE);

    return cCircle;
}

void draw_basic_collsion_circle(CollisionCircle* cCircle, SDL_FRect* camera, SDL_Renderer* renderer)
{
    draw_filled_circle(renderer, (int)(cCircle->x - camera->x), (int)(cCircle->y - camera->y), cCircle->radius, COLOR[GREEN]);
}

void draw_textured_collision_circle(CollisionCircle* cCircle, SDL_FRect* camera, SDL_Renderer* renderer)
{
    SDL_FRect renderRect = {cCircle->x - cCircle->radius - camera->x, cCircle->y - cCircle->radius - camera->y, cCircle->radius *2, cCircle->radius *2};
    SDL_RenderCopyF(renderer, cCircle->texture->mTexture, NULL, &renderRect);
}

//*************************************************************

// Circle functions
//*************************************************************
Circle circle_init(int x, int y, int radius, int maxVelX, int maxVelY, Texture* texture)
{
    Circle circle = {x, y, radius, 0, 0, maxVelX, maxVelY, texture == NULL ? NULL : texture, 0};
    return circle;
}

bool circle_detect_collision(Circle* circle, CollisionObjectList* colList)
{
    for (int i = 0; i < colList->totalObjects; i++)
    {
        if (colList->type[i] == COLLISION_BOUNDING_BOX)
        {
            BoundingBox *bb = (BoundingBox *)colList->obj[i];
            if ((circle->x + circle->radius < bb->x) || (circle->y + circle->radius < bb->y) || (circle->x + circle->radius > bb->width) || (circle->y + circle->radius > bb->height))
                return true;
        }
        else if (colList->type[i] == COLLISION_ENVIRONMENT_RECT)
        {
            CollisionRect* cR = (CollisionRect *)colList->obj[i];
            if (circle_box_collision(circle->x, circle->y, circle->radius, &cR->rect))
                return true;
        }
        else if (colList->type[i] == COLLISION_ENVIRONMENT_CIRCLE)
        {
            CollisionCircle* cC = (CollisionCircle *)colList->obj[i];
            if (radii_collision(circle->x, circle->y, circle->radius, cC->x, cC->y, cC->radius))
                return true;
        }
        else if (colList->type[i] == COLLISION_CIRCLE)
        {
            Circle *c = (Circle *)colList->obj[i];
            if (c == circle) continue; //testing for same entity
            if (radii_collision(circle->x, circle->y, circle->radius, c->x, c->y, c->radius))
                return true;
        }
        else if (colList->type[i] == COLLISION_BOX)
        {
            Box *b = (Box *)colList->obj[i];
            if (circle_box_collision(circle->x, circle->y, circle->radius, &b->rect))
                return true;
        }
    }
    return false;
}

void circle_move_free(Circle* circle, CollisionObjectList* colList, float deltaTime, enum COLLISION_CONTACT_EFFECT collEffect)
{
    // Move the obj left or right
    circle->x += circle->velX * deltaTime;
    if (colList != NULL)
    {
        if (circle_detect_collision(circle, colList))
        {
            if (collEffect == CONTACT_BOUNCE_OFF) circle->x -= circle->velX * deltaTime *2;
            else if (collEffect == CONTACT_STOP) circle->x -= circle->velX * deltaTime;
            //else if (collEffect == CONTACT_DESTROY) { /* Handle destruction if needed */
            circle->clipIndex = 10;
        }
    }

    // Move the circle up or down
    circle->y += circle->velY * deltaTime;
    if (colList != NULL)
    {
        if (circle_detect_collision(circle, colList))
        {
            if (collEffect == CONTACT_BOUNCE_OFF) circle->y -= circle->velY * deltaTime *2;
            else if (collEffect == CONTACT_STOP) circle->y -= circle->velY * deltaTime;
            //else if (collEffect == CONTACT_DESTROY) { /* Handle destruction if needed */
            circle->clipIndex = 10;
        }
    }
}

void circle_filled_draw(Circle* circle, SDL_FRect* camera, SDL_Renderer *renderer, SDL_Color colour)
{
    draw_filled_circle(renderer, circle->x - camera->x, circle->y - camera->y, circle->radius, colour);
}

void circle_outlined_draw(Circle* circle, SDL_FRect* camera, SDL_Renderer *renderer, SDL_Color colour)
{
    draw_outlined_circle(renderer, circle->x - camera->x, circle->y - camera->y, circle->radius, colour);
}

void circle_texture_render(Circle* circle, SDL_Renderer* renderer, SDL_FRect* camera)
{
    SDL_FRect worldRect = {circle->x - circle->radius, circle->y - circle->radius, circle->radius *2, circle->radius *2};
    SDL_FRect screen = world_frect_to_screen_frect(camera, &worldRect);

    if (circle->clipIndex == 0)
        SDL_RenderCopyF(renderer, circle->texture->mTexture, NULL, &screen);
    else
    {
        SDL_Rect clipRect = {0, circle->clipIndex * (circle->radius * 2) - circle->radius * 2, circle->radius *2, circle->radius *2};
        SDL_RenderCopyF(renderer, circle->texture->mTexture, &clipRect, &screen);
    }
}

//*************************************************************


//Box functions
//*************************************************************
Box box_init_basic(short x, short y, short width, short height, short maxVelX, short maxVelY, Texture* texture)
{
    Box b = {{x, y, width, height,}, (float)x, (float)y, 0, false, false, false, 0, 0, maxVelX, maxVelY, texture == NULL ? NULL : texture, 0};
    return b;
}

Box box_init_platformer_movement(short x, short y, short width, short height, float accelerating, short maxVelX, short jumpHeight, Texture* texture)
{
    Box b = {{x, y, width, height,}, (float)x, (float)y, accelerating, false, false, false, 0, 0, maxVelX, jumpHeight, texture == NULL ? NULL : texture, 0};
    return b;
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
        else if (colList->type[i] == COLLISION_CIRCLE)
        {
            Circle *c = (Circle *)colList->obj[i];
            if (circle_box_collision(c->x, c->y, c->radius, box))
            {
                if (sendBack != NULL && sendBackType == COLLISION_RETURN_FLOOR)  *sendBack = c->y - c->radius;
                else if(sendBack != NULL && sendBackType == COLLISION_RETURN_CEILING) *sendBack = c->y + c->radius;
                return true;
            }
        }
        else if (colList->type[i] == COLLISION_BOX)
        {
            Box *b = (Box *)colList->obj[i];
            if (&b->rect == box) continue; //testing if the same entity
            if (box_box_collision(box, &b->rect))
            {
                if (sendBack != NULL && sendBackType == COLLISION_RETURN_FLOOR) *sendBack = b->rect.y;
                else if(sendBack != NULL && sendBackType == COLLISION_RETURN_CEILING) *sendBack = b->rect.y + b->rect.h;
                return true;
            }
        }
    }
    return false;
}

void box_move_platformer(Box* box, CollisionObjectList* colList, float deltaTime, enum COLLISION_CONTACT_EFFECT collEffect)
{
    Box* b = box;
    // handle left movement
    if (b->leftKeyHeld)
    {
        if (b->velX > -b->maxVelX)
            b->velX -= ((b->maxVelX * deltaTime) / b->accelerating);
        else
            b->velX = -b->maxVelX;
    }
    else if (!b->leftKeyHeld && b->velX < 0)
        b->velX = 0;
    if (b->rightKeyHeld)
    {
        if(b->velX < b->maxVelX)
            b->velX += ((b->maxVelX * deltaTime) / b->accelerating);
        else b->velX = b->maxVelX;
    }
    else if(!b->rightKeyHeld && b->velX > 0)
        b->velX = 0;

    b->x += b->velX * deltaTime;
    b->rect.x = b->x;
    if (box_detect_collision(&box->rect, colList, NULL, COLLISION_RETURN_NONE))
    {
        b->x -= b->velX * deltaTime;
        b->rect.x = b->x;
    }
    //printf("%0.2f - %d\n", b->velX, b->rect.x);

    //handle vertical movement
    if (b->velY > -1) box->rect.y += 1;
    if (box_detect_collision(&box->rect, colList, NULL, COLLISION_RETURN_NONE)) //checking for floor on floor state
    {
        b->rect.y -= 1;
        b->velY = 0;
    }
    else
    {
        b->rect.y -= 1;

        if (b->velY == 0) b->velY += 5; //start falling down if no floor detected
        else if (b->velY > 0)
        {
            short floorCol = 0;  //landing of floor find the point of collision
            b->y += b->velY * deltaTime * 2;
            b->rect.y = b->y;
            if (box_detect_collision(&b->rect, colList, &floorCol, COLLISION_RETURN_FLOOR))
            {
                b->y = floorCol - b->rect.h;
                b->rect.y = b->y;
                b->velY = 0;
            }
            else
            {
                if (b->velY == b->maxVelY)
                    b->velY -= 10;
                else b->velY += 10;
            }
        }
        else if (b->velY < 0)       //on the way up
        {
            b->y += b->velY * deltaTime * 2;
            b->rect.y = b->y;
            short ceilingCol = 0;
            if (box_detect_collision(&box->rect, colList, &ceilingCol, COLLISION_RETURN_CEILING)) //checking for collision above and if so cutting jump short
            {
                b->y = ceilingCol +1;
                b->rect.y = b->y;
                b->velY = 0;
            }
            else
            {
                b->velY += 10;
            }
        }
    }

    if (b->jumpKeyHeld)
    {
        if (b->velY == 0)      //init jumping up
        {
            b->velY -= b->maxVelY;
            b->y += b->velY * deltaTime * 6;
            b->rect.y = b->y;

            short ceilingCol = 0;
            if (box_detect_collision(&box->rect, colList, &ceilingCol, COLLISION_RETURN_CEILING)) //checking for collision above and if so cutting jump short
            {
                b->y = ceilingCol +1;
                b->rect.y = b->y;
                b->velY = 0;
            }
        }
        b->jumpKeyHeld = false;
    }
}

void box_move_free(Box* box, CollisionObjectList* colList, float deltaTime, enum COLLISION_CONTACT_EFFECT collEffect)
{
    //move the obj left or right
    box->x += box->velX * deltaTime;
    box->rect.x = box->x;
    if (colList != NULL)
        if (box_detect_collision(&box->rect, colList, NULL, COLLISION_RETURN_NONE))
        {
            if (collEffect == CONTACT_BOUNCE_OFF)
                box->x -= box->velX * deltaTime * 2;
            else if (collEffect == CONTACT_STOP)
                box->x -= box->velX * deltaTime;
            box->rect.x = box->x;
        }
    box->y += box->velY * deltaTime;
    box->rect.y = box->y;
    if (colList != NULL)
        if (box_detect_collision(&box->rect, colList, NULL, COLLISION_RETURN_NONE))
        {
            if (collEffect == CONTACT_BOUNCE_OFF)
                box->y -= box->velY * deltaTime * 2;
            else if (collEffect == CONTACT_STOP)
                box->y -= box->velY * deltaTime;
            box->rect.y = box->y;
        }
}


void box_outlined_draw(Box* box, SDL_Renderer* renderer, SDL_Color color)
{
    draw_outlined_rect(renderer, &box->rect, color);
}
void box_filled_draw(Box* box, SDL_Renderer* renderer, SDL_Color color)
{
    draw_filled_rect(renderer, &box->rect, NULL, color);
}


void box_filled_draw_camera(Box* box, SDL_FRect* camera, SDL_Renderer* renderer, SDL_Color color)
{
    SDL_FRect worldRect = { box->x, box->y, box->rect.w, box->rect.h };
    SDL_FRect screen = world_frect_to_screen_frect(camera, &worldRect);

    draw_filled_rect(renderer, NULL, &screen, color);
}

void box_texture_render(Box* box, SDL_Renderer* renderer, SDL_FRect* camera)
{
    SDL_FRect worldRect = { box->x, box->y, box->rect.w, box->rect.h };
    SDL_FRect screen = world_frect_to_screen_frect(camera, &worldRect);

    if (box->clipIndex == 0)
        SDL_RenderCopyF(renderer, box->texture->mTexture, NULL, &screen);
    else
    {
        SDL_Rect clip = {0, box->clipIndex * box->rect.h - box->rect.h, box->rect.w, box->rect.h};
        SDL_RenderCopyF(renderer, box->texture->mTexture, &clip, &screen);
    }


}


//*************************************************************



//Timer functions
//*************************************************************
void init_timer(Timer* timer)
{
    timer->startTick = 0;
    timer->pausedTick = 0;
    timer->runningTicks = 0;
    timer->paused = false;
    timer->started = false;
}

void start_timer(Timer* timer)
{
    if (timer->started)
    {
        printf("WARNING timer already started!\n");
        return;
    }
    timer->started = true;
    timer->paused = false;
    timer->startTick = SDL_GetTicks();
    timer->runningTicks = 0;
}

void update_timer(Timer* timer)
{
    if (timer->started && !timer->paused)  timer->runningTicks = SDL_GetTicks() - timer->startTick;
    else if (!timer->started && !timer->paused) timer->runningTicks = 0;
}

void stop_timer(Timer* timer)
{
    timer->started = false;
    timer->paused = false;
    timer->startTick = 0;
    timer->pausedTick = 0;
    timer->runningTicks = 0;
}

void pause_timer(Timer* timer)
{
    if (timer->started && !timer->paused)
    {
        update_timer(timer);
        timer->paused = true;
        timer->pausedTick = SDL_GetTicks() - timer->startTick;
    }
    else
    {
        printf("WARNIMG timer not started or already paused!\n");
    }
}

void unpause_timer(Timer* timer)
{
    if (timer->started && timer->paused)
    {
        timer->paused = false;
        timer->startTick = SDL_GetTicks() - timer->pausedTick;
        update_timer(timer);
    }
    else
    {
        printf("WARNING timer not started or not paused!\n");
    }
}

uint32_t get_timer_ticks(Timer* timer)
{
    return timer->runningTicks;
}

void fps_timer_calc_and_render(Timer* timer, Texture* texture,SDL_Renderer* renderer, TTF_Font* font, int x, int y, uint32_t frameCount, SDL_Color colour)
{
    char fpsText[20];
    float avgFPS = frameCount / (get_timer_ticks(timer) / 1000.f);
    if (avgFPS > 2000000)
        avgFPS = 0;

    sprintf(fpsText, "FPS: %.4f", avgFPS);
    if (!load_texture_from_rendered_text(texture, fpsText, font, colour, renderer))
    {
        return;
    }

    render_texture(texture, renderer, x, y);
}

//*************************************************************






// Buttons
//*************************************************************
bool init_button_from_file(Button* button, SDL_Renderer* renderer, const char* filePath, int x, int y, int width, int height, uint8_t totalStates, SDL_Color colourKey)
{
    if (!load_texture_from_file_colourKey(&button->texture, filePath, renderer, colourKey))
    {
        printf("ERROR unable to load button texture from %s\n", filePath);
        return false;
    }

    button->xPos = x;
    button->yPos = y;

    // Assuming the button states are arranged vertically in the texture
    for (int i = 0; i < totalStates; i++)
    {
        button->spriteClip[i].x = 0;
        button->spriteClip[i].y = i * height;
        button->spriteClip[i].w = width;
        button->spriteClip[i].h = height;
    }
    button->totalStates = totalStates;
    button->currentState = BUTTON_STATE_NORMAL;

    // Initialize remaining states to to normal state
    for (int i = totalStates; i < BUTTON_STATE_TOTAL; i++)
    {
        button->spriteClip[i].x = 0;
        button->spriteClip[i].y = 0;
        button->spriteClip[i].w = width;
        button->spriteClip[i].h = height;
    }

    return true;
}

void button_handle_event(Button* button, SDL_Event* e)
{
    // Ignore non-mouse events
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {
        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is inside button
        bool inside = true;

        if (x < button->xPos)
        {
            inside = false;
        }
        else if (x > button->xPos + button->spriteClip[BUTTON_STATE_NORMAL].w)
        {
            inside = false;
        }
        else if (y < button->yPos)
        {
            inside = false;
        }
        else if (y > button->yPos + button->spriteClip[BUTTON_STATE_NORMAL].h)
        {
            inside = false;
        }

        // Mouse is outside button
        if (!inside)
        {
            button->currentState = BUTTON_STATE_NORMAL;
            return;
        }
        else
        {
            // Mouse is inside button
            switch (e->type)
            {
            case SDL_MOUSEMOTION:
                button->currentState = BUTTON_STATE_HOVERED;
                break;
            case SDL_MOUSEBUTTONDOWN:
                button->currentState = BUTTON_STATE_PRESSED;
                break;
            case SDL_MOUSEBUTTONUP:
                button->currentState = BUTTON_STATE_RELEASED;
                break;
            }
        }
    }
    return;
}

void render_button(Button* button, SDL_Renderer* renderer)
{
    render_texture_clip(&button->texture, renderer, &button->spriteClip[button->currentState], button->xPos, button->yPos);
}

void free_button(Button* button)
{
    free_texture(&button->texture);
}
//*************************************************************



