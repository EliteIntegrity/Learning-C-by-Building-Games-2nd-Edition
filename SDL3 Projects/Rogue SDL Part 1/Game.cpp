#include "Game.h"
#include <string>   // std::string, for the font's path

// The font every character is drawn in, and its size
const std::string FONT_PATH = "assets/RobotoMono-Light.ttf";
constexpr float FONT_SIZE = 18.0f;

Game::Game(SDL_Renderer* renderer)
    : renderer_(renderer), glyphs_(renderer, FONT_PATH, FONT_SIZE)
{
    makeTestRoom();
}

bool Game::isLoaded() const
{
    return glyphs_.isLoaded();
}

// Sleeps until something happens, deals with it, and draws the window again
// if anything changed
void Game::run()
{
    while (running_)
    {
        if (dirty_)
        {
            draw();
            dirty_ = false;
        }

        SDL_Event event;
        if (!SDL_WaitEvent(&event))
            break;

        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            running_ = false;
            break;
        case SDL_EVENT_WINDOW_EXPOSED:
            dirty_ = true;
            break;
        case SDL_EVENT_KEY_DOWN:
            handleKey(event.key.key);
            dirty_ = true;
            break;
        }
    }
}

// A room to try the game out in, for now: floor, with a wall all around it
// and two pillars inside
void Game::makeTestRoom()
{
    map_.fill(Terrain::Wall);
    for (int y = 12; y < 33; ++y)
    {
        for (int x = 20; x < 60; ++x)
            map_.at({ x, y }).terrain = Terrain::Floor;
    }
    map_.at({ 30, 18 }).terrain = Terrain::Wall;
    map_.at({ 49, 26 }).terrain = Terrain::Wall;
    player_.setPosition({ 40, 22 });
}

// Every key press is one action, or none
void Game::handleKey(SDL_Keycode key)
{
    int dx = 0;
    int dy = 0;
    switch (key)
    {
    case SDLK_UP:
    case SDLK_W:
        dy = -1;
        break;
    case SDLK_DOWN:
    case SDLK_S:
        dy = 1;
        break;
    case SDLK_LEFT:
    case SDLK_A:
        dx = -1;
        break;
    case SDLK_RIGHT:
    case SDLK_D:
        dx = 1;
        break;
    case SDLK_ESCAPE:
        running_ = false;
        return;
    default:
        return;
    }

    player_.tryMove(dx, dy, map_);
}

void Game::draw() const
{
    SDL_SetRenderDrawColor(renderer_, Palette::BACKGROUND.r,
                           Palette::BACKGROUND.g, Palette::BACKGROUND.b, 255);
    SDL_RenderClear(renderer_);

    map_.draw(renderer_, glyphs_);
    player_.draw(renderer_, glyphs_);

    SDL_RenderPresent(renderer_);
}
