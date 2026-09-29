#pragma once
#include "Entity.h"

// The kinds of monster, from the weakest to the strongest
enum class MonsterKind
{
    Rat,
    Goblin,
    Orc
};

constexpr int MONSTER_KINDS = 3;   // how many kinds there are

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

    MonsterKind getKind() const;
    const MonsterStats& getStats() const;
    int getHp() const;
    void setHp(int hp);
    bool isAlive() const;
    void takeDamage(int amount);

    bool isHunting() const;
    Point getLastSeen() const;
    void hunt(Point lastSeen);
    void giveUp();

private:
    MonsterKind kind_;
    int hp_;
    bool hunting_ = false;   // true once it has seen the player
    Point lastSeen_;         // where it saw the player last
};
