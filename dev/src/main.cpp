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
    }

    OUTDEBUG("We check SAT of :" << endl << solver.formulaToStr());

    if(solver.solve())
        cout << "s SATIFIABLE" << endl;
    else
        cout << "s UNSATISFIABLE" << endl;

    //Not handling ??? for the moment (time limit ? heuristics ?).

    return 0;
}// main(int, char**)
