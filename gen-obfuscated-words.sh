#!/bin/sh

for i in {1..39}
do
    tr -cd "[:alnum:]_" < /dev/urandom | fold -w $i | head -n1000
done