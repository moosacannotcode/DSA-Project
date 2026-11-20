#include "raylib.h"

int main()
{
    // Initialize window
    InitWindow(800, 600, "My First Raylib Game");

    // Set FPS
    SetTargetFPS(60);

    // Game loop
    while (!WindowShouldClose())
    {
        // Drawing
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("HELLO RAYLIB!", 300, 200, 30, BLACK);

        DrawCircle(400, 350, 50, RED);

        EndDrawing();
    }

    // Close window
    CloseWindow();

    return 0;
}