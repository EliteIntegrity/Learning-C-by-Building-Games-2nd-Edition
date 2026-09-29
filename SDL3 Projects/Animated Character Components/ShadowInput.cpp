#include "ShadowInput.h"
#include "GameObject.h"

const float GAP = 90.0f;   // how far behind her leader she keeps

ShadowInput::ShadowInput(const GameObject& leader) : leader_(leader)
{
}

void ShadowInput::update(GameObject& owner, float)
{
    // Run after her leader until she's close behind, then wait
    float distance = leader_.x - owner.x;
    if (distance > GAP)
        owner.getPhysics().run(1);
    else if (distance < -GAP)
        owner.getPhysics().run(-1);
    else
        owner.getPhysics().run(0);

    // Jump when she jumps
    if (leader_.getPhysics().isJumping())
        owner.getPhysics().jump();
}
