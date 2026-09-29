/*
    Runner
    The Chapter 17 project from Learning C++ by Building Games

    She runs, and the world scrolls past. Press Space, W, or the Up
    arrow to jump the crates and grab the coins, and the pace keeps
    rising. Hit a crate, and the run is over: press R to run again.
    Escape, or the window's X, quits.

    New in this project: a sprite sheet, with six frames of running in
    one picture; scenery in layers that scroll at different speeds;
    numbers drawn from a strip of digits; and every texture kept in a
    std::unordered_map, loaded by name.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <string>          // std::string and std::to_string
#include <unordered_map>   // std::unordered_map, for the textures
#include <vector>          // std::vector, for the layers, crates, and coins

const int   WINDOW_W  = 960;    // window width in pixels
const int   WINDOW_H  = 540;    // window height in pixels
const float MAX_DELTA = 0.1f;   // the longest frame we'll allow, in seconds

// The scenery: every layer's picture is exactly the size of the window
const float LAYER_W  = 960.0f;
const float LAYER_H  = 540.0f;
const float GROUND_Y = 452.0f;   // the line the runner and the crates stand on

// How fast the ground scrolls, in pixels per second
const float START_SPEED = 360.0f;
const float MAX_SPEED   = 620.0f;
const float SPEED_GAIN  = 6.0f;     // added every second, up to MAX_SPEED

// The runner's sprite sheet: six frames, each 112 by 180, in one row
const float FRAME_W    = 112.0f;
const float FRAME_H    = 180.0f;
const int   RUN_FRAMES = 6;
const int   JUMP_FRAME = 2;         // her longest stride doubles as her jump
const float FRAME_TIME = 0.08f;     // seconds per frame, at START_SPEED
const float FEET_GAP   = 2.0f;      // empty pixels below her feet
const float RUNNER_X   = 150.0f;    // she stays put, and the world moves
const float RUNNER_Y   = GROUND_Y + FEET_GAP - FRAME_H;   // on the ground

// Jumping
const float JUMP_SPEED = -820.0f;   // pixels per second at take-off, upward
const float GRAVITY    = 2200.0f;   // pixels per second, per second

// Her hitbox, measured from the top-left of her frame. It's smaller than
// the picture, so that brushing a crate with a flying heel doesn't count.
const SDL_FRect HITBOX = { 34.0f, 28.0f, 44.0f, 146.0f };

// The crates
const float CRATE_SIZE    = 64.0f;    // crate.png is 64 by 64
const float FIRST_CRATE   = 600.0f;   // pixels run before the first crate
const float MIN_CRATE_GAP = 420.0f;   // pixels from one crate to the next
const float MAX_CRATE_GAP = 820.0f;

// The coins
const float COIN_SIZE    = 36.0f;    // coin.png is 36 by 36
const float FIRST_COINS  = 300.0f;   // pixels run before the first coins
const float MIN_COIN_GAP = 260.0f;   // pixels from one row to the next
const float MAX_COIN_GAP = 560.0f;
const int   COINS_IN_ROW = 4;
const float COIN_SPACING = 46.0f;    // from one coin to the next in a row
const float LOW_COINS_Y  = GROUND_Y - 120.0f;   // caught just by running
const float HIGH_COINS_Y = GROUND_Y - 230.0f;   // caught only in a jump

// The numbers at the top of the window
const float DIGIT_W          = 26.0f;    // digits.png is ten digits, each
const float DIGIT_H          = 40.0f;    // 26 by 40, from 0 to 9
const float HUD_MARGIN       = 16.0f;    // pixels from the window's edges
const float PIXELS_PER_METER = 40.0f;

// Every texture, found by its name
using TextureMap = std::unordered_map<std::string, SDL_Texture*>;

// The pictures to load, each one from assets/<name>.png
const char* TEXTURE_NAMES[] = { "sky", "hills_far", "hills_near", "ground",
                                "runner", "crate", "coin", "digits",
                                "crashed" };

// One layer of scenery, scrolling at its own share of the ground's speed
struct Layer
{
    std::string texture;   // its name in the texture map
    float depth;           // 0 never moves, and 1 moves with the ground
    float offset;          // how far it has scrolled, from 0 up to LAYER_W
};

// What the runner is doing
enum class RunnerState
{
    Running,   // on the ground, legs going
    Jumping,   // in the air, going up or coming down
    Crashed    // stopped by a crate
};

// The runner. She always stays at RUNNER_X, and only moves up and down.
struct Runner
{
    RunnerState state;
    float y;            // the top of her frame on the screen
    float velY;         // pixels per second, and negative is up
    int frame;          // which of the six running frames to show
    float frameTimer;   // seconds spent on that frame so far
};

// Everything that changes as the game is played
struct Game
{
    Runner runner;
    std::vector<Layer> layers;
    std::vector<SDL_FRect> crates;
    std::vector<SDL_FRect> coins;
    float speed;           // pixels per second that the ground scrolls
    float distance;        // pixels run so far
    float nextCrateAt;     // the distance for the next crate
    float nextCoinsAt;     // the distance for the next row of coins
    int coinCount;
};

// A random number from low up to high
float randomBetween(float low, float high)
{
    return low + SDL_randf() * (high - low);
}

// Load every picture in TEXTURE_NAMES into the map, and report whether
// they all loaded. It stops at the first one that fails.
bool loadTextures(SDL_Renderer* renderer, TextureMap& textures)
{
    for (const char* name : TEXTURE_NAMES)
    {
        std::string path = std::string("assets/") + name + ".png";
        SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
        if (texture == nullptr)
        {
            SDL_Log("Couldn't load %s: %s", path.c_str(), SDL_GetError());
            return false;
        }
        textures[name] = texture;
    }
    return true;
}

// Find a texture by its name, or say so and return nullptr. A mistyped
// name can't add an entry to the map, as the square brackets would.
SDL_Texture* getTexture(const TextureMap& textures, const std::string& name)
{
    auto it = textures.find(name);
    if (it == textures.end())
    {
        SDL_Log("No texture called %s", name.c_str());
        return nullptr;
    }
    return it->second;
}

// Destroy every texture in the map, and then empty it
void destroyTextures(TextureMap& textures)
{
    for (const auto& [name, texture] : textures)
        SDL_DestroyTexture(texture);
    textures.clear();
}

// Put everything back where it starts
void resetGame(Game& game)
{
    game.runner.state = RunnerState::Running;
    game.runner.y = RUNNER_Y;
    game.runner.velY = 0.0f;
    game.runner.frame = 0;
    game.runner.frameTimer = 0.0f;

    game.layers = {
        { "sky",        0.0f,  0.0f },
        { "hills_far",  0.15f, 0.0f },
        { "hills_near", 0.45f, 0.0f },
        { "ground",     1.0f,  0.0f }
    };
    game.crates.clear();
    game.coins.clear();
    game.speed = START_SPEED;
    game.distance = 0.0f;
    game.nextCrateAt = FIRST_CRATE;
    game.nextCoinsAt = FIRST_COINS;
    game.coinCount = 0;
}

// Launch her upward, but only from the ground: no jumping in mid-air
void jump(Runner& runner)
{
    if (runner.state == RunnerState::Running)
    {
        runner.state = RunnerState::Jumping;
        runner.velY = JUMP_SPEED;
    }
}

// Move the runner on by one frame
void updateRunner(Runner& runner, float delta, float speed)
{
    switch (runner.state)
    {
    case RunnerState::Running:
        // The faster the ground scrolls, the faster her legs go
        runner.frameTimer += delta * (speed / START_SPEED);
        while (runner.frameTimer >= FRAME_TIME)
        {
            runner.frameTimer -= FRAME_TIME;
            runner.frame = (runner.frame + 1) % RUN_FRAMES;
        }
        break;

    case RunnerState::Jumping:
        runner.velY += GRAVITY * delta;
        runner.y += runner.velY * delta;
        if (runner.y >= RUNNER_Y)
        {
            runner.y = RUNNER_Y;   // she has landed
            runner.velY = 0.0f;
            runner.state = RunnerState::Running;
        }
        break;

    case RunnerState::Crashed:
        break;   // she doesn't move again until R
    }
}

// Forget everything that has gone off the left edge of the window,
// counting down, as Chapter 13 did, so that no erase makes us skip one
void removeOffScreen(std::vector<SDL_FRect>& things)
{
    for (int i = static_cast<int>(things.size()) - 1; i >= 0; i--)
    {
        if (things[i].x + things[i].w < 0.0f)
            things.erase(things.begin() + i);
    }
}

// Scroll the world to the left by this frame's share of the speed
void moveWorld(Game& game, float delta)
{
    float dx = game.speed * delta;
    game.distance += dx;

    for (Layer& layer : game.layers)
    {
        layer.offset += dx * layer.depth;
        if (layer.offset >= LAYER_W)
            layer.offset -= LAYER_W;
    }
    for (SDL_FRect& crate : game.crates)
        crate.x -= dx;
    for (SDL_FRect& coin : game.coins)
        coin.x -= dx;

    removeOffScreen(game.crates);
    removeOffScreen(game.coins);
}

// Add new things just past the right edge of the window, when the
// distance run says it's time
void spawnThings(Game& game)
{
    if (game.distance >= game.nextCrateAt)
    {
        SDL_FRect crate = { static_cast<float>(WINDOW_W),
                            GROUND_Y - CRATE_SIZE, CRATE_SIZE, CRATE_SIZE };
        game.crates.push_back(crate);
        game.nextCrateAt += randomBetween(MIN_CRATE_GAP, MAX_CRATE_GAP);
    }

    if (game.distance >= game.nextCoinsAt)
    {
        float y = SDL_rand(2) == 0 ? LOW_COINS_Y : HIGH_COINS_Y;
        for (int i = 0; i < COINS_IN_ROW; i++)
        {
            SDL_FRect coin = { WINDOW_W + i * COIN_SPACING, y,
                               COIN_SIZE, COIN_SIZE };
            game.coins.push_back(coin);
        }
        game.nextCoinsAt += COINS_IN_ROW * COIN_SPACING +
                            randomBetween(MIN_COIN_GAP, MAX_COIN_GAP);
    }
}

// Where her hitbox is on the screen right now
SDL_FRect runnerHitbox(const Runner& runner)
{
    return { RUNNER_X + HITBOX.x, runner.y + HITBOX.y, HITBOX.w, HITBOX.h };
}

// Check what her hitbox has run into
void checkCollisions(Game& game)
{
    SDL_FRect box = runnerHitbox(game.runner);

    for (const SDL_FRect& crate : game.crates)
    {
        if (SDL_HasRectIntersectionFloat(&box, &crate))
        {
            game.runner.state = RunnerState::Crashed;
            return;
        }
    }

    for (int i = static_cast<int>(game.coins.size()) - 1; i >= 0; i--)
    {
        if (SDL_HasRectIntersectionFloat(&box, &game.coins[i]))
        {
            game.coinCount++;
            game.coins.erase(game.coins.begin() + i);
        }
    }
}

// Move the whole game on by one frame
void updateGame(Game& game, float delta)
{
    if (game.runner.state == RunnerState::Crashed)
        return;

    game.speed += SPEED_GAIN * delta;
    if (game.speed > MAX_SPEED)
        game.speed = MAX_SPEED;

    updateRunner(game.runner, delta, game.speed);
    moveWorld(game, delta);
    spawnThings(game);
    checkCollisions(game);
}

// Draw a layer twice, side by side, shifted left by its offset. When the
// first copy has slid right off the window, the offset wraps back to 0,
// and the second copy is exactly where the first one started.
void drawLayer(SDL_Renderer* renderer, SDL_Texture* texture, float offset)
{
    SDL_FRect first = { -offset, 0.0f, LAYER_W, LAYER_H };
    SDL_FRect second = { LAYER_W - offset, 0.0f, LAYER_W, LAYER_H };
    SDL_RenderTexture(renderer, texture, nullptr, &first);
    SDL_RenderTexture(renderer, texture, nullptr, &second);
}

// Draw the world, from the back to the front
void drawWorld(SDL_Renderer* renderer, const TextureMap& textures,
               const Game& game)
{
    for (const Layer& layer : game.layers)
    {
        drawLayer(renderer, getTexture(textures, layer.texture),
                  layer.offset);
    }

    SDL_Texture* crate = getTexture(textures, "crate");
    for (const SDL_FRect& rect : game.crates)
        SDL_RenderTexture(renderer, crate, nullptr, &rect);

    SDL_Texture* coin = getTexture(textures, "coin");
    for (const SDL_FRect& rect : game.coins)
        SDL_RenderTexture(renderer, coin, nullptr, &rect);
}

// Draw the runner's frame from the sprite sheet
void drawRunner(SDL_Renderer* renderer, SDL_Texture* sheet,
                const Runner& runner)
{
    int frame = runner.frame;
    if (runner.y < RUNNER_Y)
        frame = JUMP_FRAME;   // she's off the ground

    SDL_FRect src = { frame * FRAME_W, 0.0f, FRAME_W, FRAME_H };
    SDL_FRect dst = { RUNNER_X, runner.y, FRAME_W, FRAME_H };

    if (runner.state == RunnerState::Crashed)
        SDL_SetTextureColorMod(sheet, 255, 110, 110);   // ouch
    SDL_RenderTexture(renderer, sheet, &src, &dst);
    SDL_SetTextureColorMod(sheet, 255, 255, 255);       // back to normal
}

// Draw a whole number, digit by digit, from the strip of digits, with its
// top-left corner at (x, y)
void drawNumber(SDL_Renderer* renderer, SDL_Texture* digits, int value,
                float x, float y)
{
    std::string text = std::to_string(value);
    for (char c : text)
    {
        int digit = c - '0';   // the characters '0' to '9' become 0 to 9
        SDL_FRect src = { digit * DIGIT_W, 0.0f, DIGIT_W, DIGIT_H };
        SDL_FRect dst = { x, y, DIGIT_W, DIGIT_H };
        SDL_RenderTexture(renderer, digits, &src, &dst);
        x += DIGIT_W;
    }
}

// How wide drawNumber will draw a number, in pixels
float numberWidth(int value)
{
    return static_cast<float>(std::to_string(value).size()) * DIGIT_W;
}

// Draw the coins collected at the top left, and the meters run at the
// top right
void drawHud(SDL_Renderer* renderer, const TextureMap& textures,
             const Game& game)
{
    SDL_Texture* digits = getTexture(textures, "digits");

    SDL_FRect icon = { HUD_MARGIN, HUD_MARGIN + (DIGIT_H - COIN_SIZE) / 2.0f,
                       COIN_SIZE, COIN_SIZE };
    SDL_RenderTexture(renderer, getTexture(textures, "coin"), nullptr, &icon);
    drawNumber(renderer, digits, game.coinCount,
               HUD_MARGIN + COIN_SIZE + 8.0f, HUD_MARGIN);

    int meters = static_cast<int>(game.distance / PIXELS_PER_METER);
    drawNumber(renderer, digits, meters,
               WINDOW_W - HUD_MARGIN - numberWidth(meters), HUD_MARGIN);
}

// Draw the crash banner in the middle of the window, a little high
void drawCrashed(SDL_Renderer* renderer, SDL_Texture* banner)
{
    float w = 0.0f;
    float h = 0.0f;
    SDL_GetTextureSize(banner, &w, &h);
    SDL_FRect dst = { (WINDOW_W - w) / 2.0f, (WINDOW_H - h) / 2.0f - 40.0f,
                      w, h };
    SDL_RenderTexture(renderer, banner, nullptr, &dst);
}

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Runner", WINDOW_W, WINDOW_H, 0);
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

    // Every picture, loaded by name
    TextureMap textures;
    if (!loadTextures(renderer, textures))
    {
        destroyTextures(textures);   // the ones that did load
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // The game, ready to run
    Game game;
    resetGame(game);

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
            // A key going down, but not the repeats from holding it
            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
            {
                switch (event.key.key)
                {
                case SDLK_SPACE:
                case SDLK_W:
                case SDLK_UP:
                    jump(game.runner);
                    break;
                case SDLK_R:
                    if (game.runner.state == RunnerState::Crashed)
                        resetGame(game);
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
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
        updateGame(game, delta);

        // Draw the frame, from the back to the front
        SDL_RenderClear(renderer);
        drawWorld(renderer, textures, game);
        drawRunner(renderer, getTexture(textures, "runner"), game.runner);
        drawHud(renderer, textures, game);
        if (game.runner.state == RunnerState::Crashed)
            drawCrashed(renderer, getTexture(textures, "crashed"));

        SDL_RenderPresent(renderer);
    }

    // Clean up, in the reverse order we created things
    destroyTextures(textures);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
