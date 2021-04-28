#include <raylib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "./utils/utils.h"

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

    Rectangle player1 = {-50, -30, movesHorizontal[1][1].width / 2, movesHorizontal[1][1].height / 1.7};
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
    Texture cloroquina = LoadTexture("./assets/Cloroquina/cloroquina2-removebg-preview.png");
    Rectangle enemy = {1000, 120, movesHorizontal[1][1].width / 2, movesHorizontal[1][1].height / 1.7};
    Rectangle enemylifeBar = {1000, 130, 100, 5};
    const int enemySpeed = 15;

    // Ciclo do jogo
    while (!WindowShouldClose() && !closeGame)
    {
        if (menuMode)
        {
            renderMenu(&mouse, screenWidth, menuOptions, &menuMode, &closeGame);
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

            // Drawing mode
            BeginDrawing();

            ClearBackground(RAYWHITE);
            DrawTexture(playerLifeBar, 0, 0, WHITE);
            DrawRectangleRec(playerlifeBar, GREEN);

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

            if (CheckCollisionRecs(weaponRight, enemy) || CheckCollisionRecs(weaponLeft, enemy))
            {
                enemylifeBar.width -= 10;
            }

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
            if (enemylifeBar.width > 0)
            {
                // DrawRectangleRec(enemy, WHITE);
                DrawTexture(cloroquina, enemy.x - (enemy.width * 0.8), enemy.y - 10, WHITE);
                DrawRectangleRec(enemylifeBar, GREEN);
            }
            else
            {
                enemy.x = 9999999;
                enemy.y = 9999999;
            }

            // Enemy Dinamic
            if (distanceToEnemy(player1.x, player1.y, enemy.x, enemy.y) <= 500)
            {
                enemyDinamic(player1, &enemy, enemySpeed, &playerlifeBar.width);
            }

            enemylifeBar.x = enemy.x;
            enemylifeBar.y = enemy.y - 10;

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
        }
    }

    CloseWindow();
    return 0;
}
