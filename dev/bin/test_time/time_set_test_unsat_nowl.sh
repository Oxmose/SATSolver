#!/bin/bash

#Set for unsat 
echo "{"
echo "    \"UNSAT\": ["

for f in test_base/time_test/unsat/*.cnf
do
    echo "        {"
    echo "            \"file\": \"$f\"," | sed s/\\.\\.\\/test_base\\/time_test\\/unsat\\///

    #MINISAT
    (/usr/bin/time -f "%e r" minisat $f) |&  grep r$ | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"minisat\":\ \"/ | sed s/$/\",/

    #STD
    (/usr/bin/time -f "%e r" ./resol $f) |&  grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"std\":\ \"/ | sed s/$/\",/

    
    #RAND0
    #(/usr/bin/time -f "%e r" ./resol -rand0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"rand0\":\ \"/ | sed s/$/\",/
    #MOMS
    #(/usr/bin/time -f "%e r" ./resol -moms $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"moms\":\ \"/ | sed s/$/\",/
    #DLIS
    #(/usr/bin/time -f "%e r" ./resol -dlis $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"dlis\":\ \"/ | sed s/$/\",/
    echo "        },"
done
echo "        { \"file\": \"end\" }"
echo "    ]"
echo "}"
