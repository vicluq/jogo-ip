#include <raylib.h>
#include <stdio.h>
#include <math.h>

void enemyDinamic(Rectangle player, Rectangle *enemy, int speed, float *playerLifeBar);
float distanceToEnemy(float pX, float pY, float eX, float eY)
{
    float expression = sqrt(((pX - eX) * (pX - eX)) + ((pY - eY) * (pY - eY)));
    return expression;
}

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(0, 0, "Game");
    if (!IsWindowFullscreen())
        ToggleFullscreen();

    const int screenWidth = GetScreenWidth(), screenHeight = GetScreenHeight();
    SetTargetFPS(60);

    // NOTE Camera
    Camera2D camera = {.offset = {screenWidth / 2.0f, screenHeight / 2.0f},
                       .target = {0},
                       .rotation = 0,
                       .zoom = 1};

    // Player related
    Rectangle player1 = {-50, -30, 60, 30};
    const float playerSpeed = 15, pLife = 100;
    float previousX = 0, previousY = 0;

    // Player Life
    Rectangle plifeBar = {10, 10, pLife, 20};
    Rectangle plifeBarBox = {5, 5, pLife + 10, 30};

    // Player Weapons
    Rectangle weaponRight = {player1.x, player1.y, 0, 0};
    Rectangle weaponLeft = {player1.x, player1.y, 0, 0};

    // Enemies
    Rectangle enemy = {1000, 120, 40, 40};
    Rectangle elifeBar = {1000, 130, 100, 5};
    const int enemySpeed = 15;

    // NOTE Ciclo do jogo
    while (!WindowShouldClose())
    {
        // Player Motion
        previousY = player1.y;
        previousX = player1.x;

        if (IsKeyPressed(KEY_W) || IsKeyDown(KEY_W))
        {
            player1.y -= playerSpeed * 0.3;
        }

        if (IsKeyPressed(KEY_D) || IsKeyDown(KEY_D))
        {
            player1.x += playerSpeed * 0.3;
        }

        if (IsKeyPressed(KEY_S) || IsKeyDown(KEY_S))
        {
            player1.y += playerSpeed * 0.3;
        }

        if (IsKeyPressed(KEY_A) || IsKeyDown(KEY_A))
        {
            player1.x -= playerSpeed * 0.3;
        }

        // Enemy Dinamic
        if (distanceToEnemy(player1.x, player1.y, enemy.x, enemy.y) <= 300)
        {
            enemyDinamic(player1, &enemy, enemySpeed, &plifeBar.width);
        }

        elifeBar.x = enemy.x;
        elifeBar.y = enemy.y - 10;

        //  Camera
        if ((previousY != player1.y) || (previousX != player1.x))
        {
            camera.target.x += ((player1.x) - camera.target.x) * 0.025;
            camera.target.y += ((player1.y) - camera.target.y) * 0.025;
        }

        // Drawing mode
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawRectangleRec(plifeBarBox, BLACK);
        DrawRectangleRec(plifeBar, GREEN);

        if (plifeBar.width < 0)
        {
            DrawText("SE FUDEEEU", (screenWidth / 2) - 600, (screenHeight / 2) - 100, 200, RED);
        }

        if (CheckCollisionRecs(weaponRight, enemy) || CheckCollisionRecs(weaponLeft, enemy))
        {
            DrawText("COLISÃOOOO", screenWidth / 2, 10, 20, RED);
            elifeBar.width -= 10;
        }

        // NOTE 2d
        BeginMode2D(camera);
        DrawText("ISSO É MELHOR QUE LOL PPRT", 0 - 600, 0, 80, BLACK);

        // Rendering Player
        if (plifeBar.width > 0)
        {
            DrawRectangleRec(player1, RED);
        }

        // Rendering Enemy
        if (elifeBar.width > 0)
        {
            DrawRectangleRec(enemy, BLACK);
            DrawRectangleRec(elifeBar, GREEN);
        }
        else
        {
            enemy.x = 9999999;
            enemy.y = 9999999;
        }

        // Combat mechanics
        if (IsKeyPressed(KEY_K))
        {
            weaponRight.width = 30;
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
            weaponLeft.width = 30;
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

    return 0;
}

void renderBasics()
{
}

void enemyDinamic(Rectangle player, Rectangle *enemy, int speed, float *playerLifeBar)
{
    if (player.x - enemy->x > 0)
    {
        enemy->x += (float)speed * 0.1;
    }
    else if (player.x - enemy->x < 0)
    {
        enemy->x -= (float)speed * 0.1;
    }

    if (player.y - enemy->y > 0)
    {
        enemy->y += (float)speed * 0.1;
    }
    else if (player.y - enemy->y < 0)
    {
        enemy->y -= (float)speed * 0.1;
    }

    if (CheckCollisionRecs(player, *enemy) && enemy->x >= player.x && enemy->y >= player.y)
    {
        *playerLifeBar -= 10;
        enemy->x += 90;
        enemy->y += 90;
    }
    if (CheckCollisionRecs(player, *enemy) && enemy->x <= player.x && enemy->y >= player.y)
    {
        *playerLifeBar -= 10;
        enemy->x -= 90;
        enemy->y += 90;
    }
    if (CheckCollisionRecs(player, *enemy) && enemy->x >= player.x && enemy->y <= player.y)
    {
        *playerLifeBar -= 10;
        enemy->x += 90;
        enemy->y -= 90;
    }
    if (CheckCollisionRecs(player, *enemy) && enemy->x <= player.x && enemy->y <= player.y)
    {
        *playerLifeBar -= 10;
        enemy->x -= 90;
        enemy->y -= 90;
    }

    // if ((enemy->x <= pX + 60 && *eX >= pX) && )
    // {
    //     *eX += 90;
    //     *eY += 90;
    // }
    // if ((*eX + 40 >= pX && *eX + 40 <= pX + 60) && )
    //     {
    //         *eX += 90;
    //         *eY += 90;
    //     }
    // if (*eX == pX && *eY == pY)
    // {
    //     *eX += 120;
    //     *eY += 120;
    // }
    // if (*eY >= pY + 30)
    // {
    //     *eX -= 30;
    //     *eY -= 30;
    // }
}
/*
   TODO Inimigo
    Movimentação do inimigo similar a câmera, para perseguir o personagem
    Comparação de posição inimigo-personagem para gerar movimento do inimigo
*/