#include "State.h"

void State::InitMaze()
{
	// Allocating on the heap in case the stack is too small.
	maze = new int[MSZ * MSZ];

	cell_maze_source = new Cell**[MSZ];
	cell_maze_target = new Cell**[MSZ];

	// Allocate and nullify all cells
	int i, j;
	for (i = 0; i < MSZ; i++)
	{
		cell_maze_source[i] = new Cell*[MSZ];
		fill_n(cell_maze_source[i], MSZ, nullptr);
		cell_maze_target[i] = new Cell*[MSZ];
		fill_n(cell_maze_target[i], MSZ, nullptr);
	}

	// the frame of maze is made of walls
	for (i = 0; i < MSZ; i++)
	{
		set_color(0, i, WALL);
        set_color(MSZ - 1, i, WALL);
        set_color(i, 0, WALL);
        set_color(i, MSZ - 1, WALL);
	}
	for (i = 1; i < MSZ - 1; i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 0) // mostly SPACES
			{
				if (rand() % PERCENT < SPACE_PERCENT)
                    set_color(i, j, SPACE);
				else
                    set_color(i, j, WALL);
			}
			else //  mostly walls
			{
				if (rand() % PERCENT < WALL_PERCENT)
					set_color(i, j, WALL);
				else
					set_color(i, j, SPACE);
			}
		}
	// set SOURCE and TARGET
	set_color(MSZ / 2, MSZ / 2, SOURCE);
	// add start Cell to grays
	Cell *pc = new Cell(MSZ / 2, MSZ / 2, nullptr);
	search_vec_source.push_back(pc);
	int target_row = rand() % (MSZ - 3) + 2;
	int target_col = rand() % (MSZ - 3) + 2;
	set_color(target_row, target_col, TARGET);
	Cell *target_pc = new Cell(target_row, target_col, nullptr);
	search_vec_target.push_back(target_pc);
}