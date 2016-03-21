#!/bin/bash
for f in test_base/cnf/serial_test/sat/*.cnf
do
echo "Testing $f"
./resol -wl $f 
a=$?
if [ $a -ne 0 ]; then
    echo "Error with $f!"
    break
else
    echo "Ok"
fi
done
for f in test_base/cnf/serial_test/unsat/*.cnf
do
echo "Testing $f"
./resol $f 
a=$?
if [ $a -ne 1 ]; then
    echo "Error with $f!"
    break
else
    echo "Ok"
fi
done
