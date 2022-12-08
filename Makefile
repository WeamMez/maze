CXX = mpicxx

CXXFLAGS = -g
FILES = "Cell.cpp" "State.cpp" "main.cpp"
LINKS = -lGL -lGLU -lglut

EXEC = mpiexec
THREADS = 2

all: 
	$(CXX) $(CXXFLAGS)  -o "./bfs" $(FILES) $(LINKS)

run:
	$(EXEC) -n $(THREADS) "./bfs"