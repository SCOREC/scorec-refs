#!/bin/bash -ex
python toascii.py $1 tmp.bib
./fixrefs tmp.bib $2
rm -f tmp.bib
rm $1
