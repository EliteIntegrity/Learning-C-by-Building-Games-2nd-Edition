#include "GameObject.h"
#include <utility>   // std::move

// The input arrives in a unique_ptr, and moves into input_, so the game
// object owns it. The other two components are copied into members
GameObject::GameObject(float startX, float startY,
                       std::unique_ptr<InputComponent> input,
                       const PhysicsComponent& physics,
                       const GraphicsComponent& graphics)
    : x(startX),
      y(startY),
      input_(std::move(input)),
      physics_(physics),
      graphics_(graphics)
{
}

// Swap one input for another while the game runs. The old one is destroyed
// here, by the unique_ptr that owned it
void GameObject::setInput(std::unique_ptr<InputComponent> input)
{
    input_ = std::move(input);
}

PhysicsComponent& GameObject::getPhysics()
{
    return physics_;
}

const PhysicsComponent& GameObject::getPhysics() const
{
    return physics_;
}

// Every frame, in the same order: decide, move, and look the part
void GameObject::update(float delta)
{
    input_->update(*this, delta);
    physics_.update(*this, delta);
    graphics_.update(*this, delta);
}

void GameObject::draw(SDL_Renderer* renderer) const
{
    graphics_.draw(renderer, *this);
}
