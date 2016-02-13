#include "Core/SATSolver.h" // SATSolver class

using namespace std;

int main(int argc, char** argv)
{
    PARSE_TYPE parserType = CNF_PARSE;
    string fileName;
    if(argc == 3)
    {
        if(string(argv[1]) != "-tseitin")
        {
            cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] <file_name>" << endl;
            return 1;
        }
        else
        {
            parserType = LOG_PARSE;
            fileName = argv[2];
        }
    }
    else if(argc != 2)
    {
        cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] <file_name>" << endl;
        return 1;
    }
    else
    {
        fileName = argv[1];
    }    

    // Create the solver
    SATSolver solver(fileName);

    // Dummy test
    if(!solver.parse(parserType))
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
