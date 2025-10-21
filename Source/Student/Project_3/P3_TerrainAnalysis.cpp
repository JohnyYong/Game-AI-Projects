#include <pch.h>
#include "Terrain/TerrainAnalysis.h"
#include "Terrain/MapMath.h"
#include "Agent/AStarAgent.h"
#include "Terrain/MapLayer.h"
#include "Projects/ProjectThree.h"

#include <iostream>

bool ProjectThree::implemented_fog_of_war() const // extra credit
{
    return false;
}

float distance_to_closest_wall(int row, int col)
{
    /*
    Check the euclidean distance from the given cell to every other wall cell,
    with cells outside the map bounds treated as walls, and return the smallest
    distance.  Make use of the is_valid_grid_position and is_wall member
    functions in the global terrain to determine if a cell is within map bounds
    and a wall, respectively.
    */

    float min_dist = 99999.f;

    //Check all cells in the map
    for (int r = 0; r < terrain->get_map_height(); r++) {
        for (int c = 0; c < terrain->get_map_width(); c++) {
            if (!terrain->is_valid_grid_position(r, c) || terrain->is_wall(r, c)) {
                float dist = (float)(sqrt((row - r) * (row - r) + (col - c) * (col - c)));
                min_dist = std::min(min_dist, dist);
            }
        }
    }

    //Check map borders (treated as walls)
    min_dist = std::min(min_dist, (float)(col + 1));              //Left border
    min_dist = std::min(min_dist, (float)(terrain->get_map_width() - col));  //Right border
    min_dist = std::min(min_dist, (float)(row + 1));              //Top border
    min_dist = std::min(min_dist, (float)(terrain->get_map_height() - row)); //Bottom border

    return min_dist;
}

bool is_clear_path(int row0, int col0, int row1, int col1)
{
    /*
    Two cells (row0, col0) and (row1, col1) are visible to each other if a line
    between their centerpoints doesn't intersect the four boundary lines of every
    wall cell.  You should puff out the four boundary lines by a very tiny amount
    so that a diagonal line passing by the corner will intersect it.  Make use of the
    line_intersect helper function for the intersection test and the is_wall member
    function in the global terrain to determine if a cell is a wall or not.
    */
    const Vec2 line_start{ col0 + 0.5f, row0 + 0.5f };
    const Vec2 line_end{ col1 + 0.5f, row1 + 0.5f };

    //First check if endpoints are in the same cell
    if (row0 == row1 && col0 == col1) {
        return !terrain->is_wall(row0, col0);
    }

    //Getting the bounding box of the line to minimize wall checks
    const int min_row = std::max(0, std::min(row0, row1) - 1);
    const int max_row = std::min(terrain->get_map_height() - 1, std::max(row0, row1) + 1);
    const int min_col = std::max(0, std::min(col0, col1) - 1);
    const int max_col = std::min(terrain->get_map_width() - 1, std::max(col0, col1) + 1);

    for (int r = min_row; r <= max_row; ++r) {
        for (int c = min_col; c <= max_col; ++c) {
            if (terrain->is_wall(r, c)) {
                //Puffed-out cell boundaries
                const Vec2 wall_left{ c - 0.0001f, r - 0.0001f };
                const Vec2 wall_right{ c + 1.0001f, r + 1.0001f };

                //Check all four edges of the wall cell
                if (line_intersect(line_start, line_end, { wall_left.x, wall_left.y }, { wall_left.x, wall_right.y }) ||  //Left edge
                    line_intersect(line_start, line_end, { wall_right.x, wall_left.y }, { wall_right.x, wall_right.y }) || //Right edge
                    line_intersect(line_start, line_end, { wall_left.x, wall_left.y }, { wall_right.x, wall_left.y }) ||   //Top edge
                    line_intersect(line_start, line_end, { wall_left.x, wall_right.y }, { wall_right.x, wall_right.y }))    //Bottom edge
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void analyze_openness(MapLayer<float>& layer)
{
    /*
    Mark every cell in the given layer with the value 1 / (d * d),
    where d is the distance to the closest wall or edge.  Make use of the
    distance_to_closest_wall helper function.  Walls should not be marked.
    */
    for (int r = 0; r < terrain->get_map_height(); r++) {
        for (int c = 0; c < terrain->get_map_width(); c++) {
            if (!terrain->is_wall(r, c)) {
                float d = distance_to_closest_wall(r, c);
                layer.set_value(r, c, 1.0f / (d * d));
            }
        }
    }
}

void analyze_visibility(MapLayer<float>& layer)
{
    /*
    Mark every cell in the given layer with the number of cells that
    are visible to it, divided by 160 (a magic number that looks good).  Make sure
    to cap the value at 1.0 as well.

    Two cells are visible to each other if a line between their centerpoints doesn't
    intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
    helper function.
    */
    for (int r = 0; r < terrain->get_map_height(); r++) {
        for (int c = 0; c < terrain->get_map_width(); c++) {
            if (!terrain->is_wall(r, c)) {
                int visible_count = 0;

                //Check every other cell once
                for (int other_r = 0; other_r < terrain->get_map_height(); other_r++) {
                    for (int other_c = 0; other_c < terrain->get_map_width(); other_c++) {
                        if (!terrain->is_wall(other_r, other_c) &&
                            is_clear_path(r, c, other_r, other_c)) {
                            visible_count++;
                        }
                    }
                }

                float value = std::min(visible_count / 160.f, 1.0f); //remember to cap to normalised value
                layer.set_value(r, c, value);
            }
        }
    }
}

void analyze_visible_to_cell(MapLayer<float>& layer, int row, int col)
{
    /*
    For every cell in the given layer mark it with 1.0
    if it is visible to the given cell, 0.5 if it isn't visible but is next to a visible cell,
    or 0.0 otherwise.

    Two cells are visible to each other if a line between their centerpoints doesn't
    intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
    helper function.
    */

    const int height = terrain->get_map_height();
    const int width = terrain->get_map_width();

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            layer.set_value(r, c, terrain->is_wall(r, c) ? 0.0f : -1.0f); //Cell clearing
        }
    }

    //Mark directly visible cells
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            if (layer.get_value(r, c) == -1.0f && is_clear_path(row, col, r, c)) {
                layer.set_value(r, c, 1.0f);
            }
        }
    }

    //Honestly only left it here to include the wall somehow being green in the sample solution when it is standing on a wall
    //I guess it technically makes sense since we can see our own floor
    if (terrain->is_valid_grid_position(row, col)) {
        layer.set_value(row, col, 1.0f);
    }

    // Mark adjacent cells (0.5) with strict checks for all 8 directions
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            if (layer.get_value(r, c) == -1.0f) {
                //Check all 8 neighbors
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        if (dr == 0 && dc == 0) continue;

                        int nr = r + dr;
                        int nc = c + dc;

                        if (terrain->is_valid_grid_position(nr, nc) &&
                            layer.get_value(nr, nc) == 1.0f) {
                            //Check if the path between neighbor and current cell is clear
                            if (is_clear_path(nr, nc, r, c)) {
                                layer.set_value(r, c, 0.5f);
                                break;
                            }
                        }
                    }
                    if (layer.get_value(r, c) == 0.5f) break;
                }

                //If no visible neighbors found
                if (layer.get_value(r, c) == -1.0f) {
                    layer.set_value(r, c, 0.0f);
                }
            }
        }
    }
}

void analyze_agent_vision(MapLayer<float>& layer, const Agent* agent)
{
    /*
    For every cell in the given layer that is visible to the given agent,
    mark it as 1.0, otherwise don't change the cell's current value.

    You must consider the direction the agent is facing.  All of the agent data is
    in three dimensions, but to simplify you should operate in two dimensions, the XZ plane.

    Take the dot product between the view vector and the vector from the agent to the cell,
    both normalized, and compare the cosines directly instead of taking the arccosine to
    avoid introducing floating-point inaccuracy (larger cosine means smaller angle).

    Give the agent a field of view slighter larger than 180 degrees.

    Two cells are visible to each other if a line between their centerpoints doesn't
    intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
    helper function.
    */

    Vec3 agentPos = agent->get_position();
    Vec3 agentForward = agent->get_forward_vector();

    GridPos agentGrid = terrain->get_grid_position(agentPos);

    float cosFOV = -0.1f; //slightly larger than 180 degrees

    for (int r = 0; r < terrain->get_map_height(); r++) {
        for (int c = 0; c < terrain->get_map_width(); c++) {
            if (terrain->is_wall(r, c)) continue;

            Vec3 cellWorldPos = terrain->get_world_position(r, c);
            Vec3 toCell = cellWorldPos - agentPos;

            if (toCell.Length() < 0.1f) continue; 

            agentForward.Normalize();
            toCell.Normalize();

            float dot = agentForward.Dot(toCell);

            if (dot > cosFOV && is_clear_path(agentGrid.row, agentGrid.col, r, c)) {
                layer.set_value(r, c, 1.0f);
            }
        }
    }
}
void propagate_solo_occupancy(MapLayer<float>& layer, float decay, float growth)    
{

    /*
    For every cell in the given layer:

        1) Get the value of each neighbor and apply decay factor
        2) Keep the highest value from step 1
        3) Linearly interpolate from the cell's current value to the value from step 2
           with the growing factor as a coefficient.  Make use of the lerp helper function.
        4) Store the value from step 3 in a temporary layer.
           A float[40][40] will suffice, no need to dynamically allocate or make a new MapLayer.

    After every cell has been processed into the temporary layer, write the temporary layer into
    the given layer;
*/

    float tempLayer[40][40];
    const int height = terrain->get_map_height();
    const int width = terrain->get_map_width();

    //1)
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            tempLayer[r][c] = layer.get_value(r, c);
        }
    }

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            if (terrain->is_wall(r, c)) continue;

            float maxDecayed = 0.0f;

            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0) continue;

                    int nr = r + dr;
                    int nc = c + dc;

                    if (terrain->is_valid_grid_position(nr, nc)) {
                        //Block diagonal propagation around corners
                        if (dr != 0 && dc != 0) {
                            if (terrain->is_wall(r + dr, c) || terrain->is_wall(r, c + dc)) continue;
                        }

                        if (terrain->is_wall(nr, nc)) continue;

                        float distance = (dr == 0 || dc == 0) ? 1.0f : sqrtf(2.0f);
                        float decayedValue = layer.get_value(nr, nc) * expf(-distance * decay);
                        //2)
                        maxDecayed = std::max(maxDecayed, decayedValue);
                    }
                }
            }
            //3)
            tempLayer[r][c] = lerp(layer.get_value(r, c), maxDecayed, growth);
        }
    }

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            layer.set_value(r, c, tempLayer[r][c]); //4)
        }
    }
}

void normalize_solo_occupancy(MapLayer<float>& layer)
{
    /*
    Determine the maximum value in the given layer, and then divide the value
    for every cell in the layer by that amount.  This will keep the values in the
    range of [0, 1].  Negative values should be left unmodified.
    */
    const int height = terrain->get_map_height();
    const int width = terrain->get_map_width();

    float maxPositive = 0.0f;

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            float val = layer.get_value(r, c);
            if (val > 0.0f) maxPositive = std::max(maxPositive, val);
        }
    }

    if (maxPositive == 0.0f) return;

    //For normalisation
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            float val = layer.get_value(r, c);
            if (val > 0.0f) {
                layer.set_value(r, c, val / maxPositive);
            }
        }
    }
}

void enemy_field_of_view(MapLayer<float>& layer, float fovAngle, float closeDistance, float occupancyValue, AStarAgent* enemy)
{
    /*
    First, clear out the old values in the map layer by setting any negative value to 0.
    Then, for every cell in the layer that is within the field of view cone, from the
    enemy agent, mark it with the occupancy value.  Take the dot product between the view
    vector and the vector from the agent to the cell, both normalized, and compare the
    cosines directly instead of taking the arccosine to avoid introducing floating-point
    inaccuracy (larger cosine means smaller angle).

    If the tile is close enough to the enemy (less than closeDistance),
    you only check if it's visible to enemy.  Make use of the is_clear_path
    helper function.  Otherwise, you must consider the direction the enemy is facing too.
    This creates a radius around the enemy that the player can be detected within, as well
    as a fov cone.
    */

    //Clear layer
    for (int r = 0; r < terrain->get_map_height(); r++) {
        for (int c = 0; c < terrain->get_map_width(); c++) {
            if (layer.get_value(r, c) < 0.0f) { 
                layer.set_value(r, c, 0.0f);
            }
        }
    }
        
    Vec3 enemyPos = enemy->get_position();
    Vec3 enemyForward = enemy->get_forward_vector();
    enemyForward.Normalize();
    GridPos enemyGrid = terrain->get_grid_position(enemyPos);

    float cosFOV = cosf(fovAngle * 3.14159265f / 360.0f);

    for (int r = 0; r < terrain->get_map_height(); r++) {
        for (int c = 0; c < terrain->get_map_width(); c++) {
            if (terrain->is_wall(r, c)) continue;

            Vec3 cellPos = terrain->get_world_position(r, c);
            Vec3 toCell = cellPos - enemyPos;
            float distance = toCell.Length();
            toCell.Normalize();

            //Close distance check
            if (distance < closeDistance) {
                if (is_clear_path(enemyGrid.row, enemyGrid.col, r, c)) {
                    layer.set_value(r, c, occupancyValue); 
                }
            }
            //FOV cone check
            else {
                float dot = enemyForward.Dot(toCell);
                if (dot > cosFOV && is_clear_path(enemyGrid.row, enemyGrid.col, r, c)) {
                    layer.set_value(r, c, occupancyValue);
                }
            }
        }
    }
}

bool enemy_find_player(MapLayer<float>& layer, AStarAgent* enemy, Agent* player)
{
    /*
        Check if the player's current tile has a negative value, ie in the fov cone
        or within a detection radius.
    */

    (void)enemy; //For warning removal

    const auto& playerWorldPos = player->get_position();

    const auto playerGridPos = terrain->get_grid_position(playerWorldPos);

    //Verify a valid position was returned
    if (terrain->is_valid_grid_position(playerGridPos) == true)
    {
        if (layer.get_value(playerGridPos) < 0.0f)
        {
            return true;
        }
    }

    // player isn't in the detection radius or fov cone, OR somehow off the map
    return false;
}

bool enemy_seek_player(MapLayer<float>& layer, AStarAgent* enemy)
{
    /*
    Attempt to find a cell with the highest nonzero value (normalization may
    not produce exactly 1.0 due to floating point error), and then set it as
    the new target, using enemy->path_to.

    If there are multiple cells with the same highest value, then pick the
    cell closest to the enemy.

    Return whether a target cell was found.
    */

    float maxVal = 0.0f;
    GridPos bestPos{ -1, -1 };
    float closestDist = FLT_MAX;
    Vec3 enemyPos = enemy->get_position();

    for (int r = 0; r < terrain->get_map_height(); r++) {
        for (int c = 0; c < terrain->get_map_width(); c++) {
            float val = layer.get_value(r, c);
            if (val > 0.0f) {
                Vec3 cellPos = terrain->get_world_position(r, c);
                float dist = (cellPos - enemyPos).Length();

                if (val > maxVal || (fabs(val - maxVal) < 0.0001f && dist < closestDist)) {
                    maxVal = val;
                    bestPos = { r, c };
                    closestDist = dist;
                }
            }
        }
    }

    if (bestPos.row != -1 && bestPos.col != -1) {
        Vec3 targetPos = terrain->get_world_position(bestPos.row, bestPos.col);
        enemy->path_to(targetPos);
        return true;
    }

    return false;
}