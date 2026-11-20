#include "raylib.h"
#pragma once
#include <cmath>


int main()
{
    // Initialize window
    InitWindow(1280, 800, "Game screen");

    SetTargetFPS(60);

    Texture2D player = LoadTexture("media and assets/masked_figure_transparent.png");

    // Check if image loaded
    bool imageLoaded = (player.id != 0);

    float scale = 0.25f;

    // Calculate scaled dimensions
    float playerWidth = player.width * scale;
    float playerHeight = player.height * scale;

    // Player position
    float playerX = (1280 - playerWidth) / 2.0f;
    float playerY = 800 - playerHeight;

    float playerSpeed = 5.0f;

    // Game loop
    while (!WindowShouldClose())
    {
        if (imageLoaded)
        {
            // Move left
            if (IsKeyDown(KEY_LEFT))
            {
                playerX -= playerSpeed;
            }

            // Move right
            if (IsKeyDown(KEY_RIGHT))
            {
                playerX += playerSpeed;
            }

            // Screen boundaries
            if (playerX < 0)
                playerX = 0;

            if (playerX > 1280 - playerWidth)
                playerX = 1280 - playerWidth;
        }

        // Drawing
        BeginDrawing();

        ClearBackground(DARKGRAY);

        if (imageLoaded)
        {
            Vector2 position = {playerX, playerY};

            DrawTextureEx(player, position, 0.0f, scale, WHITE);
        }
        else
        {
            DrawText("IMAGE FAILED TO LOAD!", 400, 350, 30, RED);
        }

        EndDrawing();
    }

    if (imageLoaded)
        UnloadTexture(player);

    CloseWindow();

    return 0;
}