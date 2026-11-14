#include <iostream>
#include <raylib.h>

// Ball class
class Ball {
private:
    int x, y;
    int speed_x, speed_y;
    int radius;

public:
    // Constructor
    Ball(int startX, int startY, int r) {
        x = startX;
        y = startY;
        radius = r;
        speed_x = 5;
        speed_y = 5;
    }

    // Update movement
    void update(int screenWidth, int screenHeight) {
        x += speed_x;
        y += speed_y;

        // Bounce horizontally
        if (x + radius >= screenWidth || x - radius <= 0) {
            speed_x *= -1;
        }

        // Bounce vertically
        if (y + radius >= screenHeight || y - radius <= 0) {
            speed_y *= -1;
        }
    }

    // Draw ball
    void draw() {
        DrawCircle(x, y, radius, PURPLE);
    }
};

int main() {
    const int SCREEN_WIDTH = 1000;
    const int SCREEN_HEIGHT = 700;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OOP Raylib Example");
    SetTargetFPS(60);

    // Create object
    Ball ball(100, 100, 15);

    while (!WindowShouldClose()) {
        // Update
        ball.update(SCREEN_WIDTH, SCREEN_HEIGHT);

        // Draw
        BeginDrawing();
        ClearBackground(RED);

        ball.draw();

        EndDrawing();
    }

    CloseWindow();
}
