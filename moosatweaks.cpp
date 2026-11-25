#include "raylib.h"
#include <cmath>
#include <iostream>
using namespace std;
// ==========================================
// SCREEN SETTINGS
// ==========================================

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;

// ==========================================
// PLAYER
// ==========================================
#include "raylib.h"
#include <cmath>
#include <iostream>
using namespace std;



class Backtracking
{
private:

    int combinations[4000][10]; //4000 possible max waves of 10 possible monsters each
    int combinationCount;

    void GenerateRecursive(
        int allowed[],  //available health values  (given values)
        int size,  //total available health ingredients lamo
        int N,        //values we must select
        int target,
        //stuff thats gonna change as we go down the recursion tree
        int current[],
        int depth, //values we have selected so far
        int sum,
        int startIndex
    )
    {
        // BASE CASE
        if (depth == N)  //all selected
        {
            if (sum == target)//valid combination found and copied
            {
                for (int i = 0; i < N; i++)
                {
                    combinations[combinationCount][i] = current[i];
                }

                combinationCount++;
            }

            return;
        }

        // RECURSIVE CASE
        for (int i = startIndex; i < size; i++) //for each allowed value we
        { 
            current[depth] = allowed[i];  //selected value 

            GenerateRecursive(
                allowed,
                size,
                N,   //number of monsters per wave
                target,
                current,
                depth + 1,
                sum + allowed[i],
                i + 1
            );
        }
    }

public:

    Backtracking()
    {
        combinationCount = 0;
    }

    void Generate(int allowed[], int size, int N, int target)  //cuz game kai douran recursion kou kaam starting value dai ga
    {
        combinationCount = 0;

        int current[10];

        GenerateRecursive(
            allowed,
            size,
            N,
            target,
            current,
            0,
            0,
            0
        );  //starting recursion
    }
// [image-comments/image-20261122-042843-ltiti4.png]

// [.vscode/image-comments/image-20261122-042843-ltiti4.png]

 int GetCombinationCount()
{
    return combinationCount;
}

int GetHealth(int combinationIndex, int monsterIndex)
{
    return combinations[combinationIndex][monsterIndex];
}
    void Display(int N)
    {
        for (int i = 0; i < combinationCount; i++)
        {
            for (int j = 0; j < N; j++)
            {
                cout << combinations[i][j] << " ";
            }

            cout << endl;
        }
    }
};


bool SelectWave(
    Backtracking& bt,
    int allowed[],
    int size,
    int N,
    int targetHealth,
    int waveHealth[],
    int& combinationIndex)
{
    bt.Generate(allowed, size, N, targetHealth);

    int count = bt.GetCombinationCount();

    if (count == 0) //backtracking kou kuch milla hee nahi
        return false;

    if (combinationIndex >= count)
        return false;

    // Cycle through valid combinations for increasing difficulty hehe
    // No looping back to the first combination; move to the next difficulty instead.

    for (int i = 0; i < N; i++)
    {
        waveHealth[i] = bt.GetHealth(combinationIndex, i);
    }

    //no loop cuz game hee multiple times call kray ga
    combinationIndex++;

    return true;
}


// [Backtracking documentation](documentation/backtracking.md)

// documentation/backtracking.md

class Player
{
public:
    Texture2D texture;
    float x, y, width, height, scale, speed;
    int health, maxHealth;
    float damageCooldown;
    float healFlashTimer;

    Player(Texture2D playerTexture)
    {
        texture = playerTexture;
        scale = 0.25f;

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
        healFlashTimer = 0.0f;
    }

    void Update(float dt)
    {
        damageCooldown -= dt;
        healFlashTimer -= dt;

        if (health <= 0) return;

        if (IsKeyDown(KEY_LEFT)) x -= speed * dt;
        if (IsKeyDown(KEY_RIGHT)) x += speed * dt;
        if (IsKeyDown(KEY_UP)) y -= speed * dt;
        if (IsKeyDown(KEY_DOWN)) y += speed * dt;

        if (x < 0) x = 0;
        if (x > SCREEN_WIDTH - width) x = SCREEN_WIDTH - width;
        if (y < 0) y = 0;
        if (y > SCREEN_HEIGHT - height) y = SCREEN_HEIGHT - height;
    }

    void Draw()
    {
        Color tint = healFlashTimer > 0 ? LIME : WHITE;

        if (texture.id != 0)
            DrawTextureEx(texture, {x, y}, 0.0f, scale, tint);
        else
            DrawRectangle((int)x, (int)y, (int)width, (int)height, tint);
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
            if (health < 0) health = 0;
            damageCooldown = 1.0f;
        }
    }

    void Heal(int amount)
    {
        if (health < maxHealth)
        {
            health += amount;
            if (health > maxHealth) health = maxHealth;
            healFlashTimer = 0.8f;
        }
    }

    void Reset()
    {
        x = 20;
        y = SCREEN_HEIGHT - height;
        health = maxHealth;
        damageCooldown = 0;
        healFlashTimer = 0;
    }
};

// ==========================================
// MONSTER NODE
// ==========================================

class Monster
{
public:
    Rectangle position;
    float speed;
    int health;
    int maxHealth;
    bool hasBeenHit;
    Monster* next;

    Monster(Texture2D texture, int monsterHealth){
    position = {
        (float)SCREEN_WIDTH + 50,
        (float)GetRandomValue(50, SCREEN_HEIGHT - 150),
        100,
        100
        };
       // [image-comments/image-20261122-044145-nxyw9t.png]
     speed = (float)GetRandomValue(100, 180);
     health = monsterHealth;
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
            DrawRectangle((int)position.x, (int)position.y - 12, (int)(position.width * ((float)health / maxHealth)), 8, RED);
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

    void Spawn(Texture2D texture, int monsterHealth){ //insert new monster at the head of the list
     Monster* newMonster = new Monster(texture, monsterHealth);

     newMonster->next = head;
     head = newMonster;
    }


    void Update(float dt, Rectangle playerRectangle, int& playerDamage, bool& monsterEscaped)
    {
        Monster* current = head;
        Monster* previous = nullptr;

        while (current != nullptr)
        {
            current->Update(dt);

            if (CheckCollisionRecs(current->position, playerRectangle) || current->position.x + current->position.width < 0)
            {
                Monster* nodeToDelete = current;

                if (previous == nullptr)
                    head = current->next;
                else
                    previous->next = current->next;

                if (CheckCollisionRecs(nodeToDelete->position, playerRectangle))
                    playerDamage++;
                else
                    monsterEscaped = true;

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

    void Clear()
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

    bool IsEmpty(){
    return head == nullptr;
}

    ~MonsterList()
    {
        Clear();
    }
};

// ==========================================
// BULLET NODE
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
        Vector2 direction = {target.x - gunPosition.x, target.y - gunPosition.y};

        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (length == 0) return;

        direction.x /= length;
        direction.y /= length;

        Vector2 velocity = {direction.x * 650, direction.y * 650};

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
            Rectangle bulletRectangle = {current->position.x - 5, current->position.y - 5, 10, 10};

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

    void RemoveBullets()
    {
        Bullet* current = head;
        Bullet* previous = nullptr;

        while (current != nullptr)
        {
            if (current->position.x < 0 || current->position.x > SCREEN_WIDTH || current->position.y < 0 || current->position.y > SCREEN_HEIGHT)
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

    void Clear()
    {
        Bullet* current = head;

        while (current != nullptr)
        {
            Bullet* temp = current;
            current = current->next;
            delete temp;
        }

        head = nullptr;
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
// START GAME
// ==========================================

void StartGame(
    Player& player,
    BulletList& bullets,
    MonsterList& monsters,
    FireflyList& fireflies,
    float& shootTimer,
    float& monsterSpawnTimer,
    float& fireflySpawnTimer,
    float& healMessageTimer,
    float& waveCompleteTimer,
    Backtracking& bt,
    int allowed[],
    int monstersPerWave,
    int& currentWave,
    int& monstersSpawned,
    int& nextMonsterIndex,
    int& targetHealth,
    int waveHealth[],
    int& combinationIndex,
    bool& validWave)
{
    player.Reset();
    bullets.Clear();
    monsters.Clear();
    fireflies.Clear();

    shootTimer = 0;
    monsterSpawnTimer = 0;
    fireflySpawnTimer = 3.0f;
    healMessageTimer = 0;
    waveCompleteTimer = 0;

    // Reset wave progression
    currentWave = 1;
    monstersSpawned = 0;
    nextMonsterIndex = 0;
    targetHealth = 1375;
    combinationIndex = 0;

    validWave = SelectWave(
        bt, allowed, 15, monstersPerWave,
        targetHealth, waveHealth, combinationIndex
    );
}

// ==========================================
// GAME STATES
// ==========================================

enum gamestate
{
    MENU,
    PLAYING,
    WAVE_COMPLETE,
    GAME_COMPLETE,
    GAME_OVER
};

// ==========================================
// MAIN
// ==========================================

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Monster Shooter");
    InitAudioDevice();
    SetTargetFPS(60);

    Texture2D playerTexture = LoadTexture("media and assets/masked_figure_transparent.png");
    Texture2D background = LoadTexture("media and assets/temp_gb.png");
    Texture2D monsterTexture = LoadTexture("media and assets/monssterr.jpg");
    Music backgroundMusic = LoadMusicStream("media and assets/Reach Out To The Truth (Instrumental).mp3");
    PlayMusicStream(backgroundMusic);

    bool backgroundLoaded = background.id != 0;
    bool monsterImageLoaded = monsterTexture.id != 0;

    Player player(playerTexture);
    BulletList bullets;
    MonsterList monsters;
    FireflyList fireflies;

    float shootTimer = 0.0f;
    const float shootCooldown = 0.2f;
    float monsterSpawnTimer = 0.0f;
    const float monsterSpawnInterval = 2.0f;
    float fireflySpawnTimer = 3.0f;
    const float fireflySpawnInterval = 5.0f;
    float healMessageTimer = 0.0f;
    float waveCompleteTimer = 0.0f;
    const float waveCompleteDuration = 2.0f;
    // WAVE SYSTEM

  int allowed[] = {
    25, 50, 75, 100, 125,
    150, 175, 200, 225, 250,
    275, 300, 325, 350, 375
   };
    int allowedSize = sizeof(allowed) / sizeof(allowed[0]); // Calculate the size of the allowed array
Backtracking bt;

int currentWave = 1;
int monstersPerWave = 10;
int monstersSpawned = 0;
int nextMonsterIndex = 0;

int targetHealth = 1375;
int waveHealth[10];

// [image-comments/image-20261122-074855-a6vbh9.png]

int combinationIndex = 0;

bool validWave = false;
    gamestate gameState = MENU;

    Rectangle startButton = {SCREEN_WIDTH / 2.0f - 110, SCREEN_HEIGHT / 2.0f + 30, 220, 65};
    Rectangle playAgainButton = {SCREEN_WIDTH / 2.0f - 110, SCREEN_HEIGHT / 2.0f + 55, 220, 65};

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        Vector2 mousePosition = GetMousePosition();
        UpdateMusicStream(backgroundMusic);
        SetMusicVolume(backgroundMusic, 0.8f);

        // ==========================================
        // MENU STATE
        // ==========================================

        if (gameState == MENU)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePosition, startButton))
            {
                StartGame(player, bullets, monsters, fireflies, shootTimer, monsterSpawnTimer, fireflySpawnTimer, healMessageTimer, waveCompleteTimer, bt, allowed, monstersPerWave, currentWave, monstersSpawned, nextMonsterIndex, targetHealth, waveHealth, combinationIndex, validWave);
                gameState = PLAYING;
            }
        }

        // ==========================================
        // PLAYING STATE
        // ==========================================

        else if (gameState == PLAYING)
        {
            shootTimer -= dt;
            monsterSpawnTimer -= dt;
            fireflySpawnTimer -= dt;
            healMessageTimer -= dt;

            player.Update(dt);

            // SHOOTING

            if (player.health > 0 && IsKeyDown(KEY_SPACE) && shootTimer <= 0)
            {
                Vector2 gunPosition = {player.x + player.width, player.y + player.height / 2.0f};
                Vector2 target = {gunPosition.x + 1, gunPosition.y};

                bullets.Shoot(gunPosition, target);
                shootTimer = shootCooldown;
            }

            // MONSTER SPAWNING

           if (monsterSpawnTimer <= 0 && player.health > 0 && monstersSpawned < monstersPerWave)
            {
                     monsters.Spawn(
                     monsterTexture,
                     waveHealth[nextMonsterIndex]
             );

                monstersSpawned++;
                nextMonsterIndex++;
                monsterSpawnTimer = monsterSpawnInterval;
            }

            // BULLET UPDATE AND COLLISIONS

            bullets.Update(dt);
            bullets.CheckMonsterCollisions(monsters);
            bullets.RemoveBullets();

            // MONSTER UPDATE AND PLAYER DAMAGE

            int monsterDamage = 0;
            bool monsterEscaped = false;
            monsters.Update(dt, player.GetRectangle(), monsterDamage, monsterEscaped);

            if (monsterEscaped)
                player.health = 0;

            if (monsterDamage > 0 && player.health > 0)
                player.TakeDamage(10);
            

            // WAVE COMPLETION

        if (monstersSpawned == monstersPerWave &&
            monsters.IsEmpty() &&
            player.health > 0)
        {
            waveCompleteTimer = waveCompleteDuration;
            gameState = WAVE_COMPLETE;
        }

            // ==========================================
            // FIREFLY SPAWNING
            // ==========================================

            if (player.health < player.maxHealth && fireflySpawnTimer <= 0)
            {
                fireflies.Spawn();
                fireflySpawnTimer = fireflySpawnInterval;
            }

            // ==========================================
            // FIREFLY COLLECTION
            // ==========================================

            bool fireflyCollected = false;
            fireflies.Update(player, fireflyCollected);

            if (fireflyCollected)
                healMessageTimer = 1.2f;

            // GAME OVER

            if (player.health <= 0)
                gameState = GAME_OVER;
        }

        // ==========================================
        // WAVE COMPLETE STATE
        // ==========================================

        else if (gameState == WAVE_COMPLETE)
        {
            waveCompleteTimer -= dt;

            if (waveCompleteTimer <= 0)
            {
                // Select the next health combination
                validWave = SelectWave(
                    bt, allowed, allowedSize, monstersPerWave,
                    targetHealth, waveHealth, combinationIndex
                );

                // All combinations for this difficulty have been used
                int maximumTarget = 0;
                for (int i = allowedSize - monstersPerWave; i < allowedSize; i++)
                    maximumTarget += allowed[i];

                while (!validWave && targetHealth < maximumTarget)
                {
                    targetHealth += 25;
                    combinationIndex = 0;

                    validWave = SelectWave(
                        bt, allowed, allowedSize, monstersPerWave,
                        targetHealth, waveHealth, combinationIndex
                    );
                }

                if (validWave)
                {
                    currentWave++;
                    monstersSpawned = 0;
                    nextMonsterIndex = 0;
                    monsterSpawnTimer = 2.0f;
                    gameState = PLAYING;
                }
                else
                {
                    gameState = GAME_COMPLETE;
                }
            }
        }

        // ==========================================
        // GAME OVER STATE
        // ==========================================

        else if (gameState == GAME_OVER)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePosition, playAgainButton))
            {
                StartGame(player, bullets, monsters, fireflies, shootTimer, monsterSpawnTimer, fireflySpawnTimer, healMessageTimer, waveCompleteTimer, bt, allowed, monstersPerWave, currentWave, monstersSpawned, nextMonsterIndex, targetHealth, waveHealth, combinationIndex, validWave);
                gameState = PLAYING;
            }
        }

        // ==========================================
        // DRAWING
        // ==========================================

        BeginDrawing();
        ClearBackground(BLACK);

        // BACKGROUND

        if (backgroundLoaded)
            DrawTexturePro(background, {0, 0, (float)background.width, (float)background.height}, {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT}, {0, 0}, 0, WHITE);

        // ==========================================
        // FIREFLY HEALING GLOW EFFECT
        // ==========================================

        if (gameState == PLAYING && player.healFlashTimer > 0)
        {
            float glowAlpha = player.healFlashTimer / 0.8f;

            // Brief green tint across the game screen
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(LIME, glowAlpha * 0.18f));

            // Layered glow behind the player
            Vector2 playerCenter = {player.x + player.width / 2.0f, player.y + player.height / 2.0f};
            DrawCircleV(playerCenter, 95, Fade(GREEN, glowAlpha * 0.12f));
            DrawCircleV(playerCenter, 70, Fade(LIME, glowAlpha * 0.22f));
            DrawCircleV(playerCenter, 48, Fade(LIME, glowAlpha * 0.30f));
        }

        // ==========================================
        // MENU SCREEN
        // ==========================================

        if (gameState == MENU)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.55f));

            DrawText("MONSTER SHOOTER", SCREEN_WIDTH / 2 - 260, 220, 55, WHITE);
            DrawText("Move: ARROW KEYS     Shoot: SPACE", SCREEN_WIDTH / 2 - 250, 300, 22, LIGHTGRAY);

            bool hovering = CheckCollisionPointRec(mousePosition, startButton);
            DrawRectangleRec(startButton, hovering ? DARKGREEN : GREEN);
            DrawRectangleLinesEx(startButton, 2, WHITE);
            DrawText("START", (int)startButton.x + 70, (int)startButton.y + 17, 30, WHITE);
        }

        // ==========================================
        // PLAYING SCREEN
        // ==========================================

        else if (gameState == PLAYING)
        {
            // FIREFLIES

            fireflies.Draw();

            // PLAYER

            player.Draw();

            // MONSTERS

            if (monsterImageLoaded)
                monsters.Draw(monsterTexture);
            else
                DrawText("MONSTER IMAGE FAILED TO LOAD", 400, 350, 30, RED);

            // BULLETS

            bullets.Draw();

            // ==========================================
            // HEALTH HUD
            // ==========================================

// ==========================================
// HEALTH HUD - BOTTOM LEFT
// ==========================================

DrawText("PLAYER HEALTH", 25, 665, 20, LIME);

DrawRectangle(25, 695, 250, 18, Fade(DARKGRAY, 0.8f));

float healthPercentage = (float)player.health / player.maxHealth;
Color healthColor = healthPercentage > 0.5f ? GREEN : healthPercentage > 0.25f ? LIME : RED;

DrawRectangle(25, 695, (int)(250 * healthPercentage), 18, healthColor);
DrawRectangleLines(25, 695, 250, 18, Fade(LIME, 0.7f));

DrawText(TextFormat("HP: %i / %i", player.health, player.maxHealth), 25, 720, 18, LIME);

// CONTROLS

DrawText("ARROWS: MOVE", 25, 750, 16, GREEN);
DrawText("SPACE: SHOOT", 25, 772, 16, GREEN);

// ==========================================
// HEALING NOTIFICATION
// ==========================================

if (healMessageTimer > 0)
{
    DrawText("+15 HEALTH! FIREFLY COLLECTED", 25, 625, 20, LIME);
}
        }

        // ==========================================
        // WAVE COMPLETE SCREEN
        // ==========================================

        else if (gameState == WAVE_COMPLETE)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.78f));
            DrawText(TextFormat("WAVE %i COMPLETED!", currentWave),
                SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 50, 45, GOLD);
            DrawText("GET READY...", SCREEN_WIDTH / 2 - 100,
                SCREEN_HEIGHT / 2 + 25, 25, LIGHTGRAY);
        }

        // ==========================================
        // GAME COMPLETE SCREEN
        // ==========================================

        else if (gameState == GAME_COMPLETE)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.78f));
            DrawText("ALL WAVES CLEARED!", SCREEN_WIDTH / 2 - 275,
                SCREEN_HEIGHT / 2 - 35, 45, GOLD);
        }

        // ==========================================
        // GAME OVER SCREEN
        // ==========================================

        else if (gameState == GAME_OVER)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.75f));

            DrawText("GAME OVER", SCREEN_WIDTH / 2 - 165, SCREEN_HEIGHT / 2 - 100, 55, RED);

            bool hovering = CheckCollisionPointRec(mousePosition, playAgainButton);
            DrawRectangleRec(playAgainButton, hovering ? DARKGREEN : GREEN);
            DrawRectangleLinesEx(playAgainButton, 2, WHITE);

            DrawText("PLAY AGAIN", (int)playAgainButton.x + 28, (int)playAgainButton.y + 17, 30, WHITE);
        }

        EndDrawing();
    }

    // ==========================================
    // CLEANUP
    // ==========================================

    UnloadTexture(background);

    if (player.texture.id != 0)
        UnloadTexture(player.texture);

    if (monsterImageLoaded)
        UnloadTexture(monsterTexture);
    UnloadMusicStream(backgroundMusic);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}