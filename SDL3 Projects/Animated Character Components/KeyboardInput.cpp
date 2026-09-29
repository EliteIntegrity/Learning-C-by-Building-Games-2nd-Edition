#include "KeyboardInput.h"
#include <SDL3/SDL.h>
#include "GameObject.h"

// The keyboard doesn't care how long the frame was, so the second parameter
// has no name
void KeyboardInput::update(GameObject& owner, float)
{
    // Run left or right while an arrow key, or A or D, is held
    const bool* keys = SDL_GetKeyboardState(nullptr);
    int direction = 0;
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
        direction -= 1;
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
        direction += 1;
    owner.getPhysics().run(direction);

    // Jump as a jump key goes down: down now, but not in the last frame
    bool jumpDown = keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_W] ||
                    keys[SDL_SCANCODE_UP];
    if (jumpDown && !jumpWasDown_)
        owner.getPhysics().jump();
    jumpWasDown_ = jumpDown;
}
