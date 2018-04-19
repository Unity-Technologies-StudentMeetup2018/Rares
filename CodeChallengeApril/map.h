#pragma once
#include <stdint.h>
#include <vector>
#include <string>

struct vector2
{
public:
    int32_t x, y;


    vector2()
    {
        x = 0;
        y = 0;
    }

    vector2(const vector2& vect)
    {
        x = vect.x;
        y = vect.y;
    }

    vector2(int32_t xN, int32_t yN)
    {
        x = xN;
        y = yN;
    }

    bool operator==(const vector2& other)
    {
        return x == other.x && y == other.y;
    }

    vector2 operator+(const vector2& other)
    {
        return vector2(x + other.x, y + other.y);
    }
};

struct Map
{
public:

    static const char cellBlocked = '#';
    static const char cellFree = '.';
    static const char cellInvalid = '?';
    static const char cellPath = '@';

    static const uint32_t kMapSize = 32;

    Map(const std::vector<char>& mapFileData, vector2 startPoint, vector2 endPoint)
    {
        uint32_t idx = 0;

        for (int32_t i = 0; i < mapFileData.size() && idx < kMapSize * kMapSize; ++i)
        {
            if (mapFileData[i] == cellBlocked || mapFileData[i] == cellFree)
            {
                m_MapData[idx % kMapSize][idx / kMapSize] = mapFileData[i];
                idx++;
            }
            // Skipping new line and eventual spaces at the end of line.
        }

        m_StartPosition = startPoint;
        m_GoalPosition = endPoint;
    }

    std::string SolutionToString()
    {
        std::string sol;
        sol.reserve(kMapSize * kMapSize);

        for (int32_t i = 0; i < kMapSize; ++i)
        {
            sol.append(m_MapData[i], kMapSize);
            sol.append("\n");
        }
        return sol;
    }

    void DisplayMap()
    {
        for (int32_t y = 0; y < kMapSize; ++y)
        {
            for (int32_t x = 0; x < kMapSize; ++x)
            {
                if (x == m_StartPosition.x && y == m_StartPosition.y)
                    putchar(cellPath);
                else if (x == m_GoalPosition.x && y == m_GoalPosition.y)
                    putchar(cellPath);
                else
                {
                    switch (m_MapData[x][y])
                    {
                        case cellBlocked: putchar(cellBlocked);
                            break;
                        case cellFree: putchar(cellFree);
                            break;
                        case cellPath: putchar(cellPath);
                            break;
                        default: putchar(cellInvalid);
                            break;
                    }
                }
            }
            putchar('\n');
        }
    }

    bool ComputePath()
    {
        return SolveRecursive(m_StartPosition);
    }

    bool SolveRecursive(vector2 currentTile)
    {
        vector2* currentNeighbors = GetNeighborsPositions(currentTile);
        m_MapData[currentTile.x][currentTile.y] = '@';
        DisplayMap();

        if (CheckEndCondition(currentTile))
        {
            return true;
        }
        // Needs to stop searching if surrounded by invalid tiles
        for (int i = 0; i < 8; ++i)
        {
            if (IsWithinBounds(currentNeighbors[i]) && !IsBlockOccupied(currentNeighbors[i]))
            {
                SolveRecursive(currentNeighbors[i]);
            }
        }
    }

    char* GetNeighbors(vector2 tile)
    {
        char* neighbors = new char[8];
        int index = 0;
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if (IsWithinBounds(tile + vector2(i, j)))
                {
                    neighbors[index++] = m_MapData[tile.x + i][tile.y + j];
                }
                else
                {
                    neighbors[index++] = '#';
                }
            }
        }
        return neighbors;
    }

    vector2* GetNeighborsPositions(vector2 tile)
    {
        vector2* neighbors = new vector2[8];
        int index = 0;
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                neighbors[index++] = tile + vector2(i, j);
            }
        }
        return neighbors;
    }

    bool CheckEndCondition(vector2 currentTile)
    {
        if (currentTile.x - m_GoalPosition.x >= -1 && currentTile.x - m_GoalPosition.x <= 1)
            if (currentTile.y - m_GoalPosition.y >= -1 && currentTile.y - m_GoalPosition.y <= 1)
                return true;
        return false;
    }

    bool IsWithinBounds(vector2 tile)
    {
        if (tile.x > 0 && tile.y > 0)
        {
            if (tile.x < kMapSize && tile.y < kMapSize)
            {
                return true;
            }
        }
        return false;
    }

    bool IsBlockOccupied(vector2 vect)
    {
        return m_MapData[vect.x][vect.y] != cellFree;
    }

private:

    char m_MapData[kMapSize][kMapSize];
    vector2 m_StartPosition;
    vector2 m_GoalPosition;
};
