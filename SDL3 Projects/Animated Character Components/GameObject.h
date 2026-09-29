#pragma once
#include <memory>   // std::unique_ptr, for the input
#include <SDL3/SDL.h>
#include "IDrawable.h"
#include "IUpdatable.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "GraphicsComponent.h"

// Anything in the game, made of three components: an input that decides
// where it goes, a physics that moves it, and a graphics that draws it. On
// its own, a game object is only a place
class GameObject : public IUpdatable, public IDrawable
{
public:
    GameObject(float startX, float startY,
               std::unique_ptr<InputComponent> input,
               const PhysicsComponent& physics,
               const GraphicsComponent& graphics);

    void setInput(std::unique_ptr<InputComponent> input);
    PhysicsComponent& getPhysics();
    const PhysicsComponent& getPhysics() const;

    void update(float delta) override;
    void draw(SDL_Renderer* renderer) const override;

    // Where it is, for every component to see: for a runner, the middle of
    // her body, across the window, and her feet, down it
    float x;
    float y;

private:
    std::unique_ptr<InputComponent> input_;
    PhysicsComponent physics_;
    GraphicsComponent graphics_;
};
