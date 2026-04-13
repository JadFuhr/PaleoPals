#pragma once
#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <limits>

class Map;

struct GridPos
{
    int row = 0;
    int col = 0;

    bool operator==(const GridPos& other) const
    {
        return row == other.row && col == other.col;
    }
};

struct PathNode
{
    GridPos pos;
    float gCost = 0.f; // cost from start
    float hCost = 0.f; // heuristic to goal
    float fCost() const { return gCost + hCost; }
    int parentIndex = -1; // index into node list
};

class AStarPathfinder
{
public:
    // Returns a path of tile coords from start to goal (inclusive).
    // Empty path means "no path found".
    static std::vector<GridPos> findPath(
        const Map& map,
        const GridPos& start,
        const GridPos& goal);
};

#endif
