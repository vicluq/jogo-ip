#include <raylib.h>
#include <stdio.h>
#include <math.h>

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(0, 0, "raylib [core] example - mouse input");
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
    Texture2D background = LoadTexture("../assets/Cave.png");

    Rectangle player1 = {-50, -30, 7.5, 7.5};
    const float playerSpeed = 5;
    int previousX = 0, previousY = 0;
    
    Rectangle enemies[50] = {};

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
            camera.target.x += ((player1.x) - camera.target.x) * 0.01;
            camera.target.y += ((player1.y) - camera.target.y) * 0.015;
        }
        // Drawing mode
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawText("Aqui é o topo", screenWidth / 2, 10, 20, BLUE);

        BeginMode2D(camera);

        Vector2 texturePos = {0, 0};
        DrawTextureEx(background, texturePos, 0, 1.5, WHITE); 
        
        DrawText("TEXTOOO", -25, -25, 20, GREEN);
        
        DrawRectangleRec(player1, RED);

        EndMode2D();
        EndDrawing();
    }

    return 0;
}