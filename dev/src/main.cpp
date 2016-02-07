#include "SATSolver.h" // SATSolver class

using namespace std;

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " <file_name>" << endl;
        return 1;
    }

    // Create the solver
    SATSolver solver(argv[1]);

    // Dummy test
    cout << "Parse result : " << solver.parse() << endl;    
    cout << "Is the formula solvable " << solver.isSolvable() << endl;
    
    return 0;
}// main(int, char**)
