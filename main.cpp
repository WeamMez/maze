
#include <stdlib.h>
#include <GL/glut.h>
#include <vector>
#include "Cell.h"
#include "State.h"
#include <iostream>
#include <mpi.h>

#define DISPLAY 0
#define CALC 1

#define NO_TAG 0
#define REQUEST_MAZE 0
#define BFS 1
#define FINISH 1

using namespace std;

const int W = 1000; // window Width
const int H = 1000; // window Height

void init()
{
	glClearColor(0, 0.3, 0.0, 0);	// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // set the coordinates system
}

void DrawMaze(int *maze)
{

	int i, j;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i * MSZ + j])
			{
			case SPACE:
				glColor3d(0.9, 0.9, 0.9); // light gray
				break;
			case WALL:
				glColor3d(0.3, 0.3, 0.3); // dark gray
				break;
			case SOURCE:
				glColor3d(1, 1, 0); // blue
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case SURFACE_SEARCH_SOURCE:
				glColor3d(0.7, 1, 0.7); // light green
				break;
			case EDGES_SEARCH_SOURCE:
				glColor3d(1, 0.3, 0); // orange
				break;
			case PATH:
				glColor3d(0.8, 0.5, 1); // purple
				break;
			case SURFACE_SEARCH_TARGET:
				glColor3d(0.667, 0.2, 0.416); // dark pink
				break;
			case EDGES_SEARCH_TARGET:
				glColor3d(0, 1, 1); // light pink
				break;
			}

			// draw square
			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j + 1, i);
			glVertex2d(j + 1, i + 1);
			glVertex2d(j, i + 1);
			glEnd();
		}
}

void RestorePath(State *s, int row, int col)
{
	Cell *source_pc = s->get_cell(row, col, true);
	Cell *target_pc = s->get_cell(row, col, false);

	while (source_pc->getParent() != nullptr)
	{
		s->set_color(source_pc->getRow(), source_pc->getColumn(), PATH);
		source_pc = source_pc->getParent();
	}

	while (target_pc->getParent() != nullptr)
	{
		s->set_color(target_pc->getRow(), target_pc->getColumn(), PATH);
		target_pc = target_pc->getParent();
	}
}

bool CheckNeighbor(int row, int column, Cell *pcurrent, State *s, bool is_source)
{
	Cell *pn = new Cell(row, column, pcurrent);
	if (s->get_cell(row, column, !is_source) != nullptr)
	{
		cout << "The solution has been found\n";
		s->set_cell(row, column, is_source, pn);
		RestorePath(s, pn->getRow(), pn->getColumn());
		return true; // Stop BFS
	}
	else // maze[row][column] is WHITE
	{
		// paint this neighbor gray
		s->add_to_search_vec(is_source, pn);
		s->set_color(row, column, is_source ? EDGES_SEARCH_SOURCE : EDGES_SEARCH_TARGET);
		s->set_cell(row, column, is_source, pn);
		return false;
	}
}

bool RunBFSIteration(State *s, bool is_source)
{
	Cell *pcurrent;
	int r, c;
	if (s->is_empty(is_source)) // no solution exists
	{
		cout << "No solution\n";
		return true; // stop running BFS
	}
	// grays is not empty

	pcurrent = s->pop_search_vec(is_source);
	r = pcurrent->getRow();
	c = pcurrent->getColumn();

	int my_surface = is_source ? SURFACE_SEARCH_SOURCE : SURFACE_SEARCH_TARGET;
	int other_surface = is_source ? SURFACE_SEARCH_TARGET : SURFACE_SEARCH_SOURCE;
	int other_edges = is_source ? EDGES_SEARCH_TARGET : EDGES_SEARCH_SOURCE;

	s->set_color(r, c, my_surface); // paint it green
	s->set_cell(r, c, is_source, pcurrent);
	// add all white neighbors of pcurrent to grays
	// UP
	int up = s->get_color(r + 1, c);
	if ((up == SPACE || up == TARGET || up == other_surface || up == other_edges))
		if (CheckNeighbor(r + 1, c, pcurrent, s, is_source))
			return true;
	// DOWN
	int down = s->get_color(r - 1, c);
	if ((down == SPACE || down == TARGET || down == other_surface || down == other_edges))
		if (CheckNeighbor(r - 1, c, pcurrent, s, is_source))
			return true;
	// LEFT
	int left = s->get_color(r, c - 1);
	if ((left == SPACE || left == TARGET || left == other_surface || left == other_edges))
		if (CheckNeighbor(r, c - 1, pcurrent, s, is_source))
			return true;
	// RIGHT
	int right = s->get_color(r, c + 1);
	if ((right == SPACE || right == TARGET || right == other_surface || right == other_edges))
		if (CheckNeighbor(r, c + 1, pcurrent, s, is_source))
			return true;

	return false;
}

void display()
{
	static int *maze = new int[MSZ * MSZ];
	static bool finished = false;

	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	if (!finished)
	{
		int message = REQUEST_MAZE;
		MPI::COMM_WORLD.Send(&message, 1, MPI::INT, CALC, NO_TAG);
		MPI::Status stat;
		MPI::COMM_WORLD.Recv(maze, MSZ * MSZ, MPI::INT, CALC, MPI::ANY_TAG, stat);
		if (stat.Get_tag() == FINISH)
			finished = true;
	}

	DrawMaze(maze);

	glutSwapBuffers(); // show all
}

void idle()
{

	glutPostRedisplay(); // indirect call to display
}

void menu(int choice)
{
	switch (choice)
	{
	case 1: // BFS
		int command = BFS;
		MPI::COMM_WORLD.Send(&command, 1, MPI::INT, CALC, NO_TAG);
		break;
	}
}

int main(int argc, char *argv[])
{
	MPI::Init(argc, argv);
	srand(time(0));

	int pn = MPI::COMM_WORLD.Get_size();
	int pid = MPI::COMM_WORLD.Get_rank();

	if (pn < 2)
	{
		MPI::COMM_WORLD.Abort(-1);
	}

	if (pid == DISPLAY)
	{

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
		glutInitWindowSize(W, H);
		glutInitWindowPosition(400, 50);
		glutCreateWindow("BFS");

		glutDisplayFunc(display); // display is the refresh function
		glutIdleFunc(idle);

		// menu
		glutCreateMenu(menu);
		glutAddMenuEntry("Run BFS", 1);
		glutAttachMenu(GLUT_RIGHT_BUTTON);

		init();

		glutMainLoop();
	}

	else if (pid == CALC)
	{
		// Initiate state, currently contains a clean maze with a source and a destination.
		State s;
		int message;
		do // While not running BFS, keep sending the empty maze.
		{
			MPI::COMM_WORLD.Recv(&message, 1, MPI::INT, DISPLAY, NO_TAG);
			if (message == REQUEST_MAZE)
			{
				int *maze = s.get_maze();
				MPI::COMM_WORLD.Send(maze, MSZ * MSZ, MPI::INT, DISPLAY, NO_TAG);
			}
		} while (message == REQUEST_MAZE);

		if (message == BFS) // If BFS signal is sent, start running BFS iterations.
		{
			// Run 2 iterations, 1 from source and 1 from target, until one of them finishes.
			while (!RunBFSIteration(&s, true) && !RunBFSIteration(&s, false))
			{
				// check if a message was sent, if true check if maze is requestec and send maze.
				if (MPI::COMM_WORLD.Iprobe(DISPLAY, NO_TAG))
				{
					int message;
					MPI::COMM_WORLD.Recv(&message, 1, MPI::INT, DISPLAY, NO_TAG);
					if (message == REQUEST_MAZE)
					{
						int *maze = s.get_maze();
						MPI::COMM_WORLD.Send(maze, MSZ * MSZ, MPI::INT, DISPLAY, NO_TAG);
					}
				}
			}

			int message;
			MPI::COMM_WORLD.Recv(&message, 1, MPI::INT, DISPLAY, NO_TAG);
			if (message == REQUEST_MAZE)
				MPI::COMM_WORLD.Send(s.get_maze(), MSZ * MSZ, MPI::INT, DISPLAY, FINISH);
		}
	}

	MPI::Finalize();

	return 0;
}