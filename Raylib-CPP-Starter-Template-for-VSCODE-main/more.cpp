#include "raylib.h"

//backtracking to generate wave difficulty  (to generate the number for monster health)

//eachtime attack hits monster monster should flicer and loose health

//arrow movement on screen moves the placeholder



#include "raylib.h"

int main()
{
    // Initialize window
    InitWindow(1280, 800, "Game screen");

    SetTargetFPS(60);

    // Load player image
    Texture2D player = LoadTexture("assets/masked_figure_transparent.png");

    // Check if image loaded
    if (player.id == 0)
    {
        TraceLog(LOG_ERROR, "PLAYER IMAGE FAILED TO LOAD!");
        CloseWindow();
        return 1;
    }

    TraceLog(LOG_INFO, "Player image loaded successfully!");
    TraceLog(LOG_INFO, "Width: %d, Height: %d", player.width, player.height);

    // Player position
    float playerX = (1280 - player.width) / 2.0f;
    float playerY = 800 - player.height;

    float playerSpeed = 5.0f;

    // Game loop
    while (!WindowShouldClose())
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

        // Prevent player from leaving screen
        if (playerX < 0)
            playerX = 0;

        if (playerX > 1280 - player.width)
            playerX = 1280 - player.width;

        BeginDrawing();

        ClearBackground(DARKGRAY);

        // TEMPORARY: Draw at a fixed position to test rendering
        DrawTexture(player, 100, 100, WHITE);

        DrawText("Testing player image", 20, 20, 20, WHITE);

        EndDrawing();
    }

    // Unload texture
    UnloadTexture(player);

    // Close window
    CloseWindow();

    return 0;
}
