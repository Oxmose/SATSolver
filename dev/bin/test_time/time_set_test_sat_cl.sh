#!/bin/bash

#Set for sat 
echo "{"
echo "    \"SAT\": ["

for f in test_base/cnf/serial_test/sat/*/*.cnf
do
    echo "        {"
    echo "            \"file\": \"$f\"," | sed s/\\.\\.\\/test_base\\/cnf\\/serial_test\\/sat\\///

    #MINISAT
    (/usr/bin/time -f "%e r" minisat $f) |&  grep r$ | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"minisat\":\ \"/ | sed s/$/\",/
    
    #CL
    (/usr/bin/time -f "%e r" ./resol -cl $f) |&  grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"cl\":\ \"/ | sed s/$/\",/

    #RAND
    (/usr/bin/time -f "%e r" ./resol -cl -rand $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clrand\":\ \"/ | sed s/$/\",/
    #RAND0
    (/usr/bin/time -f "%e r" ./resol -cl -rand0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clrand0\":\ \"/ | sed s/$/\",/
    #MOMS
    (/usr/bin/time -f "%e r" ./resol -cl -moms $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforgetmoms\":\ \"/ | sed s/$/\",/
    #DLIS
    (/usr/bin/time -f "%e r" ./resol -cl -dlis $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforgetdlis\":\ \"/ | sed s/$/\",/
    #DLIS0
    (/usr/bin/time -f "%e r" ./resol -cl -dlis0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforgetdlis0\":\ \"/ | sed s/$/\",/
    #VSIDS
    (/usr/bin/time -f "%e r" ./resol -cl -vsids $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforgetvsids\":\ \"/ | sed s/$/\",/
    
    #CLFORGET
    (/usr/bin/time -f "%e r" ./resol -cl -forget $f) |&  grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforget\":\ \"/ | sed s/$/\",/

    #RAND
    (/usr/bin/time -f "%e r" ./resol -cl -forget -rand $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforgetrand\":\ \"/ | sed s/$/\",/
    #RAND0
    (/usr/bin/time -f "%e r" ./resol -cl -rand0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforgetrand0\":\ \"/ | sed s/$/\",/
    #MOMS
    (/usr/bin/time -f "%e r" ./resol -cl -forget -moms $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforgetmoms\":\ \"/ | sed s/$/\",/
    #DLIS
    (/usr/bin/time -f "%e r" ./resol -cl -forget -dlis $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforgetdlis\":\ \"/ | sed s/$/\",/
    #DLIS0
    (/usr/bin/time -f "%e r" ./resol -cl -forget -dlis0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforgetdlis0\":\ \"/ | sed s/$/\",/
    #VSIDS
    (/usr/bin/time -f "%e r" ./resol -cl -forget -vsids $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clforgetvsids\":\ \"/ | sed s/$/\",/
    echo "        },"
done
echo "        { \"file\": \"end\" }"
echo "    ]"
echo "}"
