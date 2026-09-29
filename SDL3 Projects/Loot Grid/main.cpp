/*
    Loot Grid
    The Chapter 16 project from Learning C++ by Building Games

    Loot is scattered across a grid: coins, gems, keys, potions, and
    hearts, each its own color. Move the green square one cell at a
    time with W, A, S, and D, and walk onto an item to pick it up. Tab
    lists what you've found in the console, R starts again with a new
    world, and Escape, or the window's X, quits.

    New in this project: the collections from Chapter 15. The world is
    a std::map from cells to items, the inventory is a std::map that
    counts them, and a std::unordered_map holds each item's name and
    color.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <map>            // std::map, for the world and the inventory
#include <unordered_map>  // std::unordered_map, for the table of items
#include <utility>        // std::pair, for a cell's column and row

// The grid
const int CELL_SIZE  = 50;                      // width and height, in pixels
const int GRID_COLS  = 16;                      // cells across
const int GRID_ROWS  = 12;                      // cells down
const int WINDOW_W   = GRID_COLS * CELL_SIZE;   // 800 pixels
const int WINDOW_H   = GRID_ROWS * CELL_SIZE;   // 600 pixels
const int ITEM_COUNT = 20;                      // items in a new world

// The colors, and how far in from its cell's edges each square is drawn
const SDL_Color BACKGROUND   = { 25, 25, 30, 255 };    // almost black
const SDL_Color GRID_LINES   = { 45, 45, 55, 255 };    // a little lighter
const SDL_Color PLAYER_COLOR = { 80, 220, 100, 255 };  // green
const float     PLAYER_INSET = 6.0f;                   // in pixels
const float     ITEM_INSET   = 12.0f;

// The five kinds of loot
enum class ItemType
{
    Coin,
    Gem,
    Key,
    Potion,
    Heart
};
const int ITEM_KINDS = 5;   // how many values ItemType has

// What each kind of loot is called, and its color
struct ItemInfo
{
    const char* name;
    SDL_Color color;
};

const std::unordered_map<ItemType, ItemInfo> ITEMS = {
    { ItemType::Coin,   { "Coin",   { 230, 200,  50, 255 } } },   // gold
    { ItemType::Gem,    { "Gem",    {  80, 220, 220, 255 } } },   // cyan
    { ItemType::Key,    { "Key",    { 220, 220, 220, 255 } } },   // silver
    { ItemType::Potion, { "Potion", { 180,  80, 220, 255 } } },   // purple
    { ItemType::Heart,  { "Heart",  { 230,  70,  90, 255 } } }    // red
};

// A cell on the grid: its column, and then its row
using Cell = std::pair<int, int>;

// Everything that changes as the game is played
struct Game
{
    Cell player;                          // the cell the player is in
    std::map<Cell, ItemType> world;       // the loot, by the cell it's in
    std::map<ItemType, int> inventory;    // how many of each we've found
};

// One of the five kinds of loot, at random
ItemType randomItem()
{
    return static_cast<ItemType>(SDL_rand(ITEM_KINDS));
}

// Scatter ITEM_COUNT items across the grid, one to a cell, and never
// in the player's cell
void seedWorld(Game& game)
{
    game.world.clear();
    while (game.world.size() < ITEM_COUNT)
    {
        Cell cell = { SDL_rand(GRID_COLS), SDL_rand(GRID_ROWS) };
        if (cell != game.player && !game.world.contains(cell))
            game.world[cell] = randomItem();
    }
}

// Start again: the player in the middle, nothing found, and a new
// world of loot
void resetGame(Game& game)
{
    game.player = { GRID_COLS / 2, GRID_ROWS / 2 };
    game.inventory.clear();
    seedWorld(game);
    SDL_Log("A new world, with %d items to find", ITEM_COUNT);
}

// If there's loot in the player's cell, move it into the inventory
void pickUp(Game& game)
{
    auto it = game.world.find(game.player);
    if (it == game.world.end())
        return;   // nothing here

    ItemType type = it->second;
    game.world.erase(it);
    game.inventory[type]++;

    SDL_Log("Picked up a %s (%d so far)", ITEMS.at(type).name,
            game.inventory[type]);
    if (game.world.empty())
        SDL_Log("That's all of it! Press R for a new world.");
}

// Move the player one cell across and down, unless that's off the grid
void movePlayer(Game& game, int across, int down)
{
    int col = game.player.first + across;
    int row = game.player.second + down;
    if (col < 0 || col >= GRID_COLS || row < 0 || row >= GRID_ROWS)
        return;   // off the edge, so stay put

    game.player = { col, row };
    pickUp(game);
}

// List the inventory in the console, and how much loot is left
void logInventory(const Game& game)
{
    SDL_Log("--- Inventory ---");
    if (game.inventory.empty())
        SDL_Log("(nothing yet)");
    for (const auto& [type, count] : game.inventory)
        SDL_Log("%-7s x %d", ITEMS.at(type).name, count);
    SDL_Log("Left to find: %d", static_cast<int>(game.world.size()));
}

// Draw the lines between the cells
void drawGrid(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, GRID_LINES.r, GRID_LINES.g,
                           GRID_LINES.b, GRID_LINES.a);
    for (int col = 1; col < GRID_COLS; col++)
    {
        float x = static_cast<float>(col * CELL_SIZE);
        SDL_RenderLine(renderer, x, 0.0f, x, static_cast<float>(WINDOW_H));
    }
    for (int row = 1; row < GRID_ROWS; row++)
    {
        float y = static_cast<float>(row * CELL_SIZE);
        SDL_RenderLine(renderer, 0.0f, y, static_cast<float>(WINDOW_W), y);
    }
}

// Fill a square in a cell, inset from the cell's edges, in a color
void drawSquare(SDL_Renderer* renderer, Cell cell, float inset,
                SDL_Color color)
{
    SDL_FRect rect = {
        cell.first * CELL_SIZE + inset,     // left
        cell.second * CELL_SIZE + inset,    // top
        CELL_SIZE - 2.0f * inset,           // width
        CELL_SIZE - 2.0f * inset            // height
    };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

// Draw every item in the world, and then the player on top
void drawGame(SDL_Renderer* renderer, const Game& game)
{
    for (const auto& [cell, type] : game.world)
        drawSquare(renderer, cell, ITEM_INSET, ITEMS.at(type).color);
    drawSquare(renderer, game.player, PLAYER_INSET, PLAYER_COLOR);
}

int main(int argc, char* argv[])
{
    // Start SDL, then make the window and the renderer
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Loot Grid",
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

    // The player, the loot, and the inventory, ready to play
    Game game;
    resetGame(game);

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
                case SDLK_W:
                    movePlayer(game, 0, -1);
                    break;
                case SDLK_A:
                    movePlayer(game, -1, 0);
                    break;
                case SDLK_S:
                    movePlayer(game, 0, 1);
                    break;
                case SDLK_D:
                    movePlayer(game, 1, 0);
                    break;
                case SDLK_TAB:
                    logInventory(game);
                    break;
                case SDLK_R:
                    resetGame(game);
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }
            }
        }

        // Draw the frame: the background, the grid, and then the game
        SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g,
                               BACKGROUND.b, BACKGROUND.a);
        SDL_RenderClear(renderer);
        drawGrid(renderer);
        drawGame(renderer, game);

        SDL_RenderPresent(renderer);
    }

    // Clean up, in the reverse order we created things
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
