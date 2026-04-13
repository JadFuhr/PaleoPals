#include "Pathfinding.h"
#include "Map.h"
#include <cmath>
#include <algorithm>

static float manhattan(const GridPos& a, const GridPos& b)
{
    return static_cast<float>(std::abs(a.row - b.row) + std::abs(a.col - b.col));
}

std::vector<GridPos> AStarPathfinder::findPath(
    const Map& map,
    const GridPos& start,
    const GridPos& goal)
{
    const int rows = map.getRowCount();
    const int cols = map.getColumnCount();

    auto inBounds = [&](int r, int c)
        {
            return r >= 0 && r < rows && c >= 0 && c < cols;
        };

    auto walkable = [&](int r, int c)
        {
            return inBounds(r, c) && map.isWalkable(r, c);
        };

    // Early out if start/goal not walkable
    if (!walkable(start.row, start.col) || !walkable(goal.row, goal.col))
        return {};

    // For each tile, store index into node list or -1
    std::vector<int> nodeIndex(rows * cols, -1);

    std::vector<PathNode> nodes;
    nodes.reserve(rows * cols);

    auto indexOf = [&](int r, int c) { return r * cols + c; };

    auto addOrUpdateNode = [&](const GridPos& pos, float gCost, float hCost, int parentIdx) -> int
        {
            int idx = indexOf(pos.row, pos.col);
            int existing = nodeIndex[idx];
            if (existing == -1)
            {
                PathNode node;
                node.pos = pos;
                node.gCost = gCost;
                node.hCost = hCost;
                node.parentIndex = parentIdx;
                nodes.push_back(node);
                int newIndex = static_cast<int>(nodes.size()) - 1;
                nodeIndex[idx] = newIndex;
                return newIndex;
            }
            else
            {
                PathNode& node = nodes[existing];
                if (gCost < node.gCost)
                {
                    node.gCost = gCost;
                    node.hCost = hCost;
                    node.parentIndex = parentIdx;
                }
                return existing;
            }
        };

    struct OpenEntry
    {
        int nodeIdx;
        float fCost;
        bool operator>(const OpenEntry& other) const { return fCost > other.fCost; }
    };

    std::priority_queue<OpenEntry, std::vector<OpenEntry>, std::greater<OpenEntry>> open;
    std::vector<bool> closed(rows * cols, false);

    int startIdx = addOrUpdateNode(start, 0.f, manhattan(start, goal), -1);
    open.push({ startIdx, nodes[startIdx].fCost() });

    const GridPos dirs[4] = {
        { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
    };

    int goalNodeIdx = -1;

    while (!open.empty())
    {
        int currentIdx = open.top().nodeIdx;
        open.pop();

        const PathNode& current = nodes[currentIdx];
        int flatIdx = indexOf(current.pos.row, current.pos.col);
        if (closed[flatIdx])
            continue;

        closed[flatIdx] = true;

        if (current.pos == goal)
        {
            goalNodeIdx = currentIdx;
            break;
        }

        for (const auto& d : dirs)
        {
            GridPos next{ current.pos.row + d.row, current.pos.col + d.col };
            if (!walkable(next.row, next.col))
                continue;

            int nFlat = indexOf(next.row, next.col);
            if (closed[nFlat])
                continue;

            float gNew = current.gCost + 1.f; // cost per step
            float hNew = manhattan(next, goal);
            int nIdx = addOrUpdateNode(next, gNew, hNew, currentIdx);
            open.push({ nIdx, nodes[nIdx].fCost() });
        }
    }

    std::vector<GridPos> path;
    if (goalNodeIdx == -1)
        return path;

    // Reconstruct path
    int idx = goalNodeIdx;
    while (idx != -1)
    {
        path.push_back(nodes[idx].pos);
        idx = nodes[idx].parentIndex;
    }
    std::reverse(path.begin(), path.end());
    return path;
}
