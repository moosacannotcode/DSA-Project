
#include "raylib.h"
#include <cmath>

// ==========================================
// BULLET CLASS
// ==========================================

class Bullet
{
public:
    Vector2 position;
    Vector2 velocity;

    Bullet* next;

    Bullet(Vector2 pos, Vector2 vel)
    {
        position = pos;
        velocity = vel;
        next = nullptr;
    }

    void Update(float dt)
    {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
    }

    void Draw()
    {
        DrawCircleV(position, 5, YELLOW);
    }
};


// ==========================================
// BULLET LINKED LIST
// ==========================================

class BulletList
{
private:
    Bullet* head;

public:

    BulletList()
    {
        head = nullptr;
    }

    void Shoot(Vector2 gunPosition, Vector2 target)
    {
        Vector2 direction = {
            target.x - gunPosition.x,
            target.y - gunPosition.y
        };

        float length = std::sqrt(
            direction.x * direction.x +
            direction.y * direction.y
        );

        if (length == 0)
            return;

        direction.x /= length;
        direction.y /= length;

        Vector2 velocity = {
            direction.x * 500,
            direction.y * 500
        };

        Bullet* newBullet = new Bullet(gunPosition, velocity);

        newBullet->next = head;
        head = newBullet;
    }

    void Update(float dt)
    {
        Bullet* current = head;

        while (current != nullptr)
        {
            current->Update(dt);
            current = current->next;
        }
    }

    void Draw()
    {
        Bullet* current = head;

        while (current != nullptr)
        {
            current->Draw();
            current = current->next;
        }
    }

    void RemoveBullets()
    {
        Bullet** current = &head;

        while (*current != nullptr)
        {
            Bullet* temp = *current;

            if (temp->position.x < 0 ||
                temp->position.x > GetScreenWidth() ||
                temp->position.y < 0 ||
                temp->position.y > GetScreenHeight())
            {
                *current = temp->next;
                delete temp;
            }
            else
            {
                current = &((*current)->next);
            }
        }
    }

    ~BulletList()
    {
        while (head != nullptr)
        {
            Bullet* temp = head;
            head = head->next;
            delete temp;
        }
    }
};


// ==========================================
// MAIN GAME
// ==========================================

int main()
{
    InitWindow(1280, 800, "Game screen");

    SetTargetFPS(60);

    Texture2D player = LoadTexture(
        "media and assets/masked_figure_transparent.png"
    );

    Texture2D background = LoadTexture(
        "media and assets/temp_gb.png"
    );

    bool imageLoaded = (player.id != 0);

    // ======================================
    // PLAYER
    // ======================================

    float scale = 0.25f;

    float playerWidth = player.width * scale;
    float playerHeight = player.height * scale;

    float playerX = 20.0f;
    float playerY = 800 - playerHeight;

    float playerSpeed = 300.0f;

    // ======================================
    // BULLETS
    // ======================================

    BulletList bullets;

    float shootTimer = 0.0f;
    float shootCooldown = 0.2f;

    // ======================================
    // GAME LOOP
    // ======================================

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        shootTimer -= dt;

        // ==================================
        // PLAYER MOVEMENT
        // ==================================

        if (imageLoaded)
        {
            if (IsKeyDown(KEY_LEFT))
                playerX -= playerSpeed * dt;

            if (IsKeyDown(KEY_RIGHT))
                playerX += playerSpeed * dt;

            if (IsKeyDown(KEY_UP))
                playerY -= playerSpeed * dt;

            if (IsKeyDown(KEY_DOWN))
                playerY += playerSpeed * dt;

            // Screen boundaries

            if (playerX < 0)
                playerX = 0;

            if (playerX > 1280 - playerWidth)
                playerX = 1280 - playerWidth;

            if (playerY < 0)
                playerY = 0;

            if (playerY > 800 - playerHeight)
                playerY = 800 - playerHeight;


            // ==================================
            // SHOOTING
            // ==================================

            if (IsKeyDown(KEY_SPACE) && shootTimer <= 0)
            {
                Vector2 gunPosition = {
                    playerX + playerWidth,
                    playerY + playerHeight / 2.0f
                };

                Vector2 target = {
                    gunPosition.x + 1,
                    gunPosition.y
                };

                bullets.Shoot(gunPosition, target);

                shootTimer = shootCooldown;
            }
        }

        // ==================================
        // BULLET UPDATES
        // ==================================

        bullets.Update(dt);

        bullets.RemoveBullets();

        // ==================================
        // DRAWING
        // ==================================

        BeginDrawing();

        ClearBackground(BLACK);

        // Background

        DrawTexturePro(
            background,
            {0, 0, (float)background.width, (float)background.height},
            {0, 0, 1280, 800},
            {0, 0},
            0,
            WHITE
        );

        // Player

        if (imageLoaded)
        {
            Vector2 position = {playerX, playerY};

            DrawTextureEx(player, position, 0.0f, scale, WHITE);
        }
        else
        {
            DrawText("IMAGE FAILED TO LOAD!", 400, 350, 30, RED);
        }

        // Bullets

        bullets.Draw();

        // Controls

        DrawText("UP/DOWN: Move", 20, 20, 20, WHITE);
        DrawText("SPACE: Shoot", 20, 50, 20, WHITE);

        EndDrawing();
    }

    // ======================================
    // CLEANUP
    // ======================================

    UnloadTexture(background);

    if (imageLoaded)
        UnloadTexture(player);

    CloseWindow();

    return 0;
}
