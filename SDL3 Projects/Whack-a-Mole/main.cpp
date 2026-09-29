/*
    Whack-a-Mole
    The Chapter 11 project from Learning C++ by Building Games

    Moles pop up out of nine holes. Click one with the mallet to bonk
    it before it drops back down. You have sixty seconds, and the moles
    get quicker as the clock runs down. When time's up, press R to play
    again, or Escape to quit.

    New in this project: SDL3_image. Every picture is a PNG file in the
    assets folder, loaded into an SDL_Texture.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <string>   // std::string and std::to_string, for the title bar

const int   WINDOW_W  = 800;    // window width in pixels
const int   WINDOW_H  = 600;    // window height in pixels
const float MAX_DELTA = 0.1f;   // the longest frame we'll allow, in seconds

// The holes: three rows of three
const int   HOLE_COLS    = 3;
const int   HOLE_ROWS    = 3;
const int   HOLE_COUNT   = HOLE_COLS * HOLE_ROWS;
const float HOLE_W       = 200.0f;   // hole_back.png and hole_front.png
const float HOLE_H       = 100.0f;   // are both 200 by 100
const float FIRST_HOLE_X = 70.0f;    // the top-left hole's top-left corner
const float FIRST_HOLE_Y = 176.0f;
const float HOLE_GAP_X   = 230.0f;   // from one hole to the next, across
const float HOLE_GAP_Y   = 160.0f;   // and down

// The moles, and the round
const float MOLE_W          = 120.0f;  // mole.png is 120 by 140
const float MOLE_OFFSET_X   = 40.0f;   // the mole's left edge, from the hole's
const float MOLE_BASE_Y     = 70.0f;   // its bottom edge, hidden by the lip
const float MOLE_MAX_SHOW   = 130.0f;  // pixels showing when it's fully up
const float RISE_SPEED      = 520.0f;  // pixels per second, going up or down
const float ROUND_TIME      = 60.0f;   // seconds in a round
const float START_UP_TIME   = 1.2f;    // how long a mole waits, at the start
const float END_UP_TIME     = 0.55f;   // and by the end of the round
const float START_SPAWN_GAP = 0.9f;    // seconds between moles, at the start
const float END_SPAWN_GAP   = 0.45f;   // and by the end
const int   SPAWN_TRIES     = 10;      // random holes to try for a new mole

// Bonking, and the timers that a click starts
const float     BONK_TIME  = 0.4f;     // how long a bonked mole stays dazed
const float     FLASH_TIME = 0.1f;     // how long it flashes red
const SDL_Color FLASH_TINT = { 255, 120, 120, 255 };   // the red of the flash
const float     SWING_TIME = 0.15f;    // seconds for the mallet to swing back
const float     STAR_TIME  = 0.4f;     // seconds for a star to fade away

// The mallet, and the star that bursts out of a bonked mole
const float      MALLET_SIZE  = 128.0f;              // mallet.png is 128 by 128
const SDL_FPoint MALLET_HEAD  = { 44.0f, 44.0f };    // its head, in the image
const SDL_FPoint MALLET_PIVOT = { 110.5f, 110.5f };  // its handle's end
const double     SWING_ANGLE  = -35.0;    // degrees of tilt at the hit
const float      STAR_SIZE    = 96.0f;    // star.png is 96 by 96
const float      STAR_GROWTH  = 0.5f;     // how much bigger it grows
const float      STAR_Y       = -40.0f;   // its center, from the hole's top

// The strip across the top of the window, and the end of the round
const float     STRIP_H     = 70.0f;
const float     TITLE_X     = 12.0f;    // where the title goes in the strip
const float     TITLE_Y     = 2.0f;
const float     TITLE_SCALE = 0.4f;     // title.png shrunk to fit the strip
const SDL_FRect TIMER_BAR   = { 470.0f, 24.0f, 300.0f, 22.0f };
const float     BAR_INSET   = 4.0f;     // the gap inside the bar's frame

const SDL_Color STRIP_COLOR = { 20, 50, 20, 110 };     // see-through green
const SDL_Color FRAME_COLOR = { 255, 255, 255, 255 };  // white
const SDL_Color BAR_COLOR   = { 155, 225, 93, 255 };   // bright green
const SDL_Color DIM_COLOR   = { 0, 0, 0, 120 };        // see-through black

// Every picture in the game, each one starting as "not loaded yet"
struct Textures
{
    SDL_Texture* grass      = nullptr;
    SDL_Texture* holeBack   = nullptr;
    SDL_Texture* holeFront  = nullptr;
    SDL_Texture* mole       = nullptr;
    SDL_Texture* moleBonked = nullptr;
    SDL_Texture* mallet     = nullptr;
    SDL_Texture* star       = nullptr;
    SDL_Texture* title      = nullptr;
    SDL_Texture* timesUp    = nullptr;
};

// The five things a mole can be doing
enum class MoleState
{
    Hidden,    // down its hole
    Rising,    // on its way up
    Up,        // waiting to be bonked
    Bonked,    // dazed after a hit
    Sinking    // on its way back down
};

// One hole, and the mole that lives in it
struct Hole
{
    float x;             // the hole's top-left corner on screen
    float y;
    MoleState state;
    float shown;         // how many pixels of the mole are showing
    float timer;         // seconds left while it's Up or Bonked
    float starTimer;     // seconds left on this hole's star
    bool facingLeft;     // true to draw the mole flipped
};

// Everything that changes while you play
struct Game
{
    Hole holes[HOLE_COUNT];
    int score;
    float timeLeft;      // seconds left in the round
    float spawnTimer;    // seconds until the next mole comes up
    float swingTimer;    // seconds left in the mallet's swing
    bool gameOver;
};

// Load one PNG into a texture, or say why it failed and return nullptr
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* path)
{
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (texture == nullptr)
        SDL_Log("Couldn't load %s: %s", path, SDL_GetError());
    return texture;
}

// Load every picture, and report whether they all loaded
bool loadTextures(SDL_Renderer* renderer, Textures& t)
{
    t.grass      = loadTexture(renderer, "assets/grass.png");
    t.holeBack   = loadTexture(renderer, "assets/hole_back.png");
    t.holeFront  = loadTexture(renderer, "assets/hole_front.png");
    t.mole       = loadTexture(renderer, "assets/mole.png");
    t.moleBonked = loadTexture(renderer, "assets/mole_bonked.png");
    t.mallet     = loadTexture(renderer, "assets/mallet.png");
    t.star       = loadTexture(renderer, "assets/star.png");
    t.title      = loadTexture(renderer, "assets/title.png");
    t.timesUp    = loadTexture(renderer, "assets/times_up.png");

    return t.grass && t.holeBack && t.holeFront && t.mole && t.moleBonked
        && t.mallet && t.star && t.title && t.timesUp;
}

// Destroy one texture, and set the caller's pointer back to nullptr
void destroyTexture(SDL_Texture*& texture)
{
    if (texture != nullptr)
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

// Destroy every picture: safe even if some of them never loaded
void destroyTextures(Textures& t)
{
    destroyTexture(t.grass);
    destroyTexture(t.holeBack);
    destroyTexture(t.holeFront);
    destroyTexture(t.mole);
    destroyTexture(t.moleBonked);
    destroyTexture(t.mallet);
    destroyTexture(t.star);
    destroyTexture(t.title);
    destroyTexture(t.timesUp);
}

// Put every hole in its place, send every mole down, and reset the clock
void resetGame(Game& game)
{
    for (int i = 0; i < HOLE_COUNT; i++)
    {
        Hole& hole = game.holes[i];
        int col = i % HOLE_COLS;
        int row = i / HOLE_COLS;
        hole.x = FIRST_HOLE_X + col * HOLE_GAP_X;
        hole.y = FIRST_HOLE_Y + row * HOLE_GAP_Y;
        hole.state = MoleState::Hidden;
        hole.shown = 0.0f;
        hole.timer = 0.0f;
        hole.starTimer = 0.0f;
        hole.facingLeft = false;
    }

    game.score = 0;
    game.timeLeft = ROUND_TIME;
    game.spawnTimer = START_SPAWN_GAP;
    game.swingTimer = 0.0f;
    game.gameOver = false;
}

// A number partway from a to b: a when t is 0, and b when t is 1
float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

// Send a mole up out of a random empty hole, if one turns up quickly
void spawnMole(Game& game)
{
    for (int attempt = 0; attempt < SPAWN_TRIES; attempt++)
    {
        Hole& hole = game.holes[SDL_rand(HOLE_COUNT)];
        if (hole.state == MoleState::Hidden)
        {
            hole.state = MoleState::Rising;
            hole.facingLeft = (SDL_rand(2) == 0);
            return;
        }
    }
}

// Move one mole through its life: up, a wait (or a bonk), and down again
void updateHole(Hole& hole, float delta, float upTime)
{
    switch (hole.state)
    {
    case MoleState::Hidden:
        break;

    case MoleState::Rising:
        hole.shown += RISE_SPEED * delta;
        if (hole.shown >= MOLE_MAX_SHOW)
        {
            hole.shown = MOLE_MAX_SHOW;
            hole.state = MoleState::Up;
            hole.timer = upTime;
        }
        break;

    case MoleState::Up:
    case MoleState::Bonked:
        hole.timer -= delta;
        if (hole.timer <= 0.0f)
            hole.state = MoleState::Sinking;
        break;

    case MoleState::Sinking:
        hole.shown -= RISE_SPEED * delta;
        if (hole.shown <= 0.0f)
        {
            hole.shown = 0.0f;
            hole.state = MoleState::Hidden;
        }
        break;
    }

    if (hole.starTimer > 0.0f)
        hole.starTimer -= delta;
}

// Move the whole game on by one frame
void updateGame(Game& game, float delta)
{
    if (game.swingTimer > 0.0f)
        game.swingTimer -= delta;

    // How far through the round we are: 0 at the start, and 1 at the end
    float progress = 1.0f - game.timeLeft / ROUND_TIME;
    float upTime = lerp(START_UP_TIME, END_UP_TIME, progress);

    for (int i = 0; i < HOLE_COUNT; i++)
        updateHole(game.holes[i], delta, upTime);

    if (game.gameOver)
        return;   // the moles finish what they're doing, but no more come

    game.timeLeft -= delta;
    if (game.timeLeft <= 0.0f)
    {
        game.timeLeft = 0.0f;
        game.gameOver = true;
        return;
    }

    game.spawnTimer -= delta;
    if (game.spawnTimer <= 0.0f)
    {
        spawnMole(game);
        game.spawnTimer = lerp(START_SPAWN_GAP, END_SPAWN_GAP, progress);
    }
}

// Where the showing part of a hole's mole is, on screen
SDL_FRect moleRect(const Hole& hole)
{
    SDL_FRect rect;
    rect.x = hole.x + MOLE_OFFSET_X;
    rect.y = hole.y + MOLE_BASE_Y - hole.shown;
    rect.w = MOLE_W;
    rect.h = hole.shown;
    return rect;
}

// Bonk the mole under the mallet, if there is one, and report a hit
bool whack(Game& game, float mouseX, float mouseY)
{
    SDL_FPoint point = { mouseX, mouseY };

    for (int i = 0; i < HOLE_COUNT; i++)
    {
        Hole& hole = game.holes[i];
        bool hittable = hole.state == MoleState::Rising ||
                        hole.state == MoleState::Up;
        SDL_FRect rect = moleRect(hole);

        if (hittable && SDL_PointInRectFloat(&point, &rect))
        {
            hole.state = MoleState::Bonked;
            hole.timer = BONK_TIME;
            hole.starTimer = STAR_TIME;
            game.score++;
            return true;
        }
    }
    return false;
}

// Show the score in the title bar, and how to play again once time's up
void updateTitle(SDL_Window* window, const Game& game)
{
    std::string title = "Whack-a-Mole    Score: " +
                        std::to_string(game.score);
    if (game.gameOver)
        title += "    (press R to play again)";

    SDL_SetWindowTitle(window, title.c_str());
}

// Draw a hole's star, growing and fading as its timer runs down
void drawStar(SDL_Renderer* renderer, const Textures& t, const Hole& hole)
{
    if (hole.starTimer <= 0.0f)
        return;

    float life = hole.starTimer / STAR_TIME;   // 1 when it's new, 0 at the end
    float size = STAR_SIZE * (1.0f + STAR_GROWTH * (1.0f - life));
    float centerX = hole.x + HOLE_W / 2.0f;
    float centerY = hole.y + STAR_Y;
    SDL_FRect dst = { centerX - size / 2.0f, centerY - size / 2.0f,
                      size, size };

    SDL_SetTextureAlphaMod(t.star, static_cast<Uint8>(255.0f * life));
    SDL_RenderTexture(renderer, t.star, nullptr, &dst);
    SDL_SetTextureAlphaMod(t.star, 255);   // solid again, for next time
}

// Draw the mallet with its head over the mouse. While it swings, it's
// tilted around the end of its handle, easing back upright.
void drawMallet(SDL_Renderer* renderer, const Textures& t,
                float mouseX, float mouseY, float swingTimer)
{
    SDL_FRect dst = { mouseX - MALLET_HEAD.x, mouseY - MALLET_HEAD.y,
                      MALLET_SIZE, MALLET_SIZE };

    double angle = 0.0;
    if (swingTimer > 0.0f)
        angle = SWING_ANGLE * (swingTimer / SWING_TIME);

    SDL_RenderTextureRotated(renderer, t.mallet, nullptr, &dst,
                             angle, &MALLET_PIVOT, SDL_FLIP_NONE);
}

// Fill a rectangle with a color
void drawRect(SDL_Renderer* renderer, const SDL_FRect& rect,
              const SDL_Color& color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

// The strip across the top: the title on the left, and the timer bar
void drawHud(SDL_Renderer* renderer, const Textures& t, const Game& game)
{
    SDL_FRect strip = { 0.0f, 0.0f, static_cast<float>(WINDOW_W), STRIP_H };
    drawRect(renderer, strip, STRIP_COLOR);

    float titleW = 0.0f;
    float titleH = 0.0f;
    SDL_GetTextureSize(t.title, &titleW, &titleH);
    SDL_FRect titleRect = { TITLE_X, TITLE_Y,
                            titleW * TITLE_SCALE, titleH * TITLE_SCALE };
    SDL_RenderTexture(renderer, t.title, nullptr, &titleRect);

    SDL_SetRenderDrawColor(renderer, FRAME_COLOR.r, FRAME_COLOR.g,
                           FRAME_COLOR.b, FRAME_COLOR.a);
    SDL_RenderRect(renderer, &TIMER_BAR);

    float fraction = game.timeLeft / ROUND_TIME;   // 1 when full, 0 when empty
    SDL_FRect fill = { TIMER_BAR.x + BAR_INSET, TIMER_BAR.y + BAR_INSET,
                       (TIMER_BAR.w - 2.0f * BAR_INSET) * fraction,
                       TIMER_BAR.h - 2.0f * BAR_INSET };
    drawRect(renderer, fill, BAR_COLOR);
}

// Dim the whole field, and show the "Time's up!" banner in the middle
void drawTimesUp(SDL_Renderer* renderer, const Textures& t)
{
    SDL_FRect everything = { 0.0f, 0.0f, static_cast<float>(WINDOW_W),
                             static_cast<float>(WINDOW_H) };
    drawRect(renderer, everything, DIM_COLOR);

    float w = 0.0f;
    float h = 0.0f;
    SDL_GetTextureSize(t.timesUp, &w, &h);
    SDL_FRect dst = { (WINDOW_W - w) / 2.0f, (WINDOW_H - h) / 2.0f, w, h };
    SDL_RenderTexture(renderer, t.timesUp, nullptr, &dst);
}

// Draw one hole: its back, then its mole, then its front lip
void drawHole(SDL_Renderer* renderer, const Textures& t, const Hole& hole)
{
    SDL_FRect holeRect = { hole.x, hole.y, HOLE_W, HOLE_H };
    SDL_RenderTexture(renderer, t.holeBack, nullptr, &holeRect);

    if (hole.shown > 0.0f)
    {
        // Just the top of the mole: the part that's out of the hole
        SDL_FRect src = { 0.0f, 0.0f, MOLE_W, hole.shown };
        SDL_FRect dst = moleRect(hole);

        bool bonked = hole.state == MoleState::Bonked;
        SDL_Texture* picture = bonked ? t.moleBonked : t.mole;
        SDL_FlipMode flip = hole.facingLeft ? SDL_FLIP_HORIZONTAL
                                            : SDL_FLIP_NONE;

        // A quick red flash, just after a hit
        bool flashing = bonked && hole.timer > BONK_TIME - FLASH_TIME;
        if (flashing)
        {
            SDL_SetTextureColorMod(picture, FLASH_TINT.r, FLASH_TINT.g,
                                   FLASH_TINT.b);
        }

        SDL_RenderTextureRotated(renderer, picture, &src, &dst,
                                 0.0, nullptr, flip);

        if (flashing)
            SDL_SetTextureColorMod(picture, 255, 255, 255);   // back to normal
    }

    SDL_RenderTexture(renderer, t.holeFront, nullptr, &holeRect);
}

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Whack-a-Mole",
                                          WINDOW_W, WINDOW_H, 0);
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Show each frame in step with the monitor's refresh
    SDL_SetRenderVSync(renderer, 1);

    // Let see-through colors blend with whatever is behind them
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // The window's icon is a surface: a picture in ordinary memory
    SDL_Surface* icon = IMG_Load("assets/icon.png");
    if (icon != nullptr)
    {
        SDL_SetWindowIcon(window, icon);
        SDL_DestroySurface(icon);   // the window keeps its own copy
    }

    // Load every picture, or tidy up and stop if any of them is missing
    Textures textures;
    if (!loadTextures(renderer, textures))
    {
        destroyTextures(textures);   // the ones that did load
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // The whole game, in its starting state
    Game game{};
    resetGame(game);
    updateTitle(window, game);

    SDL_HideCursor();   // the mallet is the mouse pointer now

    // The time at the last frame, in milliseconds
    Uint64 lastTime = SDL_GetTicks();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        // Handle every event that's waiting
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
            {
                if (event.key.key == SDLK_ESCAPE)
                    running = false;
                else if (event.key.key == SDLK_R && game.gameOver)
                {
                    resetGame(game);
                    updateTitle(window, game);
                }
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                event.button.button == SDL_BUTTON_LEFT)
            {
                game.swingTimer = SWING_TIME;
                if (!game.gameOver &&
                    whack(game, event.button.x, event.button.y))
                {
                    updateTitle(window, game);
                }
            }
        }

        // Delta time, never more than MAX_DELTA
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;
        if (delta > MAX_DELTA)
            delta = MAX_DELTA;

        // Move the game on by one frame
        bool wasOver = game.gameOver;
        updateGame(game, delta);
        if (game.gameOver != wasOver)
            updateTitle(window, game);   // the round has just ended

        // Draw the frame, starting with the grass
        SDL_RenderTextureTiled(renderer, textures.grass, nullptr, 1.0f,
                               nullptr);

        for (int i = 0; i < HOLE_COUNT; i++)
            drawHole(renderer, textures, game.holes[i]);
        for (int i = 0; i < HOLE_COUNT; i++)
            drawStar(renderer, textures, game.holes[i]);

        drawHud(renderer, textures, game);
        if (game.gameOver)
            drawTimesUp(renderer, textures);

        float mouseX = 0.0f;
        float mouseY = 0.0f;
        SDL_GetMouseState(&mouseX, &mouseY);
        drawMallet(renderer, textures, mouseX, mouseY, game.swingTimer);

        SDL_RenderPresent(renderer);
    }

    // Clean up: the textures first, while their renderer still exists
    destroyTextures(textures);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
