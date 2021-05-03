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
char GameTitle[80] = "              CUCABELUDO EM:\n LUTA CONTRA O CAPITÃO BOSTÃONARO\n";
char GameCredits[51] = "Created by computer engeneering students at UFPE ©";

void renderMenu(Texture2D MenuCape, Vector2 *mouse, int screenWidth, int screenHeight, Rectangle menuOptions[2], int *menuMode, int *introMode, double *introStart, int *closeGame)
{
    *mouse = GetMousePosition();

    BeginDrawing();

    ClearBackground(WHITE);

    MenuCape.width = screenWidth;
    MenuCape.height = screenHeight;
    DrawTextureEx(MenuCape, (Vector2){.x = 0, .y = 0}, 0, 1, WHITE);

    DrawText(GameTitle, (screenWidth / 2) - (MeasureText(GameTitle, 50) / 2), screenHeight / 4, 50, WHITE);
    DrawText(GameCredits, (screenWidth / 2) - (MeasureText(GameTitle, 20) / 1.5), screenHeight - 80, 20, WHITE);

    if (CheckCollisionPointRec(*mouse, menuOptions[0]))
        DrawRectangleRec(menuOptions[0], GREEN);
    else
        DrawRectangleRec(menuOptions[0], BLUE);

    if (CheckCollisionPointRec(*mouse, menuOptions[1]))
        DrawRectangleRec(menuOptions[1], GREEN);
    else
        DrawRectangleRec(menuOptions[1], BLUE);

    DrawText("Start", menuOptions[0].x + (MeasureText("Start", screenWidth / 30) / 2), menuOptions[0].y + (menuOptions[0].height / 4.2), screenWidth / 30, BLACK);
    DrawText("Close", menuOptions[1].x + (MeasureText("Close", screenWidth / 30) / 2), menuOptions[1].y + (menuOptions[1].height / 4.2), screenWidth / 30, BLACK);

    // Mouse
    DrawCircleV(*mouse, 10, GREEN);

    if (CheckCollisionPointRec(*mouse, menuOptions[0]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *menuMode = 0;
        *introMode = 1;
        *introStart = GetTime();
    }

    if (CheckCollisionPointRec(*mouse, menuOptions[1]) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        *closeGame = 1;
    }

    EndDrawing();
}

// Intro Elements
char introText[400] = "Ao voltar do exílio, Lula Bombado encontra um Brasil devastado\ne comandado pelo ditador Capitão Bostonaro e seu exército de cloroquinas.\nAbalado, ele deseja salvar o país com o poder da vacina e derrotar\no tirano opressor, para isso, Lula invade o covil maligno\nde Bostonaro para derrotá-lo.";
char instructions[90] = "Use [W] [A] [S] [D] para se mover e [J] [K] [M] [N] para atacar";

// Enemy Utilities
Vector2 enemiesPositions[20] = {
    {.x = 100, .y = 40},    // 1
    {.x = 1305, .y = 636},  // 2
    {.x = 530, .y = 1213},  // 3
    {.x = 1600, .y = 1920}, // 4
    {.x = 960, .y = 3000},  // 5
    {.x = 2770, .y = 3100}, // 6
    {.x = 4100, .y = 3060}, // 7
    {.x = 3400, .y = 950},  // 8
    {.x = 4400, .y = 875},  // 9
    {.x = 5290, .y = 2814}, // 10
    {.x = 5678, .y = 570},  // 11
    {.x = 6500, .y = 1300}, // 12
    {.x = 6700, .y = 2700}, // 13
    {.x = 5560, .y = 3375}, // 14
    {.x = 7520, .y = 2344}, // 15
    {.x = 7130, .y = 620},  // 16
    {.x = 2270, .y = 770},  // 17
    {.x = 6280, .y = 3360}, // 18
    {.x = 7484, .y = 2979}, // 19
    {.x = 3095, .y = 1931}, // 20
};
void enemyDynamic(Rectangle player, Rectangle *enemy, int speed, float *playerLife)
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
        *playerLife -= 10;
        enemy->x += 150;
        enemy->y += 150;
    }
    if (CheckCollisionRecs(player, *enemy) && enemy->x <= player.x && enemy->y >= player.y)
    {
        *playerLife -= 10;
        enemy->x -= 150;
        enemy->y += 150;
    }
    if (CheckCollisionRecs(player, *enemy) && enemy->x >= player.x && enemy->y <= player.y)
    {
        *playerLife -= 10;
        enemy->x += 150;
        enemy->y -= 150;
    }
    if (CheckCollisionRecs(player, *enemy) && enemy->x <= player.x && enemy->y <= player.y)
    {
        *playerLife -= 10;
        enemy->x -= 150;
        enemy->y -= 150;
    }
}

void renderEnemies(float playerH, float playerW, float pX, float pY, Rectangle *enemies, int enemyAmount)
{
    for (int i = 0; i < enemyAmount; ++i)
    {
        enemies[i].height = playerH / 1.7;
        enemies[i].width = playerW / 2;

        enemies[i].x = enemiesPositions[i].x;
        enemies[i].y = enemiesPositions[i].y;
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