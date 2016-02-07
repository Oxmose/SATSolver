#include "SATSolver.h" // SATSolver class

using namespace std;

int main(int argc, char** argv)
{
	SATSolver solver("test.cnf");
	cout << "Parse result : " << solver.parse() << endl;
	
	cout << "Is the formula solvable " << solver.isSolvable() << endl;
	
	return 0;
}// main(int, char**)
