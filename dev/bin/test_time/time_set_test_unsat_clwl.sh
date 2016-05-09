#!/bin/bash

#Set for UNSAT 
echo "{"
echo "    \"UNSAT\": ["

for f in test_base/time_test/unsat/*.cnf
do
    echo "        {"
    echo "            \"file\": \"$f\"," | sed s/\\.\\.\\/test_base\\/time_test\\/unsat\\///

    #MINISAT
    (/usr/bin/time -f "%e r" minisat $f) |&  grep r$ | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"minisat\":\ \"/ | sed s/$/\",/
    
    #CLWL
    (/usr/bin/time -f "%e r" ./resol -cl -wl $f) |&  grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwl\":\ \"/ | sed s/$/\",/

    #RAND0
    #(/usr/bin/time -f "%e r" ./resol -cl -wl -rand0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlrand0\":\ \"/ | sed s/$/\",/
    #MOMS
    #(/usr/bin/time -f "%e r" ./resol -cl -wl -moms $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlmoms\":\ \"/ | sed s/$/\",/
    #DLIS
    #(/usr/bin/time -f "%e r" ./resol -cl -wl -dlis $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwldlis\":\ \"/ | sed s/$/\",/
    #VSIDS
    #(/usr/bin/time -f "%e r" ./resol -cl -wl -vsids $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlvsids\":\ \"/ | sed s/$/\",/

    
    echo "        },"
done
echo "        { \"file\": \"end\" }"
echo "    ]"
echo "}"
