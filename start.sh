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

mkdir -p data result
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

mpisubmit.bg --nproc 1 build/bin/matrix_gen
printf "Generating matrices"
while [ ! -f $matrix_fn ]
do
  sleep 1
  printf "."
done
# for i in {1..10};do printf "." && sleep 1; done
echo

printf "List of matrices:\n"
  ls data | cat -n
echo

# iterate through number of processes
rm -f result/*.txt
proc_num=("32" "64" "128" "256" "512")
count=0
for i in "${proc_num[@]}"
do
  ((++count))
  # Multiply vector by matrix
  mpisubmit.bg --nproc $i --wtime 00:05:00 --stdout "result/time${i}.txt" --stderr "result/error${i}.txt" build/bin/solve
done

printf "Multiplying $count times\n"

printf "Multiplication"
sleep 1

#watch -n1 ls result

while [ "$(cat result/time*.txt 2>/dev/null| wc -l)" != $count ]
do
  sleep 1
  tmp=$(cat result/time*.txt 2>/dev/null| wc -l)
  printf "${tmp} of ${count} files ready\n"
done
echo
while [ "$(ls -l result/time*.err 2>/dev/null| wc -l)" != "0" ]
do
  sleep 1
  ls -l result/time*.err 2>/dev/null| wc -l
  printf "."
done
echo
for i in "${proc_num[@]}"
do
  # collect results in data/result.txt
  ptime=$(cat result/time${i}.txt)
  time_all="$i, $ptime"
  echo $time_all >>result/result.txt
done
printf "Table:\n"
cat result/result.txt
printf "Errors:\n"
if [  -f result/error*.txt ]; then
  cat result/error*.txt
fi

echo "Starting gnuplot..."
gnuplot plot/plot_max.gpl
