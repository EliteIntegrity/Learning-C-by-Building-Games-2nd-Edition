#pragma once

// GameObject.h includes this file, so this file can't include it back. To
// take a reference to a GameObject, it's enough to know that it's a class
class GameObject;

// What a runner does at the window's sides
enum class Edges
{
    Stop,   // she stops at them
    Wrap,   // off one side, and back on at the other
    Free    // she pays them no attention
};

// How a runner moves: she runs at her own pace, jumps, falls, and lands, and
// keeps to her rule at the window's sides
class PhysicsComponent
{
public:
    PhysicsComponent(float groundY, float windowWidth, Edges edges,
                     float pace = 1.0f);

    void run(int direction);
    void jump();
    int getDirection() const;
    bool isJumping() const;
    float getPace() const;

    void update(GameObject& owner, float delta);

private:
    void keepToEdges(GameObject& owner) const;

    float groundY_;             // the line her feet land on
    float windowWidth_;
    Edges edges_;               // her rule at the window's sides
    float pace_;                // 1 for the usual speed, 2 for twice as fast
    float velY_ = 0.0f;         // pixels per second, and negative is up
    int direction_ = 0;         // -1 for left, 1 for right, 0 for standing
    bool jumping_ = false;
};
