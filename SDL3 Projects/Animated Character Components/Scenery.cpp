#include "Scenery.h"

// Each picture is loaded as its member is made, so the scenery owns all four
Scenery::Scenery(SDL_Renderer* renderer)
    : sky_(renderer, "assets/sky.png"),
      farHills_(renderer, "assets/hills_far.png"),
      nearHills_(renderer, "assets/hills_near.png"),
      ground_(renderer, "assets/ground.png")
{
}

bool Scenery::isLoaded() const
{
    return sky_.isLoaded() && farHills_.isLoaded() &&
           nearHills_.isLoaded() && ground_.isLoaded();
}

void Scenery::draw(SDL_Renderer* renderer) const
{
    sky_.draw(renderer, nullptr, nullptr);
    farHills_.draw(renderer, nullptr, nullptr);
    nearHills_.draw(renderer, nullptr, nullptr);
    ground_.draw(renderer, nullptr, nullptr);
}
