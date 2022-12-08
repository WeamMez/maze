#ifndef __STATE_H
#define __STATE_H

#include "Cell.h"
#include <vector>
#include <cstdlib>
#include <algorithm>

#define PERCENT 100
#define WALL_PERCENT 60
#define SPACE_PERCENT 80

const int MSZ = 100;

const int SPACE = 0;
const int WALL = 1;
const int SOURCE = 2;
const int TARGET = 3;
const int SURFACE_SEARCH_SOURCE = 4;
const int EDGES_SEARCH_SOURCE = 5;
const int PATH = 6;
const int SURFACE_SEARCH_TARGET = 7;
const int EDGES_SEARCH_TARGET = 8;

using namespace std;

class State
{
private:
    int *maze;
    Cell ***cell_maze_source;
    Cell ***cell_maze_target;
    vector<Cell *> search_vec_source;
    vector<Cell *> search_vec_target;

    void InitMaze();

public:
    int get_color(int row, int col) { return maze[row * MSZ + col]; }
    void set_color(int row, int col, int color) { maze[row * MSZ + col] = color; }
    Cell *get_cell(int row, int col, bool is_source) { return (is_source ? cell_maze_source : cell_maze_target)[row][col]; }
    void set_cell(int row, int col, bool is_source, Cell *cell) { (is_source ? cell_maze_source : cell_maze_target)[row][col] = cell; }
    Cell *pop_search_vec(bool is_source)
    {
        vector<Cell *> *this_vec = &(is_source ? search_vec_source : search_vec_target);
        vector<Cell *>::iterator it = this_vec->begin();
        Cell *ans = *it;
        this_vec->erase(it);
        return ans;
    }
    void add_to_search_vec(bool is_source, Cell *c) { (is_source ? search_vec_source : search_vec_target).push_back(c); }
    bool is_empty(bool is_source) { return (is_source ? search_vec_source : search_vec_target).empty(); }

    int *get_maze() {return maze;}

    State()
    {
        InitMaze();
    }
};

#endif //__STATE_H