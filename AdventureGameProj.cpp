#include <iostream>
#include "console.h"
using namespace std;
#include "AdeventureGame.h"






int main(int argc, char* argv[]) {
    bool isLoad = argc > 1 && string(argv[1]) == "-load";
    bool isSave = argc > 1 && string(argv[1]) == "-save";
    bool isSilent = isLoad && argc > 2 && string(argv[2]) == "-silent";


    // if both provided, prefer load
    if (isLoad) isSave = false;

    AdeventureGame* game = new AdeventureGame;
    game->run(isLoad, isSave, isSilent);
    delete game;
}


