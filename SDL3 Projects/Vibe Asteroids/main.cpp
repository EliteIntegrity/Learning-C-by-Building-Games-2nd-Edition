/*
    Vibe Asteroids
    The Chapter 27 project from Learning C++ by Building Games

    Classic Asteroids. Turn with the left and right arrows, or A and D,
    thrust with the up arrow or W, and fire with Space. Shoot a large
    asteroid, and it splits into two medium ones; shoot a medium one, and
    it splits into two small ones; shoot a small one, and it's gone. Fly
    into an asteroid, and you lose a life. Clear the screen, and a new
    wave arrives. When your lives run out, press R to play again, or
    Escape to quit.

    This is the version I ended up with after nine rounds of describing,
    reading, running, and editing with an AI. Yours will look different.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cmath>    // std::cos and std::sin, for turning angles into steps
#include <string>   // std::string and std::to_string, for the title bar
#include <vector>   // std::vector, for the bullets and the asteroids

// The window, and the longest frame we'll allow, in seconds
constexpr int WINDOW_W = 1024;
constexpr int WINDOW_H = 768;
constexpr float MAX_DELTA = 0.1f;

// Angles are in radians: a full turn, and a quarter turn back from the
// right, which is straight up, because y points down
constexpr float TWO_PI = 6.2831853f;
constexpr float FACING_UP = -TWO_PI / 4.0f;

// The ship
constexpr float SHIP_RADIUS = 14.0f;       // from its center to its nose
constexpr float SHIP_THRUST = 220.0f;      // pixels per second, every second
constexpr float SHIP_TURN_SPEED = 4.0f;    // radians per second
constexpr float SHIP_DRAG = 0.6f;          // how quickly drag slows the ship
constexpr float RESPAWN_S = 1.5f;          // seconds before a lost ship returns
constexpr float SAFE_DISTANCE = 6.0f * SHIP_RADIUS;   // no rock appears nearer

// The bullets
constexpr float BULLET_SPEED = 520.0f;     // pixels per second
constexpr float BULLET_LIFE_S = 0.8f;      // seconds before a bullet is gone
constexpr float BULLET_RADIUS = 2.0f;      // for hitting things
constexpr float FIRE_COOLDOWN_S = 0.18f;   // seconds between shots

// The game
constexpr int START_LIVES = 3;
constexpr int WAVE_SIZE = 4;               // large asteroids in every wave

// The colors
constexpr SDL_Color BACKGROUND = { 8, 8, 16, 255 };          // almost black
constexpr SDL_Color SHIP_COLOR = { 230, 230, 240, 255 };     // white
constexpr SDL_Color FLAME_COLOR = { 255, 160, 60, 255 };     // orange
constexpr SDL_Color BULLET_COLOR = { 255, 240, 200, 255 };   // pale yellow
constexpr SDL_Color ROCK_COLOR = { 200, 200, 200, 255 };     // light gray

// An asteroid's size, which is also its place in the three tables below
enum class AstSize
{
    Large,
    Medium,
    Small
};
constexpr float AST_RADIUS[] = { 38.0f, 22.0f, 12.0f };      // pixels
constexpr float AST_MAX_SPEED[] = { 90.0f, 130.0f, 180.0f }; // pixels a second
constexpr int AST_SCORE[] = { 20, 50, 100 };                 // points

// A position, a velocity, or a step: x across, and y down
struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;
};

Vec2 operator+(const Vec2& a, const Vec2& b)
{
    return Vec2{ a.x + b.x, a.y + b.y };
}

Vec2 operator*(const Vec2& v, float scale)
{
    return Vec2{ v.x * scale, v.y * scale };
}

// The step one unit long in the direction of an angle, as in Chapter 12
Vec2 direction(float angle)
{
    return Vec2{ std::cos(angle), std::sin(angle) };
}

// Bring a position that has left the window back in at the opposite edge
void wrap(Vec2& p)
{
    while (p.x < 0.0f)
        p.x += WINDOW_W;
    while (p.x >= WINDOW_W)
        p.x -= WINDOW_W;
    while (p.y < 0.0f)
        p.y += WINDOW_H;
    while (p.y >= WINDOW_H)
        p.y -= WINDOW_H;
}

// A random float from low up to high, as in Chapter 9
float randomBetween(float low, float high)
{
    return low + SDL_randf() * (high - low);
}

// The player's ship
struct Ship
{
    Vec2 pos;
    Vec2 vel;
    float angle = FACING_UP;     // radians, turning clockwise from the right
    bool thrusting = false;
    bool alive = true;
    float fireCooldown = 0.0f;   // seconds until it can fire again
};

// A bullet, which lasts BULLET_LIFE_S seconds
struct Bullet
{
    Vec2 pos;
    Vec2 vel;
    float lifeRemaining = 0.0f;  // seconds
};

// A lumpy rock that drifts, and slowly turns
struct Asteroid
{
    Vec2 pos;
    Vec2 vel;
    AstSize size = AstSize::Large;
    float angle = 0.0f;          // how far it has turned, in radians
    float spin = 0.0f;           // radians per second
};

// Everything that changes while you play
struct Game
{
    Ship ship;
    std::vector<Bullet> bullets;
    std::vector<Asteroid> asteroids;
    int score = 0;
    int lives = START_LIVES;
    bool gameOver = false;
    float respawnTimer = 0.0f;   // seconds until a lost ship returns
};

// Do two circles overlap? Their centers must be closer than their two
// radii added together, and comparing the squares avoids a square root.
bool circleHit(Vec2 a, float radiusA, Vec2 b, float radiusB)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float reach = radiusA + radiusB;
    return dx * dx + dy * dy <= reach * reach;
}

// A new asteroid at pos, heading in a random direction, at a random speed
Asteroid makeAsteroid(Vec2 pos, AstSize size)
{
    float maxSpeed = AST_MAX_SPEED[static_cast<int>(size)];
    Asteroid rock;
    rock.pos = pos;
    rock.vel = direction(randomBetween(0.0f, TWO_PI)) *
               randomBetween(20.0f, maxSpeed);
    rock.size = size;
    rock.spin = randomBetween(-1.5f, 1.5f);
    return rock;
}

// A random point on one of the window's four edges
Vec2 randomEdge()
{
    float w = static_cast<float>(WINDOW_W);
    float h = static_cast<float>(WINDOW_H);
    switch (SDL_rand(4))
    {
    case 0:
        return Vec2{ randomBetween(0.0f, w), 0.0f };   // the top
    case 1:
        return Vec2{ w, randomBetween(0.0f, h) };      // the right
    case 2:
        return Vec2{ randomBetween(0.0f, w), h };      // the bottom
    default:
        return Vec2{ 0.0f, randomBetween(0.0f, h) };   // the left
    }
}

// A new wave of large asteroids, on the edges, but never near the ship
void spawnWave(Game& game)
{
    game.asteroids.clear();
    for (int i = 0; i < WAVE_SIZE; i++)
    {
        Vec2 pos;
        do
        {
            pos = randomEdge();
        } while (circleHit(pos, 0.0f, game.ship.pos, SAFE_DISTANCE));
        game.asteroids.push_back(makeAsteroid(pos, AstSize::Large));
    }
}

// Is every asteroid at least SAFE_DISTANCE from pos?
bool clearOfRocks(const Game& game, Vec2 pos)
{
    for (const Asteroid& rock : game.asteroids)
    {
        if (circleHit(pos, 0.0f, rock.pos, SAFE_DISTANCE))
            return false;
    }
    return true;
}

// Put the ship back in the middle, still, and facing up
void resetShip(Ship& ship)
{
    ship.pos = Vec2{ WINDOW_W / 2.0f, WINDOW_H / 2.0f };
    ship.vel = Vec2{};
    ship.angle = FACING_UP;
    ship.alive = true;
    ship.fireCooldown = 0.0f;
}

// Start a new game
void resetGame(Game& game)
{
    resetShip(game.ship);
    game.bullets.clear();
    spawnWave(game);
    game.score = 0;
    game.lives = START_LIVES;
    game.gameOver = false;
    game.respawnTimer = 0.0f;
}

// Turn, thrust, and fire, from the keys that are held down
void steerShip(Game& game, const bool* keys, float delta)
{
    Ship& ship = game.ship;
    ship.thrusting = false;
    if (!ship.alive || game.gameOver)
        return;

    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
        ship.angle -= SHIP_TURN_SPEED * delta;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
        ship.angle += SHIP_TURN_SPEED * delta;
    ship.thrusting = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];

    if (keys[SDL_SCANCODE_SPACE] && ship.fireCooldown <= 0.0f)
    {
        Bullet bullet;
        bullet.pos = ship.pos + direction(ship.angle) * SHIP_RADIUS;
        bullet.vel = direction(ship.angle) * BULLET_SPEED;
        bullet.lifeRemaining = BULLET_LIFE_S;
        game.bullets.push_back(bullet);
        ship.fireCooldown = FIRE_COOLDOWN_S;
    }
}

// Move the ship, or, once it's been lost, bring it back when it's safe
void updateShip(Game& game, float delta)
{
    Ship& ship = game.ship;
    if (!ship.alive)
    {
        Vec2 middle = Vec2{ WINDOW_W / 2.0f, WINDOW_H / 2.0f };
        game.respawnTimer -= delta;
        if (!game.gameOver && game.respawnTimer <= 0.0f &&
            clearOfRocks(game, middle))
        {
            resetShip(ship);
        }
        return;
    }

    if (ship.thrusting)
        ship.vel = ship.vel + direction(ship.angle) * (SHIP_THRUST * delta);

    // Drag takes a share of the speed, but never more than all of it
    float dragFactor = 1.0f - SHIP_DRAG * delta;
    if (dragFactor < 0.0f)
        dragFactor = 0.0f;
    ship.vel = ship.vel * dragFactor;

    ship.pos = ship.pos + ship.vel * delta;
    wrap(ship.pos);

    if (ship.fireCooldown > 0.0f)
        ship.fireCooldown -= delta;
}

// Move the bullets, and remove the ones whose time is up
void updateBullets(Game& game, float delta)
{
    for (Bullet& bullet : game.bullets)
    {
        bullet.pos = bullet.pos + bullet.vel * delta;
        wrap(bullet.pos);
        bullet.lifeRemaining -= delta;
    }
    std::erase_if(game.bullets,
                  [](const Bullet& b) { return b.lifeRemaining <= 0.0f; });
}

// Drift and turn every asteroid
void updateAsteroids(Game& game, float delta)
{
    for (Asteroid& rock : game.asteroids)
    {
        rock.pos = rock.pos + rock.vel * delta;
        wrap(rock.pos);
        rock.angle += rock.spin * delta;
    }
}

// Shoot asteroid i: it splits into two of the next size down, or, if it's
// small, it's gone. Returns the points it scores.
int splitAsteroid(Game& game, size_t i)
{
    AstSize size = game.asteroids[i].size;
    Vec2 pos = game.asteroids[i].pos;

    if (size == AstSize::Large)
    {
        game.asteroids.push_back(makeAsteroid(pos, AstSize::Medium));
        game.asteroids.push_back(makeAsteroid(pos, AstSize::Medium));
    }
    else if (size == AstSize::Medium)
    {
        game.asteroids.push_back(makeAsteroid(pos, AstSize::Small));
        game.asteroids.push_back(makeAsteroid(pos, AstSize::Small));
    }

    game.asteroids.erase(game.asteroids.begin() + i);
    return AST_SCORE[static_cast<int>(size)];
}

// Every bullet against every asteroid. A hit splits the asteroid, scores,
// and uses up the bullet.
void shootAsteroids(Game& game)
{
    for (Bullet& bullet : game.bullets)
    {
        for (size_t i = 0; i < game.asteroids.size(); i++)
        {
            const Asteroid& rock = game.asteroids[i];
            float radius = AST_RADIUS[static_cast<int>(rock.size)];
            if (circleHit(bullet.pos, BULLET_RADIUS, rock.pos, radius))
            {
                game.score += splitAsteroid(game, i);
                bullet.lifeRemaining = 0.0f;   // removed next frame
                break;                         // the asteroids have changed
            }
        }
    }
}

// The ship against every asteroid. A hit costs a life.
void crashShip(Game& game)
{
    if (!game.ship.alive)
        return;

    for (const Asteroid& rock : game.asteroids)
    {
        float radius = AST_RADIUS[static_cast<int>(rock.size)];
        if (circleHit(game.ship.pos, SHIP_RADIUS, rock.pos, radius))
        {
            game.ship.alive = false;
            game.lives--;
            game.respawnTimer = RESPAWN_S;
            if (game.lives <= 0)
                game.gameOver = true;
            return;
        }
    }
}

// Show the score and the lives in the window's title bar
void showStatus(SDL_Window* window, const Game& game)
{
    std::string title = "Vibe Asteroids - Score: " +
                        std::to_string(game.score) +
                        "   Lives: " + std::to_string(game.lives);
    if (game.gameOver)
        title += "   GAME OVER";
    SDL_SetWindowTitle(window, title.c_str());
}

// One frame of the game
void update(Game& game, float delta, SDL_Window* window)
{
    updateShip(game, delta);
    updateBullets(game, delta);
    updateAsteroids(game, delta);
    shootAsteroids(game);
    crashShip(game);

    // A clear screen brings the next wave
    if (game.asteroids.empty() && !game.gameOver)
        spawnWave(game);

    showStatus(window, game);
}

// The ship: a narrow triangle, with a flame behind it while it thrusts
void drawShip(SDL_Renderer* renderer, const Ship& ship)
{
    if (!ship.alive)
        return;

    // Turn a point from the ship's own frame, where the nose points along
    // x, by the ship's angle, and move it to the ship's position
    float c = std::cos(ship.angle);
    float s = std::sin(ship.angle);
    auto toWorld = [&ship, c, s](Vec2 p)
    {
        return Vec2{ ship.pos.x + p.x * c - p.y * s,
                     ship.pos.y + p.x * s + p.y * c };
    };
    Vec2 nose = toWorld(Vec2{ SHIP_RADIUS, 0.0f });
    Vec2 leftRear = toWorld(Vec2{ -0.8f * SHIP_RADIUS, -0.7f * SHIP_RADIUS });
    Vec2 rightRear = toWorld(Vec2{ -0.8f * SHIP_RADIUS, 0.7f * SHIP_RADIUS });

    SDL_SetRenderDrawColor(renderer, SHIP_COLOR.r, SHIP_COLOR.g,
                           SHIP_COLOR.b, SHIP_COLOR.a);
    SDL_RenderLine(renderer, nose.x, nose.y, leftRear.x, leftRear.y);
    SDL_RenderLine(renderer, leftRear.x, leftRear.y, rightRear.x, rightRear.y);
    SDL_RenderLine(renderer, rightRear.x, rightRear.y, nose.x, nose.y);

    if (ship.thrusting)
    {
        Vec2 tail = toWorld(Vec2{ -0.8f * SHIP_RADIUS, 0.0f });
        Vec2 flame = toWorld(Vec2{ -1.6f * SHIP_RADIUS, 0.0f });
        SDL_SetRenderDrawColor(renderer, FLAME_COLOR.r, FLAME_COLOR.g,
                               FLAME_COLOR.b, FLAME_COLOR.a);
        SDL_RenderLine(renderer, tail.x, tail.y, flame.x, flame.y);
    }
}

// An asteroid: ten corners around a circle, pushed in and out in three
// lumps. Each lump comes from the corner's angle around the rock itself,
// so the lumps turn with the rock.
void drawAsteroid(SDL_Renderer* renderer, const Asteroid& rock)
{
    const int SIDES = 10;
    float radius = AST_RADIUS[static_cast<int>(rock.size)];

    SDL_SetRenderDrawColor(renderer, ROCK_COLOR.r, ROCK_COLOR.g,
                           ROCK_COLOR.b, ROCK_COLOR.a);
    Vec2 prev;
    for (int i = 0; i <= SIDES; i++)
    {
        float around = TWO_PI * (i % SIDES) / SIDES;
        float lump = 0.85f + 0.15f * std::sin(around * 3.0f);
        Vec2 corner = rock.pos +
                      direction(around + rock.angle) * (radius * lump);
        if (i > 0)
            SDL_RenderLine(renderer, prev.x, prev.y, corner.x, corner.y);
        prev = corner;
    }
}

// Draw one frame: the asteroids, the bullets, and the ship
void render(SDL_Renderer* renderer, const Game& game)
{
    SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g,
                           BACKGROUND.b, BACKGROUND.a);
    SDL_RenderClear(renderer);

    for (const Asteroid& rock : game.asteroids)
        drawAsteroid(renderer, rock);

    SDL_SetRenderDrawColor(renderer, BULLET_COLOR.r, BULLET_COLOR.g,
                           BULLET_COLOR.b, BULLET_COLOR.a);
    for (const Bullet& bullet : game.bullets)
    {
        SDL_FRect dot = { bullet.pos.x - 1.5f, bullet.pos.y - 1.5f,
                          3.0f, 3.0f };
        SDL_RenderFillRect(renderer, &dot);
    }

    drawShip(renderer, game.ship);
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Vibe Asteroids",
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

    Game game;
    resetGame(game);

    bool running = true;
    Uint64 lastTicks = SDL_GetTicksNS();
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE)
                    running = false;
                if (event.key.scancode == SDL_SCANCODE_R && game.gameOver)
                    resetGame(game);
            }
        }

        // Delta time, in seconds, never more than MAX_DELTA
        Uint64 now = SDL_GetTicksNS();
        float delta = (now - lastTicks) / 1.0e9f;
        lastTicks = now;
        if (delta > MAX_DELTA)
            delta = MAX_DELTA;

        steerShip(game, SDL_GetKeyboardState(nullptr), delta);
        update(game, delta, window);
        render(renderer, game);
    }

    // Clean up, in the reverse order we created things
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
