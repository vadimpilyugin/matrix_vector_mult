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
echo "Cleaning working directory..."
make clean
printf "Building project"

make all >/dev/null &
pid=$!
while [ -d /proc/$pid ]
do
  printf "."
  sleep 1
done
echo

PS3='Please enter your choice: '
options=("Generate matrices" "Multiply vector by matrix" "Check for errors" "Quit")
select opt in "${options[@]}"
do
    case $opt in
        "Generate matrices")
            echo "you chose choice 1"
            rm -f data/*.bin
	    mpisubmit.bg build/bin/matrix_gen
	    watch -n1 ls
	    ;;
        "Multiply vector by matrix")
            echo "you chose choice 2"
            mpisubmit.bg  -n $total_proc_num build/bin/solve
	    watch -n1 ls
	    ;;
        "Check for errors")
            echo "you chose choice 3"
	    mpisubmit.bg build/bin/matrix_check 
            watch -n1 ls
	    cat matrix_check*.err
	    ;;
        "Quit")
            break
            ;;
        *) echo invalid option;;
    esac
done

#mpisubmit.bg build/bin/matrix_gen
#mpisubmit.bg  -n $total_proc_num build/bin/solve
#mpiexec build/bin/matrix_check
