#include "main.h"

using namespace std;

int main(int argc, char const *argv[])
{
    settings_s.debug_s = argc > 2;
    settings_s.wl_s = false;
    settings_s.cl_s = true;

    SATSolver solver;

    int commandError = parseCommand(argc, argv);
    
    if(commandError == 1)
    {
        cerr << "Error, wrong arguments." << endl << "Usage : " << argv[0] << " [-tseitin] [-cl | -cl-interac] [-naiveuip] [-wl] [-forget] [-vsids | -rand | -rand0 | -moms | -dlis | -dlis0] [-debug] <file_name>" << endl;
        return 1;
    }
    else if(commandError == -1)
        return 0;

    read_input(settings_s.filename_s, solver);

    if(solver.solve())
    {
        printf("s UNSATISFIABLE\n");
        return 0;
    }


    printf("s SATISFIABLE\n");    
    for(auto v : solver.valuation)
    {
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

/*
    Only requirement for parser:
        When tautologie is found, should ignore it BUT
        has to set its var's valuation to -1 to notify
        solver their existence.
*/
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

        if(cleaned)
        {
            string ancient = the_clause.to_str();
            the_clause.literal.clear();
            for(auto l : vu)
                the_clause.literal.push_back(l.second);
            OUTDEBUG(fprintf(stderr,"%s cleaned to %s.\n", ancient.c_str(), the_clause.to_str().c_str()));
        }

        if(taut)
        {
            for(auto l : the_clause.literal)
                solver.valuation[abs(l)] = -1;
            OUTDEBUG(fprintf(stderr,"%s is tautological, not added.\n", the_clause.to_str().c_str()));
            continue;
        }

        solver.add_clause(the_clause, true);
    }
    OUTDEBUG(fprintf(stderr,"Input read successfuly.\n"));
    OUTDEBUG(fprintf(stderr, "\n"));
}

int parseCommand(int argc, const char **argv)
{
    // Init settings values
    //settings_s.parser_s = CNF_PARSE;
    //settings_s.bet_s = NORM;
    settings_s.debug_s = false;
    settings_s.wl_s = false;
    settings_s.cl_s = false;
    settings_s.clinterac_s = false;
    settings_s.forget_s = false;
    //settings_s.naiveuip_s = false;

    int commandError = 0;
    bool argsValid[7] = {false, false, false, false, false, false, false};
    
    if(argc == 1)
    {
        // Display user interface and enable user to choose the action
        displayMenu(argv[0]);
        menuChoice();
        return -1;
    }
    else
    {
        for(int i = 1; i < argc; ++i)
        {
            string value = string(argv[i]);
            if(i == argc - 1)
            {
                settings_s.filename_s = value;
            }
            else if(value == "-tseitin" && !argsValid[0])
            {
                assert(false);
                //settings_s.parser_s = LOG_PARSE;
                argsValid[0] = true;
            }
            else if(value == "-wl" && !argsValid[1])
            {
                settings_s.wl_s = true;
                argsValid[1] = true;
            }
            /*
            else if(value == "-rand" && !argsValid[2])
            {
                settings_s.bet_s = RAND0;
                argsValid[2] = true;
            }
            else if(value == "-rand0" && !argsValid[2])
            {
                settings_s.bet_s = RAND1;
                argsValid[2] = true;
            }
            else if(value == "-moms" && !argsValid[2])
            {
                settings_s.bet_s = MOMS;
                argsValid[2] = true;
            }
            else if(value == "-dlis" && !argsValid[2])
            {
                settings_s.bet_s = DLIS;
                argsValid[2] = true;
            }
            else if(value == "-dlis0" && !argsValid[2])
            {
                settings_s.bet_s = DLIS1;
                argsValid[2] = true;
            }
            else if(value == "-vsids" && !argsValid[2])
            {
                settings_s.bet_s = VSIDS;
                argsValid[2] = true;
            }*/        
            else if(value == "-debug" && !argsValid[3])
            {
                settings_s.debug_s = true;
                argsValid[3] = true;
            }  
            else if(value == "-cl" && !argsValid[4])
            {
                settings_s.cl_s = true;
                argsValid[4] = true;
            }
            else if(value == "-cl-interac" && !argsValid[4])
            {
                settings_s.cl_s = true;
                settings_s.clinterac_s = true;
                argsValid[4] = true;
            }
            else if(value == "-forget" && !argsValid[5])
            {
                settings_s.forget_s = true;
                argsValid[5] = true;
            }
            /*else if(value == "-naiveuip" && !argsValid[6])
            {
                settings_s.naiveuip_s = true;
                argsValid[6] = true;
            }*/
            else
            {
                commandError = 1;
                break;
            }
        }
        int count = 0;
        for(unsigned int i = 0; i < 7; ++i)
        {
            if(argsValid[i])
                ++count;
        }/*
        if((settings_s.naiveuip_s || settings_s.bet_s == VSIDS || settings_s.forget_s) && !settings_s.cl_s)
        {
            OUTWARNING("Imcompatible heuristic, you must use -cl to enable this heuristic, set heuristic to standard.");
            settings_s.bet_s = NORM;
        }*/
        if(count != argc - 2)
        {
            commandError = 1;
        }
        return commandError;
    }

    return commandError;
}// int parseComment(int, char**, Settings_s&)

void displayMenu(const char *softName)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    unsigned int total = 0;

    for(int i = 0; i < w.ws_col / 2 - 8; ++i)
    {
        cout << "=";
        ++total;
    }
    
    cout << " SATSolver V" << VERSION << " ";
    total += 16;
    for(unsigned int i = total; i < w.ws_col; ++i)
        cout << "=";
    cout << endl;
    total = 0;
    cout << "#";
    for(int i = 0; i < w.ws_col / 2 - 19; ++i)
    {
        cout << " ";
        ++total;
    }
    cout << " By Tristan Sterin and Alexy Torres ";
    total += 36;
    for(int i = total; i < w.ws_col - 2; ++i)
        cout << " ";
    cout << "#" << endl;
    for(int i = 0; i < w.ws_col; ++i)
        cout << "=";
    cout << endl;

    cout << "How to use :" << endl;
    cout << "\t If you have a DIMACS CNF file to solve, please run this software as : " << endl;
    cout << "\t\t" << softName << " [-wl] [-cl | -cl-interac] [-vsids | -forget | -rand | -rand0 | -moms | -dlis | -dlis0] [-debug] <file_name>" << endl;
    cout << "\t If you have a logic formula file to solve, please run this software as : " << endl;
    cout << "\t\t" << softName << " [-tseitin] [-wl] [-cl | -cl-interac] [-naiveuip] [-vsids | -forget | -rand | -rand0 | -moms | -dlis | -dlis0] [-debug] <file_name>" << endl;
    cout << "\t The argument -wl enable the watched literals method." << endl;
    cout << "\t The argument -cl enable clauses learning. -cl-interac enable interactive clauses learning." << endl;
    cout << "\t The argument -naiveuip enable naive uip detection algorithm." << endl;
    cout << "\t [-rand | -moms | -dlis ...] define the used heuristic to bet on the next literal :" << endl;
    cout << "\t\t -vsids : next bet on the most active literal" << endl;
    cout << "\t\t -forget : get track on clause activity." << endl;
    cout << "\t\t -rand : next bet on a random literal." << endl;
    cout << "\t\t -rand0 : next bet on a random literal (the bet is random here)." << endl;
    cout << "\t\t -moms : next bet on the variable which has the maximum occurences count in clauses of minimum size." << endl;
    cout << "\t\t -dlis : next bet on next variable which can satify the maximum count of clauses." << endl;
    cout << "\t\t -dlis0 : next bet on next variable which can satify the maximum count of clauses with score sum(pow(2, -C)) with C the size of the clauses the variable appears in." << endl;

    cout << endl;
    cout << "\t To generate DIMAC CNF file, press 1 and then [ENTER]" << endl;
    cout << "\t To read the credits, press 2 and then [ENTER]" << endl;
    cout << "\t To quit, press 3 and then [ENTER]" << endl;

} // displayMenu()

void menuChoice()
{
    // Get the user choice
    unsigned short int choice;
    bool ok = false;
    do
    {
        cout << ">";
        cin >> choice;
        if(choice == 1)
        {
            assert(false);
            string fileName;
            cout << "Please enter a file name to save the formula: ";
            cin >> fileName;
            //randomGenerator(fileName);
            ok = true;
        }
        else if(choice == 2)
        {
            cout << endl << endl << "SATSolver V " << VERSION << endl;
            cout << "Created by Tristan Sterin and Alexy Torres." << endl;
            cout << "Project realized at the ENS (Ecole Normale Superieure) of Lyon." << endl;
            cout << "Year 2015/2016" << endl;
            ok = true;
        }
        else if(choice == 3)
        {
            ok = true;
        }
        else
            cout << "Wrong input, please try again." << endl;

    } while(!ok);

    cout << "Thanks, bye friendly user!" << endl;
} // menuChoice()
