#include "../include/Game.h"
#include <raylib.h>
#include <assert.h>
#include <iostream>

struct Entity{
    float x;
    int y;
    int velocity;
    bool isActive;
    bool isScorable;
};

Entity player;
Entity obstacles[10];

int delayObstcle = 5;
int gapSize = 250;
int obstcleWidth = 150;
int obstacleSpeed = 100;

int playerSize = 25; 

int width;
int height;
// Text
int score;
int scoreText_x;
int scoreText_y;
const int fontSize = 45;

Game::Game(int _width, int _height, std::string title){
    assert(!GetWindowHandle());
    // Game
    width = _width;
    height = _height;
    SetTargetFPS(70); 
    InitWindow(width, height, title.c_str());
    // Player
    player.x = width/2;
    player.y = height/2;
    player.velocity = 250;
    // Text
    int x = MeasureText(TextFormat("Score: %i", score) , fontSize);
    scoreText_x = width/2 - x/2;
    scoreText_y = 0;
    // Obstcle
    for (int i = 0; i < 10; i++){
        obstacles[i].x = width;
        obstacles[i].y = GetRandomValue(0, height);
        obstacles[i].isActive = false;
        obstacles[i].isScorable = true;
    }
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
    for (int i = 0; i < 10; i++){
        if(obstacles[i].isActive == true){
            DrawRectangle(obstacles[i].x, 0, obstcleWidth, obstacles[i].y - (gapSize/2), YELLOW); // First obstcle draw
            DrawRectangle(obstacles[i].x, obstacles[i].y + (gapSize/2), obstcleWidth, height - (obstacles[i].y + (gapSize/2)) , YELLOW); // First obstcle draw
        }
    }
    DrawCircle(player.x, player.y, playerSize, MAROON);

    DrawText(TextFormat("Score: %i", score), scoreText_x, scoreText_y, fontSize, GREEN);
}

int Clamp(int num, int min, int max){
    if(num < min) return min;
    if(num > max) return max;
    return num;
}

void PlayerUpdate(float delta){
    player.velocity += 300 * delta;                                             // Gravity
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) player.velocity -= 200;
    player.velocity = Clamp(player.velocity, -100, 100); 
    if(player.velocity == 0) player.velocity = 100;
    player.y += delta * player.velocity;                                        // Apply velocity
    player.y = Clamp(player.y, playerSize, height - playerSize); 
}

float spawnTime;
void Game::Update(){
    float delta = GetFrameTime();
    PlayerUpdate(delta);

    for (int i = 0; i < 10; i++){
        if(obstacles[i].isActive){
            obstacles[i].x -= obstacleSpeed * delta;

            if(obstacles[i].isScorable){
                if(player.x + (playerSize/2) > obstacles[i].x + (obstcleWidth/2)){
                    score++;
                    obstacles[i].isScorable = false;
                }
            }

            if ((obstacles[i].x + obstcleWidth) <= 0) obstacles[i].isActive = false;
        }
    }

    spawnTime += delta;
    if(spawnTime >= delayObstcle){
        spawnTime = 0;
        for (int i = 0; i < 10; i++){
            if(obstacles[i].isActive == false){
                obstacles[i].x = width;
                obstacles[i].isActive = true;
                obstacles[i].isScorable = true;
                return;
            }
        }
    }
}