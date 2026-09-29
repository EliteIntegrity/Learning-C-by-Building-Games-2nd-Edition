#include "Game.h"
#include <string>   // std::string and std::to_string
#include "FOV.h"
#include "MapGenerator.h"

// The font every character is drawn in, and its size
const std::string FONT_PATH = "assets/RobotoMono-Light.ttf";
constexpr float FONT_SIZE = 18.0f;

Game::Game(SDL_Renderer* renderer)
    : renderer_(renderer), glyphs_(renderer, FONT_PATH, FONT_SIZE)
{
    newLevel();
    hud_.addMessage("Welcome to Rogue SDL. Find the stairs down: >");
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

// Builds a new level, puts the player at its start, and looks around
void Game::newLevel()
{
    MapGenerator generator(map_);
    player_.setPosition(generator.generate());
    FOV::compute(map_, player_.getPosition(), SIGHT_RADIUS);
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
    case SDLK_PERIOD:
        takeStairs();
        return;
    case SDLK_ESCAPE:
        running_ = false;
        return;
    default:
        return;
    }

    if (player_.tryMove(dx, dy, map_))
    {
        FOV::compute(map_, player_.getPosition(), SIGHT_RADIUS);
        if (map_.at(player_.getPosition()).terrain == Terrain::StairsDown)
            hud_.addMessage("There are stairs down here. Press . to go down.");
    }
}

// Goes down to a new level, if the player is standing on the stairs
void Game::takeStairs()
{
    if (map_.at(player_.getPosition()).terrain != Terrain::StairsDown)
    {
        hud_.addMessage("There are no stairs here.");
        return;
    }

    ++depth_;
    newLevel();
    hud_.addMessage("You go down the stairs to depth " +
                    std::to_string(depth_) + ".");
}

void Game::draw() const
{
    SDL_SetRenderDrawColor(renderer_, Palette::BACKGROUND.r,
                           Palette::BACKGROUND.g, Palette::BACKGROUND.b, 255);
    SDL_RenderClear(renderer_);

    map_.draw(renderer_, glyphs_);
    player_.draw(renderer_, glyphs_);
    hud_.draw(renderer_, glyphs_, depth_);

    SDL_RenderPresent(renderer_);
}
