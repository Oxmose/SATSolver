#!/bin/bash
for f in */*.cnf
do
echo $f
sed -i -e s/^0$\// -e s/%// $f
done
