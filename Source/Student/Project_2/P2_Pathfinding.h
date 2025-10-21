#pragma once
#include "Misc/PathfindingDetails.hpp"
#include <bitset>

struct Node {
    //Points to previous nodes in the current best path
    Node* parent = nullptr; //8 Bytes
    //Cost from start node to current node
    float g = 1e30f; //4 Bytes
    //Cost form current node to the goal node
    float h = 0.0f; //4 Bytes
    //Total Cost
    float f = 1e30f; //4 Bytes
    //Distinguish node between different path requests
    int version = 0; //4 Bytes

    GridPos pos = { 0,0 }; //8 Bytes
    Vec3 worldPos = { };

    //For resetting
    void reset() {
        parent = nullptr;
        g = 1e30f;
        h = 0.0f;
        f = 1e30f;
        version = 0;
    }
}; //32 Bytes
namespace {
    constexpr int MAX_MAP_SIZE = 40;
    static Node nodeMap[MAX_MAP_SIZE * MAX_MAP_SIZE]; //50 KB
    constexpr float bucketSize = 0.25f;
    constexpr int MAX_BUCKETS = 512; //Max number of f values
    constexpr int BUCKET_CAP = 512; //Max nodes per bucket
}

//To access node, 1D array version
inline Node& get_node(int row, int col, int width) {
    return nodeMap[row * width + col];
}

class AStarPather {
public:
    bool initialize();
    void shutdown();

    //Main A* Logic
    PathResult compute_path(PathRequest& request);
    //Rubberbanding
    void RubberbandPath(WaypointList& path, Terrain* terrain);
    //Catmull-Spline
    void AddIntermediatePoints(WaypointList& path);
    void SmoothPath(WaypointList& path, bool rubberband);

    float heuristic(GridPos a, GridPos b, Heuristic type);
    void reset_nodes();

private:
    int currentVersion = 1; //4 bytes

    Node* openBuckets[MAX_BUCKETS][BUCKET_CAP]; //2 MB
    int bucketHeads[MAX_BUCKETS] = {}; //2048 bytes
    int bucketTails[MAX_BUCKETS] = {}; //2048 bytes
    int minBucket = -1; //4 bytes

    //Bucket Functions
    void push_to_bucket(Node* node);
    Node* pop_from_bucket();
    void clear_buckets();
};
