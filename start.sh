#! /usr/bin/bash

# остановка после первой ошибки
set -o errexit 

export PATH=/home/vadim/mpich-install/bin:$PATH

# Параметры программы
source ./config.cfg

# Программируем программу выдачи параметров
cat <<Input >include/params.h
#pragma once
const int m = $m;
const int n = $n;
const char *matrix_fn = "$matrix_fn";
const char *vector_fn = "$vector_fn";
const char *output_file = "$output_file";
Input

mkdir -p data
make clean
make all
build/bin/matrix_gen
mpiexec -n $total_proc_num build/bin/solve
build/bin/matrix_check