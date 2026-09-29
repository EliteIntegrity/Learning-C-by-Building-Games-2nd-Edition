#pragma once
#include <SDL3/SDL.h>
#include "Animator.h"
#include "Texture.h"

// GameObject.h includes this file, so this file can't include it back. To
// take a reference to a GameObject, it's enough to know that it's a class
class GameObject;

// How a runner looks: a frame from the runner's sprite sheet, chosen by what
// she's doing, turned to face the way she's going, and tinted
class GraphicsComponent
{
public:
    GraphicsComponent(const Texture& sheet,
                      SDL_Color tint = { 255, 255, 255, 255 });

    void update(const GameObject& owner, float delta);
    void draw(SDL_Renderer* renderer, const GameObject& owner) const;

private:
    const Texture& sheet_;      // the runner's sprite sheet, used but not owned
    Animator animator_;         // counts through her running frames
    SDL_Color tint_;            // her color, and how see-through she is
    bool facingLeft_ = false;
};
