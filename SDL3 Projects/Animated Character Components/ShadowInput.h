#pragma once
#include "InputComponent.h"

// A shadow's input: she follows her leader wherever she goes, and jumps
// whenever she jumps
class ShadowInput : public InputComponent
{
public:
    ShadowInput(const GameObject& leader);

    void update(GameObject& owner, float delta) override;

private:
    const GameObject& leader_;  // the one she follows, used but not owned
};
