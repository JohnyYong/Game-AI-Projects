#include <pch.h>
#include "Projects/ProjectTwo.h"
#include "P2_Pathfinding.h"
#include <cmath>

#pragma region Extra Credit
bool ProjectTwo::implemented_floyd_warshall() { return false; }
bool ProjectTwo::implemented_goal_bounding() { return false; }
#pragma endregion

namespace {
    constexpr float SQRT2 = 1.4142135f;
    static uint8_t validDirs[MAX_MAP_SIZE * MAX_MAP_SIZE];
    //DR = delta row, DC = delta column, COST = movement cost
    constexpr int DR[8] = { -1,-1,0,1,1,1,0,-1 }; //N, NE, E, SE, S, SW, W, NW
    constexpr int DC[8] = { 0, 1,1,1,0,-1,-1,-1 };
    constexpr float COST[8] = { 1.f, SQRT2, 1.f, SQRT2, 1.f, SQRT2, 1.f, SQRT2 };
}

bool AStarPather::initialize()
{
    Callback cb = std::bind(&AStarPather::reset_nodes, this);
    Messenger::listen_for_message(Messages::MAP_CHANGE, cb);
    return true;
}

void AStarPather::shutdown() {}

//Only called in initialisation
void AStarPather::reset_nodes()
{
    int height = terrain->get_map_height();
    int width = terrain->get_map_width();

    //Iteration over rows and columns
    for (int r = 0; r < height; ++r)
    {
        for (int c = 0; c < width; ++c)
        {
            int index = r * width + c; //Since I am already using a flattened array, a single index would be sufficient
            Node& node = nodeMap[index];
            node.reset(); //Clears ghfparent and node
            node.pos = { r, c }; //New Pos
            node.worldPos = terrain->get_world_position(node.pos);

            //REMEMBER AVOID CORNER CLIPPING
            uint8_t mask = 0; //Store valid movements
            const int lastRow = height - 1;
            const int lastCol = width - 1;

            //Directional Checks to be placed into the bit
            //1 bit = 1 direction
            if (r > 0 && !terrain->is_wall(r - 1, c)) mask |= 1 << 0; // North
            if (r > 0 && c < lastCol &&
                !terrain->is_wall(r - 1, c + 1) &&
                !terrain->is_wall(r - 1, c) &&
                !terrain->is_wall(r, c + 1)) mask |= 1 << 1; // NorthEast
            if (c < lastCol && !terrain->is_wall(r, c + 1)) mask |= 1 << 2; // East
            if (r < lastRow && c < lastCol &&
                !terrain->is_wall(r + 1, c + 1) &&
                !terrain->is_wall(r + 1, c) &&
                !terrain->is_wall(r, c + 1)) mask |= 1 << 3; // SouthEast
            if (r < lastRow && !terrain->is_wall(r + 1, c)) mask |= 1 << 4; // South
            if (r < lastRow && c > 0 &&
                !terrain->is_wall(r + 1, c - 1) &&
                !terrain->is_wall(r + 1, c) &&
                !terrain->is_wall(r, c - 1)) mask |= 1 << 5; // SouthWest
            if (c > 0 && !terrain->is_wall(r, c - 1)) mask |= 1 << 6; // West
            if (r > 0 && c > 0 &&
                !terrain->is_wall(r - 1, c - 1) &&
                !terrain->is_wall(r - 1, c) &&
                !terrain->is_wall(r, c - 1)) mask |= 1 << 7; // NorthWest

            validDirs[index] = mask; //Store valid directions
        }
    }
}
//Basically all the movements
float AStarPather::heuristic(GridPos a, GridPos b, Heuristic type)
{
    int dx = std::abs(a.col - b.col);
    int dy = std::abs(a.row - b.row);

    //Purposely put OCTILE as first so that they won't go through the rest of the types for Speed Test :P
    //Euclidean & Inconsistent purposefully placed below due to expensive sqrtf usage
    if (type == Heuristic::OCTILE)
        return std::min(dx, dy) * SQRT2 + std::max(dx, dy) - std::min(dx, dy);
    if (type == Heuristic::MANHATTAN)
        return static_cast<float>(dx + dy);
    if (type == Heuristic::CHEBYSHEV)
        return static_cast<float>(std::max(dx, dy));
    if (type == Heuristic::EUCLIDEAN)
        return sqrtf(static_cast<float>(dx * dx + dy * dy));
    if (type == Heuristic::INCONSISTENT)
    {
        if ((a.row + a.col) % 2 > 0)
            return sqrtf(static_cast<float>(dx * dx + dy * dy));
        else
            return 0.0f;
    }

    return 0.0f;
}

#pragma region Buckets
void AStarPather::push_to_bucket(Node* node)
{
    //Mapping floats to different buckets
    int bucketIndex = static_cast<int>(node->f * 4.0f); //Node->f is the esimated cost, multiplied by X for bucket spread

    if (bucketIndex < 0) bucketIndex = 0;
    if (bucketIndex >= MAX_BUCKETS) bucketIndex = MAX_BUCKETS - 1;

    int& tail = bucketTails[bucketIndex];
    if (tail < BUCKET_CAP)
    {
        //Node placement
        openBuckets[bucketIndex][tail++] = node;
        //If it is the lowest node seen so far, update minBucket for faster access
        if (minBucket == -1 || bucketIndex < minBucket)
            minBucket = bucketIndex;
    }
}

Node* AStarPather::pop_from_bucket()
{
    while (minBucket < MAX_BUCKETS)
    {
        //Checking current known minimum bucket index
        int& head = bucketHeads[minBucket];
        int& tail = bucketTails[minBucket];

        while (head < tail)
        {
            //Walk through the nodes from head to tail and skips outdated or invalid nodes via version checking (aka how we see our closed nodes)
            Node* candidate = openBuckets[minBucket][head++];
            if (candidate->version == currentVersion)
                return candidate;
        }
        ++minBucket; //Move to next cheapest bucket if none is found
    }
    return nullptr;
}

void AStarPather::clear_buckets()
{
    //Setting the buckets manually to 0
    std::memset(bucketHeads, 0, sizeof(bucketHeads));
    std::memset(bucketTails, 0, sizeof(bucketTails));
    minBucket = -1;
}

#pragma endregion

#pragma region Rubber-Banding
void AStarPather::RubberbandPath(WaypointList& path, Terrain* terrainTaken)
{
    if (path.size() < 3) return; //If Path is short what is the point of cutting paths

    //The 3 main paths, prev, mid, next
    WaypointList newPath;
    auto prev = path.begin();
    auto mid = std::next(prev);
    auto next = std::next(mid);

    newPath.push_back(*prev);

    while (next != path.end())
    {
        GridPos p0 = terrainTaken->get_grid_position(*prev);
        GridPos p2 = terrainTaken->get_grid_position(*next);

        bool hitWall = false;
        int rowMin = std::min(p0.row, p2.row);
        int rowMax = std::max(p0.row, p2.row);
        int colMin = std::min(p0.col, p2.col);
        int colMax = std::max(p0.col, p2.col);

        //Checks if there is a wall between the prev and next, force it to not be able to skip
        //A type of bounding box check
        for (int r = rowMin; r <= rowMax && !hitWall; ++r)
        {
            for (int c = colMin; c <= colMax; ++c)
            {
                if (terrainTaken->is_wall(r, c))
                {
                    hitWall = true;
                    break;
                }
            }
        }

        //If a wall is hit, then we keep mid in the new path, skip it otherwise
        if (hitWall)
        {
            newPath.push_back(*mid);  //Keep mid
            prev = mid;
            mid = next;
            next = std::next(next);
        }
        else
        {
            //Skip mid entirely
            mid = next;
            next = std::next(next);
        }

    }

    newPath.push_back(*mid);
    if (mid != path.end())
        newPath.push_back(path.back());

    path = std::move(newPath);
}
#pragma endregion

#pragma region CatMullRomSpline
void AStarPather::AddIntermediatePoints(WaypointList& path)
{
    if (path.size() < 3) return;

    WaypointList newPath;
    auto prevIt = path.begin();
    auto currIt = std::next(prevIt);

    newPath.push_back(*prevIt);

    while (currIt != path.end())
    {
        const Vec3& prev = *prevIt;
        const Vec3& curr = *currIt;

        //Convert both points to GridPos
        GridPos gridPrev = terrain->get_grid_position(prev);
        GridPos gridCurr = terrain->get_grid_position(curr);

        int dx = abs(gridPrev.col - gridCurr.col);
        int dy = abs(gridPrev.row - gridCurr.row);
        float gridDist = std::sqrt(static_cast<float>(dx * dx + dy * dy));

        if (gridDist > 1.5f) //REMEMBER 1.5 DISTANCE OR ELSE WROOOOONG
        {
            int splits = static_cast<int>(std::ceil(gridDist / 1.5f));
            for (int j = 1; j < splits; ++j)
            {
                float t = static_cast<float>(j) / splits;
                Vec3 interp = Vec3::Lerp(prev, curr, t);
                newPath.push_back(interp);
            }
        }

        newPath.push_back(curr);
        prevIt = currIt;
        ++currIt;
    }

    path = std::move(newPath);
}

void AStarPather::SmoothPath(WaypointList& path, bool rubber)
{
    if (path.size() < 4) return;

    WaypointList newPath;

    if (rubber) //Rubberband + Smoothing
    {
        AddIntermediatePoints(path); //Ensure spacing for the 1.5 addback

        auto it0 = path.begin();
        auto it1 = path.begin();
        auto it2 = std::next(it1);
        auto it3 = std::next(it2);

        while (it2 != std::prev(path.end()))
        {

            for (int i = 1; i < 4; ++i)
            {
                float t = i * 0.25f;
                Vec3 newPoint = Vec3::CatmullRom(*it0, *it1, *it2, *it3, t);
                newPath.push_back(newPoint);
            }

            newPath.push_back(*it2); // Keep original P2

            if (it0 == it1)
            {
                it1 = it2;
                it2 = it3;
                ++it3;
            }
            else if (it3 == std::prev(path.end()))
            {
                it0 = it1;
                it1 = it2;
                it2 = it3;
            }
            else
            {
                it0 = it1;
                it1 = it2;
                it2 = it3;
                ++it3;
            }
        }

        if (it2 != path.end())
            newPath.push_back(*it2);

    }
    else //Smoothing only
    {
        auto it0 = path.begin();
        auto it1 = it0;
        auto it2 = std::next(it1);
        auto it3 = std::next(it2);

        newPath.push_back(*it0);
        while (true)
        {
            bool isFinalSegment = (it3 == path.end());

            Vec3 p3 = isFinalSegment ? *it2 : *it3;

            for (int i = 1; i < 4; ++i)
            {
                float t = i * 0.25f;
                Vec3 newPoint = Vec3::CatmullRom(*it0, *it1, *it2, p3, t);
                newPath.push_back(newPoint);
            }

            if (!(isFinalSegment && *it2 == path.back()))
                newPath.push_back(*it2);

            if (isFinalSegment)
                break;

            it0 = it1;
            it1 = it2;
            it2 = it3;
            ++it3;
        }

    }

    path = std::move(newPath);
}

#pragma endregion

//A-Star Pathfinding
PathResult AStarPather::compute_path(PathRequest& request)
{
    GridPos start = terrain->get_grid_position(request.start);
    GridPos goal = terrain->get_grid_position(request.goal);

    int width = terrain->get_map_width();

    float weight = request.settings.weight;
    Heuristic heuristicType = request.settings.heuristic;

    //Receiving the first version of the request bucket lists
    if (request.newRequest)
    {
        currentVersion++;
        clear_buckets();

        //Pushing available node into the initial color
        Node& startNode = get_node(start.row, start.col, width);
        startNode.version = currentVersion;
        startNode.g = 0.0f;
        startNode.h = heuristic(start, goal, heuristicType);
        startNode.f = startNode.h * weight;
        startNode.parent = nullptr;

        push_to_bucket(&startNode);
    }

    const bool doDebugColor = request.settings.debugColoring; //So that I don't have to type such long text everytime

    while (true)
    {
        Node* current = pop_from_bucket();
        if (!current) break;

        if (doDebugColor)
            terrain->set_color(current->pos, Colors::Yellow);

        if (current->pos == goal) //If current position is goal
        {
            request.path.clear();  //Clear existing path in case reused

            //For tracking path backward from the Goal
            while (current)
            {
                request.path.push_back(current->worldPos);
                current = current->parent;
            }

            //From path to Go
            std::reverse(request.path.begin(), request.path.end());

            if (doDebugColor)
                terrain->set_color(goal, Colors::Blue);

            //For adding RubberBanding && Catmull
            if (request.settings.rubberBanding)
                RubberbandPath(request.path, terrain.get());

            if (request.settings.smoothing)
                SmoothPath(request.path, request.settings.rubberBanding ? true : false);

            clear_buckets(); //Clean the bucket up for future use
            return PathResult::COMPLETE;
        }

        const uint8_t dirMask = validDirs[current->pos.row * width + current->pos.col]; //Bitmask of legal directions

        //Neighbour checker macro
        //Unrolled for loop in order to optimise for Speed Test
        //Versioning system used for closed list
        // ================================================================
        //How does versioning System work?
        //Normally in A* we have to track the closed list, and reset the map before each search. 
        //With the implementation of node struct, and the AStarPathFinder storing a version, we say that any nodes with version lower than current version is from an old search
        //As such, it would be treated as uninitialised. Thus when we touch a node, we have a lazy reset so that we do not clear the entire map at once. Thus becoming a closed list substitute.
        //=================================================================
#define TRY_DIR(i) \
        if (dirMask & (1 << i)) { \
            int nr = current->pos.row + DR[i]; \
            int nc = current->pos.col + DC[i]; \
            int nodeIndex = nr * width + nc; \
            Node& neighbor = nodeMap[nodeIndex]; \
            if (neighbor.version != currentVersion) { \
                neighbor.reset(); \
                neighbor.version = currentVersion; \
            } \
            float tentativeG = current->g + COST[i]; \
            if (tentativeG < neighbor.g) { \
                neighbor.parent = current; \
                neighbor.g = tentativeG; \
                neighbor.h = heuristic(neighbor.pos, goal, heuristicType); \
                neighbor.f = neighbor.g + neighbor.h * weight; \
                push_to_bucket(&neighbor); \
                if (doDebugColor) terrain->set_color(neighbor.pos, Colors::Blue); \
            } \
        }
        //Each block checks 1 direction bit and processes if valid
        TRY_DIR(0); // N
        TRY_DIR(2); // E
        TRY_DIR(4); // S
        TRY_DIR(6); // W
        TRY_DIR(1); // NE
        TRY_DIR(3); // SE
        TRY_DIR(5); // SW
        TRY_DIR(7); // NW

#undef TRY_DIR

        if (request.settings.singleStep)
            return PathResult::PROCESSING;
    }

    clear_buckets(); //Clear bucket for future use
    return PathResult::IMPOSSIBLE;
}
