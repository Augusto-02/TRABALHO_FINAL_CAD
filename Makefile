# Compiladores e flags
CC     = gcc
MPICC  = mpicc
CFLAGS = -O2 -Wall
OMPFLAGS = -fopenmp

# Alvos principais
all: odd_even_serial odd_even_openmp odd_even_mpi benchmark_comparativo

# Serial
odd_even_serial: odd_even_serial.c
	$(CC) $(CFLAGS) -o odd_even_serial odd_even_serial.c

# OpenMP
odd_even_openmp: odd_even_openmp.c
	$(CC) $(CFLAGS) $(OMPFLAGS) -o odd_even_openmp odd_even_openmp.c

# MPI
odd_even_mpi: odd_even_mpi.c
	$(MPICC) $(CFLAGS) -o odd_even_mpi odd_even_mpi.c


# Limpeza de binários e arquivos temporários
clean:
	rm -f odd_even_serial odd_even_openmp odd_even_mpi 
	rm -f temp_output.txt *.csv


