#! /usr/bin/bash
set -o errexit # остановка после первой ошибки
export PATH=/home/vadim/mpich-install/bin:$PATH
mpicc -Wall src/test.cpp -o build/bin/test
mpiexec -n 5 build/bin/test
