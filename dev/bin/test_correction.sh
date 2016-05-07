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
        echo "STD ERROR"
        ERROR=1
    else
        echo "STD OK"
    fi

    S11=$((/usr/bin/time -f "%e r" ./resol -rand $f) |& grep s)
    if [ "$S11" != "$SOK" ];
    then
        echo "STD RAND ERROR"
        ERROR=1
    else
        echo "STD RAND OK"
    fi

    S111=$((/usr/bin/time -f "%e r" ./resol -rand0 $f) |& grep s)
    if [ "$S111" != "$SOK" ];
    then
        echo "STD RAND0 ERROR"
        ERROR=1
    else
        echo "STD RAND0 OK"
    fi

    S1111=$((/usr/bin/time -f "%e r" ./resol -dlis $f) |& grep s)
    if [ "$S1111" != "$SOK" ];
    then
        echo "STD DLIS ERROR"
        ERROR=1
    else
        echo "STD DLIS OK"
    fi

    S11111=$((/usr/bin/time -f "%e r" ./resol -dlis0 $f) |& grep s)
    if [ "$S11111" != "$SOK" ];
    then
        echo "STD DLIS0 ERROR"
        ERROR=1
    else
        echo "STD DLIS0 OK"
    fi

    #WL
    S2=$((/usr/bin/time -f "%e r" ./resol -wl $f) |& grep s)
    if [ "$S2" != "$SOK" ];
    then
        echo "WL ERROR"
        ERROR=1
    else
        echo "WL OK"
    fi
    #CL
    S3=$((/usr/bin/time -f "%e r" ./resol -cl $f) |& grep s)
    if [ "$S3" != "$SOK" ];
    then
        echo "CL ERROR"
        ERROR=1
    else
        echo "CL OK"
    fi

    #CL WL
    S4=$((/usr/bin/time -f "%e r" ./resol -cl -wl $f) |& grep s)
    if [ "$S4" != "$SOK" ];
    then
        echo "CL WL ERROR"
        ERROR=1
    else
        echo "CL WL OK"
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
    echo "File: $f" | sed s/test_base\\/cnf\\/corr_test\\/sat\\///
    SOK='s UNSATISFIABLE'

    #STD
    S1=$((/usr/bin/time -f "%e r" ./resol $f) |& grep s)
    if [ "$S1" != "$SOK" ];
    then
        echo "STD ERROR"
        ERROR=1
    else
        echo "STD OK"
    fi
    #WL
    S2=$((/usr/bin/time -f "%e r" ./resol -wl $f) |& grep s)
    if [ "$S2" != "$SOK" ];
    then
        echo "WL ERROR"
        ERROR=1
    else
        echo "WL OK"
    fi
    #CL
    S3=$((/usr/bin/time -f "%e r" ./resol -cl $f) |& grep s)
    if [ "$S3" != "$SOK" ];
    then
        echo "CL ERROR"
        ERROR=1
    else
        echo "CL OK"
    fi
    #CL WL
    S4=$((/usr/bin/time -f "%e r" ./resol -cl -wl $f) |& grep s)
    if [ "$S4" != "$SOK" ];
    then
        echo "CL WL ERROR"
        ERROR=1
    else
        echo "CL WL OK"
    fi
done

if [ $ERROR != 0 ];
then
     echo "ERROR DURING SET TEST"
else
     echo "NO ERROR DETECTED"
fi
echo "DONE"
