#include "../include/Game.h"

int main(void){
   
    Game game{1280, 720, "test"};

    while (!game.GameShouldClose()){
        game.Tick();
    }
    
    return 0;
}