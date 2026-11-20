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

    // Constructor
    Bullet(Vector2 pos, Vector2 vel)
    {
        position = pos;
        velocity = vel;
        next = nullptr;
    }

    // Update bullet position
    void Update(float dt)
    {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
    }

    // Draw bullet
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

    // Constructor
    BulletList()
    {
        head = nullptr;
    }

    // Shoot a bullet and insert it into the linked list
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

        // Normalize direction
        direction.x /= length;
        direction.y /= length;

        // Bullet velocity
        Vector2 velocity = {
            direction.x * 500,
            direction.y * 500
        };

        // Create new bullet node
        Bullet* newBullet = new Bullet(gunPosition, velocity);

        // Insert at head of linked list
        newBullet->next = head;
        head = newBullet;
    }

    // Update all bullets
    void Update(float dt)
    {
        Bullet* current = head;

        while (current != nullptr)
        {
            current->Update(dt);

            current = current->next;
        }
    }

    // Draw all bullets
    void Draw()
    {
        Bullet* current = head;

        while (current != nullptr)
        {
            current->Draw();

            current = current->next;
        }
    }

    // Remove bullets outside screen
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
                // Remove node
                *current = temp->next;

                delete temp;
            }
            else
            {
                // Move to next node
                current = &((*current)->next);
            }
        }
    }

    // Destructor
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
    // Initialize window
    InitWindow(1280, 800, "Game screen");

    SetTargetFPS(60);


    // ======================================
    // PLAYER
    // ======================================

    Texture2D player = LoadTexture(
        "D:/Uni stuff/dsa shoota/DSA-Project-main/DSA-Project-main/media and assets/masked_figure_transparent.png"
    );

    // Check if image loaded
    bool imageLoaded = (player.id != 0);

    float scale = 0.25f;

    // Calculate scaled dimensions
    float playerWidth = player.width * scale;
    float playerHeight = player.height * scale;

    // Player position
    float playerX = 20.0f;

    float playerY = 800 - playerHeight;

    float playerSpeed = 5.0f;


    // ======================================
    // BULLETS
    // ======================================

    BulletList bullets;

    // Shooting cooldown
    float shootTimer = 0.0f;

    float shootCooldown = 0.2f;


    // ======================================
    // GAME LOOP
    // ======================================

    while (!WindowShouldClose())
    {
        // Delta time
        float dt = GetFrameTime();

        // Update shooting timer
        shootTimer -= dt;


        // ==================================
        // PLAYER MOVEMENT
        // ==================================

        if (imageLoaded)
        {
            // Move up
            if (IsKeyDown(KEY_UP))
            {
                playerY -= playerSpeed;
            }

            // Move down
            if (IsKeyDown(KEY_DOWN))
            {
                playerY += playerSpeed;
            }

            // Screen boundaries
            if (playerY < 0)
            {
                playerY = 0;
            }

            if (playerY > 800 - playerHeight)
            {
                playerY = 800 - playerHeight;
            }


            // ==================================
            // SHOOTING
            // ==================================

            if (IsKeyDown(KEY_SPACE) && shootTimer <= 0)
            {
                // Bullet starts at the right side of player
                Vector2 gunPosition = {
                    playerX + playerWidth,
                    playerY + playerHeight / 2.0f
                };

                // Target directly to the right
                Vector2 target = {
                    gunPosition.x + 1,
                    gunPosition.y
                };

                // Create bullet
                bullets.Shoot(gunPosition, target);

                // Reset shooting cooldown
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

        ClearBackground(DARKGRAY);


        // Draw player
        if (imageLoaded)
        {
            Vector2 position = {
                playerX,
                playerY
            };

            DrawTextureEx(
                player,
                position,
                0.0f,
                scale,
                WHITE
            );
        }
        else
        {
            DrawText(
                "IMAGE FAILED TO LOAD!",
                400,
                350,
                30,
                RED
            );
        }


        // Draw bullets
        bullets.Draw();


        // Optional: Display controls
        DrawText(
            "UP/DOWN: Move",
            20,
            20,
            20,
            WHITE
        );

        DrawText(
            "SPACE: Shoot",
            20,
            50,
            20,
            WHITE
        );


        EndDrawing();
    }


    // ======================================
    // CLEANUP
    // ======================================

    if (imageLoaded)
    {
        UnloadTexture(player);
    }

    CloseWindow();

    return 0;
}