#! /usr/bin/bash

# остановка после первой ошибки
set -o errexit 

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
mkdir -p result
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

# Generate matrices
rm -f data/*.bin

mpisubmit.bg build/bin/matrix_gen
printf "Generating matrices"
while [ ! -f ./matrix_gen*.out ]
do
  sleep 1
  printf "."
done
rm *.out *.err >/dev/null
for i in {1..10};do printf "." && sleep 1; done
echo

printf "List of matrices:\n"
  ls data | cat -n
echo

# iterate through number of processes
:>result/result.txt
rm -f result/time.txt
rm -f result/error.txt
proc_num=("32" "64" "128" "256" "512")
for i in "${proc_num[@]}"
do
  i=256
  # Multiply vector by matrix
  mpisubmit.bg --nproc $i --wtime 00:05:00 --stdout result/time.txt --stderr result/error.txt build/bin/solve
  printf "Multiplication with $i processes"
  while [ ! -f result/time.txt ]
  do
    sleep 1
    printf "."
  done
  for j in {1..15};do printf "." && sleep 1; done
  ptime=$(cat result/time.txt)
  printf "Done in $ptime seconds\n"
  time_all="$i, $ptime"
  echo $time_all >>result/result.txt
  printf "Table:\n"
  cat result/result.txt
  printf "Errors:\n"
  if [  -f result/error.txt ]; then
    cat result/error.txt
    rm -f result/error.txt
  fi
  rm -f result/time.txt

done