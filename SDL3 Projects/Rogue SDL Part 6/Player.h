#pragma once
#include <vector>   // std::vector, for the inventory
#include "Entity.h"
#include "Item.h"

class Map;

constexpr int PLAYER_MAX_HP = 20;   // the player's health, when it's full
constexpr int PLAYER_ATTACK = 4;    // the damage the player does with a hit
constexpr int POTION_HEAL = 8;      // the health a potion gives back
constexpr int INVENTORY_SIZE = 12;  // slots, one for each letter a to l

// You: the @
class Player : public Entity
{
public:
    Player();

    bool tryMove(int dx, int dy, const Map& map);

    int getHp() const;
    void setHp(int hp);
    int getAttack() const;
    int getGold() const;
    void setGold(int gold);
    const std::vector<ItemKind>& getInventory() const;
    bool isAlive() const;

    void takeDamage(int amount);
    void heal(int amount);
    void addGold(int amount);
    bool addToInventory(ItemKind kind);
    void removeFromInventory(int slot);
    void reset();

private:
    int hp_ = PLAYER_MAX_HP;
    int gold_ = 0;
    std::vector<ItemKind> inventory_;   // everything carried, but gold
};
