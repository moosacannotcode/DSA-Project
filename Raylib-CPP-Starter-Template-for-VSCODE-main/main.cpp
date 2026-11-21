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
    void Reset(){
    x = 20;
    y = SCREEN_HEIGHT - height;
    health = maxHealth;
    damageCooldown = 0;}

};


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
            current->Update(dt); //calling monser update function not list ka

            if (CheckCollisionRecs(current->position, playerRectangle) ||
                current->position.x + current->position.width < 0)
            {
                Monster* nodeToDelete = current;

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
    void Clear(){
        Monster* current = head;
        while (current != nullptr)
        {
            Monster* temp = current;
            current = current->next;
            delete temp;
        }
        head=nullptr;
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
        Bullet* current = head;
        Bullet* previous = nullptr;

        while (current != nullptr)
        {
            Rectangle bulletRectangle = {
                current->position.x - 5,
                current->position.y - 5,
                10,
                10
            };

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
            if (
                current->position.x < 0 ||
                current->position.x > SCREEN_WIDTH ||
                current->position.y < 0 ||
                current->position.y > SCREEN_HEIGHT
            )
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
    void Clear(){
    Bullet* current = head;

    while (current != nullptr)
    {
        Bullet* temp = current;
        current = current->next;
        delete temp;
    }

    head = nullptr;}   
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
void StartGame(
    Player& player,
    BulletList& bullets,
    MonsterList& monsters,
    float& shootTimer,
    float& monsterSpawnTimer)
{
    player.Reset();

    bullets.Clear();
    monsters.Clear();

    shootTimer = 0;
    monsterSpawnTimer = 0;
}

enum gamestate
{
    MENU,
    PLAYING,
    GAME_OVER
};

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
    MonsterList monsters;

    float shootTimer = 0.0f;
    const float shootCooldown = 0.2f;
    float monsterSpawnTimer = 0.0f;
    const float monsterSpawnInterval = 2.0f;

    // GAME STATES AND BUTTONS
    gamestate gameState = MENU;

    Rectangle startButton = {
        SCREEN_WIDTH / 2.0f - 110, SCREEN_HEIGHT / 2.0f + 30, 220, 65
    };

    Rectangle playAgainButton = {
        SCREEN_WIDTH / 2.0f - 110, SCREEN_HEIGHT / 2.0f + 55, 220, 65
    };

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        Vector2 mousePosition = GetMousePosition();

        // MENU STATE
        if (gameState == MENU)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                CheckCollisionPointRec(mousePosition, startButton))
            {
                StartGame(player, bullets, monsters, shootTimer, monsterSpawnTimer);
                gameState = PLAYING;
            }
        }

        // PLAYING STATE
        else if (gameState == PLAYING)
        {
            shootTimer -= dt;
            monsterSpawnTimer -= dt;

            player.Update(dt);

            // SHOOTING
            if (player.health > 0 && IsKeyDown(KEY_SPACE) && shootTimer <= 0)
            {
                Vector2 gunPosition = {
                    player.x + player.width,
                    player.y + player.height / 2.0f
                };

                Vector2 target = {gunPosition.x + 1, gunPosition.y};

                bullets.Shoot(gunPosition, target);
                shootTimer = shootCooldown;
            }

            // MONSTER SPAWNING
            if (monsterSpawnTimer <= 0 && player.health > 0)
            {
                monsters.Spawn(monsterTexture);
                monsterSpawnTimer = monsterSpawnInterval;
            }

            // BULLET UPDATE AND COLLISIONS
            bullets.Update(dt);
            bullets.CheckMonsterCollisions(monsters);
            bullets.RemoveBullets();

            // MONSTER UPDATE AND PLAYER DAMAGE
            int monsterDamage = 0;
            monsters.Update(dt, player.GetRectangle(), monsterDamage);

            if (monsterDamage > 0 && player.health > 0)
                player.TakeDamage(10);

            // CHANGE STATE WHEN PLAYER DIES
            if (player.health <= 0)
                gameState = GAME_OVER;
        }

        // GAME OVER STATE
        else if (gameState == GAME_OVER)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                CheckCollisionPointRec(mousePosition, playAgainButton))
            {
                StartGame(player, bullets, monsters, shootTimer, monsterSpawnTimer);
                gameState = PLAYING;
            }
        }

        // DRAWING
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw background in every state
        if (backgroundLoaded)
        {
            DrawTexturePro(
                background,
                {0, 0, (float)background.width, (float)background.height},
                {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT},
                {0, 0}, 0, WHITE
            );
        }

        // MENU SCREEN
        if (gameState == MENU)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.55f));

            DrawText("MONSTER SHOOTER",
                     SCREEN_WIDTH / 2 - 260, 220, 55, WHITE);

            DrawText("Move: ARROW KEYS     Shoot: SPACE",
                     SCREEN_WIDTH / 2 - 250, 300, 22, LIGHTGRAY);

            bool hovering = CheckCollisionPointRec(mousePosition, startButton);
            DrawRectangleRec(startButton, hovering ? DARKGREEN : GREEN);
            DrawRectangleLinesEx(startButton, 2, WHITE);

            DrawText("START",
                     (int)startButton.x + 70, (int)startButton.y + 17, 30, WHITE);
        }

        // PLAYING SCREEN
        else if (gameState == PLAYING)
        {
            player.Draw();

            if (monsterImageLoaded)
                monsters.Draw(monsterTexture);
            else
                DrawText("MONSTER IMAGE FAILED TO LOAD", 400, 350, 30, RED);

            bullets.Draw();

            DrawText("PLAYER HEALTH", 20, 20, 20, WHITE);
            DrawRectangle(20, 50, 200, 20, DARKGRAY);
            DrawRectangle(
                20, 50,
                (int)(200 * ((float)player.health / player.maxHealth)),
                20, GREEN
            );
            DrawRectangleLines(20, 50, 200, 20, WHITE);

            DrawText(TextFormat("%i / %i", player.health, player.maxHealth),
                     230, 50, 20, WHITE);

            DrawText("ARROW KEYS: MOVE", 20, 90, 20, WHITE);
            DrawText("SPACE: SHOOT", 20, 120, 20, WHITE);
        }

        // GAME OVER SCREEN
        else if (gameState == GAME_OVER)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.75f));

            DrawText("GAME OVER",
                     SCREEN_WIDTH / 2 - 165, SCREEN_HEIGHT / 2 - 100, 55, RED);

            bool hovering = CheckCollisionPointRec(mousePosition, playAgainButton);
            DrawRectangleRec(playAgainButton, hovering ? DARKGREEN : GREEN);
            DrawRectangleLinesEx(playAgainButton, 2, WHITE);

            DrawText("PLAY AGAIN",
                     (int)playAgainButton.x + 28,
                     (int)playAgainButton.y + 17,
                     30, WHITE);
        }

        EndDrawing();
    }

    // CLEANUP
    UnloadTexture(background);

    if (player.texture.id != 0)
        UnloadTexture(player.texture);

    if (monsterImageLoaded)
        UnloadTexture(monsterTexture);

    CloseWindow();
    return 0;
}
