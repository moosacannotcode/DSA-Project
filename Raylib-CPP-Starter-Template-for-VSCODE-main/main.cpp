#include "raylib.h"
#include <cmath>

// ==========================================
// SCREEN SETTINGS
// ==========================================

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;



class Player
{
public:
    Texture2D texture;

    float x;
    float y;
    float width;
    float height;
    float scale;
    float speed;

    int health;
    int maxHealth;

    float damageCooldown;

    Player(Texture2D playerTexture)
    {
        texture = playerTexture;

        scale = 0.25f;

        // Use the loaded texture dimensions, or a fallback size.
        if (texture.id != 0)
        {
            width = texture.width * scale;
            height = texture.height * scale;
        }
        else
        {
            width = 100;
            height = 100;
        }

        x = 20.0f;
        y = SCREEN_HEIGHT - height;

        speed = 300.0f;

        health = 100;
        maxHealth = 100;

        damageCooldown = 0.0f;
    }

    void Update(float dt)
    {
        damageCooldown -= dt;

        if (health <= 0)
            return;

        if (IsKeyDown(KEY_LEFT))  x -= speed * dt;
        if (IsKeyDown(KEY_RIGHT)) x += speed * dt;
        if (IsKeyDown(KEY_UP))    y -= speed * dt;
        if (IsKeyDown(KEY_DOWN))  y += speed * dt;

        // Keep the player inside the screen.
        if (x < 0) x = 0;
        if (x > SCREEN_WIDTH - width) x = SCREEN_WIDTH - width;
        if (y < 0) y = 0;
        if (y > SCREEN_HEIGHT - height) y = SCREEN_HEIGHT - height;
    }

    void Draw()
    {
        if (texture.id != 0)
        {
            DrawTextureEx(texture, {x, y}, 0.0f, scale, WHITE);
        }
        else
        {
            DrawRectangle((int)x, (int)y, (int)width, (int)height, BLUE);
        }
    }

    Rectangle GetRectangle()
    {
        return {x, y, width, height};
    }

    void TakeDamage(int amount)
    {
        if (health > 0 && damageCooldown <= 0)
        {
            health -= amount;

            if (health < 0)
                health = 0;

            damageCooldown = 1.0f;
        }
    }
};

// ==========================================
// MONSTER CLASS
// ==========================================

class Monster // monster nodes
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

    bool Hit(Rectangle bulletRectangle, int bulletDamage)
    {
        Monster* current = head;

        while (current != nullptr)
        {
            if (CheckCollisionRecs(current->position, bulletRectangle))
            {
                current->health -= bulletDamage;
                current->hasBeenHit = true;

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
            direction.x * 650,
            direction.y * 650
        };

        Bullet* newBullet = new Bullet(gunPosition, velocity, 25);

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

            Rectangle bulletRectangle = {
                bullet->position.x - 5,
                bullet->position.y - 5,
                10,
                10
            };

            if (monsters.Hit(bulletRectangle, bullet->damage))
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

    void RemoveBullets() // Delete bullets that move off-screen.
    {
        Bullet** current = &head;

        while (*current != nullptr)
        {
            Bullet* bullet = *current;

            if (
                bullet->position.x < 0 ||
                bullet->position.x > SCREEN_WIDTH ||
                bullet->position.y < 0 ||
                bullet->position.y > SCREEN_HEIGHT
            )
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

    Texture2D playerTexture =
        LoadTexture("media and assets/masked_figure_transparent.png");

    Texture2D background =
        LoadTexture("media and assets/temp_gb.png");

    Texture2D monsterTexture =
        LoadTexture("media and assets/monssterr.jpg");

    bool backgroundLoaded = background.id != 0;
    bool monsterImageLoaded = monsterTexture.id != 0;

    // ======================================
    // PLAYER
    // ======================================

    Player player(playerTexture);

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

        // ==================================
        // PLAYER UPDATE
        // ==================================

        player.Update(dt);

        // ==================================
        // SHOOTING
        // ==================================

        if (player.health > 0 &&
            IsKeyDown(KEY_SPACE) &&
            shootTimer <= 0)
        {
            Vector2 gunPosition = {
                player.x + player.width,
                player.y + player.height / 2.0f
            };

            Vector2 target = {
                gunPosition.x + 1,
                gunPosition.y
            };

            bullets.Shoot(gunPosition, target);

            shootTimer = shootCooldown;
        }

        // ==================================
        // MONSTER SPAWNING
        // ==================================

        if (monsterSpawnTimer <= 0 && player.health > 0)
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

        int monsterDamage = 0;

        monsters.Update(dt, player.GetRectangle(), monsterDamage);

        if (monsterDamage > 0 && player.health > 0)
        {
            player.TakeDamage(10);
        }

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
            DrawTexturePro(
                background,
                {0, 0, (float)background.width, (float)background.height},
                {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
                {0, 0},
                0,
                WHITE
            );
        }

        // ==================================
        // PLAYER
        // ==================================

        player.Draw();

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

        DrawRectangle(
            20,
            50,
            (int)(200 * ((float)player.health / player.maxHealth)),
            20,
            GREEN
        );

        DrawRectangleLines(20, 50, 200, 20, WHITE);

        DrawText(
            TextFormat("%i / %i", player.health, player.maxHealth),
            230,
            50,
            20,
            WHITE
        );

        // ==================================
        // CONTROLS
        // ==================================

        DrawText("ARROW KEYS: MOVE", 20, 90, 20, WHITE);
        DrawText("SPACE: SHOOT", 20, 120, 20, WHITE);

        // ==================================
        // GAME OVER
        // ==================================

        if (player.health <= 0)
        {
            DrawRectangle(
                0,
                0,
                SCREEN_WIDTH,
                SCREEN_HEIGHT,
                Fade(BLACK, 0.65f)
            );

            DrawText(
                "GAME OVER",
                SCREEN_WIDTH / 2 - 150,
                SCREEN_HEIGHT / 2 - 50,
                50,
                RED
            );
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
