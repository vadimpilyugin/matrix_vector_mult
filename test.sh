#! /usr/bin/bash

bluegene=false

# the_world_is_flat=true
# # ...do something interesting...
# if [ "$the_world_is_flat" = true ] ; then
#     echo 'Be careful not to fall off!'
# fi

# остановка после первой ошибки
set -o errexit 

export PATH=/home/vadim/mpich-install/bin:$PATH

# Параметры программы
source ./config.cfg

mkdir -p include
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

PS3='Please enter your choice: '
options=("Generate matrices" "Multiply vector by matrix" "Check for errors" "All in one go" "Generate mapping" "Quit")
select opt in "${options[@]}"
do
    case $opt in
        "Generate matrices")
            echo "you chose choice 1"
            make clean
			make all
            rm -f data/*.bin
		    ./build/bin/matrix_gen
		    echo "Matrices: "
		    ls data | cat -n
	    ;;
        "Multiply vector by matrix")
            echo "you chose choice 2"
            printf "Time in seconds: "
		    mpiexec -n $total_proc_num build/bin/solve
		    echo
    	;;
        "Check for errors")
            echo "you chose choice 3"
		if [ "$bluegene" = true ]; then
		  mpisubmit.bg --nproc 1 build/bin/matrix_check
		else
	    	  mpiexec build/bin/matrix_check 
		fi
	    ;;
	    "All in one go")
			make clean
			make all
			rm -f data/*.bin
		    ./build/bin/matrix_gen
		    echo "Matrices: "
		    ls data | cat -n
			printf "Time in seconds: "
		    mpiexec -n $total_proc_num build/bin/solve
		    echo
		    mpiexec build/bin/matrix_check
		;;
		"Generate mapping")
			rm -f data/topology.map
                        g++ -o build/bin/topology src/topology.cpp
                        if [ "$bluegene" = true ]; then
                          mpisubmit.bg --nproc 1 ./build/bin/topology
                        else
                          build/bin/topology
                        fi
                        watch -n1 ls data

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

