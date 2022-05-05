#include "../include/Game.h"
#include <raylib.h>
#include <assert.h>

#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"

struct Entity{
    float x;
    int y;
    int velocity;
};

struct Obstacle{
    Entity object;
    bool isActive;
    bool isScorable;
};

Entity player;
Obstacle obstacles[10];

// Can change
int delayObstcle = 5;
int gapSize = 250;
int obstcleWidth = 150;
int obstacleSpeed = 100;
int playerSize = 25;
const int fontSize = 45;

// cant change
int score = 0;
int highestScore = 0;
int scoreText_x = 0;
int scoreText_y = 0;
float spawnTime = 0;
int screenWidth = 0;
int screenHeight = 0;

Music music;
Sound getScoreSound;
Sound hitSound;
Sound upSound;

float musicVolume;
float sfxVolume;

Vector2 mousePoint = { 0.0f, 0.0f };

enum Menus{MAIN = 0, GAMEMENU, ENDING, SETTINGS};
Menus currentMenu;

void SetGame(int _width, int _height, std::string title){
    screenWidth = _width;
    screenHeight = _height;
    SetTargetFPS(70); 
    InitWindow(screenWidth, screenHeight, title.c_str());
    InitAudioDevice();
    currentMenu = MAIN;
}

void SetAudio(){
    music = LoadMusicStream("../resources/music.wav");
    SetMusicVolume(music, musicVolume);
    PlayMusicStream(music);
    getScoreSound = LoadSound("../resources/score.wav");
    hitSound = LoadSound("../resources/hit.mp3");
    upSound = LoadSound("../resources/up.wav");
}

void SetPlayer(int x){
    player.x = x;
    player.y = screenHeight/2;
    player.velocity = 250;
}

void SetText(){
    int x = MeasureText(TextFormat("Score: %i", score) , fontSize);
    scoreText_x = screenWidth/2 - x/2;
    scoreText_y = 0;
}

void SetObstacles(){
    for (int i = 0; i < 10; i++){
        obstacles[i].object.x = screenWidth;
        obstacles[i].object.y = GetRandomValue(0, screenHeight);
        obstacles[i].isActive = false;
        obstacles[i].isScorable = true;
        obstacles[i].object.velocity = -obstacleSpeed;
    }
    obstacles[0].isActive = true;
}

Game::Game(int _width, int _height, std::string title){
    assert(!GetWindowHandle());
    SetGame(_width, _height, title);
    SetAudio();
    SetPlayer(screenWidth/2 - 250);
    SetText();
    SetObstacles();
}

Game::~Game() noexcept{
    assert(GetWindowHandle());
    UnloadMusicStream(music);
    CloseAudioDevice(); 
    StopSoundMulti(); 
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

void DrawObstacles(){
    for (int i = 0; i < 10; i++){
        if(!obstacles[i].isActive) continue;
        DrawRectangle(obstacles[i].object.x, 0, obstcleWidth, obstacles[i].object.y - (gapSize/2), YELLOW); // First obstcle draw
        DrawRectangle(obstacles[i].object.x, obstacles[i].object.y + (gapSize/2), obstcleWidth, screenHeight - (obstacles[i].object.y + (gapSize/2)) , YELLOW); // First obstcle draw
    }
}

void Game::Draw(){
    ClearBackground(RAYWHITE);
    switch (currentMenu){
        case MAIN:{
            if(GuiButton({(float)screenWidth/2 - 500/2, (float)screenHeight/2, 500, 125}, "Play")){
                currentMenu = GAMEMENU;
            }
            else if(GuiButton({(float)screenWidth/2 - 500/2, (float)screenHeight/2+150, 500, 125}, "Settings")){
                currentMenu = SETTINGS;
            }
            break;
        }
        case GAMEMENU:{
            DrawObstacles();
            DrawCircle(player.x, player.y, playerSize, MAROON);
            DrawText(TextFormat("Score: %i", score), scoreText_x, scoreText_y, fontSize, GREEN);
            break;
        }
        case ENDING:{
            DrawText(TextFormat("Your score: %i", score),
            screenWidth/2 - MeasureText(TextFormat("Your score: %i", score) , 100)/2,
            0,
            100,
            GREEN);   

            DrawText(TextFormat("Higest score: %i", highestScore),
            screenWidth/2 - MeasureText(TextFormat("Higest score: %i", highestScore) , 100)/2,
            125,
            100,
            GREEN);    

            if(GuiButton({
                (float)screenWidth/2 - 500/2,
                (float)screenHeight-150,
                500,
                125
            }, "Back to main menu")){
                currentMenu = MAIN;
                score = 0;
                spawnTime = 0;
                SetPlayer(screenWidth/2 - 250);
                SetObstacles();
            }
            else if(GuiButton({
                (float)screenWidth/2 - 500/2,
                (float)screenHeight-375,
                500,
                125
            }, "Retry")){
                score = 0;
                spawnTime = 0;
                SetPlayer(screenWidth/2 - 250);
                SetObstacles();
                currentMenu = GAMEMENU;
            }
            break;
        }
        case SETTINGS:{
            DrawText("Music volume",
            screenWidth/2 - MeasureText("Music volume" , 35)/2,
            0,
            35,
            BLACK);

            musicVolume = GuiSliderBar({
                (float)screenWidth/2 - 500/2,
                75,
                500,
                125
            }, "0" , "5", musicVolume, 0, 5.0f);

            DrawText("Sfx volume", screenWidth/2 - MeasureText("Sfx volume" , 35)/2,
            300,
            35,
            BLACK);

            sfxVolume = GuiSliderBar({
                (float)screenWidth/2 - 500/2,
                375,
                500,
                125
            }, "0" , "5", sfxVolume, 0, 5.0f);

            if(GuiButton({
                (float)screenWidth/2 - 500/2,
                (float)screenHeight-125,
                500,
                125
            }, "Back")){
                currentMenu = MAIN;
            }

            SetMusicVolume(music, musicVolume);
            SetSoundVolume(hitSound, sfxVolume);
            SetSoundVolume(upSound, sfxVolume);
            break;
        }
    }
}

int Clamp(int num, int min, int max){
    if(num < min) return min;
    if(num > max) return max;
    return num;
}

void PlayerUpdate(float delta){
    player.velocity += 300 * delta;                                            
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        player.velocity -= 200; 
        PlaySoundMulti(upSound);
    }
    player.velocity = Clamp(player.velocity, -100, 100); 
    if(player.velocity == 0) player.velocity = 100;
    player.y += delta * player.velocity;                                       
    player.y = Clamp(player.y, playerSize, screenHeight - playerSize); 
}

void SpawnObstacle(){
    for (int i = 0; i < 10; i++){
        if(obstacles[i].isActive) continue;
        obstacles[i].object.x = screenWidth;
        obstacles[i].isActive = true;
        obstacles[i].isScorable = true;
        obstacles[i].object.velocity = -obstacleSpeed;
        return;
    }
}

void ObstaclesUpdate(float delta){
    for (int i = 0; i < 10; i++){
        if(!obstacles[i].isActive) continue;
        obstacles[i].object.x += obstacles[i].object.velocity * delta;
        if ((obstacles[i].object.x + obstcleWidth) <= 0) obstacles[i].isActive = false;
        if(!obstacles[i].isScorable) continue;
        if(player.x + (playerSize/2) < obstacles[i].object.x + (obstcleWidth/2)) continue;
        score++;
        PlaySoundMulti(getScoreSound);
        obstacles[i].isScorable = false;
    }
    spawnTime += delta;
    if(spawnTime < delayObstcle) return;
    spawnTime = 0;
    SpawnObstacle();
}

void Hit(){
    PlaySoundMulti(hitSound);
    currentMenu = ENDING;
    if(score > highestScore) highestScore = score;
}

void CheckCollision(){
    int x;
    int y;
    for (int i = 0; i < 10; i++){
        if(!obstacles[i].isActive) continue;
        x = obstacles[i].object.x;
        y = obstacles[i].object.y;
        if(!(player.x + playerSize > x  && player.x + playerSize < x + obstcleWidth) || !(player.x - playerSize > x && player.x - playerSize < x + obstcleWidth)) continue;
        if((player.y + playerSize > 0 && player.y + playerSize < y - (gapSize/2)) || (player.y - playerSize > 0 && player.y - playerSize < y - (gapSize/2))){
            Hit();
        }
        if((player.y + playerSize > y + (gapSize/2) && player.y + playerSize < screenHeight) || (player.y - playerSize > y + (gapSize/2) && player.y - playerSize < screenHeight)){
            Hit();
        }
    }
}

void Game::Update(){
    switch (currentMenu){
        case GAMEMENU:{
            UpdateMusicStream(music);
            float delta = GetFrameTime();
            PlayerUpdate(delta);
            ObstaclesUpdate(delta);
            CheckCollision();
            break;
        }
        case ENDING:break;
        case SETTINGS:break;
        default:break;
    }
}