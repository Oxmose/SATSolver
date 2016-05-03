#include "main.h"

int main(int argc, char const *argv[])
{
    settings_s.debug_s = argc > 2;
    settings_s.wl_s = true;
    settings_s.cl_s = false;

    SATSolver solver;

    read_input(argv[1], solver);

    if(solver.solve())
    {
        printf("s UNSATISFIABLE\n");
        return 0;
    }

    
    for(auto v : solver.valuation)
    {
        printf("s SATISFIABLE\n");
        return 0;
        int index = v.first;
        bool value = (v.second == -1) ? true : v.second;
        string modifier = "";
        if(!value)
            modifier = "-";
        printf("%s%d ", modifier.c_str(), index);
    }
    printf("\n");

    return 0;
}

void read_input(string file, SATSolver& solver)
{
    ifstream infile(file);
    
    OUTDEBUG(fprintf(stderr,"Reading %s\n", file.c_str()));

    string line;
    while (std::getline(infile, line))
    {
        bool should_continue = false;
        for(auto c : line)
            if(c == 'c' || c == 'p')
            {
                should_continue = true;
                break;
            }
        if(should_continue)
            continue;

        istringstream iss(line);
        
        clause the_clause(solver.formula.size());
        map<int,int> vu;
        bool cleaned = false;
        bool taut = false;

        int l;
        while(iss >> l)
        {
            if(l != 0)
            {
                if(vu.find(abs(l)) != vu.end())
                {
                    if(vu[abs(l)] == -l)
                        taut = true;
                    else if(vu[abs(l)] == l)
                        cleaned = true;
                    else
                        assert(false);
                }
                if(vu.find(abs(l)) == vu.end())
                    vu[abs(l)] = l;
                the_clause.literal.push_back(l);
            }
        }

        if(taut)
        {
            OUTDEBUG(fprintf(stderr,"%s is tautological, not added.\n", the_clause.to_str().c_str()));
            continue;
        }
        if(cleaned)
        {
            string ancient = the_clause.to_str();
            the_clause.literal.clear();
            for(auto l : vu)
                the_clause.literal.push_back(l.second);
            OUTDEBUG(fprintf(stderr,"%s cleaned to %s.\n", ancient.c_str(), the_clause.to_str().c_str()));
        }

        for(auto l : the_clause.literal)
            solver.valuation[abs(l)] = -1;
        solver.add_clause(the_clause);
    }
    OUTDEBUG(fprintf(stderr,"Input read successfuly.\n"));
    OUTDEBUG(fprintf(stderr, "\n"));
}
