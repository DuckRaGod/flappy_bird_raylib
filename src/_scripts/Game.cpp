#include "../include/Game.h"
#include <raylib.h>
#include <assert.h>

struct Entity{
    int x;
    int y;
    int velocity;
};

Entity player;
int width;
int height;

int score;
int scoreText_x;
int scoreText_y;
const int fontSize = 45;

int Clamp(int num, int min, int max){
    if(num > min) return min;
    if(num < max) return max;
    return num;
}

Game::Game(int _width, int _height, std::string title){
    assert(!GetWindowHandle());
    SetTargetFPS(70); 
    width = _width;
    height = _height;
    InitWindow(width, height, title.c_str());
    player.x = width/2;
    player.y = height/2;
    player.velocity = 250;
    int x = MeasureText(TextFormat("Score: %i", score) , fontSize);
    scoreText_x = width/2 - x/2;
    scoreText_y = 0;
}

Game::~Game() noexcept{
    assert(GetWindowHandle());
    CloseWindow();
}

bool Game::GameShouldClose() const{
    return WindowShouldClose();
}

void Game::Tick(){
    Update();
    BeginDrawing();
    Draw();
    EndDrawing();
}

void Game::Draw(){
    ClearBackground(RAYWHITE);
    DrawCircle(player.x, player.y, 25, MAROON);
    DrawText(TextFormat("Score: %i", score), scoreText_x, scoreText_y, fontSize, GREEN);
}

void PlayerUpdate(float delta){
    player.velocity += 100 * delta;                                             // Gravity
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) player.velocity -= 200;
    player.velocity = Clamp(player.velocity, 100, -100); 
    if(player.velocity == 0) player.velocity = 100;
    player.y += delta * player.velocity;                                        // Apply velocity
}

void Game::Update(){
    float delta = GetFrameTime();
    PlayerUpdate(delta);
}