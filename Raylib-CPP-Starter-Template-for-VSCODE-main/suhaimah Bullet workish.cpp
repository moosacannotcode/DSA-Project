#include <iostream>
#include "raylib.h"
#pragma once
#include <cmath>
using namespace std;

class Bullet //simple linkedlist for bullet
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

    void Update(float dt)  //what would dt be? lo
    {
        position.x += velocity.x * dt;  //delta time to update the bullets position and make it frame rate independent
        position.y += velocity.y * dt;
    }

    void Draw()
    {
        DrawCircleV(position, 5, YELLOW);
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

    // Create and insert a bullet
    void Shoot(Vector2 gunPosition, Vector2 target)
    {
        Vector2 direction = {
            target.x - gunPosition.x,
            target.y - gunPosition.y
        };

        float length = sqrt(
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

    // Move all bullets
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

    // Delete bullets outside screen
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

    // Free remaining memory
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

