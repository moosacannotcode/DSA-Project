#include "raylib.h"
#include <cmath>

// ==========================================
// SCREEN SETTINGS
// ==========================================

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;

// ==========================================
// MONSTER CLASS
// ==========================================

class Monster
{
public:
    Rectangle position;
    float speed;
    int health;
    bool hasBeenHit;
    Monster* next;

    Monster(Texture2D texture)
    {
        position = {
            (float)SCREEN_WIDTH + 50,
            (float)GetRandomValue(50, SCREEN_HEIGHT - 150),
            100,
            100
        };

        speed = (float)GetRandomValue(100, 180);
        health = 100;
        hasBeenHit = false;
        next = nullptr;
    }

    void Update(float dt)
    {
        position.x -= speed * dt;
        //subtracting speed from x position to move left across the screen
    }

    void Draw(Texture2D texture)
    {
        DrawTexturePro(
            texture,
            {0, 0, (float)texture.width, (float)texture.height},
            position,
            {0, 0},
            0,
            WHITE
        );

        if (hasBeenHit)
        {
            DrawRectangle(
                (int)position.x,
                (int)position.y - 12,
                (int)position.width,
                8,
                DARKGRAY
            );

            DrawRectangle(
                (int)position.x,
                (int)position.y - 12,
                (int)(position.width * ((float)health / 100)),
                8,
                RED
            );
        }
    }
};

// ==========================================
// MONSTER LINKED LIST
// ==========================================

class MonsterList  //double linked list for monsters
{
private:
    Monster* head;

public:
    MonsterList()
    {
        head = nullptr;
    }

    void Spawn(Texture2D texture)
    {
        Monster* newMonster = new Monster(texture);
        newMonster->next = head;
        head = newMonster;
    }

    void Update(float dt, Rectangle playerRectangle, int& playerDamage)
    {
        Monster* current = head;
        Monster* previous = nullptr;

        while (current != nullptr)
        {
            Monster* monster = *current;

                if (previous == nullptr)
                    head = current->next;
                else
                    previous->next = current->next;

                if (CheckCollisionRecs(nodeToDelete->position, playerRectangle))
                    playerDamage++;

                current = nodeToDelete->next;
                delete nodeToDelete;
            }
            else
            {
                previous = current;
                current = current->next;
            }
        }
    }

    bool Hit(Rectangle bulletRectangle, int bulletDamage)
    {
        Monster* current = head;

        while (current != nullptr)
        {
            if (CheckCollisionRecs(current->position, bulletRectangle))
            {
                current->health -= bulletDamage;
                current->hasBeenHit = true;

                if (current->health <= 0)
                    return true;

                return true;
            }

            current = current->next;
        }

        return false;
    }

    void RemoveDeadMonsters()
    {
        Monster* current = head;
        Monster* previous = nullptr;

        while (current != nullptr)
        {
            if (current->health <= 0)
            {
                Monster* nodeToDelete = current;

                if (previous == nullptr)
                    head = current->next;
                else
                    previous->next = current->next;

                current = nodeToDelete->next;
                delete nodeToDelete;
            }
            else
            {
                previous = current;
                current = current->next;
            }
        }
    }

    void Draw(Texture2D texture)
    {
        Monster* current = head;

        while (current != nullptr)
        {
            current->Draw(texture);
            current = current->next;
        }
    }

    ~MonsterList()
    {
        Monster* current = head;

        while (current != nullptr)
        {
            Monster* temp = current;
            current = current->next;
            delete temp;
        }

        head = nullptr;
    }

    ~MonsterList()
    {
        Clear();
    }
};

// ==========================================
// BULLET CLASS
// ==========================================

class Bullet  //will be a singly
{
public:
    Vector2 position;
    Vector2 velocity;
    Bullet* next;
    int damage;

    Bullet(Vector2 pos, Vector2 vel, int d)
    {
        position = pos;
        velocity = vel;
        next = nullptr;
        damage = d;
    }

    void Update(float dt)
    {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
    }

    void Draw()
    {
        DrawCircleV(position, 5, LIME);
    }
};



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

        if (length == 0) return;

        direction.x /= length;
        direction.y /= length;

        Vector2 velocity = {
            direction.x * 650,
            direction.y * 650
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

    void CheckMonsterCollisions(MonsterList& monsters)
    {
        Bullet* current = head;
        Bullet* previous = nullptr;

        while (current != nullptr)
        {
            Bullet* bullet = *current;

            Rectangle bulletRectangle = {bullet->position.x - 5, bullet->position.y - 5, 10, 10};

            if (monsters.Hit(bulletRectangle, current->damage))
            {
                Bullet* nodeToDelete = current;

                if (previous == nullptr)
                    head = current->next;
                else
                    previous->next = current->next;

                current = nodeToDelete->next;
                delete nodeToDelete;
            }
            else
            {
                previous = current;
                current = current->next;
            }
        }

        monsters.RemoveDeadMonsters();
    }

    void RemoveBullets() // Delete bullets that move off-screen.
    {
        Bullet* current = head;
        Bullet* previous = nullptr;

        while (current != nullptr)
        {
            Bullet* bullet = *current;

            if (bullet->position.x < 0 || bullet->position.x > SCREEN_WIDTH || bullet->position.y < 0 || bullet->position.y > SCREEN_HEIGHT)
            {
                Bullet* nodeToDelete = current;

                if (previous == nullptr)
                    head = current->next;
                else
                    previous->next = current->next;

                current = nodeToDelete->next;
                delete nodeToDelete;
            }
            else
            {
                previous = current;
                current = current->next;
            }
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

    ~BulletList()
    {
        Clear();
    }
};

// ==========================================
// FIREFLY NODE
// ==========================================

class Firefly
{
public:
    Vector2 position;
    float radius;
    Firefly* next;

    Firefly(Vector2 pos)
    {
        position = pos;
        radius = 12;
        next = nullptr;
    }

    void Draw()
    {
        DrawCircleV(position, radius + 10, Fade(YELLOW, 0.10f));
        DrawCircleV(position, radius + 5, Fade(GOLD, 0.25f));
        DrawCircleV(position, radius, YELLOW);
        DrawCircleV(position, 5, WHITE);
    }
};

// ==========================================
// FIREFLY LINKED LIST
// ==========================================

class FireflyList
{
private:
    Firefly* head;

public:
    FireflyList()
    {
        head = nullptr;
    }

    void Spawn()
    {
        Vector2 position = {(float)GetRandomValue(80, SCREEN_WIDTH - 80), (float)GetRandomValue(80, SCREEN_HEIGHT - 80)};

        Firefly* newFirefly = new Firefly(position);
        newFirefly->next = head;
        head = newFirefly;
    }

    void Update(Player& player, bool& collected)
    {
        Firefly* current = head;
        Firefly* previous = nullptr;

        while (current != nullptr)
        {
            if (CheckCollisionCircleRec(current->position, current->radius, player.GetRectangle()))
            {
                player.Heal(15);
                collected = true;

                Firefly* nodeToDelete = current;

                if (previous == nullptr)
                    head = current->next;
                else
                    previous->next = current->next;

                current = nodeToDelete->next;
                delete nodeToDelete;
            }
            else
            {
                previous = current;
                current = current->next;
            }
        }
    }

    void Draw()
    {
        Firefly* current = head;

        while (current != nullptr)
        {
            current->Draw();
            current = current->next;
        }
    }

    void Clear()
    {
        Firefly* current = head;

        while (current != nullptr)
        {
            Firefly* temp = current;
            current = current->next;
            delete temp;
        }

        head = nullptr;
    }

    ~FireflyList()
    {
        Clear();
    }
};

// ==========================================
// MAIN GAME
// ==========================================

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Monster Shooter");
    SetTargetFPS(60);

    Texture2D playerTexture = LoadTexture("media and assets/masked_figure_transparent.png");
    Texture2D background = LoadTexture("media and assets/temp_gb.png");
    Texture2D monsterTexture = LoadTexture("media and assets/monssterr.jpg");

    bool backgroundLoaded = background.id != 0;
    bool monsterImageLoaded = monsterTexture.id != 0;

    Player player(playerTexture);
    BulletList bullets;

    float shootTimer = 0.0f;
    const float shootCooldown = 0.2f;
    float monsterSpawnTimer = 0.0f;
    float monsterSpawnInterval = 2.0f;

    // ======================================
    // GAME LOOP
    // ======================================

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        Vector2 mousePosition = GetMousePosition();

        // ==================================
        // PLAYER MOVEMENT
        // ==================================

        if (playerHealth > 0)
        {
            if (IsKeyDown(KEY_LEFT)) playerX -= playerSpeed * dt;
            if (IsKeyDown(KEY_RIGHT)) playerX += playerSpeed * dt;
            if (IsKeyDown(KEY_UP)) playerY -= playerSpeed * dt;
            if (IsKeyDown(KEY_DOWN)) playerY += playerSpeed * dt;

            if (playerX < 0) playerX = 0;
            if (playerX > SCREEN_WIDTH - playerWidth) playerX = SCREEN_WIDTH - playerWidth;
            if (playerY < 0) playerY = 0;
            if (playerY > SCREEN_HEIGHT - playerHeight) playerY = SCREEN_HEIGHT - playerHeight;

            // ==================================
            // SHOOTING
            // ==================================

            if (IsKeyDown(KEY_SPACE) && shootTimer <= 0)
            {
                Vector2 gunPosition = {playerX + playerWidth, playerY + playerHeight / 2.0f};
                Vector2 target = {gunPosition.x + 1, gunPosition.y};

                bullets.Shoot(gunPosition, target);
                shootTimer = shootCooldown;
            }

        // ==================================
        // MONSTER SPAWNING
        // ==================================

        if (monsterSpawnTimer <= 0 && playerHealth > 0)
        {
            monsters.Spawn(monsterTexture);
            monsterSpawnTimer = monsterSpawnInterval;
        }

        // ==================================
        // BULLET UPDATES
        // ==================================

        bullets.Update(dt);
        bullets.CheckMonsterCollisions(monsters);
        bullets.RemoveBullets();

        // ==================================
        // MONSTER UPDATES
        // ==================================

        Rectangle playerRectangle = {playerX, playerY, playerWidth, playerHeight};

        int monsterDamage = 0;

        monsters.Update(dt, playerRectangle, monsterDamage);

        if (monsterDamage > 0 && playerDamageCooldown <= 0 && playerHealth > 0)
        {
            playerHealth -= 10;
            playerDamageCooldown = 1.0f;
        }

        if (playerHealth < 0)
            playerHealth = 0;

        // ==================================
        // DRAWING
        // ==================================

        BeginDrawing();
        ClearBackground(BLACK);

        // ==================================
        // BACKGROUND
        // ==================================

        if (backgroundLoaded)
        {
            DrawTexturePro(background, {0, 0, (float)background.width, (float)background.height}, {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT}, {0, 0}, 0, WHITE);
        }

        // ==================================
        // PLAYER
        // ==================================

        if (playerImageLoaded)
        {
            DrawTextureEx(player, {playerX, playerY}, 0.0f, playerScale, WHITE);
        }
        else
        {
            DrawRectangle((int)playerX, (int)playerY, (int)playerWidth, (int)playerHeight, BLUE);
        }

        // ==================================
        // MONSTERS
        // ==================================

        if (monsterImageLoaded)
        {
            monsters.Draw(monsterTexture);
        }
        else
        {
            DrawText("MONSTER IMAGE FAILED TO LOAD", 400, 350, 30, RED);
        }

        // ==================================
        // BULLETS
        // ==================================

        bullets.Draw();

        // ==================================
        // PLAYER HEALTH BAR
        // ==================================

        DrawText("PLAYER HEALTH", 20, 20, 20, WHITE);
        DrawRectangle(20, 50, 200, 20, DARKGRAY);
        DrawRectangle(20, 50, (int)(200 * ((float)playerHealth / maxPlayerHealth)), 20, GREEN);
        DrawRectangleLines(20, 50, 200, 20, WHITE);

        DrawText(TextFormat("%i / %i", playerHealth, maxPlayerHealth), 230, 50, 20, WHITE);

        // ==================================
        // CONTROLS
        // ==================================

        DrawText("ARROW KEYS: MOVE", 20, 90, 20, WHITE);
        DrawText("SPACE: SHOOT", 20, 120, 20, WHITE);

        // ==================================
        // GAME OVER
        // ==================================

        if (playerHealth <= 0)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.65f));
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 50, 50, RED);
        }

        EndDrawing();
    }

    // ======================================
    // CLEANUP
    // ======================================

    UnloadTexture(background);

    if (player.texture.id != 0)
        UnloadTexture(player.texture);

    if (monsterImageLoaded)
        UnloadTexture(monsterTexture);

    CloseWindow();
    return 0;
}
