#ifndef TEXTGAME_H
#define TEXTGAME_H
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "json.hpp"
using json = nlohmann::json;
// using namespace std;
//
struct Room
{
    std::string id;
    std::string desc;
    std::map<std::string, std::string> exits;
};
struct Object
{
    std::string id;
    std::string desc;
    std::string initialroom;
    bool taken;
};
struct Enemy
{
    std::string id;
    std::string desc;
    bool alive;
    int aggressiveness;
    std::string initialroom;
    std::vector<std::string> killedby;
};
struct Objective
{
    std::string type;
    std::vector<std::string> what;
};
struct GameData
{
    std::vector<Room> rooms;
    std::vector<Object> objects;
    std::vector<Enemy> enemies;
    Objective objective;
};
class TextGame
{
public:
    // load the map
    TextGame(const std::string &mapFile);
    // lod the game map
    void loadMap(const json &mapFile);
    // initialize the game
    void initializeGame();
    // start the game
    void startGame();
    // print the room details
    void printRoomDetails();
    // game handle commands
    void handleCommand(const std::string &command);
    // handle go direction commands
    void handleGoCommand();
    // handle take commands
    void handleTakeCommand();
    // handle kill commands
    void handleKillCommand();
    // handle look commands
    void handleLookCommand();
    // handle look commands
    void handleListItemsCommand();
    // update objects
    void updateObjectTaken(std::vector<Object> &objects, const std::string &objectId, bool val);
    // enemy attack the player
    bool enemyAttackPlayer();
    // enemy attack the player
    void checkPlayerWin();

private:
    GameData gameData_;
    std::string currentRoom_;
    std::string command_;
    bool playerDead_;
    bool playerWon_;
    std::vector<std::string> takenObjects_;
    std::vector<std::string> killedEnemies_;
};

#endif // TEXTGAME_H