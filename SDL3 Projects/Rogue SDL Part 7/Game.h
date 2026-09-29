#pragma once
#include <SDL3/SDL.h>
#include <memory>   // std::unique_ptr, for the states
#include <vector>   // std::vector, for the monsters, treasure, and states
#include "Enemy.h"
#include "FlashEffects.h"
#include "GameState.h"
#include "GlyphCache.h"
#include "HUD.h"
#include "Item.h"
#include "Map.h"
#include "Player.h"
#include "Sound.h"

// The whole game. It owns the glyphs, the map, the player, the monsters,
// the treasure, the HUD, the sounds, and the flashes, and runs the loop
// that waits for a key, and hands it to the state on top of the stack
class Game
{
public:
    Game(SDL_Renderer* renderer);

    bool isLoaded() const;
    void run();

    // What the states can look at
    const GlyphCache& getGlyphs() const;
    const Map& getMap() const;
    const Player& getPlayer() const;
    bool isGameOver() const;
    bool canBurn(Point cell, Point center) const;

    // What the states can ask the game to do
    void newGame();
    void quit();
    void moveOrAttack(int dx, int dy);
    void useItem(int slot);
    bool castFireball(int slot, Point target);
    void takeStairs();
    void saveGame();
    void loadGame();

private:
    void newLevel();
    void handleKey(SDL_Keycode key);
    void attack(Enemy& enemy);
    void burn(Enemy& enemy);
    void pickUp();
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
    Sounds sounds_;
    FlashEffects flashes_;
    std::vector<std::unique_ptr<GameState>> states_;   // the top is last
    int depth_ = 1;          // how many levels down the player is
    bool gameOver_ = false;  // true once the player has died
    bool running_ = true;
    bool dirty_ = true;      // true when the window needs drawing again
};
