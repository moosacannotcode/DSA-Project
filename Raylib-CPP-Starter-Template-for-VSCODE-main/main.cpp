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
        position = { (float)SCREEN_WIDTH + 50, (float)GetRandomValue(50, SCREEN_HEIGHT - 150), 100, 100 };
        speed = (float)GetRandomValue(100, 180);
        health = 100;
        hasBeenHit = false;
        next = nullptr;
    }

    void Update(float dt)
    {
        position.x -= speed * dt;
    }

    void Draw(Texture2D texture)
    {
        DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height}, position, {0, 0}, 0, WHITE);

        if (hasBeenHit)
        {
            DrawRectangle((int)position.x, (int)position.y - 12, (int)position.width, 8, DARKGRAY);
            DrawRectangle((int)position.x, (int)position.y - 12, (int)(position.width * ((float)health / 100)), 8, RED);
        }
    }
};

// ==========================================
// MONSTER LINKED LIST
// ==========================================

class MonsterList
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
        Monster** current = &head;

        while (*current != nullptr)
        {
            Monster* monster = *current;

            monster->Update(dt);

            if (CheckCollisionRecs(monster->position, playerRectangle))
            {
                playerDamage++;

                *current = monster->next;
                delete monster;
            }
            else if (monster->position.x + monster->position.width < 0)
            {
                *current = monster->next;
                delete monster;
            }
            else
            {
                current = &((*current)->next);
            }
        }
    }

    bool Hit(Rectangle bulletRectangle)
    {
        Monster* current = head;

        while (current != nullptr)
        {
            if (CheckCollisionRecs(current->position, bulletRectangle))
            {
                current->health -= 25;
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
        Monster** current = &head;

        while (*current != nullptr)
        {
            Monster* monster = *current;

            if (monster->health <= 0)
            {
                *current = monster->next;
                delete monster;
            }
            else
            {
                current = &((*current)->next);
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
        while (head != nullptr)
        {
            Monster* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

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
        DrawCircleV(position, 5, LIME);
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
        Vector2 direction = {target.x - gunPosition.x, target.y - gunPosition.y};

        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length == 0)
            return;

        direction.x /= length;
        direction.y /= length;

        Vector2 velocity = {direction.x * 650, direction.y * 650};

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
        Bullet** current = &head;

        while (*current != nullptr)
        {
            Bullet* bullet = *current;

            Rectangle bulletRectangle = {bullet->position.x - 5, bullet->position.y - 5, 10, 10};

            if (monsters.Hit(bulletRectangle))
            {
                *current = bullet->next;
                delete bullet;
            }
            else
            {
                current = &((*current)->next);
            }
        }

        monsters.RemoveDeadMonsters();
    }

    void RemoveBullets()
    {
        Bullet** current = &head;

        while (*current != nullptr)
        {
            Bullet* bullet = *current;

            if (bullet->position.x < 0 || bullet->position.x > SCREEN_WIDTH || bullet->position.y < 0 || bullet->position.y > SCREEN_HEIGHT)
            {
                *current = bullet->next;
                delete bullet;
            }
            else
            {
                current = &((*current)->next);
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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Monster Shooter");
    SetTargetFPS(60);

    Texture2D player = LoadTexture("media and assets/masked_figure_transparent.png");
    Texture2D background = LoadTexture("media and assets/temp_gb.png");

    Texture2D monsterTexture = LoadTexture("media and assets/monssterr.jpg");

    bool playerImageLoaded = player.id != 0;
    bool backgroundLoaded = background.id != 0;
    bool monsterImageLoaded = monsterTexture.id != 0;

    // ======================================
    // PLAYER
    // ======================================

    float playerScale = 0.25f;
    float playerWidth = player.width * playerScale;
    float playerHeight = player.height * playerScale;

    float playerX = 20.0f;
    float playerY = SCREEN_HEIGHT - playerHeight;

    float playerSpeed = 300.0f;

    int playerHealth = 100;
    int maxPlayerHealth = 100;

    float playerDamageCooldown = 0.0f;

    // ======================================
    // BULLETS
    // ======================================

    BulletList bullets;

    float shootTimer = 0.0f;
    float shootCooldown = 0.2f;

    // ======================================
    // MONSTERS
    // ======================================

    MonsterList monsters;

    float monsterSpawnTimer = 0.0f;
    float monsterSpawnInterval = 2.0f;

    // ======================================
    // GAME LOOP
    // ======================================

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        shootTimer -= dt;
        monsterSpawnTimer -= dt;
        playerDamageCooldown -= dt;

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

    if (playerImageLoaded)
        UnloadTexture(player);

    if (monsterImageLoaded)
        UnloadTexture(monsterTexture);

    CloseWindow();

    return 0;
}