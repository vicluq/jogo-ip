#include <raylib.h>
#include <stdio.h>
#include <math.h>
#include "./utils/utils.h"

void renderEnemies(float playerH, float playerW, float pX, float pY, Rectangle *enemies, int enemyAmount);
void renderEnemyLifeBar(Rectangle *enemies, Rectangle *lifebars, int enemyAmount);

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(0, 0, "Game");

    if (!IsWindowFullscreen())
    {
        ToggleFullscreen();
    }

    // Tempo Related
    double gameTime = 0, gameOverTime = 0;

    // Telas e controles de transição
    int menuMode = 1, closeGame = 0, gameOverMode = 0;

    const int screenWidth = GetScreenWidth(), screenHeight = GetScreenHeight();
    SetTargetFPS(60);

    // Mouse
    Vector2 mouse = {.x = 0, .y = 0};

    // Menu Elements
    char menuOptionsText[2][10] = {"Start", "Close"};
    Rectangle menuOptions[2] = {
        {.height = 100, .width = 400, .x = (screenWidth / 2) - 200, .y = 500},
        {.height = 100, .width = 400, .x = (screenWidth / 2) - 200, .y = 500 + (1.5 * menuOptions[0].height)},
    };

    // Camera
    Camera2D camera = {.offset = {screenWidth / 2.0f, screenHeight / 2.0f},
                       .target = {0},
                       .rotation = 0,
                       .zoom = 0.5};

    // Player related
    char filename[31];
    Texture movesHorizontal[2][5];
    Texture movesVertical[2][3];
    int currentMoveXL = 0, currentMoveXR = 0, currentMoveYU = 0, currentMoveYD = 0;

    for (int j = 1; j <= 5; ++j)
    {
        sprintf(filename, "./assets/Base/baseA1%d.png", j);
        movesHorizontal[0][j - 1] = LoadTexture(filename);

        sprintf(filename, "./assets/Base/baseA2%d.png", j);
        movesHorizontal[1][j - 1] = LoadTexture(filename);
    }

    for (int j = 1; j <= 3; ++j)
    {
        sprintf(filename, "./assets/Base/baseB1%d.png", j);
        movesVertical[0][j - 1] = LoadTexture(filename);

        sprintf(filename, "./assets/Base/baseB2%d.png", j);
        movesVertical[1][j - 1] = LoadTexture(filename);
    }

    Rectangle player1 = {0, 0, movesHorizontal[1][1].width / 2, movesHorizontal[1][1].height / 1.7};
    int score = 0;
    char scoreText[50];
    const float playerSpeed = 15, pLife = 393;
    float previousX = 0, previousY = 0;

    // Player Life
    int isAlive = 1;
    Texture playerLifeBar = LoadTexture("./assets/HUD/barra.png");
    Rectangle playerlifeBar = {92, 37, pLife, 18};

    // Player Weapons
    Rectangle weaponRight = {player1.x, player1.y, 0, 0};
    Rectangle weaponLeft = {player1.x, player1.y, 0, 0};

    // Enemies
    const int enemySpeed = 15, enemyAmount = 15;
    Texture cloroquina = LoadTexture("./assets/Cloroquina/cloroquina2-removebg-preview.png");
    Rectangle *enemies = (Rectangle *)malloc(sizeof(Rectangle) * enemyAmount);
    Rectangle *enemylifeBars = (Rectangle *)malloc(sizeof(Rectangle) * enemyAmount);

    if (enemies != NULL && enemylifeBars != NULL)
    {
        renderEnemies(movesHorizontal[1][1].height, movesHorizontal[1][1].width, player1.x, player1.y, enemies, enemyAmount);
        renderEnemyLifeBar(enemies, enemylifeBars, enemyAmount);
    }
    else
    {
        CloseWindow();
        exit(1);
    }

    // Ciclo do jogo
    while (!WindowShouldClose() && !closeGame)
    {
        if (menuMode)
        {
            renderMenu(&mouse, screenWidth, screenHeight, menuOptions, &menuMode, &closeGame);
        }

        else
        {
            gameTime = GetTime();

            // Player Motion
            previousY = player1.y;
            previousX = player1.x;

            if (IsKeyPressed(KEY_W) || IsKeyDown(KEY_W))
            {
                player1.y -= playerSpeed * 0.55;
            }

            if (IsKeyPressed(KEY_D) || IsKeyDown(KEY_D))
            {
                player1.x += playerSpeed * 0.55;
            }

            if (IsKeyPressed(KEY_S) || IsKeyDown(KEY_S))
            {
                player1.y += playerSpeed * 0.55;
            }

            if (IsKeyPressed(KEY_A) || IsKeyDown(KEY_A))
            {
                player1.x -= playerSpeed * 0.55;
            }

            //  Camera
            if ((previousY != player1.y) || (previousX != player1.x))
            {
                camera.target.x += ((player1.x) - camera.target.x) * 0.025;
                camera.target.y += ((player1.y) - camera.target.y) * 0.025;
            }

            for (int i = 0; i < enemyAmount; ++i)
            {
                if (enemylifeBars[i].width <= 0)
                {
                    ++score;
                }
            }
            sprintf(scoreText, "SCORE: %d", score);

            // Drawing mode
            BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawTexture(playerLifeBar, 0, 0, WHITE);
            DrawRectangleRec(playerlifeBar, GREEN);
            DrawText(scoreText, 10, 100, 20, BLACK);

            // Mecânica de vida do player e GameOver -> Nova branch
            if (playerlifeBar.width <= 0)
            {
                if (isAlive)
                {
                    gameOverTime = GetTime();
                    isAlive = 0;
                }

                if (!isAlive && (gameTime - gameOverTime) <= 5)
                {
                    DrawText("  SE FUDEEEU\nCLOROQUINADO", (screenWidth / 2) - 800, (screenHeight / 2) - 280, 200, RED);
                }
                else
                {
                    gameOverMode = 1;
                }
            }

            // Modo 2D
            BeginMode2D(camera);

            DrawText("ISSO É MELHOR QUE LOL PPRT", 0 - 600, 0, 80, BLACK);

            // Rendering Player
            if (playerlifeBar.width > 0)
            {
                // DrawRectangleRec(player1, RED);
                if (previousX > player1.x)
                {
                    if (currentMoveXL == 0)
                    {
                        DrawTexture(movesHorizontal[0][0], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveXL;
                    }
                    else if (currentMoveXL == 1)
                    {
                        DrawTexture(movesHorizontal[0][1], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveXL;
                    }
                    else if (currentMoveXL == 2)
                    {
                        DrawTexture(movesHorizontal[0][2], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveXL;
                    }
                    else if (currentMoveXL == 3)
                    {
                        DrawTexture(movesHorizontal[0][3], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveXL;
                    }
                    else if (currentMoveXL == 4)
                    {
                        DrawTexture(movesHorizontal[0][4], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        currentMoveXL = 0;
                    }
                }
                else if (previousX < player1.x)
                {
                    if (currentMoveXR == 0)
                    {
                        DrawTexture(movesHorizontal[1][0], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveXR;
                    }
                    else if (currentMoveXR == 1)
                    {
                        DrawTexture(movesHorizontal[1][1], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveXR;
                    }
                    else if (currentMoveXR == 2)
                    {
                        DrawTexture(movesHorizontal[1][2], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveXR;
                    }
                    else if (currentMoveXR == 3)
                    {
                        DrawTexture(movesHorizontal[1][3], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveXR;
                    }
                    else if (currentMoveXR == 4)
                    {
                        DrawTexture(movesHorizontal[1][4], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        currentMoveXR = 0;
                    }
                }
                else if (previousY < player1.y)
                {
                    if (currentMoveYU == 0)
                    {
                        DrawTexture(movesVertical[0][0], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveYU;
                    }
                    else if (currentMoveYU == 1)
                    {
                        DrawTexture(movesVertical[0][1], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveYU;
                    }
                    else if (currentMoveYU == 2)
                    {
                        DrawTexture(movesVertical[0][2], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        currentMoveYU = 0;
                    }
                }
                else if (previousY > player1.y)
                {
                    if (currentMoveYD == 0)
                    {
                        DrawTexture(movesVertical[1][0], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveYD;
                    }
                    else if (currentMoveYD == 1)
                    {
                        DrawTexture(movesVertical[1][1], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        ++currentMoveYD;
                    }
                    else if (currentMoveYD == 2)
                    {
                        DrawTexture(movesVertical[1][2], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                        currentMoveYD = 0;
                    }
                }
                else if (previousX == player1.x)
                {
                    DrawTexture(movesVertical[0][1], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                }
                else if (previousY == player1.y)
                {
                    DrawTexture(movesVertical[0][1], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                }
            }

            // Rendering Enemy
            for (int i = 0; i < enemyAmount; ++i)
            {
                if (CheckCollisionRecs(weaponRight, enemies[i]) || CheckCollisionRecs(weaponLeft, enemies[i]))
                {
                    enemylifeBars[i].width -= 10;
                }
                // Rendering each enemy box
                if (enemylifeBars[i].width > 0)
                {
                    DrawTexture(cloroquina, enemies[i].x - (enemies[i].width * 0.8), enemies[i].y - 10, WHITE);
                    DrawRectangleRec(enemylifeBars[i], GREEN);
                }
                else
                {
                    enemies[i].x = 9999999;
                    enemies[i].y = 9999999;
                }

                // Enemy Dinamic
                if (distanceToEnemy(player1.x, player1.y, enemies[i].x, enemies[i].y) <= 600)
                {
                    enemyDinamic(player1, &enemies[i], enemySpeed, &playerlifeBar.width);
                }
                enemylifeBars[i].x = enemies[i].x;
                enemylifeBars[i].y = enemies[i].y - 10;
            }

            // Combat mechanics
            if (IsKeyPressed(KEY_K))
            {
                weaponRight.width = 60;
                weaponRight.height = 15;
                weaponRight.x = player1.x + player1.width;
                weaponRight.y = player1.y + 2;
                DrawRectangleRec(weaponRight, GREEN);
            }
            else
            {
                weaponRight.width = 0;
                weaponRight.height = 0;
                weaponRight.x = player1.x + (player1.width / 2);
                weaponRight.y = player1.y + (player1.height / 2);
            }

            if (IsKeyPressed(KEY_J))
            {
                weaponLeft.width = 60;
                weaponLeft.height = 15;
                weaponLeft.x = player1.x - weaponLeft.width;
                weaponLeft.y = player1.y + 2;
                DrawRectangleRec(weaponLeft, GREEN);
            }
            else
            {
                weaponLeft.width = 0;
                weaponLeft.height = 0;
                weaponLeft.x = player1.x + (player1.width / 2);
                weaponLeft.y = player1.y + (player1.height / 2);
            }

            EndMode2D();
            EndDrawing();
            score = 0;
        }
    }

    CloseWindow();
    free(enemies);
    free(enemylifeBars);
    return 0;
}

void renderEnemies(float playerH, float playerW, float pX, float pY, Rectangle *enemies, int enemyAmount)
{
    for (int i = 0; i < enemyAmount; ++i)
    {
        enemies[i].height = playerH / 1.7;
        enemies[i].width = playerW / 2;

        enemies[i].x = (i + (pX * 2) + 1) * 100;
        enemies[i].y = (i + (pY * 2) + 1) + (i * enemies[i].x);
    }
}

void renderEnemyLifeBar(Rectangle *enemies, Rectangle *lifeBars, int enemyAmount)
{
    for (int i = 0; i < enemyAmount; ++i)
    {
        lifeBars[i].height = 5;
        lifeBars[i].width = 100;

        lifeBars[i].x = enemies[i].x;
        lifeBars[i].y = enemies[i].y + 10;
    }
}