#include "TextGame.h"

// constructor with file name
TextGame::TextGame(const std::string &mapFileName)
{
    std::ifstream fin(mapFileName);
    json jsonObject;
    fin >> jsonObject;
    loadMap(jsonObject);
    initializeGame();
}
// initialize
void TextGame::initializeGame()
{
    playerDead_ = false;
    playerWon_ = false;
    takenObjects_.clear();
    printRoomDetails();
    startGame();
}
// load map
void TextGame::loadMap(const json &jsonObject)
{
    // set vector size to pre-allocate memory as i alredy know the size
    gameData_.rooms.reserve(jsonObject["rooms"].size());
    gameData_.objects.reserve(jsonObject["objects"].size());
    gameData_.enemies.reserve(jsonObject["enemies"].size());
    // set rooms data
    for (const auto &roomData : jsonObject["rooms"])
    {
        Room room;
        room.id = roomData["id"];
        room.desc = roomData["desc"];
        for (const auto &exitData : roomData["exits"].items())
        {
            room.exits[exitData.key()] = exitData.value();
        }
        gameData_.rooms.push_back(std::move(room));
    }
    // set objects data
    for (const auto &objectData : jsonObject["objects"])
    {
        Object object;
        object.id = objectData["id"];
        object.desc = objectData["desc"];
        object.initialroom = objectData["initialroom"];
        object.taken = false;
        gameData_.objects.push_back(std::move(object));
    }
    // set enemies data
    for (const auto &enemyData : jsonObject["enemies"])
    {
        Enemy enemy;
        enemy.id = enemyData["id"];
        enemy.desc = enemyData["desc"];
        enemy.alive = true;
        enemy.aggressiveness = enemyData["aggressiveness"];
        enemy.initialroom = enemyData["initialroom"];
        enemy.killedby = enemyData["killedby"];

        gameData_.enemies.push_back(std::move(enemy));
    }

    currentRoom_ = jsonObject["player"]["initialroom"];
    // set objective
    gameData_.objective.type = jsonObject["objective"]["type"];
    gameData_.objective.what = jsonObject["objective"]["what"];
}
// printDetails template function
template <typename T, typename Condition, typename PrintFunction>
void printDetails(const std::vector<T> &items, const Condition &condition, const PrintFunction &printFunc)
{
    for (const auto &item : items)
    {
        if (condition(item))
        {
            printFunc(item, true);
        }
    }
}
// to print room
void printRoom(const Room &room, const bool response)
{
    if (response)
        std::cout << "You are at " << room.id << ". " << room.desc << std::endl;
}
// to print object
void printObject(const Object &object, const bool response)
{
    if (response)
        std::cout << "There is a " << object.id << " here." << std::endl;
}
// to print enemy
void printEnemy(const Enemy &enemy, const bool response)
{
    if (response)
        std::cout << "In this room there is an enemy " << enemy.id << std::endl;
}

// print rooms details
void TextGame::printRoomDetails()
{
    // print room
    printDetails(
        gameData_.rooms, [this](const Room &room)
        { return room.id == currentRoom_; },
        printRoom);
    // print room objects
    printDetails(
        gameData_.objects, [this](const Object &object)
        { return (object.initialroom == currentRoom_ && !object.taken); },
        printObject);
    // print room enemies
    printDetails(
        gameData_.enemies, [this](const Enemy &enemy)
        { return (enemy.initialroom == currentRoom_ && enemy.alive); },
        printEnemy);
};
// game start
void TextGame::startGame()
{
    while (!playerDead_ || !playerWon_)
    {
        // printObjectsDetails();
        // printEnemiesDetails();

        std::cout << ">";
        getline(std::cin, command_);
        // convert string to lowercase
        transform(command_.begin(), command_.end(), command_.begin(), ::tolower);
        handleCommand(command_);

        if (playerDead_)
        {
            std::cout << "You are dead." << std::endl;
            exit(1);
        }
        if (playerWon_)
        {
            std::cout << "Congratulations! You have won the game." << std::endl;
            exit(1);
        }
    }
}
//
// handle command
void TextGame::handleCommand(const std::string &val)
{
    std::vector<std::string> commands;
    std::string type, command, text;
    // read the string
    std::istringstream iss(val);
    // divide the string
    while (iss >> text)
    {
        commands.push_back(text);
    }
    type = commands[0];
    // merge the commands
    if (commands.size() >= 2)
    {
        for (int i = 1; i < commands.size(); i++)
        {

            command += commands[i];
            if (i != commands.size() - 1)
                command += ' ';
        }
    }
    // std::cout << "type: " << type << std::endl;
    // std::cout << "command: " << command << std::endl;
    command_ = command;
    //
    if ((type + command) == "look" || (type + " " + command) == "look around")
    {
        printRoomDetails();
    }
    else if (type == "go" && command != "")
    {
        handleGoCommand();
    }
    else if (type == "take" && command != "")
    {
        handleTakeCommand();
    }
    else if (type == "kill" && command != "")
    {
        handleKillCommand();
    }
    else if (type == "look" && command != "")
    {
        handleLookCommand();
    }
    else if ((type + " " + command) == "list items")
    {
        handleListItemsCommand();
    }
    else
    {
        std::cout << "I don't understand that." << std::endl;
    }
    // check win conditon
    checkPlayerWin();
};
// handle go commands
void TextGame::handleGoCommand()
{
    // bool found = false;
    // for (const auto &room : gameData_.rooms)
    // {
    //     if (room.id == currentRoom_)
    //     {
    //         for (const auto &exit : room.exits)
    //         {
    //             if (exit.first == val)
    //             {
    //                 found = true;
    //                 currentRoom_ = exit.second;
    //                 printRoomDetails();
    //                 break;
    //             }
    //         }
    //         break; // No need to continue searching once found
    //     }
    // }
    //
    auto roomIt = find_if(
        gameData_.rooms.begin(),
        gameData_.rooms.end(),
        [this](const Room &room)
        { return room.id == currentRoom_; });

    if (roomIt != gameData_.rooms.end())
    {
        auto exitIt = find_if(
            roomIt->exits.begin(),
            roomIt->exits.end(),
            [this](const std::pair<const std::string, std::string> &exit)
            { return exit.first == command_; });

        if (exitIt != roomIt->exits.end())
        {
            if (!enemyAttackPlayer())
            {
                currentRoom_ = exitIt->second;
                printRoomDetails();
            }
        }
        else
        {
            std::cout << "You can't go there" << std::endl;
        }
    }
    // if (!found)
    //     std::cout << "You can't go there" << std::endl;
}
// handle take commands
void TextGame::handleTakeCommand()
{
    bool found = false;
    for (const auto &object : gameData_.objects)
    {
        if (object.initialroom == currentRoom_)
        {
            if (!object.taken && object.id == command_)
            {
                takenObjects_.push_back(object.id);
                updateObjectTaken(gameData_.objects, command_, true);
                std::cout << "Taken." << std::endl;
                found = true;
                // if found stop searching
                break;
            }
        }
    }
    if (!found)
        std::cout << "I do not see that here." << std::endl;
}
// handle look commands
void TextGame::handleLookCommand()
{
    auto objectIt = find_if(gameData_.objects.begin(), gameData_.objects.end(), [this](const Object &object)
                            { return ((object.id == command_) && ((find(takenObjects_.begin(), takenObjects_.end(), command_) != takenObjects_.end()) || (object.initialroom == currentRoom_ && !object.taken))); });
    if (objectIt != gameData_.objects.end())
    {
        std::cout << objectIt->desc << std::endl;
    }
    else
    {
        auto enemyIt = find_if(gameData_.enemies.begin(), gameData_.enemies.end(), [this](const Enemy &enemy)
                               { return ((enemy.id == command_) && (enemy.initialroom == currentRoom_) && enemy.alive); });
        if (enemyIt != gameData_.enemies.end())
            std::cout << enemyIt->desc << std::endl;
        else
            std::cout << "I do not see that here." << std::endl;
    }
}
// handle look commands
void TextGame::handleListItemsCommand()
{
    if (takenObjects_.empty())
        std::cout << "You haven't selected any objects yet." << std::endl;
    else
    {
        std::cout << "Your selected items are:" << std::endl;
        for (const auto &val : takenObjects_)
            std::cout << val << std::endl;
    }
}
// handle kill commands
void TextGame::handleKillCommand()
{
    auto enemyIt = find_if(gameData_.enemies.begin(), gameData_.enemies.end(),
                           [this](const Enemy &enemy)
                           {
                               return enemy.id == command_ && enemy.alive && enemy.initialroom == currentRoom_;
                           });
    if (enemyIt != gameData_.enemies.end())
    {
        bool eligibleToKill = includes(
            takenObjects_.begin(), takenObjects_.end(),
            enemyIt->killedby.begin(), enemyIt->killedby.end());

        if (eligibleToKill)
        {
            std::cout << "You have successfully killed " << command_ << "." << std::endl;
            enemyIt->alive = false;
            killedEnemies_.push_back(enemyIt->id);
        }
        else
        {
            std::cout << "An enemy attacked you, and you died." << std::endl;
            playerDead_ = true;
        }
    }
    else
    {
        std::cout << "No such enemy in the room." << std::endl;
    }
}
// update object taken
void TextGame::updateObjectTaken(std::vector<Object> &objects, const std::string &objectId, bool val)
{
    auto it = find_if(objects.begin(), objects.end(), [objectId](const Object &obj)
                      { return obj.id == objectId; });
    if (it != objects.end())
    {
        it->taken = val;
    }
}
// check player attacked by enemy or not
bool TextGame::enemyAttackPlayer()
{
    int randomAttack = rand() % 101;
    auto enemyIt = find_if(gameData_.enemies.begin(), gameData_.enemies.end(),
                           [this](const Enemy &enemy)
                           {
                               return enemy.aggressiveness > 0 && enemy.alive && enemy.initialroom == currentRoom_;
                           });
    // std::cout << "randomAttack" << randomAttack << std::endl;
    if (enemyIt != gameData_.enemies.end())
    {
        if (randomAttack < enemyIt->aggressiveness)
        {
            playerDead_ = true;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
// check player attacked by enemy or not
void TextGame::checkPlayerWin()
{
    std::string winType = gameData_.objective.type;
    if (winType == "kill")
    {
        bool checkWinCondition = includes(
            killedEnemies_.begin(), killedEnemies_.end(),
            gameData_.objective.what.begin(), gameData_.objective.what.end());
        if (checkWinCondition)
        {
            playerWon_ = true;
        }
    }
    else if (winType == "room")
    {
        auto winIt = std::find(gameData_.objective.what.begin(), gameData_.objective.what.end(), currentRoom_);
        if (winIt != gameData_.objective.what.end())
            playerWon_ = true;
    }
    else if (winType == "collect")
    {
        bool checkWinCondition = all_of(
            gameData_.objective.what.begin(), gameData_.objective.what.end(),
            [this](const std::string &obj)
            {
                return find(takenObjects_.begin(), takenObjects_.end(), obj) != takenObjects_.end();
            });
        if (checkWinCondition)
        {
            playerWon_ = true;
        }
    }
    else
    {
        playerWon_ = false;
    }
}
//