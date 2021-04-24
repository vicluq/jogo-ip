#include <raylib.h>
#include <stdio.h>
#include <math.h>

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(0, 0, "Game");
    if (!IsWindowFullscreen())
        ToggleFullscreen();

    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();

    SetTargetFPS(60);

    // NOTE Camera
    Camera2D camera = {.offset = {screenWidth / 2.0f, screenHeight / 2.0f},
                       .target = {0},
                       .rotation = 0,
                       .zoom = 1};

    // TODO Load das texturas e player
    Texture2D background = LoadTexture("./assets/Cave.png");

    Rectangle lifeBar = {10, 10, 100, 100};

    Rectangle player1 = {-50, -30, 60, 30};
    const float playerSpeed = 15;
    int previousX = 0, previousY = 0;

    Rectangle weapon = {0, 0, 30, 15};

    Rectangle enemy = {100, 120, 40, 40};
    // Rectangle enemies[50] = {};

    // TODO criar mecânica co player

    // NOTE Ciclo do jogo
    while (!WindowShouldClose())
    {
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

        //  Camera
        if ((previousY != player1.y) || (previousX != player1.x))
        {
            camera.target.x += ((player1.x) - camera.target.x) * 0.025;
            camera.target.y += ((player1.y) - camera.target.y) * 0.025;
        }

        // Drawing mode
        BeginDrawing();

        ClearBackground(RAYWHITE);

        if (CheckCollisionRecs(weapon, enemy))
        {
            DrawText("COLISÃOOOO", screenWidth / 2, 10, 20, RED);
        }

        // NOTE 2d
        BeginMode2D(camera);

        DrawTextureEx(background, (Vector2){.x = 0, .y = 0}, 0, 3, WHITE);

        DrawRectangleRec(player1, RED);
        DrawRectangleRec(enemy, BLACK);

        // Combate TODO tentar J
        if (IsKeyPressed(KEY_K))
        {
            weapon.x = player1.x + player1.width;
            weapon.y = player1.y + 2;
            weapon.width = 30;
            weapon.height = 15;
            DrawRectangleRec(weapon, GREEN);
        }
        else
        {
            weapon.width = 0;
            weapon.height = 0;
        }

        EndMode2D();
        EndDrawing();
    }

    return 0;
}
/*
   TODO Inimigo
    Movimentação do inimigo similar a câmera
*/