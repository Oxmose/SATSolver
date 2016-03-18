#!/bin/bash

#Set for sat 
echo "{"
echo "    \"SAT\": ["

for f in test_base/cnf/serial_test/sat/*/*.cnf
do
    echo "        {"
    echo "            \"file\": \"$f\"," | sed s/test_base\\/cnf\\/serial_test\\/sat\\///

    #STD
    (/usr/bin/time -f "%e r" ./resol $f) |& sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"std\":\ \"/ | sed s/$/\",/
    #WL
    (/usr/bin/time -f "%e r" ./resol -wl $f) |& sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"wl\":\ \"/ | sed s/$/\",/

    #RAND
    (/usr/bin/time -f "%e r" ./resol -rand $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"rand\":\ \"/ | sed s/$/\",/
    #RAND0
    (/usr/bin/time -f "%e r" ./resol -rand0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"rand0\":\ \"/ | sed s/$/\",/
    #MOMS
    (/usr/bin/time -f "%e r" ./resol -moms $f) |& sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"moms\":\ \"/ | sed s/$/\",/
    #DLIS
    (/usr/bin/time -f "%e r" ./resol -dlis $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"dlis\":\ \"/ | sed s/$/\",/
    #DLIS0
    (/usr/bin/time -f "%e r" ./resol -dlis0 $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"dlis0\":\ \"/ | sed s/$/\",/

    #HBET + WL
    #RAND
    #(/usr/bin/time -f "%e r" ./resol -rand -wl $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"randwl\":\ \"/ | sed s/$/\",/
    #RAND0
    #(/usr/bin/time -f "%e r" ./resol -rand0 -wl $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"rand0wl\":\ \"/ | sed s/$/\",/
    #MOMS
    #(/usr/bin/time -f "%e r" ./resol -moms -wl $f) |& sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"momswl\":\ \"/ | sed s/$/\",/
    #DLIS
    #(/usr/bin/time -f "%e r" ./resol -dlis -wl $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"dliswl\":\ \"/ | sed s/$/\",/
    #DLIS0
    #(/usr/bin/time -f "%e r" ./resol -dlis0 -wl $f) |& grep r | sed s/\ r// | sed s/^/\ \ \ \ \ \ \ \ \ \ \ \ \"dlis0wl\":\ \"/ | sed s/$/\"/
    echo "        },"
done
echo "        { \"file\": \"end\" }"
echo "    ]"
echo "}"
