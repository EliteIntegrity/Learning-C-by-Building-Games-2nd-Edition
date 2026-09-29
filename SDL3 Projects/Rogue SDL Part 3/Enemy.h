#pragma once
#include "Entity.h"

// The kinds of monster, from the weakest to the strongest
enum class MonsterKind
{
    Rat,
    Goblin,
    Orc
};

// What every monster of one kind is like
struct MonsterStats
{
    const char* name;
    char glyph;
    SDL_Color color;
    int maxHp;
    int attack;   // the damage it does with each hit
    int sight;    // how far away it can notice the player, in cells
};

const MonsterStats& statsOf(MonsterKind kind);

// A monster, of one of the kinds above
class Enemy : public Entity
{
public:
    Enemy(MonsterKind kind, Point position);

    const MonsterStats& getStats() const;
    int getHp() const;
    bool isAlive() const;
    void takeDamage(int amount);

private:
    MonsterKind kind_;
    int hp_;
};
