#include <raylib.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "utils.h"

// Misc Utilities
float distanceToEnemy(float pX, float pY, float eX, float eY)
{
    float expression = sqrt(((pX - eX) * (pX - eX)) + ((pY - eY) * (pY - eY)));
    return expression;
}

// Menu Utilities
char GameTitle[31] = "ANTI BOZO PATROL\n";
char GameCredits[51] = "Created by computer engeneering students at UFPE ©";
void renderMenu(Vector2 *mouse, int screenWidth, int screenHeight, Rectangle menuOptions[2], int *menuMode, int *closeGame)
{
    *mouse = GetMousePosition();

    BeginDrawing();

    ClearBackground(WHITE);

    DrawText(GameTitle, (screenWidth / 2) - (MeasureText(GameTitle, 70) / 2), 300, 70, BLUE);
    DrawText(GameCredits, screenWidth / 2 - (MeasureText(GameTitle, 70) / 2.5), screenHeight - 40, 20, BLACK);

    if (CheckCollisionPointRec(*mouse, menuOptions[0]))
        DrawRectangleRec(menuOptions[0], GREEN);
    else
        DrawRectangleRec(menuOptions[0], BLUE);

    if (CheckCollisionPointRec(*mouse, menuOptions[1]))
        DrawRectangleRec(menuOptions[1], GREEN);
    else
        DrawRectangleRec(menuOptions[1], BLUE);

    DrawText("Start", menuOptions[0].x + (MeasureText("Start", 70) / 2), menuOptions[0].y + (menuOptions[0].height / 4.2), 70, BLACK);
    DrawText("Close", menuOptions[1].x + (MeasureText("Close", 70) / 2), menuOptions[1].y + (menuOptions[1].height / 4.2), 70, BLACK);

    // Mouse
    DrawCircleV(*mouse, 10, GREEN);

    if (CheckCollisionPointRec(*mouse, menuOptions[0]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *menuMode = 0;
    }

    if (CheckCollisionPointRec(*mouse, menuOptions[1]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *closeGame = 1;
    }

    EndDrawing();
}

// Enemy Utilities
void enemyDinamic(Rectangle player, Rectangle *enemy, int speed, float *playerLifeBar)
{

    if (player.x - enemy->x > 0)
    {
        enemy->x += (float)speed * 0.2;
    }
    else if (player.x - enemy->x < 0)
    {
        enemy->x -= (float)speed * 0.2;
    }

    if (player.y - enemy->y > 0)
    {
        enemy->y += (float)speed * 0.2;
    }
    else if (player.y - enemy->y < 0)
    {
        enemy->y -= (float)speed * 0.2;
    }

    if (CheckCollisionRecs(player, *enemy) && enemy->x >= player.x && enemy->y >= player.y)
    {
        printf("COLIDIU\n");
        *playerLifeBar -= 10;
        enemy->x += 150;
        enemy->y += 150;
    }
    if (CheckCollisionRecs(player, *enemy) && enemy->x <= player.x && enemy->y >= player.y)
    {
        printf("COLIDIU\n");
        *playerLifeBar -= 10;
        enemy->x -= 150;
        enemy->y += 150;
    }
    if (CheckCollisionRecs(player, *enemy) && enemy->x >= player.x && enemy->y <= player.y)
    {
        printf("COLIDIU\n");
        *playerLifeBar -= 10;
        enemy->x += 150;
        enemy->y -= 150;
    }
    if (CheckCollisionRecs(player, *enemy) && enemy->x <= player.x && enemy->y <= player.y)
    {
        printf("COLIDIU\n");
        *playerLifeBar -= 10;
        enemy->x -= 150;
        enemy->y -= 150;
    }
}
