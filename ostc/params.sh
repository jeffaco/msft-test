#! /bin/bash

echo "Parameters specified: $#"
count=0

for i in "$@"; do
    count=$(( $count + 1 ))
    echo "$count: $i"
done
