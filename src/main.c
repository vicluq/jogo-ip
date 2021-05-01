#include <raylib.h>
#include <stdio.h>
#include <math.h>
#include "./utils/utils.h"

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(0, 0, "Game");
    InitAudioDevice();

    const int screenWidth = GetScreenWidth(), screenHeight = GetScreenHeight();
    SetTargetFPS(60);

    if (!IsWindowFullscreen())
    {
        ToggleFullscreen();
    }

    // Tempo Related
    double PROCESS_TIME = 0, gameTime = 0, introStart = 0, gameOverTime = 0;
    char timeCounter[20];

    // Telas e controles de transição
    int menuMode = 1, closeGame = 0, introMode = 0, gameOverMode = 0;
    int bossPhase = 0, phaseAnnouncement = 0, cloroquinaPhase = 1;

    double phaseTransitionTime = 0;
    char phaseTransitionText[] = "PREPARE-SE PARA A BATALHA FINAL\nDERROTE O BOSTÃO E OS BOSTINHAS";

    Vector2 introTextPosition = {.x = (screenWidth / 2) - (MeasureText(introText, 40) / 2.2), .y = (screenHeight / 2) - (20 * 4)};

    // Sounds
    Sound audioBozo = LoadSound("./assets/bozo-pal.wav");
    SetMasterVolume(0.5f);

    // Mouse
    Vector2 mouse = {.x = 0, .y = 0};

    // Menu Elements
    char menuOptionsText[2][10] = {"Start", "Close"};
    Texture2D menuCape = LoadTexture("./assets/menu.png");
    Rectangle menuOptions[2] = {
        {.height = 100, .width = 400, .x = (screenWidth / 2) - 200, .y = 500},
        {.height = 100, .width = 400, .x = (screenWidth / 2) - 200, .y = 500 + (1.5 * menuOptions[0].height)},
    };

    // Camera
    Camera2D camera = {.offset = {screenWidth / 2.0f, screenHeight / 2.0f},
                       .target = {0},
                       .rotation = 0,
                       .zoom = 0.5};

    // Map
    Texture2D EnemyMap = LoadTexture("./assets/mapa-teste.png");
    Texture2D BossMap = LoadTexture("./assets/mapa-teste2.png");

    // Player related
    char filename[31];
    Texture movesHorizontal[2][4];
    Texture movesVertical[2][3];
    int currentMoveXL = 0, currentMoveXR = 0, currentMoveYU = 0, currentMoveYD = 0;

    for (int j = 1; j <= 4; ++j)
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

    Rectangle player1 = {0, 0, movesHorizontal[1][1].width / 2.0f, movesHorizontal[1][1].height / 1.7f};
    int score = 0;
    char scoreText[50];
    const float playerSpeed = 15;
    float previousX = 0, previousY = 0, animationTime = 0.0f, pLife = 800;

    // Player Life
    int isAlive = 1;
    Texture playerLifeBar = LoadTexture("./assets/HUD/barra.png");
    float lifeBarPosX = 0, lifeBarPosY = 0;

    // Player Weapons
    Rectangle weaponRight = {player1.x, player1.y, 0, 0};
    Rectangle weaponLeft = {player1.x, player1.y, 0, 0};

    // Enemies
    const int enemySpeed = 15, enemyAmount = 4;
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
        PROCESS_TIME = GetTime();
        if (menuMode)
        {
            renderMenu(menuCape, &mouse, screenWidth, screenHeight, menuOptions, &menuMode, &introMode, &introStart, &closeGame);
        }
        else if (introMode && !menuMode)
        {
            // Animar Star Wars Like Intro
            BeginDrawing();
            ClearBackground(BLACK);

            // sprintf(timeCounter, "%lf", (PROCESS_TIME - introStart));
            // DrawText(timeCounter, (screenWidth / 2.0f) - 50, screenHeight / 2.0f, 50, YELLOW);]
            if (introTextPosition.y >= -40 * 5)
            {
                DrawText(introText, introTextPosition.x, introTextPosition.y, 40, YELLOW);
                introTextPosition.y -= 0.5;
            }
            else
            {
                DrawText(instructions, (screenWidth / 2) - (MeasureText(instructions, 40) / 2), screenHeight / 2, 40, YELLOW);
                DrawText("Aperte [Enter] para continuar", (screenWidth / 2) - (MeasureText("Aperte [Enter] para continuar", 40) / 2), (screenHeight / 2) + 50, 40, RED);
            }

            if (IsKeyPressed(KEY_ENTER))
                introMode = 0;

            EndDrawing();
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

            // Checando Score
            for (int i = 0; i < enemyAmount; ++i)
            {
                if (enemylifeBars[i].width <= 0)
                {
                    ++score;
                }
            }

            // Tela Transição entre fase cloroquina e boss
            if (score >= enemyAmount && phaseAnnouncement == 0 && bossPhase == 0)
            {
                phaseTransitionTime = GetTime();
                cloroquinaPhase = 0;
                phaseAnnouncement = 1;
                PlaySound(audioBozo); // Renderizar na tela de vitória
            }
            else if (score >= enemyAmount && gameTime - phaseTransitionTime > 33 && bossPhase == 0)
            {
                phaseAnnouncement = 0;
                bossPhase = 1;
            }

            // Drawing mode
            BeginDrawing();
            ClearBackground(BLACK);

            if (phaseAnnouncement)
            {
                DrawText(phaseTransitionText, (screenWidth / 2) - (MeasureText(phaseTransitionText, 70) / 2), (screenHeight / 2) - 50, 70, RED);
            }

            // Modo 2D
            BeginMode2D(camera);

            // Tela Mapa
            if (cloroquinaPhase)
            {
                DrawTextureEx(EnemyMap, (Vector2){.x = -2 * camera.offset.x, .y = -2 * camera.offset.y}, 0, 5, WHITE);
            }
            else if (bossPhase)
            {
                DrawTextureEx(BossMap, (Vector2){.x = -2 * camera.offset.x, .y = -2 * camera.offset.y}, 0, 3, WHITE);
            }

            // Rendering Player - Exceto na phase announcemnt
            if (!phaseAnnouncement)
            {
                if (pLife > 0)
                {
                    // DrawRectangleRec(player1, RED);
                    if (previousX > player1.x)
                    {
                        animationTime += GetFrameTime();
                        if (currentMoveXL == 0)
                        {
                            DrawTexture(movesHorizontal[0][0], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                ++currentMoveXL;
                            }
                        }
                        else if (currentMoveXL == 1)
                        {
                            DrawTexture(movesHorizontal[0][1], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                ++currentMoveXL;
                            }
                        }
                        else if (currentMoveXL == 2)
                        {
                            DrawTexture(movesHorizontal[0][2], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                ++currentMoveXL;
                            }
                        }
                        else if (currentMoveXL == 3)
                        {
                            DrawTexture(movesHorizontal[0][3], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                currentMoveXL = 0;
                            }
                        }
                    }
                    else if (previousX < player1.x)
                    {
                        animationTime += GetFrameTime();
                        if (currentMoveXR == 0)
                        {
                            DrawTexture(movesHorizontal[1][0], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                ++currentMoveXR;
                            }
                        }
                        else if (currentMoveXR == 1)
                        {
                            DrawTexture(movesHorizontal[1][1], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                ++currentMoveXR;
                            };
                        }
                        else if (currentMoveXR == 2)
                        {
                            DrawTexture(movesHorizontal[1][2], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                ++currentMoveXR;
                            }
                        }
                        else if (currentMoveXR == 3)
                        {
                            DrawTexture(movesHorizontal[1][3], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                currentMoveXR = 0;
                            }
                        }
                    }
                    else if (previousY < player1.y)
                    {
                        animationTime += GetFrameTime();
                        if (currentMoveYU == 0)
                        {
                            DrawTexture(movesVertical[0][0], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                ++currentMoveYU;
                            }
                        }
                        else if (currentMoveYU == 1)
                        {
                            DrawTexture(movesVertical[0][1], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                ++currentMoveYU;
                            }
                        }
                        else if (currentMoveYU == 2)
                        {
                            DrawTexture(movesVertical[0][2], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                currentMoveYU = 0;
                            }
                        }
                    }
                    else if (previousY > player1.y)
                    {
                        animationTime += GetFrameTime();
                        if (currentMoveYD == 0)
                        {
                            DrawTexture(movesVertical[1][0], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                ++currentMoveYD;
                            }
                        }
                        else if (currentMoveYD == 1)
                        {
                            DrawTexture(movesVertical[1][1], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                ++currentMoveYD;
                            }
                        }
                        else if (currentMoveYD == 2)
                        {
                            DrawTexture(movesVertical[1][2], player1.x - (player1.width / 1.8), player1.y - (player1.height / 2.5), WHITE);
                            if (animationTime >= 0.2)
                            {
                                animationTime = 0;
                                currentMoveYD = 0;
                            }
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
            }

            // Rendering Enemy - Cloroquinas - Telas Cloroquinas
            if (cloroquinaPhase)
            {
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
                        enemyDinamic(player1, &enemies[i], enemySpeed, &pLife);
                    }
                    enemylifeBars[i].x = enemies[i].x;
                    enemylifeBars[i].y = enemies[i].y - 10;
                }
            }
            else if (bossPhase)
            {
                // TODO Bosses
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

            // Mecânica de vida do player e GameOver -> Nova branch
            if (pLife <= 0)
            {
                if (isAlive)
                {
                    gameOverTime = GetTime();
                    isAlive = 0;
                }

                if (!isAlive && (gameTime - gameOverTime) <= 5)
                {
                    DrawText("  SE FUDEEEU\nCLOROQUINADO!!!", camera.target.x - 800, camera.target.y - 100, 200, RED);
                }
                else
                {
                    gameOverMode = 1;
                }
            }

            // Exibindo barra de vida e score - Exceto na phase announcement
            if (!phaseAnnouncement)
            {
                // Showcasing Life Bar
                lifeBarPosX = camera.target.x - (2 * camera.offset.x);
                lifeBarPosY = camera.target.y - (2 * camera.offset.y);

                DrawTextureEx(playerLifeBar, (Vector2){.x = lifeBarPosX, .y = lifeBarPosY}, 0, 2, WHITE);
                DrawRectangle(lifeBarPosX + 181, lifeBarPosY + 75, pLife, 31, GREEN);

                // Showcasing Score
                sprintf(scoreText, "SCORE: %d", score);
                DrawText(scoreText, lifeBarPosX + 30, lifeBarPosY + playerLifeBar.height + 50, 50, WHITE);
            }

            score = 0;
            EndMode2D();
            EndDrawing();
        }
    }

    // Descarregando texturas movimentos
    for (int i = 0; i < 4; ++i)
    {
        UnloadTexture(movesHorizontal[0][i]);
        UnloadTexture(movesHorizontal[1][i]);

        if (i < 3)
        {
            UnloadTexture(movesVertical[0][i]);
            UnloadTexture(movesVertical[1][i]);
        }
    }

    UnloadSound(audioBozo);

    CloseAudioDevice();
    CloseWindow();
    free(enemies);
    free(enemylifeBars);
    return 0;
}
