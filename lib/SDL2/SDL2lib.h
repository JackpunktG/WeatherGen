#ifndef SDL2LIB_H
#define SDL2LIB_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdint.h>

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

// Struct to hold window and renderer information auto with IMG enabled
typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;
} WindowConstSize;

// function to start a sized window with renderer
bool init_SDL2_basic(WindowConstSize* windowStruct, const char* title, uint32_t width, uint32_t height);

// function to start a sized window with renderer with VSYNC
bool init_SDL2_basic_vsync(WindowConstSize* windowStruct, const char* title, uint32_t width, uint32_t height);

// Free SDL2 resources
void free_SDL2(WindowConstSize* windowStruct);


// Init TTF
bool init_TTF();
//need to then call TTF_Font* fone = TTF_OpenFont("path to font", font size);
// Free TTF
void free_TTF(TTF_Font* font);

// Struct to hold texture information
typedef struct
{
    SDL_Texture* mTexture;
    uint32_t width;
    uint32_t height;
} Texture;

// Texture functions
void init_texture(Texture * texture);
void free_texture(Texture* texture);
// Load texture from file (functions also init_texutre first)
bool load_texture_from_file(Texture* texture, const char* path, SDL_Renderer* renderer);
bool load_texture_from_file_colourKey(Texture* texture, const char* text, SDL_Renderer* renderer, SDL_Color colour);

//load texture from text requires TTF
bool load_texture_from_rendered_text(Texture* texture, const char* text, TTF_Font* font, SDL_Color textColor, SDL_Renderer* renderer);

//render texture
void render_texture(Texture* texture, SDL_Renderer* renderer, int x, int y);
void render_texture_clip(Texture* texture, SDL_Renderer* renderer, SDL_Rect* clip, int x, int y);
void render_texture_fullscreen(Texture* texture, SDL_Renderer* renderer, uint32_t screenWidth, uint32_t screenHeight);
void render_texture_scaled(Texture* texture, SDL_Renderer* renderer, int x, int y, int width, int height);
void render_texture_rotated_clipped(Texture* texture, SDL_Renderer* renderer, SDL_Rect* clip, int x, int y, double angle, SDL_Point* center, SDL_RendererFlip flip);
void render_texture_all_options(Texture* texture, SDL_Renderer* renderer,  SDL_Rect* clip, int x, int y, int width, int height, double angle, SDL_Point* center, SDL_RendererFlip flip);
//center is point in which the rotation occurs around, if NULL it is center of texture

//clear streen with colour
void clear_screen_with_color(SDL_Renderer* renderer, SDL_Color colour);

//Setting texute colour
void set_texture_colour(Texture* texture, uint8_t r, uint8_t g, uint8_t b);
//Setting texture alpha
void set_texture_alpha(Texture* texture, uint8_t alpha);
//Set blend mode
void set_texture_blend_mode(Texture* texture, SDL_BlendMode blendMode);



//Geometric rendering function:x
void draw_line(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, SDL_Color colour);
void draw_line_float(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, SDL_Color colour);
void draw_point(SDL_Renderer* renderer, int x, int y, SDL_Color colour);
void draw_outlined_rect(SDL_Renderer* renderer, SDL_Rect* rect, SDL_Color colour);
void draw_filled_rect(SDL_Renderer* renderer, SDL_Rect* rect, SDL_FRect* fRect,  SDL_Color colour);
void draw_outlined_circle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color colour);
void draw_filled_circle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color colour);
void draw_outlined_triangle(SDL_Renderer* renderer, SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Color colour);
void draw_filled_triangle(SDL_Renderer* renderer, SDL_Point p1, SDL_Point p2, SDL_Point p3, SDL_Color colour);


//Collision detection functions
bool radii_collision(int x1, int y1, int r1, int x2, int y2, int r2);
bool box_box_collision(SDL_Rect* a, SDL_Rect* b);
bool circle_box_collision(int circleX, int circleY, int radius, SDL_Rect* box);
void debug_collision_detection(bool state);
//Debugging collision detection draws the collision shapes
bool radii_collision_DEBUGG(SDL_Renderer* renderer, int x1, int y1, int r1, int x2, int y2, int r2);
bool box_box_collision_DEBUGG(SDL_Renderer* renderer, SDL_Rect* a, SDL_Rect* b);
bool circle_box_collision_DEBUGG(SDL_Renderer* renderer, int circleX, int circleY, int radius, SDL_Rect* box);







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
    COLLISION_BOUNDING_BOX  //i.e screen of other square enclosing spaces
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


typedef enum MOTION_TYPE
{
    MOTION_FREE, //no gravity, free moving like space
    MOTION_PLATFORMER, //gravity like platformer games with jumping and landing on collision
} MOTION_TYPE;

//motion
void motion_handle_event_wasd(void* object, enum OBJ_TYPE type, SDL_Event* e, enum MOTION_TYPE motionType);
void motion_handle_event_arrow_keys(void* object, enum OBJ_TYPE type, SDL_Event* e, enum MOTION_TYPE motionType);
//objects must have x, y, (r for circles), velX, velY, maxVelX, maxVelY members


typedef enum COLLISION_CONTACT_EFFECT  //Effects how interaction on collision
{
    CONTACT_BOUNCE_OFF,
    CONTACT_STOP,
    CONTACT_DESTROY
} BOUNCE_TYPE;


// Bounding box
typedef struct
{
    short x, y, width, height;
} BoundingBox;

//init boundingBox
BoundingBox bounding_box_init_screen(short screenWidth, short screenHeight);
BoundingBox bounding_box_init(short x, short y, short width, short height);


// Circle
typedef struct
{
    float x, y; //center of circle
    short radius;
    float velX, velY;
    short maxVelX, maxVelY;
} Circle;


//init circle
Circle circle_init(int x, int y, int radius, int maxVelX, int maxVelY);

//motion
//free "Space" style motion - can be modded with different collision effects
void circle_move_free(Circle* circle, CollisionObjectList* colList, float deltaTime, enum COLLISION_CONTACT_EFFECT collEffect);

//quick functions for rendering circle shapes
//draw filled circle
void circle_filled_draw(Circle* circle, SDL_Renderer *renderer, SDL_Color colour);
//draw outlined circle
void circle_outlined_draw(Circle* circle, SDL_Renderer *renderer, SDL_Color colour);


// Box
typedef struct
{
    SDL_Rect rect; //for rendering and collision
    float x, y; //needed for deltaTime movement precision
    float accelerating; //used for long quick to reach max x-axis velocity
    bool leftKeyHeld;
    bool rightKeyHeld;
    bool jumpKeyHeld;
    float velX, velY;
    short maxVelX, maxVelY; //maxY with platformer is changed max jump height and terminal velocity
} Box;

//init box
Box box_init_basic(short x, short y, short width, short height, short maxVelX, short maxVelY);
Box box_init_platformer_movement(short x, short y, short width, short height, float accelerating, short maxVelX, short jumpHeight);

//for box collision with send back
bool box_detect_collision(SDL_Rect* box, CollisionObjectList* colList, short *sendBack, enum COLLISION_RETURN sendBackType);

//motion
//free "Space" style motion - can be modded with different collision effects
void box_move_free(Box* box, CollisionObjectList* colList, float deltaTime, enum COLLISION_CONTACT_EFFECT collEffect);
//required init_platformer setup
// platformer style motion with gravity, jumping, left and right movement - effects
void box_move_platformer(Box* box, CollisionObjectList* colList, float deltaTime, enum COLLISION_CONTACT_EFFECT collEffect);

//quick rendering for basic Box shapes
void box_outlined_draw(Box* box, SDL_Renderer* renderer, SDL_Color color);
void box_filled_draw(Box* box, SDL_Renderer* renderer, SDL_Color color);


//Timer functions
typedef struct
{
    uint32_t startTick;
    uint32_t pausedTick;
    uint32_t runningTicks;
    bool paused;
    bool started;
} Timer;

void init_timer(Timer* timer);
void start_timer(Timer* timer);
void stop_timer(Timer* timer);
void pause_timer(Timer* timer);
void unpause_timer(Timer* timer);
void update_timer(Timer* timer); //call this at the start of each loop
uint32_t get_timer_ticks(Timer* timer);
//Calling this each loop to render fps to texture - need to be after the clear screen
void fps_timer_calc_and_render(Timer* timer, Texture* texture, SDL_Renderer* renderer, TTF_Font* font, int x, int y, uint32_t frameCount, SDL_Color colour);


//Buttons
enum ButtonState
{
    BUTTON_STATE_NORMAL = 0,
    BUTTON_STATE_PRESSED = 1,
    BUTTON_STATE_HOVERED = 2,
    BUTTON_STATE_RELEASED = 3,
    BUTTON_STATE_TOTAL = 4
};
typedef struct
{
    Texture texture;
    SDL_Rect spriteClip[BUTTON_STATE_TOTAL]; //  states: normal, hovered, pressed, released
    int xPos;
    int yPos;
    uint8_t totalStates;
    uint8_t currentState;
} Button;

// Initialize buttons assuing texture are all the same size and below one another
bool init_button_from_file(Button* button, SDL_Renderer* renderer, const char* filePath, int x, int y, int width, int height, uint8_t totalStates, SDL_Color colourKey);
void button_handle_event(Button* button, SDL_Event* e);
void render_button(Button* button, SDL_Renderer* renderer);
void free_button(Button* button);






/*
   Call back functions
SDL_TimerID <name> = SDL_AddTimer(<interval in ms>, <callback function>, <param>);

uint32_t callback(uint32_t interval, void* param)
{
    //Print call back msg
    printf("Call back called back with message: hehe ;)\n");

    Dot* dot = (Dot*)param; //void pointers need to be passed back into the right type
    //reset dot position
    dot->mPosX = 0;
    dot->mPosY= 0;
    return 0;
}

SDL_RemoveTimer(<name>);
*/

#endif // SDL2LIB_H
