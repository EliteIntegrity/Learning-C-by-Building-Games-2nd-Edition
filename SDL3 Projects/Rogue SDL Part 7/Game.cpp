#include "Game.h"
#include <cstdlib>   // std::abs
#include <string>   // std::string and std::to_string
#include <utility>   // std::move
#include "AStar.h"
#include "FOV.h"
#include "MapGenerator.h"
#include "PlayingState.h"
#include "SaveLoad.h"

// The font every character is drawn in, and its size
const std::string FONT_PATH = "assets/RobotoMono-Light.ttf";
constexpr float FONT_SIZE = 18.0f;

// A frame, in milliseconds, at 60 frames a second
constexpr Sint32 FRAME_MS = 16;

// The save file. It goes in the working directory, which is the project
// folder when the game runs from Visual Studio
const std::string SAVE_PATH = "rogue_save.txt";

// The game starts with one state on its stack: playing
Game::Game(SDL_Renderer* renderer)
    : renderer_(renderer), glyphs_(renderer, FONT_PATH, FONT_SIZE)
{
    states_.push_back(std::make_unique<PlayingState>());
    newGame();
}

bool Game::isLoaded() const
{
    return glyphs_.isLoaded();
}

const GlyphCache& Game::getGlyphs() const
{
    return glyphs_;
}

const Map& Game::getMap() const
{
    return map_;
}

const Player& Game::getPlayer() const
{
    return player_;
}

bool Game::isGameOver() const
{
    return gameOver_;
}

// Whether a fireball bursting on one cell would burn another: it must be
// no more than FIREBALL_RADIUS steps away, open ground, and in sight
bool Game::canBurn(Point cell, Point center) const
{
    int steps = std::abs(cell.x - center.x) + std::abs(cell.y - center.y);
    return steps <= FIREBALL_RADIUS && !map_.isBlocked(cell) &&
           map_.at(cell).visible;
}

void Game::quit()
{
    running_ = false;
}

// Sleeps until something happens, deals with it, and draws the window again
// if anything changed. While anything is flashing, it wakes up every frame
// as well, to draw the flash fading
void Game::run()
{
    while (running_)
    {
        if (dirty_)
        {
            draw();
            dirty_ = false;
        }

        SDL_Event event;
        if (flashes_.isEmpty())
        {
            if (!SDL_WaitEvent(&event))
                break;
        }
        else
        {
            // Wait for an event, but for no longer than one frame
            bool gotEvent = SDL_WaitEventTimeout(&event, FRAME_MS);
            flashes_.removeFinished();
            dirty_ = true;
            if (!gotEvent)
                continue;
        }

        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            running_ = false;
            break;
        case SDL_EVENT_WINDOW_EXPOSED:
            dirty_ = true;
            break;
        case SDL_EVENT_KEY_DOWN:
            handleKey(event.key.key);
            dirty_ = true;
            break;
        }
    }
}

// Starts again from the top, with a fresh player at depth 1
void Game::newGame()
{
    depth_ = 1;
    gameOver_ = false;
    player_.reset();
    hud_.clear();
    newLevel();
    hud_.addMessage("Welcome to Rogue SDL. Find the stairs down: >");
}

// Builds a new level, with its monsters and treasure, puts the player at
// its start, and looks around
void Game::newLevel()
{
    enemies_.clear();
    items_.clear();
    MapGenerator generator(map_);
    player_.setPosition(generator.generate(depth_, enemies_, items_));
    FOV::compute(map_, player_.getPosition(), SIGHT_RADIUS);
}

// Hands a key to the state on top of the stack, and then changes the stack
// as it asks. A state is only ever closed here, after it has finished with
// the key, and never while it's still dealing with it
void Game::handleKey(SDL_Keycode key)
{
    StateChange change = states_.back()->handleKey(*this, key);
    if (change.close)
        states_.pop_back();
    if (change.open)
        states_.push_back(std::move(change.open));
}

// Attacks the monster in the way, if there is one. Otherwise, steps, looks
// around, and picks up anything lying there. Either way, it's a turn
void Game::moveOrAttack(int dx, int dy)
{
    Point next = { player_.getPosition().x + dx,
                   player_.getPosition().y + dy };
    if (Enemy* enemy = enemyAt(next))
    {
        attack(*enemy);
        endTurn();
        return;
    }

    if (player_.tryMove(dx, dy, map_))
    {
        FOV::compute(map_, player_.getPosition(), SIGHT_RADIUS);
        if (map_.at(player_.getPosition()).terrain == Terrain::StairsDown)
            hud_.addMessage("There are stairs down here. Press . to go down.");
        pickUp();
        endTurn();
    }
}

// The player hits a monster, which may die
void Game::attack(Enemy& enemy)
{
    int damage = player_.getAttack();
    enemy.takeDamage(damage);
    flashes_.add(enemy.getPosition(), Palette::FLASH_HIT);

    std::string name = enemy.getStats().name;
    if (enemy.isAlive())
    {
        sounds_.hit.play();
        hud_.addMessage("You hit the " + name + " for " +
                        std::to_string(damage) + ".");
    }
    else
    {
        sounds_.kill.play();
        hud_.addMessage("You kill the " + name + "!");
    }
}

// Picks up anything lying where the player stands
void Game::pickUp()
{
    Point here = player_.getPosition();
    Item* item = itemAt(here);
    if (!item)
        return;

    std::string name = statsOf(item->getKind()).name;
    if (item->getKind() == ItemKind::Gold)
    {
        player_.addGold(item->getAmount());
        hud_.addMessage("You pick up " + std::to_string(item->getAmount()) +
                        " gold.");
    }
    else if (player_.addToInventory(item->getKind()))
    {
        hud_.addMessage("You pick up a " + name + ".");
    }
    else
    {
        hud_.addMessage("Your inventory is full, so you leave the " + name +
                        ".");
        return;
    }
    sounds_.pickup.play();

    // It's the player's now, so it isn't lying on the floor anymore
    std::erase_if(items_, [here](const Item& lying)
    {
        return lying.getPosition() == here;
    });
}

// Uses the item in a slot of the inventory, which takes a turn
void Game::useItem(int slot)
{
    ItemKind kind = player_.getInventory()[slot];
    player_.removeFromInventory(slot);
    switch (kind)
    {
    case ItemKind::Potion:
        player_.heal(POTION_HEAL);
        sounds_.drink.play();
        flashes_.add(player_.getPosition(), Palette::FLASH_HEAL);
        hud_.addMessage("You drink the potion, and feel better.");
        break;
    case ItemKind::MagicMapping:
        map_.exploreAll();
        sounds_.scroll.play();
        hud_.addMessage("The scroll shows you the whole level.");
        break;
    case ItemKind::Dagger:
    case ItemKind::Sword:
    case ItemKind::Warhammer:
        player_.wield(kind);
        sounds_.pickup.play();
        hud_.addMessage("You wield the " + std::string(statsOf(kind).name) +
                        ".");
        break;
    default:
        break;
    }
    endTurn();
}

// Reads the scroll of fireball in a slot, and bursts the fireball on a
// cell, burning everything near it. Only a cell the player can see will
// do: for any other, it returns false, and nothing happens
bool Game::castFireball(int slot, Point target)
{
    if (!map_.at(target).visible)
    {
        hud_.addMessage("You can only aim at a place you can see.");
        return false;
    }

    player_.removeFromInventory(slot);
    sounds_.scroll.play();
    hud_.addMessage("The scroll bursts into a ball of fire!");
    for (int dy = -FIREBALL_RADIUS; dy <= FIREBALL_RADIUS; ++dy)
    {
        for (int dx = -FIREBALL_RADIUS; dx <= FIREBALL_RADIUS; ++dx)
        {
            Point cell = { target.x + dx, target.y + dy };
            if (!canBurn(cell, target))
                continue;

            flashes_.add(cell, Palette::FLASH_FIRE);
            if (Enemy* enemy = enemyAt(cell))
                burn(*enemy);
        }
    }
    endTurn();
    return true;
}

// A monster caught in a fireball
void Game::burn(Enemy& enemy)
{
    enemy.takeDamage(FIREBALL_DAMAGE);
    std::string name = enemy.getStats().name;
    if (enemy.isAlive())
    {
        hud_.addMessage("The fire burns the " + name + ".");
    }
    else
    {
        sounds_.kill.play();
        hud_.addMessage("The fire kills the " + name + "!");
    }
}

// Goes down to a new level, if the player is standing on the stairs
void Game::takeStairs()
{
    if (map_.at(player_.getPosition()).terrain != Terrain::StairsDown)
    {
        hud_.addMessage("There are no stairs here.");
        return;
    }

    ++depth_;
    newLevel();
    sounds_.stairs.play();
    hud_.addMessage("You go down the stairs to depth " +
                    std::to_string(depth_) + ".");
}

// Saves the game, and says whether it worked. Saving doesn't take a turn
void Game::saveGame()
{
    if (SaveLoad::save(SAVE_PATH, map_, player_, enemies_, items_, depth_))
        hud_.addMessage("Game saved.");
    else
        hud_.addMessage("The game couldn't be saved.");
}

// Loads the saved game in place of this one, if there's one to load
void Game::loadGame()
{
    if (!SaveLoad::load(SAVE_PATH, map_, player_, enemies_, items_, depth_))
    {
        hud_.addMessage("There's no saved game, or it couldn't be read.");
        return;
    }

    FOV::compute(map_, player_.getPosition(), SIGHT_RADIUS);
    hud_.addMessage("Game loaded.");
}

// The player has taken a turn, so now the monsters take theirs. The dead
// are cleared away first
void Game::endTurn()
{
    std::erase_if(enemies_, [](const Enemy& enemy)
    {
        return !enemy.isAlive();
    });

    for (Enemy& enemy : enemies_)
    {
        monsterTurn(enemy);
        if (!player_.isAlive())
        {
            gameOver_ = true;
            hud_.addMessage("You die.");
            return;
        }
    }
}

// A monster that sees the player hunts them. It attacks if it's next to
// them, and otherwise steps along the shortest path to where it saw them
// last, so that it can follow them around corners
void Game::monsterTurn(Enemy& enemy)
{
    if (notices(enemy))
        enemy.hunt(player_.getPosition());
    if (!enemy.isHunting())
        return;

    Point from = enemy.getPosition();
    Point to = player_.getPosition();
    if (std::abs(to.x - from.x) + std::abs(to.y - from.y) == 1)
    {
        int damage = enemy.getStats().attack;
        player_.takeDamage(damage);
        sounds_.hurt.play();
        flashes_.add(to, Palette::FLASH_HIT);
        hud_.addMessage("The " + std::string(enemy.getStats().name) +
                        " hits you for " + std::to_string(damage) + ".");
        return;
    }

    // An empty path means it's where it saw the player last, and they're
    // gone, or that there's no way there. Either way, it loses the trail
    std::vector<Point> path = AStar::findPath(map_, from, enemy.getLastSeen());
    if (path.empty())
    {
        enemy.giveUp();
        return;
    }

    // It waits, if another monster is in the way
    if (!enemyAt(path[0]))
        enemy.setPosition(path[0]);
}

// A monster notices the player when it stands where the player can see it,
// and the player is within its own sight
bool Game::notices(const Enemy& enemy) const
{
    Point from = enemy.getPosition();
    Point to = player_.getPosition();
    int dx = to.x - from.x;
    int dy = to.y - from.y;
    int sight = enemy.getStats().sight;
    return map_.at(from).visible && dx * dx + dy * dy <= sight * sight;
}

// The monster at a cell, or nullptr if there isn't one. The pointer is
// only good until the vector of monsters next changes
Enemy* Game::enemyAt(Point cell)
{
    for (Enemy& enemy : enemies_)
    {
        if (enemy.getPosition() == cell)
            return &enemy;
    }
    return nullptr;
}

// The item at a cell, or nullptr if there isn't one, with the same warning
Item* Game::itemAt(Point cell)
{
    for (Item& item : items_)
    {
        if (item.getPosition() == cell)
            return &item;
    }
    return nullptr;
}

void Game::draw() const
{
    SDL_SetRenderDrawColor(renderer_, Palette::BACKGROUND.r,
                           Palette::BACKGROUND.g, Palette::BACKGROUND.b, 255);
    SDL_RenderClear(renderer_);

    map_.draw(renderer_, glyphs_);

    // Treasure, then monsters, wherever the player can see them
    for (const Item& item : items_)
    {
        if (map_.at(item.getPosition()).visible)
            item.draw(renderer_, glyphs_);
    }
    for (const Enemy& enemy : enemies_)
    {
        if (map_.at(enemy.getPosition()).visible)
            enemy.draw(renderer_, glyphs_);
    }

    player_.draw(renderer_, glyphs_);
    flashes_.draw(renderer_);
    hud_.draw(renderer_, glyphs_, player_, depth_, gameOver_);

    // Anything the states add, over the top, starting at the bottom
    for (const std::unique_ptr<GameState>& state : states_)
        state->draw(renderer_, *this);

    SDL_RenderPresent(renderer_);
}
