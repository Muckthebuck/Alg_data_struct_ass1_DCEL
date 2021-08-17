
voronoi1: read_input.o print_to_file.o dcel.o main.o 
	gcc -Wall -o voronoi1 main.o dcel.o read_input.o print_to_file.o 

dcel.o: dcel.c dcel_struct.h vect.h dcel_struct.h dcel.h
	gcc -Wall -o dcel.o dcel.c -c

read_input.o: read_input.c
	gcc -Wall -o read_input.o read_input.c -c 

print_to_file.o: print_to_file.c
	gcc -Wall -o print_to_file.o print_to_file.c -c

main.o: main.c dcel.h read_input.h dcel_struct.h dcel.h watchtower.h vect.h print_to_file.h
	gcc -Wall -o main.o main.c -c


clean: voronoi1
	rm *.o voronoi1
