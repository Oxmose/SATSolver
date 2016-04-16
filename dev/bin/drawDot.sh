#!/bin/bash

echo "Drawing"
for f in *.dot
do
    printf "."
    dot -Tpng $f -o $f.png
done
echo
echo "END"
