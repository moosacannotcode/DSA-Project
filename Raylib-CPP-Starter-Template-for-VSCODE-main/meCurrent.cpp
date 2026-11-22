#include "raylib.h"
#include <cmath>
#include <iostream>
using namespace std;



class Backtracking
{
private:

    int combinations[100][10]; //100 possible max waves of 10 monsters each
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

    int count = bt.GetCombinationCount(); //checking for total combinations generated

    if (count == 0) //backtracking kou kuch milla hee nahi
        return false;

    // Cycle through valid combinations for increasing difficulty hehe
    combinationIndex = combinationIndex % count; //no loop cuz game hee multiple times call kray ga

    for (int i = 0; i < N; i++)
    {
        waveHealth[i] = bt.GetHealth(combinationIndex, i);
    }

    combinationIndex++;

    return true;
}


int main()
{
    int allowed[] = {25, 50, 75, 100, 125, 150};

    Backtracking bt;

    bt.Generate(allowed, 6, 3, 225);

    bt.Display(3);

    return 0;
}

// [Backtracking documentation](documentation/backtracking.md)

// documentation/backtracking.md