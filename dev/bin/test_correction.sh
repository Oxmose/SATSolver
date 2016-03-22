#!/bin/bash

echo "Testing  SAT"

ERROR=0
for f in test_base/cnf/corr_test/sat/*.cnf
do
    echo "File: $f" | sed s/test_base\\/cnf\\/corr_test\\/sat\\///
    SOK='s SATIFIABLE'

    #STD
    S1=$((/usr/bin/time -f "%e r" ./resol $f) |& grep s)
    if [ "$S1" != "$SOK" ];
    then
        echo "ERROR"
        ERROR=1
    fi
    #WL
    S2=$((/usr/bin/time -f "%e r" ./resol -wl $f) |& grep s)
    if [ "$S2" != "$SOK" ];
    then
        echo "ERROR"
        ERROR=1
    fi
done

if [ $ERROR != 0 ];
then
     echo "ERROR"
fi
echo "DONE"
echo

echo "Testing  UNSAT"
ERROR=0
for f in test_base/cnf/corr_test/unsat/*.cnf
do
    echo "File: $f" | sed s/test_base\\/cnf\\/corr_test\\/unsat\\///
    SOK2='s UNSATISFIABLE'

    #STD
    S3=$((/usr/bin/time -f "%e r" ./resol $f) |& grep s)
    if [ "$S3" != "$SOK2" ];
    then
        echo "ERROR"
        ERROR=1
    fi
    #WL
    S4=$((/usr/bin/time -f "%e r" ./resol -wl $f) |& grep s)
    if [ "$S4" != "$SOK2" ];
    then
        echo "ERROR"
        ERROR=1
    fi 
done
if [ $ERROR != 0 ];
then
     echo "ERROR"
fi
echo "DONE"
