#pragma once
#include <SDL3/SDL.h>
#include <vector>   // std::vector, for the monsters and the treasure
#include "Enemy.h"
#include "GlyphCache.h"
#include "HUD.h"
#include "Item.h"
#include "Map.h"
#include "Player.h"

// The whole game. It owns the glyphs, the map, the player, the monsters,
// the treasure, and the HUD, and runs the loop that waits for a key, acts
// on it, and draws what happened
class Game
{
public:
    Game(SDL_Renderer* renderer);

    bool isLoaded() const;
    void run();

private:
    void newGame();
    void newLevel();
    void handleKey(SDL_Keycode key);
    void moveOrAttack(int dx, int dy);
    void attack(Enemy& enemy);
    void pickUp();
    void drinkPotion();
    void takeStairs();
    void saveGame();
    void loadGame();
    void endTurn();
    void monsterTurn(Enemy& enemy);
    bool notices(const Enemy& enemy) const;
    Enemy* enemyAt(Point cell);
    Item* itemAt(Point cell);
    void draw() const;

    SDL_Renderer* renderer_;
    GlyphCache glyphs_;
    Map map_;
    Player player_;
    std::vector<Enemy> enemies_;
    std::vector<Item> items_;
    HUD hud_;
    int depth_ = 1;          // how many levels down the player is
    bool gameOver_ = false;  // true once the player has died
    bool running_ = true;
    bool dirty_ = true;      // true when the window needs drawing again
};
