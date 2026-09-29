/*
    Vibe Snake
    The Chapter 26 project from Learning C++ by Building Games

    Classic Snake. Steer with the arrow keys, or W, A, S, and D, eat the
    red apples to grow longer, and don't run into the walls or into
    yourself. Every apple scores a point, and makes the snake a little
    faster. When you crash, press R to play again, or Escape to quit.

    This is the version I ended up with after six rounds of describing,
    reading, running, and editing with an AI. Yours will be different,
    and that's the whole point.
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <string>   // std::string and std::to_string, for the title bar
#include <vector>   // std::vector, for the snake

// The grid, and the window it fills
constexpr int CELL_PX  = 20;                 // a cell's size, in pixels
constexpr int GRID_W   = 40;                 // cells across
constexpr int GRID_H   = 30;                 // cells down
constexpr int WINDOW_W = CELL_PX * GRID_W;   // 800 pixels
constexpr int WINDOW_H = CELL_PX * GRID_H;   // 600 pixels

// Milliseconds between moves: each apple takes 5 off, down to the minimum
constexpr Uint64 START_TICK_MS = 125;
constexpr Uint64 MIN_TICK_MS   = 60;

// The colors
constexpr SDL_Color BACKGROUND  = { 16, 16, 20, 255 };    // almost black
constexpr SDL_Color GRID_LINES  = { 28, 28, 36, 255 };    // a little lighter
constexpr SDL_Color APPLE_COLOR = { 230, 60, 60, 255 };   // red
constexpr SDL_Color HEAD_COLOR  = { 120, 230, 120, 255 }; // bright green
constexpr SDL_Color BODY_COLOR  = { 80, 180, 90, 255 };   // darker green
constexpr SDL_Color DIM_COLOR   = { 0, 0, 0, 160 };       // see-through black

// A cell on the grid, or a step from one cell to the next
struct Cell
{
    int x;   // the column, counting from 0 at the left
    int y;   // the row, counting from 0 at the top
};

// Are a and b the same cell?
bool sameCell(Cell a, Cell b)
{
    return a.x == b.x && a.y == b.y;
}

// The rectangle a cell fills in the window, pulled in by inset pixels on
// every side, so that neighbors don't touch
SDL_FRect cellRect(Cell cell, float inset)
{
    float size = static_cast<float>(CELL_PX);
    return { cell.x * size + inset, cell.y * size + inset,
             size - 2.0f * inset, size - 2.0f * inset };
}

// Everything that changes while you play
struct Game
{
    std::vector<Cell> snake;         // the head is snake[0]
    Cell dir;                        // the direction the head is moving
    Cell pendingDir;                 // the direction the last key asked for
    Cell apple;
    int score = 0;
    bool gameOver = false;
    Uint64 lastMove = 0;             // when the snake last moved
    Uint64 tickMs = START_TICK_MS;   // milliseconds between moves
};

// A random cell that the snake isn't in, for the next apple
Cell randomEmptyCell(const std::vector<Cell>& occupied)
{
    for (int tries = 0; tries < 1000; tries++)
    {
        Cell cell = { SDL_rand(GRID_W), SDL_rand(GRID_H) };
        bool occupiedHere = false;
        for (Cell segment : occupied)
        {
            if (sameCell(segment, cell))
            {
                occupiedHere = true;
                break;
            }
        }
        if (!occupiedHere)
            return cell;
    }
    return Cell{ 0, 0 };   // 1000 misses: the grid is as good as full
}

// Start a new game: a snake three cells long in the middle, heading right
void resetGame(Game& game)
{
    int midX = GRID_W / 2;
    int midY = GRID_H / 2;
    game.snake.clear();
    game.snake.push_back({ midX, midY });       // the head
    game.snake.push_back({ midX - 1, midY });
    game.snake.push_back({ midX - 2, midY });

    game.dir = { 1, 0 };                        // moving right
    game.pendingDir = game.dir;
    game.apple = randomEmptyCell(game.snake);
    game.score = 0;
    game.gameOver = false;
    game.lastMove = SDL_GetTicks();
    game.tickMs = START_TICK_MS;
}

// Turn at the next move, unless that would reverse the snake into itself
void queueDir(Game& game, int dx, int dy)
{
    if (dx == -game.dir.x && dy == -game.dir.y)
        return;
    game.pendingDir = { dx, dy };
}

// Show the score in the window's title bar
void showScore(SDL_Window* window, int score)
{
    std::string title = "Vibe Snake - Score: " + std::to_string(score);
    SDL_SetWindowTitle(window, title.c_str());
}

// Move the snake one cell. It grows when it eats the apple, and the game
// is over when it hits a wall or itself.
void stepSnake(Game& game, SDL_Window* window)
{
    if (game.gameOver)
        return;

    game.dir = game.pendingDir;

    Cell head = game.snake.front();
    Cell newHead = { head.x + game.dir.x, head.y + game.dir.y };

    // Off the grid is into a wall
    if (newHead.x < 0 || newHead.x >= GRID_W ||
        newHead.y < 0 || newHead.y >= GRID_H)
    {
        game.gameOver = true;
        return;
    }

    // Into the snake is a crash too, except for the tail, which is leaving
    for (size_t i = 0; i + 1 < game.snake.size(); i++)
    {
        if (sameCell(game.snake[i], newHead))
        {
            game.gameOver = true;
            return;
        }
    }

    game.snake.insert(game.snake.begin(), newHead);

    if (sameCell(newHead, game.apple))
    {
        // Keep the tail, so the snake grows, and speed up a little
        game.score++;
        if (game.tickMs > MIN_TICK_MS)
            game.tickMs -= 5;
        game.apple = randomEmptyCell(game.snake);
        showScore(window, game.score);
    }
    else
    {
        game.snake.pop_back();   // the tail follows the head
    }
}

// Draw one frame: the background, the grid, the apple, and the snake, all
// dimmed once the game is over
void render(SDL_Renderer* renderer, const Game& game)
{
    SDL_SetRenderDrawColor(renderer, BACKGROUND.r, BACKGROUND.g,
                           BACKGROUND.b, BACKGROUND.a);
    SDL_RenderClear(renderer);

    // Faint grid lines, down every column's edge and across every row's
    SDL_SetRenderDrawColor(renderer, GRID_LINES.r, GRID_LINES.g,
                           GRID_LINES.b, GRID_LINES.a);
    for (int col = 0; col <= GRID_W; col++)
    {
        float x = static_cast<float>(col * CELL_PX);
        SDL_RenderLine(renderer, x, 0.0f, x, static_cast<float>(WINDOW_H));
    }
    for (int row = 0; row <= GRID_H; row++)
    {
        float y = static_cast<float>(row * CELL_PX);
        SDL_RenderLine(renderer, 0.0f, y, static_cast<float>(WINDOW_W), y);
    }

    // The apple, 2 pixels in from its cell's edges
    SDL_SetRenderDrawColor(renderer, APPLE_COLOR.r, APPLE_COLOR.g,
                           APPLE_COLOR.b, APPLE_COLOR.a);
    SDL_FRect appleRect = cellRect(game.apple, 2.0f);
    SDL_RenderFillRect(renderer, &appleRect);

    // The snake, 1 pixel in, with its head brighter than its body
    for (size_t i = 0; i < game.snake.size(); i++)
    {
        SDL_Color color = (i == 0) ? HEAD_COLOR : BODY_COLOR;
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_FRect segmentRect = cellRect(game.snake[i], 1.0f);
        SDL_RenderFillRect(renderer, &segmentRect);
    }

    // Game over: cover everything with see-through black
    if (game.gameOver)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, DIM_COLOR.r, DIM_COLOR.g,
                               DIM_COLOR.b, DIM_COLOR.a);
        SDL_FRect everything = { 0.0f, 0.0f, static_cast<float>(WINDOW_W),
                                 static_cast<float>(WINDOW_H) };
        SDL_RenderFillRect(renderer, &everything);
    }

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

    SDL_Window* window = SDL_CreateWindow("Vibe Snake",
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

    Game game;
    resetGame(game);
    showScore(window, game.score);

    bool running = true;
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
                switch (event.key.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    running = false;
                    break;
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    queueDir(game, 0, -1);
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    queueDir(game, 0, 1);
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    queueDir(game, -1, 0);
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    queueDir(game, 1, 0);
                    break;
                case SDL_SCANCODE_R:
                    if (game.gameOver)
                    {
                        resetGame(game);
                        showScore(window, game.score);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        // Move the snake whenever its time between moves is up
        Uint64 now = SDL_GetTicks();
        if (now - game.lastMove >= game.tickMs)
        {
            stepSnake(game, window);
            game.lastMove = now;
        }

        render(renderer, game);
        SDL_Delay(8);   // rest a moment, rather than loop flat out
    }

    // Clean up, in the reverse order we created things
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
