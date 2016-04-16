#!/bin/bash

#Set for UNSAT 
echo "{"
echo "    \"UNSAT\": ["

for f in test_base/cnf/serial_test/unsat/*/*.cnf
do
    echo "        {"
    echo "            \"file\": \"$f\"," | sed s/\\.\\.\\/test_base\\/cnf\\/serial_test\\/unsat\\///

    #MINISAT
    (/usr/bin/time -f "%e r" minisat $f) |&  grep r$ | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"minisat\":\ \"/ | sed s/$/\",/
    
    #CLWL
    (/usr/bin/time -f "%e r" ./resol -cl -wl $f) |&  grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwl\":\ \"/ | sed s/$/\",/

    #RAND
    (/usr/bin/time -f "%e r" ./resol -cl -wl -rand $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlrand\":\ \"/ | sed s/$/\",/
    #RAND0
    (/usr/bin/time -f "%e r" ./resol -cl -wl -rand0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlrand0\":\ \"/ | sed s/$/\",/
    #MOMS
    (/usr/bin/time -f "%e r" ./resol -cl -wl -moms $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlmoms\":\ \"/ | sed s/$/\",/
    #DLIS
    (/usr/bin/time -f "%e r" ./resol -cl -wl -dlis $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwldlis\":\ \"/ | sed s/$/\",/
    #DLIS0
    (/usr/bin/time -f "%e r" ./resol -cl -wl -dlis0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwldlis0\":\ \"/ | sed s/$/\",/
    #VSIDS
    (/usr/bin/time -f "%e r" ./resol -cl -wl -vsids $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlvsids\":\ \"/ | sed s/$/\",/

    #CLWL FORGET
    (/usr/bin/time -f "%e r" ./resol -cl -wl -forget $f) |&  grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlforget\":\ \"/ | sed s/$/\",/

    #RAND
    (/usr/bin/time -f "%e r" ./resol -cl -wl -forget -rand $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlforgetrand\":\ \"/ | sed s/$/\",/
    #RAND0
    (/usr/bin/time -f "%e r" ./resol -cl -wl -forget -rand0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlforgetrand0\":\ \"/ | sed s/$/\",/
    #MOMS
    (/usr/bin/time -f "%e r" ./resol -cl -wl -forget -moms $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlforgetmoms\":\ \"/ | sed s/$/\",/
    #DLIS
    (/usr/bin/time -f "%e r" ./resol -cl -wl -forget -dlis $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlforgetdlis\":\ \"/ | sed s/$/\",/
    #DLIS0
    (/usr/bin/time -f "%e r" ./resol -cl -wl -forget -dlis0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlforgetdlis0\":\ \"/ | sed s/$/\",/
    #VSIDS
    (/usr/bin/time -f "%e r" ./resol -cl -wl -forget -vsids $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"clwlforgetvsids\":\ \"/ | sed s/$/\",/
    echo "        },"
done
echo "        { \"file\": \"end\" }"
echo "    ]"
echo "}"
