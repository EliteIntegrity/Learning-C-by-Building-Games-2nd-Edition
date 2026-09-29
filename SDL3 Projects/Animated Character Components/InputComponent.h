#pragma once

// GameObject.h includes this file, so this file can't include it back. To
// take a reference to a GameObject, it's enough to know that it's a class
class GameObject;

// Whatever decides where a game object goes: the player at the keyboard, a
// ghost's own whims, or a shadow following her leader
class InputComponent
{
public:
    virtual ~InputComponent() = default;

    virtual void update(GameObject& owner, float delta) = 0;
};
