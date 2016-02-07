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
    if(!solver.parse())
    {
        cerr << "Error while parsing the file." << endl;
        return 2;
    }

    int isSolvable = solver.isSolvable();

    if(isSolvable == 1)
        cout << "s SATIFIABLE" << endl;
    else if(isSolvable == -1)
	cout << "s ???" << endl;
    else
        cout << "s UNSATISFIABLE" << endl;
    
    return 0;
}// main(int, char**)
