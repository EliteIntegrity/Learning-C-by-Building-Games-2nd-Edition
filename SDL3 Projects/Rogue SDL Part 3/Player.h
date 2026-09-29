#pragma once
#include "Entity.h"

class Map;

constexpr int PLAYER_MAX_HP = 20;   // the player's health, when it's full
constexpr int PLAYER_ATTACK = 4;    // the damage the player does with a hit
constexpr int POTION_HEAL = 8;      // the health a potion gives back

// You: the @
class Player : public Entity
{
public:
    Player();

    bool tryMove(int dx, int dy, const Map& map);

    int getHp() const;
    int getAttack() const;
    int getGold() const;
    int getPotions() const;
    bool isAlive() const;

    void takeDamage(int amount);
    void addGold(int amount);
    void addPotion();
    bool drinkPotion();
    void reset();

private:
    int hp_ = PLAYER_MAX_HP;
    int gold_ = 0;
    int potions_ = 0;
};
