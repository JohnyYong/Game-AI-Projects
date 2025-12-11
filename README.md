# Game AI Projects
Controls:

F1 - Opens Project 1

F2 - Opens Project 2

F3 - Opens Project 3

## Project 1: Behavior Tree & Finite State Machine

A small animation of basic behaviors of a fantasy party fighting a boss have been implemented through usage of behavior trees. 

AIs consisting of:

### Warrior States:
 - Normal Attack
 - Special Attack
 - Ultimate Attack
 - Moving to Boss
   
### Healer States:
 - Recover Health of lowest ally
 - Hiding behind tank

### Tank States:
 - Defend
 - Normal Attack
 - Search & Defend lowest health ally
 - Stun Attack

### Boss States:
 - Stunned
 - Melee Attack
 - Laser Attack

User can create edit and create more behavior trees through BehaviorTreeEditor.exe 

## Project 2: A* Pathfinding & Heuristics

This project focuses on implementing the A* pathfinding algorithm along with common heuristic functions such as Octile, Euclidean, Manhattan, and Chebyshev.
As a bonus challenge, additional optimizations were implemented to improve runtime performance and memory efficiency within the provided engine.

Optimizations Applied:

### Versioning System 
- Used to efficiently reset node states between pathfinding runs without reinitializing the entire grid, reducing overhead and improving performance.

### Bucket-Based Open List
- Replaced the standard priority queue with a bucket list system to group nodes by their f-cost, enabling faster insertions and retrievals during path expansion.

### Unrolled neighbor expansion (macro)
- Uses a single TRY_DIR(i) macro and an unrolled sequence of 8 checks to minimize loop/branch overhead during expansion. The macro keeps the neighbor-relax logic in one place (less code dup), while the unroll cuts control-flow cost.

These optimizations significantly reduced search time while maintaining correctness.

### Directional Bitmask
- For each grid cell, valid movement directions are precomputed and stored as a bitmask. This allows the algorithm to check movement legality using simple bitwise operations instead of terrain lookups. It reduces branching and improves performance in the neighbor expansion stage.

### Flattened Array
- All nodes are stored in a single 1D array using row-column indexing. This improves cache locality and avoids pointer chasing. As a result, node access is faster and more memory-efficient.

## Project 3: Terrain Analysis

This project focused on analyzing environmental data to simulate realistic visibility, openness, and perception systems for AI agents.
It built upon A* pathfinding foundations to introduce terrain-based awareness, line-of-sight detection, and field-of-view logic within a 2D grid environment.

Key Systems Implemented:

### Openness Analysis

Calculates 1 / (d²) values where d is the distance to the nearest wall, helping identify tight corridors and open areas.

### Visibility Mapping

Determines how many cells are visible from each tile using line intersection checks.

Supports dynamic analysis of visibility between any two points.

### Agent Vision & Field of View (FOV)

Uses vector dot products and cosine comparisons for directional FOV detection.

Considers occlusion and proximity (radius-based detection within close range).

### Solo Occupancy Propagation

Simulates decaying influence maps over time using exponential decay and linear interpolation for smooth propagation.

### AI Behavior Integration:

Enemy agents use the generated layers to detect, pursue, and navigate toward the player using normalized influence maps.

Includes detection radius and cone-shaped visibility logic for more realistic enemy perception.
