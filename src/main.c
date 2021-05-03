#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "./utils/utils.h"

typedef struct
{
    float initialX, initialY;
    Rectangle weapon;
} LongDistWeapon;

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(0, 0, "Game");
    InitAudioDevice();
    SetTargetFPS(60);
    const int screenWidth = GetScreenWidth(), screenHeight = GetScreenHeight();

    if (!IsWindowFullscreen())
    {
        ToggleFullscreen();
    }

    // General elements
    float generalFontSize = screenWidth / 50;

    // HUD
    char timerDisplay[40], scoreText[25], vacAvaliableText[15];
    Texture2D vaccineTexture = LoadTexture("./assets/weapons/vac_hud.png");

    // Tempo Related
    int playingSound = 1;
    SetMasterVolume(0.5f);
    double PROCESS_TIME = 0, gameTime = 0, introStart = 0, gameOverTime = 0, gameInitTime = 0;

    // Telas e controles de transição
    int menuMode = 1, closeGame = 0, introMode = 0, winMode = 0;
    int bossPhase = 0, phaseAnnouncement = 0, cloroquinaPhase = 1;

    double phaseTransitionTime = 0;
    char phaseTransitionText[] = "PREPARE-SE PARA A BATALHA FINAL\nDERROTE O BOSTÃO E OS BOSTINHAS";

    Vector2 introTextPosition = {.x = (screenWidth / 2) - (MeasureText(introText, generalFontSize) / 2.2), .y = (screenHeight / 2) - (20 * 4)};

    // Sounds
    Sound openingSW = LoadSound("./assets/sw-opening.wav");
    Sound audioBozo = LoadSound("./assets/bozo-pal.wav");
    Sound bozoLaugh = LoadSound("./assets/bozo-laugh.wav");
    Sound cloroquinaSound = LoadSound("./assets/dramatic.wav");
    Sound bossSound = LoadSound("./assets/got-audio.wav");

    // Mouse
    Vector2 mouse = {.x = 0, .y = 0};

    // Menu Elements
    Texture2D menuCape = LoadTexture("./assets/menu.png");
    Rectangle menuOptions[2] = {
        {.height = screenHeight / 10, .width = screenWidth / 4.8, .x = (screenWidth / 2) - 200, .y = (screenHeight / 2) - 50},
        {.height = screenHeight / 10, .width = screenWidth / 4.8, .x = (screenWidth / 2) - 200, .y = menuOptions[0].y + (1.5 * menuOptions[0].height)},
    };

    // Map
    Texture2D EnemyMap = LoadTexture("./assets/cloroquina-map.png");
    Texture2D BossMap = LoadTexture("./assets/map-boss.png");
    Texture2D WinMap = LoadTexture("./assets/HUD/LulaWin.png");
    Texture2D LoseMap = LoadTexture("./assets/HUD/bozo-lose.png");
    Texture2D FireCircle = LoadTexture("./assets/HUD/fire-map.png");

    // Camera
    Camera2D camera = {.offset = {screenWidth / 2.0f, screenHeight / 2.0f},
                       .target = {.x = 4045, .y = 1720},
                       .rotation = 0,
                       .zoom = 0.5};

    // Player related
    char filename[31], posText[50];
    Texture movesHorizontal[2][4];
    Texture movesVertical[2][3];
    int currentMoveXL = 0, currentMoveXR = 0, currentMoveYU = 0, currentMoveYD = 0;
    int setNewPhaseCoord = 0;

    for (int j = 1; j <= 4; ++j)
    {
        sprintf(filename, "./assets/LulaBase/baseA1%d.png", j);
        movesHorizontal[0][j - 1] = LoadTexture(filename);

        sprintf(filename, "./assets/LulaBase/baseA2%d.png", j);
        movesHorizontal[1][j - 1] = LoadTexture(filename);
    }

    for (int j = 1; j <= 3; ++j)
    {
        sprintf(filename, "./assets/LulaBase/baseB1%d.png", j);
        movesVertical[0][j - 1] = LoadTexture(filename);

        sprintf(filename, "./assets/LulaBase/baseB2%d.png", j);
        movesVertical[1][j - 1] = LoadTexture(filename);
    }

    Rectangle player1 = {4045, 1720, movesHorizontal[1][1].width / 2.0f, movesHorizontal[1][1].height / 1.7f};
    int score = 0;
    const float playerSpeed = 15;
    float previousX = 0, previousY = 0, animationTime = 0.0f, pLife = 795;

    // Player Life
    int isAlive = 1;
    Texture playerLifeBar = LoadTexture("./assets/HUD/barra.png");
    float lifeBarPosX = 0, lifeBarPosY = 0;

    // Player Weapons
    Texture2D GunLeft = LoadTexture("./assets/weapons/vac_L.png");
    Texture2D GunRight = LoadTexture("./assets/weapons/vac_D.png");
    Rectangle weaponRight = {player1.x, player1.y, 0, 0};
    Rectangle weaponLeft = {player1.x, player1.y, 0, 0};

    int shootingVac = 0, vacCollided = 0;
    const int vaccineAmount = 10;
    int vaccinesAvaliable = vaccineAmount;
    Texture2D vacLeft = LoadTexture("./assets/weapons/vac_L.png");
    Texture2D vacRight = LoadTexture("./assets/weapons/vac_D.png");
    LongDistWeapon vaccines[vaccineAmount];

    for (int k = 0; k < vaccineAmount; ++k)
    {
        vaccines[k].initialX = 0;
        vaccines[k].initialY = 0;
        vaccines[k].weapon.height = 0;
        vaccines[k].weapon.width = 0;
        vaccines[k].weapon.x = player1.x;
        vaccines[k].weapon.y = player1.y;
    }

    // Enemies
    const int enemySpeed = 15, enemyAmount = 20;
    Texture cloroquina = LoadTexture("./assets/Cloroquina/cloroquina-enemy.png");
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

    // Bosses (caluxo, dudu, fla, ren)
    char bossFilename[50];
    Texture2D bossesTexture[4];
    for (int i = 0; i < 4; ++i)
    {
        sprintf(bossFilename, "./assets/Bolsonaros/boss-%d.png", i + 1);
        bossesTexture[i] = LoadTexture(bossFilename);
    }

    Rectangle bosses[4] = {
        {.x = 626, .y = 850, .width = bossesTexture[0].width * 0.7, .height = bossesTexture[0].height * 0.7},
        {.x = 4730, .y = 1030, .width = bossesTexture[1].width * 0.7, .height = bossesTexture[1].height * 0.7},
        {.x = 660, .y = 2800, .width = bossesTexture[2].width * 0.7, .height = bossesTexture[2].height * 0.7},
        {.x = 4745, .y = 2913, .width = bossesTexture[3].width * 0.7, .height = bossesTexture[3].height * 0.7}};
    Rectangle bossesLifeBars[4] = {
        {.x = bosses[0].x, .y = bosses[0].y + 20, .width = bossesTexture[0].width * 0.7, .height = 20},
        {.x = bosses[1].x, .y = bosses[1].y + 20, .width = bossesTexture[1].width * 0.7, .height = 20},
        {.x = bosses[2].x, .y = bosses[2].y + 20, .width = bossesTexture[2].width * 0.7, .height = 20},
        {.x = bosses[3].x, .y = bosses[3].y + 20, .width = bossesTexture[3].width * 0.7, .height = 20}};

    // Main Boss
    Texture2D BolsonaroBoss = LoadTexture("./assets/Bolsonaros/boss-main.png");
    Rectangle BolsonaroRec = {.x = 2700, .y = 1750, .width = BolsonaroBoss.width, .height = BolsonaroBoss.height};
    Rectangle BolsonaroLifeBar = {.x = BolsonaroRec.x, .y = BolsonaroRec.y + 20, .width = BolsonaroBoss.width * 1, .height = 20};

    // Ciclo do jogos
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
            if (playingSound)
            {
                PlaySound(openingSW);
                playingSound = 0;
            }

            if (introTextPosition.y >= -40 * 5)
            {
                DrawText(introText, introTextPosition.x, introTextPosition.y, generalFontSize, YELLOW);
                introTextPosition.y -= 0.5;
            }
            else
            {
                DrawText(instructions, (screenWidth / 2) - (MeasureText(instructions, generalFontSize) / 2), screenHeight / 2, generalFontSize, YELLOW);
                DrawText("Aperte [Enter] para continuar", (screenWidth / 2) - (MeasureText("Aperte [Enter] para continuar", generalFontSize) / 2), (screenHeight / 2) + 50, generalFontSize, RED);
            }

            if (IsKeyPressed(KEY_ENTER))
            {
                gameInitTime = GetTime();
                introMode = 0;
                playingSound = 1;
            }

            EndDrawing();
        }
        else if (winMode && !menuMode && !introMode)
        {

            BeginDrawing();

            if (winMode == 1)
            {
                StopSound(bossSound);

                if (playingSound)
                {
                    printf("AUDIO BOZO TRISTE\n");
                    PlaySound(audioBozo);
                    playingSound = 0;
                }

                WinMap.width = screenWidth;
                WinMap.height = screenHeight;

                ClearBackground(BLACK);
                DrawTextureEx(WinMap, (Vector2){.x = 0, .y = 0}, 0, 1, WHITE);
                DrawText("VOCÊ DERROTOU O BOZO", (screenWidth / 2) - (MeasureText("VOCÊ DERROTOU O BOZO", screenWidth / 30) / 2), screenHeight / 2, screenWidth / 30, GREEN);

                sprintf(timerDisplay, "você terminou a sua missão em %.2lfs", gameTime - gameInitTime);
                DrawText(timerDisplay, (screenWidth / 2) - (MeasureText(timerDisplay, generalFontSize) / 2), (screenHeight / 2) + 80, generalFontSize, YELLOW);
            }
            else if (winMode == -1)
            {

                if (cloroquinaPhase && !bossPhase)
                {
                    printf("BYE MUSICA DO CLOROQUINA VEM BOZO RINDO\n");
                    StopSound(cloroquinaSound);
                }
                else
                {
                    printf("BYE MUSICA DO BOSS VEM BOZO RINDO\n");
                    StopSound(bossSound);
                }

                if (playingSound)
                {
                    printf("BYE MUSICA DO BOSS VEM BOZO XINGANDO\n");
                    PlaySound(bozoLaugh);
                    playingSound = 0;
                }

                LoseMap.width = screenWidth;
                LoseMap.height = screenHeight;

                ClearBackground(BLACK);
                DrawTextureEx(LoseMap, (Vector2){.x = 0, .y = 0}, 0, 1, WHITE);
                DrawText("VOCÊ PERDEU PARA O BOZO", (screenWidth / 2) - (MeasureText("VOCÊ PERDEU PARA O BOZO", 70) / 2), screenHeight / 2, screenWidth / 30, RED);
            }

            EndDrawing();
        }
        else
        {
            gameTime = GetTime();
            StopSound(openingSW);

            // Checando Score
            for (int i = 0; i < enemyAmount; ++i)
            {
                if (enemylifeBars[i].width <= 0)
                {
                    ++score;
                }
            }

            for (int i = 0; i < 4; ++i)
            {
                if (bossesLifeBars[i].width <= 0)
                {
                    ++score;
                }
            }

            if (BolsonaroLifeBar.width <= 0)
            {
                ++score;
            }

            if (score == enemyAmount + 5)
            {
                if (!playingSound)
                {
                    playingSound = 1;
                }
                // Parando musica da fase anterior
                winMode = 1;
            }

            // Player Motion
            previousY = player1.y;
            previousX = player1.x;

            if (setNewPhaseCoord)
            {
                player1.x = BossMap.width / 2;
                player1.y = BossMap.height / 2;

                camera.target.x = BossMap.width / 2;
                camera.target.y = BossMap.height / 2;

                vaccinesAvaliable = vaccineAmount;

                setNewPhaseCoord = 0;
            }

            if (IsKeyPressed(KEY_W) || IsKeyDown(KEY_W))
            {
                if (bossPhase)
                {
                    if (player1.y - (playerSpeed * 0.55) >= 450)
                    {
                        if (score < enemyAmount + 4)
                        {
                            if (!(player1.y - (playerSpeed * 0.55) <= 2390 && player1.y - (playerSpeed * 0.55) >= 1185 && player1.x >= 2120 && player1.x <= 3330))
                            {
                                player1.y -= playerSpeed * 0.55;
                            }
                        }
                        else
                        {

                            player1.y -= playerSpeed * 0.55;
                        }
                    }
                }
                else if (cloroquinaPhase && !bossPhase)
                {
                    if (player1.y - (playerSpeed * 0.55) >= -45)
                    {
                        player1.y -= playerSpeed * 0.55;
                    }
                }
            }

            if (IsKeyPressed(KEY_D) || IsKeyDown(KEY_D))
            {
                if (bossPhase)
                {
                    if (player1.x + (playerSpeed * 0.55) <= 5150)
                    {
                        if (score < enemyAmount + 4)
                        {
                            if (!(player1.x + (playerSpeed * 0.55) > 2120 && player1.x + (playerSpeed * 0.55) < 3330 && player1.y >= 1200 && player1.y <= 2390))
                            {
                                player1.x += playerSpeed * 0.55;
                            }
                        }
                        else
                        {

                            player1.x += playerSpeed * 0.55;
                        }
                    }
                }
                else if (cloroquinaPhase && !bossPhase)
                {

                    if (player1.x + (playerSpeed * 0.55) <= 8020)
                    {
                        player1.x += playerSpeed * 0.55;
                    }
                }
            }

            if (IsKeyPressed(KEY_S) || IsKeyDown(KEY_S))
            {
                if (bossPhase)
                {
                    if (player1.y + (playerSpeed * 0.55) <= 3194)
                    {
                        if (score < enemyAmount + 4)
                        {
                            if (!(player1.y + (playerSpeed * 0.55) >= 1185 && player1.y + (playerSpeed * 0.55) <= 2390 && player1.x >= 2120 && player1.x <= 3330))
                            {
                                player1.y += playerSpeed * 0.55;
                            }
                        }
                        else
                        {
                            player1.y += playerSpeed * 0.55;
                        }
                    }
                }
                else if (cloroquinaPhase && !bossPhase)
                {
                    if (player1.y + (playerSpeed * 0.55) <= 3830)
                    {
                        player1.y += playerSpeed * 0.55;
                    }
                }
            }

            if (IsKeyPressed(KEY_A) || IsKeyDown(KEY_A))
            {
                if (bossPhase)
                {
                    if (player1.x - (playerSpeed * 0.55) >= 300)
                    {
                        if (score < enemyAmount + 4)
                        {
                            if (!(player1.x - (playerSpeed * 0.55) < 3330 && player1.x - (playerSpeed * 0.55) > 2120 && player1.y >= 1200 && player1.y <= 2390))
                            {
                                player1.x -= playerSpeed * 0.55;
                            }
                        }
                        else
                        {

                            player1.x -= playerSpeed * 0.55;
                        }
                    }
                }
                else if (cloroquinaPhase && !bossPhase)
                {
                    if (player1.x - (playerSpeed * 0.55) >= 80)
                    {
                        player1.x -= playerSpeed * 0.55;
                    }
                }
            }

            //  Camera
            if ((previousY != player1.y) || (previousX != player1.x))
            {
                camera.target.x += ((player1.x) - camera.target.x) * 0.025;
                camera.target.y += ((player1.y) - camera.target.y) * 0.025;
            }

            // Tela Transição entre fase cloroquina e boss
            if (score >= enemyAmount && phaseAnnouncement == 0 && bossPhase == 0)
            {
                phaseTransitionTime = GetTime();
                cloroquinaPhase = 0;
                phaseAnnouncement = 1;
            }
            else if (score >= enemyAmount && gameTime - phaseTransitionTime > 5 && bossPhase == 0)
            {
                phaseAnnouncement = 0;
                bossPhase = 1;
                setNewPhaseCoord = 1;
            }

            // Drawing mode
            BeginDrawing();
            ClearBackground(BLACK);

            // Tela anuncio de mudança de fase
            if (phaseAnnouncement)
            {
                if (!playingSound)
                {
                    StopSound(cloroquinaSound); // Setando para a proxima fase
                    playingSound = 1;           // Parando musica da fase anterior
                }

                DrawText(phaseTransitionText, (screenWidth / 2) - (MeasureText(phaseTransitionText, screenWidth / 30) / 2), (screenHeight / 2) - 50, screenWidth / 30, RED);
            }

            // Modo 2D
            BeginMode2D(camera);

            // Tela Mapa
            if (cloroquinaPhase)
            {
                if (playingSound)
                {
                    PlaySound(cloroquinaSound);
                    playingSound = 0;
                }

                DrawTextureEx(EnemyMap, (Vector2){.x = 0, .y = 0}, 0, 2, WHITE);
            }
            else if (bossPhase && score < enemyAmount + 5)
            {
                if (playingSound)
                {
                    PlaySound(bossSound);
                    playingSound = 0;
                }

                DrawTextureEx(BossMap, (Vector2){.x = 0, .y = 0}, 0, 1.8, WHITE);
                if (score < enemyAmount + 4)
                {
                    DrawTextureEx(FireCircle, (Vector2){.x = 0, .y = 0}, 0, 1.8, WHITE);
                }
            }

            // Rendering Player - Exceto na phase announcemnt
            if (!phaseAnnouncement)
            {
                if (pLife > 0)
                {
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
                    if (CheckCollisionRecs(vaccines[vaccinesAvaliable - 1].weapon, enemies[i]))
                    {
                        enemylifeBars[i].width -= 30;
                        vacCollided = 1;
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
                        enemyDynamic(player1, &enemies[i], enemySpeed, &pLife);
                    }
                    enemylifeBars[i].x = enemies[i].x;
                    enemylifeBars[i].y = enemies[i].y - 10;
                }
            }
            else if (bossPhase)
            {
                // Bozo Dynamic
                if (BolsonaroLifeBar.width > 0)
                {
                    DrawTextureEx(BolsonaroBoss, (Vector2){.x = BolsonaroRec.x, .y = BolsonaroRec.y}, 0, 1, WHITE);
                    DrawRectangleRec(BolsonaroLifeBar, GREEN);
                }
                if ((CheckCollisionRecs(weaponRight, BolsonaroRec) || CheckCollisionRecs(weaponLeft, BolsonaroRec)) && score == enemyAmount + 4)
                {
                    BolsonaroLifeBar.width -= 10;
                }
                if (CheckCollisionRecs(vaccines[vaccinesAvaliable - 1].weapon, BolsonaroRec) && score == enemyAmount + 4)
                {
                    BolsonaroLifeBar.width -= 30;
                    vacCollided = 1;
                }
                if ((distanceToEnemy(player1.x, player1.y, BolsonaroRec.x, BolsonaroRec.y) <= 1000) && score == enemyAmount + 4)
                {
                    enemyDynamic(player1, &BolsonaroRec, enemySpeed * 1.15, &pLife);
                }

                BolsonaroLifeBar.x = BolsonaroRec.x;
                BolsonaroLifeBar.y = BolsonaroRec.y - 20;

                // Bozo Kids
                for (int i = 0; i < 4; ++i)
                {
                    if (CheckCollisionRecs(weaponRight, bosses[i]) || CheckCollisionRecs(weaponLeft, bosses[i]))
                    {
                        bossesLifeBars[i].width -= 10;
                    }
                    if (CheckCollisionRecs(vaccines[vaccinesAvaliable - 1].weapon, bosses[i]))
                    {
                        bossesLifeBars[i].width -= 30;
                        vacCollided = 1;
                    }
                    // Rendering each enemy box
                    if (bossesLifeBars[i].width > 0)
                    {
                        DrawTextureEx(bossesTexture[i], (Vector2){.x = bosses[i].x, .y = bosses[i].y}, 0, 0.7, WHITE);
                        DrawRectangleRec(bossesLifeBars[i], GREEN);
                    }
                    else
                    {
                        bosses[i].x = 9999999;
                        bosses[i].y = 9999999;
                    }

                    // Enemy Dynamic
                    if (distanceToEnemy(player1.x, player1.y, bosses[i].x, bosses[i].y) <= 1000)
                    {
                        enemyDynamic(player1, &bosses[i], enemySpeed * 1.15, &pLife);
                    }

                    bossesLifeBars[i].x = bosses[i].x;
                    bossesLifeBars[i].y = bosses[i].y - 20;
                }
            }

            // Mecânica de vida do player e GameOver
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
                    if (!playingSound)
                    {
                        playingSound = 1; // Parando musica da fase anterior
                    }
                    winMode = -1;
                }
            }

            // Exibindo barra de vida, score e ataques - Exceto na phase announcement
            if (!phaseAnnouncement)
            {
                // Combat mechanics
                if (IsKeyPressed(KEY_K))
                {
                    weaponRight.width = GunRight.width * 0.3;
                    weaponRight.height = GunRight.height * 0.3;
                    weaponRight.x = player1.x + player1.width;
                    weaponRight.y = player1.y + (player1.height / 2);
                    DrawTextureEx(GunRight, (Vector2){.x = weaponRight.x, .y = weaponRight.y}, 0, 0.3, WHITE);
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
                    weaponLeft.width = GunLeft.width * 0.3;
                    weaponLeft.height = GunLeft.height * 0.3;
                    weaponLeft.x = player1.x - weaponLeft.width;
                    weaponLeft.y = player1.y + (player1.height / 2);
                    DrawTextureEx(GunLeft, (Vector2){.x = weaponLeft.x, .y = weaponLeft.y}, 0, 0.3, WHITE);
                }
                else
                {
                    weaponLeft.width = 0;
                    weaponLeft.height = 0;
                    weaponLeft.x = player1.x + (player1.width / 2);
                    weaponLeft.y = player1.y + (player1.height / 2);
                }

                if (IsKeyPressed(KEY_N))
                {
                    if (vaccinesAvaliable > 0 && !shootingVac)
                    {
                        vaccines[vaccinesAvaliable - 1].weapon.width = vacRight.width * 0.3;
                        vaccines[vaccinesAvaliable - 1].weapon.height = vacRight.height * 0.3;

                        vaccines[vaccinesAvaliable - 1].initialX = player1.x + player1.width;
                        vaccines[vaccinesAvaliable - 1].initialY = player1.y + 2;

                        vaccines[vaccinesAvaliable - 1].weapon.x = player1.x + player1.width;
                        vaccines[vaccinesAvaliable - 1].weapon.y = player1.y + 2;
                        shootingVac = 1;
                    }
                }
                else if (!shootingVac && vaccinesAvaliable > 0)
                {
                    vaccines[vaccinesAvaliable - 1].weapon.width = 0;
                    vaccines[vaccinesAvaliable - 1].weapon.height = 0;

                    vaccines[vaccinesAvaliable - 1].weapon.x = player1.x + (player1.width / 2);
                    vaccines[vaccinesAvaliable - 1].weapon.y = player1.y + (player1.height / 2);
                }

                if (IsKeyPressed(KEY_M))
                {
                    if (vaccinesAvaliable > 0 && !shootingVac)
                    {
                        vaccines[vaccinesAvaliable - 1].weapon.width = vacRight.width * 0.3;
                        vaccines[vaccinesAvaliable - 1].weapon.height = vacRight.height * 0.3;

                        vaccines[vaccinesAvaliable - 1].initialX = player1.x + player1.width;
                        vaccines[vaccinesAvaliable - 1].initialY = player1.y + 2;

                        vaccines[vaccinesAvaliable - 1].weapon.x = player1.x + player1.width;
                        vaccines[vaccinesAvaliable - 1].weapon.y = player1.y + 2;
                        shootingVac = 2;
                    }
                }
                else if (!shootingVac && vaccinesAvaliable > 0)
                {
                    vaccines[vaccinesAvaliable - 1].weapon.width = 0;
                    vaccines[vaccinesAvaliable - 1].weapon.height = 0;

                    vaccines[vaccinesAvaliable - 1].weapon.x = player1.x + (player1.width / 2);
                    vaccines[vaccinesAvaliable - 1].weapon.y = player1.y + (player1.height / 2);
                }

                if (shootingVac)
                {
                    // Criar função para checar isso
                    if (distanceToEnemy(vaccines[vaccinesAvaliable - 1].weapon.x, 0, vaccines[vaccinesAvaliable - 1].initialX, 0) < 1600 && !vacCollided)
                    {

                        if (shootingVac == 1)
                        {
                            DrawTextureEx(vacLeft, (Vector2){.x = vaccines[vaccinesAvaliable - 1].weapon.x, .y = vaccines[vaccinesAvaliable - 1].weapon.y}, 0, 0.3, WHITE);
                            vaccines[vaccinesAvaliable - 1].weapon.x -= enemySpeed * 0.8;
                        }
                        else if (shootingVac == 2)
                        {
                            DrawTextureEx(vacRight, (Vector2){.x = vaccines[vaccinesAvaliable - 1].weapon.x, .y = vaccines[vaccinesAvaliable - 1].weapon.y}, 0, 0.3, WHITE);
                            vaccines[vaccinesAvaliable - 1].weapon.x += enemySpeed * 0.8;
                        }
                    }
                    else
                    {
                        vaccines[vaccinesAvaliable - 1].weapon.x = 999999;
                        vaccines[vaccinesAvaliable - 1].weapon.y = 999999;
                        --vaccinesAvaliable;
                        shootingVac = 0;
                        vacCollided = 0;
                    }
                }

                // Showcasing Life Bar
                lifeBarPosX = camera.target.x - (2 * camera.offset.x);
                lifeBarPosY = camera.target.y - (2 * camera.offset.y);

                DrawTextureEx(playerLifeBar, (Vector2){.x = lifeBarPosX, .y = lifeBarPosY}, 0, 2, WHITE);
                DrawRectangle(lifeBarPosX + 181, lifeBarPosY + 75, pLife, 31, GREEN);

                // Showcasing Score
                sprintf(scoreText, "SCORE: %d", score);
                DrawText(scoreText, lifeBarPosX + 30, lifeBarPosY + playerLifeBar.height + 50, 50, WHITE);

                // Showcasing Vacs Avaliable
                sprintf(vacAvaliableText, "x%d", vaccinesAvaliable);
                DrawTextureEx(vaccineTexture, (Vector2){.x = lifeBarPosX + 100 + MeasureText(scoreText, 50), .y = lifeBarPosY + playerLifeBar.height + 50}, 0, 0.3, WHITE);
                DrawText(vacAvaliableText, lifeBarPosX + 100 + MeasureText(scoreText, 50) + (vaccineTexture.width * 0.3), lifeBarPosY + playerLifeBar.height + 65, 50, WHITE);

                // Showcasing Time Elapsed
                sprintf(timerDisplay, "timer: %.2lf", gameTime - gameInitTime);
                DrawText(timerDisplay, lifeBarPosX + 30, lifeBarPosY + playerLifeBar.height + 120, 50, WHITE);
            }

            score = 0;
            EndMode2D();
            EndDrawing();
        }
    }

    // Descarregando texturas e sons
    for (int i = 0; i < 4; ++i)
    {
        UnloadTexture(movesHorizontal[0][i]);
        UnloadTexture(movesHorizontal[1][i]);

        UnloadTexture(bossesTexture[i]);

        if (i < 3)
        {
            UnloadTexture(movesVertical[0][i]);
            UnloadTexture(movesVertical[1][i]);
        }
    }

    UnloadTexture(menuCape);
    UnloadTexture(EnemyMap);
    UnloadTexture(BossMap);
    UnloadTexture(WinMap);
    UnloadTexture(LoseMap);
    UnloadTexture(FireCircle);
    UnloadTexture(playerLifeBar);
    UnloadTexture(GunLeft);
    UnloadTexture(GunRight);
    UnloadTexture(vacLeft);
    UnloadTexture(vacRight);
    UnloadTexture(cloroquina);
    UnloadTexture(BolsonaroBoss);

    UnloadSound(openingSW);
    UnloadSound(audioBozo);
    UnloadSound(bossSound);
    UnloadSound(cloroquinaSound);

    CloseAudioDevice();
    CloseWindow();
    free(enemies);
    free(enemylifeBars);
    return 0;
}
