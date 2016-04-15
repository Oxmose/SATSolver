#!/bin/bash

echo "Testing  SAT"

ERROR=0
for f in test_base/cnf/corr_test/sat/*.cnf
do
    echo "File: $f" | sed s/test_base\\/cnf\\/corr_test\\/sat\\///
    SOK='s SATISFIABLE'

    #STD
    S1=$((/usr/bin/time -f "%e r" ./resol $f) |& grep s)
    if [ "$S1" != "$SOK" ];
    then
        echo "ERROR"
        ERROR=1
    else
        echo "STD OK"
    fi
    #WL
    S2=$((/usr/bin/time -f "%e r" ./resol -wl $f) |& grep s)
    if [ "$S2" != "$SOK" ];
    then
        echo "ERROR"
        ERROR=1
    else
        echo "WL OK"
    fi
    #CL
    S3=$((/usr/bin/time -f "%e r" ./resol -cl -vsids $f) |& grep s)
    echo "S3 : $S3"
    if [ "$S3" != "$SOK" ];
    then
        echo "ERROR"
        ERROR=1
    else
        echo "CL OK"
    fi
done

if [ $ERROR != 0 ];
then
     echo "ERROR DURING SET TEST"
else
     echo "NO ERROR DETECTED"
fi
echo "DONE"
echo

echo "Testing  UNSAT"
ERROR=0
for f in test_base/cnf/corr_test/unsat/*.cnf
do
    echo "File: $f" | sed s/test_base\\/cnf\\/corr_test\\/unsat\\///
    SOK2='s UNSATISFIABLE'

    
    #CL
    S3=$((/usr/bin/time -f "%e r" ./resol -cl -wl $f) |& grep s)
    if [ "$S3" != "$SOK2" ];
    then
        echo "ERROR"
        ERROR=1
    fi 
done
if [ $ERROR != 0 ];
then
     echo "ERROR ON SET"
else
       echo "NO ERROR"
fi
echo "DONE"
