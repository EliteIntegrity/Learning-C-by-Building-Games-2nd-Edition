#include "AStar.h"
#include <algorithm>   // std::reverse
#include <cstdlib>   // std::abs
#include <functional>   // std::greater
#include <queue>   // std::priority_queue
#include <unordered_map>   // std::unordered_map
#include "Map.h"

namespace
{
    // A cell waiting to be looked at, and a guess at the whole length of a
    // path through it: the steps it took to get there, plus the steps
    // still to go, if there were no walls in the way
    struct Candidate
    {
        Point cell;
        int guess;

        // So that std::greater can say which of two is farther from the
        // front of the queue
        bool operator>(const Candidate& other) const
        {
            return guess > other.guess;
        }
    };

    // The steps from one cell to another, if there were no walls in the
    // way: across, plus down
    int distance(Point a, Point b)
    {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }

    // The four cells next to a cell: up, down, left, and right
    constexpr Point DIRECTIONS[] = { { 0, -1 }, { 0, 1 }, { -1, 0 }, { 1, 0 } };
}

namespace AStar
{
    // The cells to step through to get from one cell to another, in order,
    // not counting the first cell, but counting the last. It's empty if
    // there's no way through
    std::vector<Point> findPath(const Map& map, Point from, Point to)
    {
        // The cells still to look at, the one with the smallest guess
        // first, and for every cell reached so far, the fewest steps it
        // takes to get there, and the cell it's reached from
        std::priority_queue<Candidate, std::vector<Candidate>,
                            std::greater<Candidate>> waiting;
        std::unordered_map<Point, int> steps;
        std::unordered_map<Point, Point> cameFrom;

        waiting.push({ from, distance(from, to) });
        steps[from] = 0;
        while (!waiting.empty())
        {
            Point cell = waiting.top().cell;
            waiting.pop();
            if (cell == to)
                break;

            // Reach each open neighbor in one more step, unless it's
            // already been reached in as few
            for (Point direction : DIRECTIONS)
            {
                Point next = { cell.x + direction.x, cell.y + direction.y };
                if (map.isBlocked(next))
                    continue;

                int nextSteps = steps[cell] + 1;
                if (!steps.contains(next) || nextSteps < steps[next])
                {
                    steps[next] = nextSteps;
                    cameFrom[next] = cell;
                    waiting.push({ next, nextSteps + distance(next, to) });
                }
            }
        }

        // Follow the trail back from the end to the start, then turn it
        // around
        std::vector<Point> path;
        if (!cameFrom.contains(to))
            return path;

        for (Point cell = to; cell != from; cell = cameFrom[cell])
            path.push_back(cell);
        std::reverse(path.begin(), path.end());
        return path;
    }
}
